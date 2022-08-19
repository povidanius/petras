#include "config.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>

 int WriteToLog(char *Msg)
 {
 /* pid_t t = getpid();
  char fname[128];
  memset(fname,0,sizeof(fname));
  sprintf(fname,"%s%d","./logs/",t);
  strcat(fname,".log");
  FILE *fp = fopen(fname,"a+");
  if (fp == NULL)
  {
   perror("WriteToLog():");
   return -1;
  }
  fprintf(fp,"%s\n",Msg);
  fclose(fp);*/
  return 1;
 }
