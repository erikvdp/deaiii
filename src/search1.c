#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SHIFTOR.h"

#define BUFFER_SIZE 100000 //MOET GROTER ZIJN DAN JE ROLLOVER

int check=0; //globale variabele staat op 1 als zoekstring > 64 chars

int main(int argc, char **argv){
	if(argc <= 2)
		printf("syntax: ./searchx  <zoekstringfile>  <bestand>  [<bestand>  ...]\n");
	else{
		unsigned long long *S =(long long *)malloc(sizeof(long long)*256);
		int pos=0,zoekl,zoekl2;
		int c=0,i,aantal,exit=0;
		unsigned char * buffer;
		char * zoek;
		char * zoek2 = NULL;
		FILE *t,*z;

		/* zoekstring max 64 chars in 1 keer indien meer => bruteforce voor de rest*/
		z = fopen(argv[1],"rb");
		if(!z){
			printf("could not open %s\n",argv[1]);
			return -1;
		}
		c=fgetc(z);
		if(c == EOF){
			printf("%s is an empty file\n",argv[1]);
			return -1;
		}
		zoek = (char *)malloc(64*sizeof(char)); 
		buffer=(char*)malloc(sizeof(char)*(BUFFER_SIZE+1)); //n+1
		zoek[pos++]=(char)c;
		do{ //inlezen
			c = fgetc(z);
			if(c != EOF)
				zoek[pos++] = (char)c;
		}while(c != EOF && pos < 63);
		zoek[pos] = 0;
		zoekl=pos;
		pos=0;
		if(c != EOF){ //de rest in een extra buffer
			check=1;
			zoek2 =(char *)malloc(1024*sizeof(char));
			do{
				c = fgetc(z);
				if( c!=EOF){
					zoek2[pos++]= (char)c;
					if(pos % 1024 ==0)
						zoek2=(char *)realloc(zoek2,pos*2);
				}
			}while(c != EOF);
			zoek2[pos]=0;
		}
		zoekl2=pos;
		if(zoekl+zoekl2 > BUFFER_SIZE){
			printf("Opgegeven zoekstring is te groot\n");
			printf("Probeer een kleinere zoekstring in te geven\n");
		}
		bereken_karakteristieke_vectoren(zoek,zoekl,S);
		aantal=0;


		/* files overlopen */
		for(i=2; i < argc; i++){
			t = fopen(argv[i],"rb");
			if(t) {
				readAll(t,buffer,zoek,zoek2,zoekl,zoekl2,S,&aantal,argv[i]);
				//printf("aantal voorkomens in %s %d\n",argv[i],aantal);
				//aantal=0;
			}
			else{
				printf("could not open %s\n",argv[i]);
				fclose(t);
				fclose(z);
				free(zoek);
				if(zoek2)
					free(zoek2);
				free(S);
				free(buffer);
				return -1;
			}
		}
		fclose(t);
		fclose(z);
		free(zoek);
		if(zoek2)
			free(zoek2);
		free(S);
		free(buffer);
		return 0;
	}
}

void readAll(FILE * f,unsigned char *c,unsigned char * zoek, unsigned char * zoek2,int zoekl, int zoekl2,unsigned long long S[],int * aantal,char * filename){

	int startindex=1,tekstl,i=0,end=0; //start=index van het eerste char dat in de buffer zit
	int n=BUFFER_SIZE; //het aantal in te lezen chars in 1 keer DEFAULT= 1000000
	int rollover = zoekl+zoekl2-1; //moet de grootte van de zoekstring zijn -1

	//eerste maal inlezen
	i= fread(c+i,1,BUFFER_SIZE,f);
	c[i] = 0;
	SHIFTOR(zoek2,c,S,aantal,zoekl,zoekl2,i,startindex,filename);
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
		SHIFTOR(zoek2,c,S,aantal,zoekl,zoekl2,tekstl,startindex,filename);
		memmove(c,c+(n-rollover),rollover); //rollover vooraan zetten
	}
}

void bereken_karakteristieke_vectoren(unsigned char *zoek, int zoekl, unsigned long long S[]) { 
	unsigned long long j, max=0;
	int i=0;
	//kar vectoren invullen
	for (i = 0; i < 256; i++) 
		S[i] = ~0; 
	for (j = 1,i=0; i < zoekl; i++) { 
		S[zoek[i]] &= ~j; 
		j <<=1; //j=j*2
	} 
} 

void print_karakteristieke_vectoren(unsigned long long S[]){
	int i;
	for(i=97; i <= 122; i++)
		printf("%c %llu\n",i,S[i]);
}

void SHIFTOR(unsigned char * zoek2,unsigned char *tekst, unsigned long long S[],int * aantal,int zoekl,int zoekl2,int tekstl,int startindex,char * filename) { 
	unsigned long long kolom,mask=1; 
	int j,x=0;
	//mask maken
	mask = mask<<(zoekl-1);
	mask = ~mask;
	/* zoeken */
	kolom = ~0;
	for (j = 0; j < tekstl; j++) { 
		kolom = (kolom<<1) | S[tekst[j]]; 
		if ((kolom | mask) == mask){
			//bruteforce controleren indien nodig
			if(check == 1){
				j++;
				while(x < zoekl2 && zoek2[x] == tekst[j]){
					j++;
					x++;
				}
				if(x==zoekl2){
					printf("%s:%d\n",filename,startindex+j-(zoekl+zoekl2));
					(*aantal)++;
				}
			}
			else{
				printf("%s:%d\n",filename,startindex+j-(zoekl-1));
				(*aantal)++;
			}
		}
	} 
}
