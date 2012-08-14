#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include "KMP.h"

#define BUFFER_SIZE 1000000
int vergelijk_strings(unsigned char * zoek, unsigned char * tekst, int start, int gelijke_tekens,int zoekl){
	while(zoek[gelijke_tekens] == tekst[start+gelijke_tekens] && gelijke_tekens < zoekl)
		gelijke_tekens++;
	return gelijke_tekens;
}
int * berekenv(unsigned char* zoek,int zoekl){
	int *v = (int*)malloc(sizeof(int)*zoekl);/*dynamisch alloceren*/
	int start=1,gelijke_tekens=0,eerste_mismatch=0,i;

	v[0]=1;
	if(zoekl == 1) //lengte van de zoekstring is 1
		return v;
	v[1]=1;

	while(start < zoekl-1){
		eerste_mismatch=vergelijk_strings(zoek,zoek,start,gelijke_tekens,zoekl);

		if(eerste_mismatch == 0){
			v[start+1]=start+1;
			gelijke_tekens=0;
		}
		else{
			for(i=1; i <=eerste_mismatch && (start+i)<zoekl; i++){
				if(v[start+i]< 1 || v[start+i]>zoekl) //fix
				v[start+i]=start;
			}
		gelijke_tekens=eerste_mismatch-v[eerste_mismatch];
		}
		start=start+v[eerste_mismatch];
	}
	return v;
}

void printv(unsigned char * zoek, int * v){
	int j= 0;
	int max=strlen(zoek);
	while(j < max){
		printf("%c%d\n",*zoek,*v);
		j++;	
		zoek++;
		v++;
	}
}
void KMP(unsigned char * zoek, unsigned char * tekst,int * v,int* aantal,int startindex,int m, int n,char * filename){
	int start=0; 
	int gelijke_tekens=0;
	int eerste_mismatch;
	//printv(zoek,v);

	while(start <= n - m){
		eerste_mismatch = vergelijk_strings(zoek,tekst,start,gelijke_tekens,m);
		if(eerste_mismatch == m){
			printf("%s:%d\n",filename,start+startindex);
			(*aantal)++;
			start++;
			eerste_mismatch=0;
		}
		else{
			start=start+v[eerste_mismatch];
			if(eerste_mismatch==0)
				gelijke_tekens=0;
			else
				gelijke_tekens=eerste_mismatch-v[eerste_mismatch];
		}
	}
}

void readAll(FILE * f,unsigned char * c,unsigned char * zoek,int *v,int * aantal,int zoekl,char * filename){

	int startindex=1,tekstl,i=0,end=0; //start=index van het eerste char dat in de buffer zit
	int n=BUFFER_SIZE; //het aantal in te lezen chars in 1 keer DEFAULT= 1000000
	int rollover = zoekl-1; //moet de grootte van de zoekstring zijn -1

	//eerste maal inlezen
	i= fread(c+i,1,BUFFER_SIZE,f);
	if(i==0) //lege file
		return;
	c[i] = 0;
	KMP(zoek,c,v,aantal,startindex,zoekl,i,filename);
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
		KMP(zoek,c,v,aantal,startindex,zoekl,tekstl,filename);
		memmove(c,c+(n-rollover),rollover); //rollover vooraan zetten
	}
}

int main(int argc, char **argv){
	if(argc <= 2)
		printf("syntax: ./searchx  <zoekstringfile>  <bestand>  [<bestand>  ...]\n");
	else{
		FILE *t,*z;
		int  * v;
		int aantal=0,i,c=0,zoekl=0,exit=0;
		unsigned char * zoek, *buffer;
		/* zoekstring */
		z=fopen(argv[1],"rb");
		if(!z){
			printf("Could not find %s\n",argv[1]);
			return -1;
		}
		c =fgetc(z);
		if(c == EOF){
			printf("%s is an empty file\n",argv[1]);
			return -1;
		}
		zoek = malloc(sizeof(char)*1024); //zoekstring default 1024 groot
		buffer = malloc(sizeof(char)*(BUFFER_SIZE+1));
		zoek[zoekl++]=(char)c;
		do{
			c = fgetc(z);
			if(c != EOF) zoek[zoekl++] = (char)c;
			if(zoekl % 1024 == 0)
				zoek = realloc(zoek,sizeof(char)*strlen(zoek)*2);
		}while(c != EOF);
		zoek[zoekl] = 0;
		if(zoekl > BUFFER_SIZE){
			printf("Opgegeven zoekstring is te groot\n");
			printf("Probeer een kleinere zoekstring in te geven\n");
		}
		v= berekenv(zoek,zoekl);

		/* files overlopen */
		for(i=2; i < argc; i++){
			t=fopen(argv[i],"rb");
			if(!t){
				printf("Could not find %s\n",argv[i]);
				fclose(t);
				fclose(z);
				free(v);
				free(zoek);
				free(buffer);
				return -1;
			}
			readAll(t,buffer,zoek,v,&aantal,zoekl,argv[i]);
			//printf("aantal matches voor %s: %d\n",argv[i],aantal);
			//aantal=0;
		}	
		fclose(t);
		fclose(z);
		free(v);
		free(zoek);
		free(buffer);
		return exit;
	}
}
