#ifndef FUNCTIES_H
#define FUNCTIES_H

void readAll(FILE * f,char * buffer,unsigned char * zoek,int zoekl,int * s,int * tab,int * aantal,char * filename);
void makejumptable(unsigned char * zoek, int * tab,int zoekl);
void bmSearch(unsigned char *zoek,int zoekl,unsigned char *tekst,int tekstl,int *s,int *tab,int *aantal,int startindex,char * filename);
void goodsuff(unsigned char *zoek,int *f,int*s,int m);
#endif
