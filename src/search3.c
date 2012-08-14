#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include "BMH.h"
#define max(x1,x2) ((x1) > (x2) ? (x1):(x2))

#define BUFFER_SIZE 100000

int main(int argc, char **argv){
	if(argc <= 2)
		printf("syntax: ./searchx  <zoekstringfile>  <bestand>  [<bestand>  ...]\n");
	else{
		int tab[256];
		int * temp, m,aantal=0;
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
		//preprocessing
		makejumptable(zoek,tab,m);


		/* files overlopen */
		for(i=2; i < argc; i++){
			t = fopen(argv[i],"rb");
			if(t){
				//printf("resultaten voor %s\n",argv[i]);
				readAll(t,buffer,zoek,m,tab,&aantal,argv[i]);
			}
			else{
				printf("could not open %s\n",argv[i]);
				fclose(t);
				fclose(z);
				free(zoek);
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
	}
}

void readAll(FILE * f,char * c,unsigned char * zoek,int zoekl,int * tab,int * aantal,char * filename){

	int startindex=1,tekstl,i=0,end=0; //start=index van het eerste char dat in de buffer zit
	int n=BUFFER_SIZE; //het aantal in te lezen chars in 1 keer DEFAULT= 1000000
	int rollover = strlen(zoek)-1; //moet de grootte van de zoekstring zijn -1

	//eerste maal inlezen
	i= fread(c+i,1,BUFFER_SIZE,f);
	if(i==0) //lege file
		return;
	c[i] = 0;
	bmSearch(zoek,zoekl,c,i,tab,aantal,startindex,filename);
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
		bmSearch(zoek,zoekl,c,tekstl,tab,aantal,startindex,filename);
		memmove(c,c+(n-rollover),rollover); //rollover vooraan zetten
	}
}

void makejumptable(unsigned char * zoek, int * tab,int m){
	int i;
	//opvullen met max jump
	for(i=0; i < 256; i++)
		tab[i]=-1;
	for(i=0; i<m-1;i++)
		tab[zoek[i]]=i;
	//es uitprinten
	//for(i=0; i < 256; i++)
	//	printf("%c %d \n",i,tab[i]);
}


void bmSearch(unsigned char *zoek,int zoekl,unsigned char *tekst,int tekstl,int *tab,int * aantal,int startindex,char * filename)
{
	int i=0, j=0; //i=index waarop je telkens het begin van P in de T zet
	while (i<=tekstl-zoekl){ //binnen bereik blijven
		j=zoekl-1;
		//einde & begin matchen
		if(zoek[zoekl-1] == tekst[i+j] && zoek[0] == tekst[i]){
			j--;
			while (j>=0 && zoek[j]==tekst[i+j]){ 
				j--; //alles gaat goed
			}
			if (j<0){
				printf("%s:%d\n",filename,i+startindex);
				(*aantal)++;
			}
		}
		i+=zoekl-1;
		i-=tab[tekst[i]]; //shift over bad char
	}
}
