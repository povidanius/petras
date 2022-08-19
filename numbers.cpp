#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int
 _atoi(char *x)
{
 char *p;
 p = x;
 while (*p)
  if (!isdigit(*p++)) return -1;
 return atoi(x);
}

int                     
 isfloat(char *x)      
 {    
  unsigned int i;                
  int t = 0;
  if (!isdigit(x[strlen(x) -1 ])) 
    return -1; 
  for (i = 0; i<strlen(x); i++)
   {
    if ((!isdigit(x[i])) && (x[i] != '.')) return -1; 
    if (x[i]  == '.') t++;
    if (t > 1) return -1;
   }    
  return 1;
 }

