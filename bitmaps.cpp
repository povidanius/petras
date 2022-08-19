/*
   This is simple routines to do graphics of MSE function
   2004.03.01 
   Author: Povilas Daniushis, pdaniusis@ik.su.lt
*/   


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>


typedef struct
 {
  unsigned short int type;
  unsigned int size;
  unsigned short int reserved1,reserved2;
  unsigned int offset;
 } HEADER;
  
typedef struct
{
  unsigned int size;
  int width,height;
  unsigned short int planes;
  unsigned short int bits;
  unsigned int compression;
  unsigned int imagesize;
  int xresolution,yresolution;
  unsigned int ncolors;
  unsigned int importantcolors;
} INFOHEADER; 

typedef struct 
{
  unsigned short int r,g,b;
}  TRUECOLOR_BITMAP;



void 
 MSE_graph(float *mse_data,int size)
{
 char *bmp_type="BM";
 HEADER hdr;
 INFOHEADER ihdr;
 TRUECOLOR_BITMAP image[256][256];
 int i,j;
 FILE *fp = fopen("out.bmp","w");
 FILE *fp1 = fopen("mask.bmp","r");
 
 if (fp == NULL || fp1 == NULL) exit(printf("io error in function bmp_open()\n"));

 fseek(fp1,54,SEEK_SET);
 fread(&image,256*256*3,1,fp1); // nuskaitom sablona
 fclose(fp1);
 
 
/* uzpildom BMP headeri */

 memcpy(&hdr.type,bmp_type,2);
 hdr.size = 196662;			//failo dydis
 hdr.reserved1 =  hdr.reserved2 = 0;
 hdr.offset = 54; 			// header + infoheader 

/* uzpildom infoheaderi */
 ihdr.size = 40;
 ihdr.width = 256;
 ihdr.height = 256;
 ihdr.planes = 1;
 ihdr.bits   = 24;
 ihdr.compression = 0; 
 ihdr.imagesize = 196608; 
 ihdr.xresolution = 2834;
 ihdr.yresolution = 2834;
 ihdr.ncolors =	0; 
 ihdr.importantcolors = 5;
 
 /* irasom abu headerius */
 fwrite(&hdr.type,2,1,fp);
 fwrite(&hdr.size,4,1,fp);
 fwrite(&hdr.reserved1,2,1,fp);
 fwrite(&hdr.reserved2,2,1,fp);
 fwrite(&hdr.offset,4,1,fp);
 
 fwrite(&ihdr,40,1,fp);

 /* geriau rasyt ant sablono ir sablona irasyt */
 for (j = 0; j<ihdr.height; j++)
   for (i = 0; i<ihdr.width; i++)
  {
     if (j == ceil(mse_data[i]*500 + 1))  
      {
       image[j][i].b = 0;
       image[j][i].g = 0;
       image[j][i].r = 0;
      } 
  } 
  
 fwrite(image,ihdr.height * ihdr.width * 3,1,fp);
 fclose(fp);
}

