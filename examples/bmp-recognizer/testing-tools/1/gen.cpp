#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

int
 main()
 {
   FILE *fp;
   FILE *f;
   char out[128];
   char in[128];
   char c;
   int   i,j,b;

  srand(time(NULL));
  printf("Generating validation set : \n");
  f = fopen("validation.set","w");
  for (i = 0; i>-1; i++)
  {
   memset(out,0,sizeof(out));
   sprintf(out,"%d",i+1);
   strcat(out,".bmp");
   printf("Reading %s\n",out);
   fp = fopen(out,"r");
   if (fp == NULL)
       exit(printf("Complete"));
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
   fclose(fp);
   }
   fclose(f);
 }

