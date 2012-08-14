#ifndef FUNCTIES_H
#define FUNCTIES_H


void readAll(FILE * f,unsigned char * buffer,unsigned char * zoek,unsigned char * zoek2,int zoekl,int zoekl2,unsigned long long S[],int * aantal,char * filename);
void bereken_karakteristieke_vectoren(unsigned char *zoek, int zoekl, unsigned long long S[]);
void SHIFTOR(unsigned char *zoek2, unsigned char *tekst, unsigned long long S[],int * aantal,int zoekl,int zoekl2,int tekstl,int startindex,char * filename);
void print_karakteristieke_vectoren(unsigned long long S[]);



#endif

