#ifndef FUNCTIES_H
#define FUNCTIES_H

void readAll(FILE * f,unsigned char * buffer,unsigned char * zoek,int *v,int * aantal,int zoekl,char * filename);
int * berekenv(unsigned char * zoek,int zoekl);
int vergelijk_strings(unsigned char * zoek, unsigned char * tekst, int start, int gelijke_tekens,int zoekl);
void KMP(unsigned char * zoek, unsigned char * tekst, int  *v,int * aantal,int startindex,int zoekl,int tekstl,char * filename);
void printv(unsigned char * zoek, int * v);
#endif
