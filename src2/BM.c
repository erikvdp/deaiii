#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include "BM.h"
#define max(x1,x2) ((x1) > (x2) ? (x1):(x2))

#define BUFFER_SIZE 100000

int main(int argc, char **argv){
	if(argc <= 2)
		printf("syntax: ./searchx  <zoekstringfile>  <bestand>  [<bestand>  ...]\n");
	else{
		int tab[256];
		int * temp, *tab2,m,aantal=0;
		char *zoek, *buffer;
		FILE *t;
		int c=0,pos=0,i;

		/* zoekstring */
		FILE *z = fopen(argv[1],"rb");
		if(z==NULL){
			printf("could not open %s\n",argv[1]);
			return -1;
		}
		c=fgetc(z);
		if(c == EOF){
			printf("%s is an empty file\n",argv[1]);
			return -1;
		}
		zoek = malloc(sizeof(char)*1024); //zoekstring default 1024 groot
		buffer = malloc(sizeof(char)*BUFFER_SIZE+1);
		zoek[pos++] = (char)c;
		do{ // read one line
			c = fgetc(z);
			if(c != EOF) zoek[pos++] = (char)c;
			if(pos % 1024 == 0)
				zoek=realloc(zoek,sizeof(char)*strlen(zoek)*2);
		}while(c != EOF);
		zoek[pos] = 0;
		m=pos;
		if(m > BUFFER_SIZE){
			printf("Opgegeven zoekstring is te groot\n");
			printf("Probeer een kleinere zoekstring in te geven\n");
			free(zoek);
			free(buffer);
			fclose(z);
			return -1;
		}
		//array f en s dynamisch alloceren
		temp=malloc(sizeof(int)*(m+1));
		tab2=malloc(sizeof(int)*(m+1));
		//preprocessing
		makejumptable(zoek,tab,m);
		goodsuff(zoek,temp,tab2,m);


		/* files overlopen */
		for(i=2; i < argc; i++){
			t = fopen(argv[i],"rb");
			if(t){
				//printf("resultaten voor %s\n",argv[i]);
				readAll(t,buffer,zoek,m,tab2,tab,&aantal,argv[i]);
			}
			else{
				printf("could not open %s\n",argv[i]);
				fclose(t);
				fclose(z);
				free(temp);
				free(zoek);
				free(tab2);
				free(buffer);
				return -1;
			}
			//printf("aantal matches voor %s: %d\n",argv[i],aantal);
			//aantal=0;
		}	
		fclose(t);
		fclose(z);
		free(buffer);
		free(zoek);
		free(temp);
		free(tab2);
	}
}

void readAll(FILE * f,char * c,unsigned char * zoek,int zoekl,int * s,int * tab,int * aantal,char * filename){

	int startindex=1,tekstl,i=0,end=0; //start=index van het eerste char dat in de buffer zit
	int n=BUFFER_SIZE; //het aantal in te lezen chars in 1 keer DEFAULT= 1000000
	int rollover = strlen(zoek)-1; //moet de grootte van de zoekstring zijn -1

	//eerste maal inlezen
	i= fread(c+i,1,BUFFER_SIZE,f);
	if(i==0) //lege file
		return;
	c[i] = 0;
	bmSearch(zoek,zoekl,c,i,s,tab,aantal,startindex,filename);
	memmove(c,c+(n-rollover),rollover); //rollover vooraan zetten

	//lees BUFFER_SIZE characters in
	while(!feof(f)){
		startindex=startindex+BUFFER_SIZE-rollover; //startindex updaten
		i = fread(c+rollover,1,BUFFER_SIZE-rollover,f);
		if(i== BUFFER_SIZE-rollover){ //niet op het einde 
			c[BUFFER_SIZE]=0;
			tekstl=BUFFER_SIZE;
		}
		else{
			c[i+rollover]=0;
			tekstl=i+rollover;
		}
		bmSearch(zoek,zoekl,c,tekstl,s,tab,aantal,startindex,filename);
		memmove(c,c+(n-rollover),rollover); //rollover vooraan zetten
	}
}

void makejumptable(unsigned char * zoek, int * tab,int m){
	int i;
	//opvullen met max jump
	for(i=0; i < 256; i++)
		tab[i]=-1; //waarde in tab2 is altijd groter!
	for(i=0; i<m;i++)
		tab[zoek[i]]=i;
}
/*
* Implementatie van goodsuffix heuristiek
* Resultaat komt in tab2, en tab2[i] bevat 
* de shift indien er een mismatch op positie i-1 gebeurd
* temp[i] bevat de grootste border van de suffix startend op pos i
* border van x = substring die zowel prefix als suffix van x is.
* border(abcdabc)=abc => temp[0] = 4
*/
void goodsuff(unsigned char *zoek,int *temp,int *tab2,int m){
	int i, j=m+1;
	//arrays initieel op 0 zetten
	for(i=0; i < m+1; i++){
	  temp[i]=0;
	  tab2[i]=0;
	}
	//case1
	temp[m]=j;
	i=m;
	while (i>0){//werken van rechts naar links
		while (j<m+1 && zoek[i-1]!=zoek[j-1]){
			if (tab2[j]==0) 
			   tab2[j]=j-i; 
			j=temp[j];
		}
		i--;
		j--;
		temp[i]=j;
	}
	//case 2
	j=temp[0]; //in temp[0] zit de max border van het volledige patroon, overal invullen waar tab2[i] nog 0 is
	for (i=0; i<m+1; i++){
		if (tab2[i]==0) 
		   tab2[i]=j;
		if (i==j) 
		   j=temp[j];
	}
}


void bmSearch(unsigned char *zoek,int zoekl,unsigned char *tekst,int tekstl,int *tab2,int *tab,int * aantal,int startindex,char * filename)
{
	int i=0, j=0;
	while (i<=tekstl-zoekl){ //binnen bereik blijven
		j=zoekl-1;
		while (j>=0 && zoek[j]==tekst[i+j]){ 
			j--; //alles gaat goed
		}
		if (j<0){
			printf("%s:%d\n",filename,i+startindex);
			(*aantal)++;
			i+=tab2[0]; //case2 indien het beginpatroon vd zoekstring ng elders in de zoekstring voorkomt anders
		}
		else 
			i+=max(tab2[j+1], j-tab[tekst[i+j]]); //shift over de langste afstand bad/good heuristiek
	}
}
