/*
  This programs generates smiles.ann from bitmap files.
  File smiles.ann is training data for perceptron training
  system Petras.
  Author: Povilas Daniushis.
*/  
 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define HI        0.9
#define LO        0.1
#define PATERNS   321

int
 main()
 {
   FILE *fp;
   FILE *f;
   char out[128];
   char in[128];
   char c;
   int   i,j,b;
   int   nl=0,vn=0;
   int  targs[] = {1,1,1,0,1,0,1,0,0,1,
                   1,0,1,1,1,0,1,0,1,0,
                   0,0,1,1,0,0,0,1,1,0,
                   1,1,1,1,1,1,0,0,1,0,
                   1,0,1,0,1,1,1,1,1,1,
                   1,1,0,0,0,0,0,1,0,1,
                   0,0,1,0,1,0,0,1,0,0,
                   1,0,1,1,0,0,1,1,0,1,
                   0,1,0,0,1,1,0,1,1,1,
                   1,0,0,0,1,0,1,1,1,0,
                   1,1,1,0,1,1,0,0,1,1,
                   1,1,0,0,0,0,0,0,0,1,
                   0,0,1,1,1,1,1,1,1,1,
                   1,0,0,0,0,1,0,1,0,0,
                   0,1,0,0,0,0,0,0,1,0,
                   1,1,0,1,0,1,0,0,1,1,
                   0,1,0,1,1,0,1,0,0,0,
                   0,1,0,0,0,1,0,0,0,0,
                   0,0,1,0,1,1,0,0,0,0,
                   1,0,0,0,0,1,1,1,0,1,
                   0,1,0,1,1,1,0,1,0,1,
                   0,0,1,1,0,0,1,0,1,1,
                   0,1,1,0,1,0,0,1,0,1,
                   1,0,1,1,0,1,0,1,0,1,
                   0,0,1,1,0,1,0,1,1,1,
                   0,0,0,1,0,1,0,0,0,1,
                   1,1,0,1,1,1,0,1,1,1,
                   1,0,1,0,0,1,0,1,0,1,
                   0,0,0,1,0,1,1,1,0,1,
                   0,1,0,1,0,1,0,0,0,0,
                   1,0,0,1,0,1,1,0,1,1,
                   1,1,0,0,1,0,1,0,0,1,
                   1};

  srand(time(NULL)); 
  printf("Generating ANN script : \n");
  if ((f = fopen("smiles.ann","w")) == NULL)
   exit(printf("Error opening smiles.ann for writing . \n"));
  fprintf(f,"0\n3\n27 30 1\n0.25\n0.01\n10000\n%d\n",PATERNS);
  for (i = 0; i<PATERNS; i++)
  {
   memset(out,0,sizeof(out));
   sprintf(out,"%d",i+1);
   strcat(out,".bmp");
   vn+=targs[i];
   printf("Reading  %s (1 %d 0 %d)\n",out,vn,i-vn);
   fp = fopen(out,"r");
   if (fp == NULL)
     exit(printf("I/O error \n"));
   fprintf(f,"\n\n");
   j = 0;
   b = 0;
    while (!feof(fp))
    {
     j++;
     fscanf(fp,"%c",&c);
     if (j>62 && j<62+4*9)
      {
        if ((j - 62)%4)
        {
          fprintf(f,"%d ",c + rand()%7);
          b++;
        }
      }
    }
    printf("%d bytes\n",b);
   if (targs[i] == 1) fprintf(f,"\n %f \n",HI);
    else fprintf(f,"\n %f \n",LO);
   fclose(fp);
   }
    printf("Complete.\n");
   fclose(f);  
 }
 
 
