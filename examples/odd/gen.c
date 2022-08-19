#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
 int i;
 int a,b,c;
  
 srand(time(NULL));
 
 a = 9; b=9; c=9;
 
 printf("%f %f %f\n",(float)1/a,(float)1/b,(float)1/c);
 exit(0);
 
 i = 0;
 while (i<15)
  {
     a = (rand()/10)%10;
     b = (rand()/10)%10;
     c = (rand()/10)%10;
     
    if ((a!=0) && (b!=0) && (c!=0) && ((c%2) != 0)) 
    {
     i++;
     printf("%f %f %f\n",(float)1/a,(float)1/b,(float)1/c);
/*     if ((c%2) == 0) printf("1\n");
      else printf("0\n");*/
    }    
   }
}
