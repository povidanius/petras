/*
  This is console for Petras

  Author: Povilas Daniushis,paralax@freemail.lt,2003

  Version:      0.2
  History:      0.0 0.1 0.2

  TODO:
   padaryt,kad svorius po viena butu galima kaitaliot.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <memory.h>

#include "config.h"

#ifdef UNIX
#define SHELLCMD  "/bin/sh"
#define HELPCMD   "less intercom.txt"
#else
#define SHELLCMD "cmd"
#define HELPCMD   "edit intercom.txt"
#endif
#define BUFLEN    64
#define TRUE      1
#define FALSE     0

#define NONE      0
#define SLP       1
#define MLP       2

extern int patterns;
extern float **datas,**targs; //,**outputs;
extern int stop_training;

struct          /*isstatint*/
 {
    NET  Net;
    int  active;
    char type;
 } Nets[MAXNETS];

#ifdef UNIX
void s_ign()
 {
/*
 This program is desiged 4 freeBSD,in some linux systems you
 may have tho change signal() syntax.
 */
 signal(SIGINT,(__sighandler_t*)s_ign);
 puts("\nAuch! SIGINT cought.\nAll processes stopped\n(Type exit if you want quit console)");
 stop_training = 1; //whein sigint() cought training is aborted.
}
#endif

#ifndef UNIX	 //do this stuff 4 windoze if you are'nt wery lazy at this moment :).
void s_ign()     // kol kas neveikia !!!!!!!!!!!!!!!!
{
// signal(SIGINT,(_p_sig_fn_t)s_ign);
// puts("\nAuch! SIGINT cought.\nAll processes stopped\n(Type exit if you want quit console)");
// stop_training = 1;
}
#endif

/* This function parses *cmd into separate strings
*
*
*/
int
 parse(char *cmd,char prsed[][BUFLEN])
{
 char ptrn[]=" ",*q;
 int  i = -1;
  q = strtok(cmd,ptrn);
  while (q)
   {
    i++;
    strcpy(*prsed++,q);
    q = strtok(NULL,ptrn);
   }
   return i;
}
/*
* Returns index of first non active net.
*
*/
int retindex()
{
 int i;
  for (i = 1; i<=MAXNETS; i++)
   if (Nets[i].active == FALSE) return i;
  return 0;
}
/*
* Returns NET type. (ar this moment MLP || MLP :)
*
*/
char
 *Nettype(char code)
 {
   switch (code)
   {
    case 0: return "NONE";
    case 1: return "SLP";
    case 2: return "MLP";
    default: return "Unknown";
   }
 }

void
 Interactive()
 {
  NET *Net;
  char cmd[BUFLEN];
  char *p,parsed[BUFLEN][BUFLEN];
  int  i,j;
  int failed;
  int *neurons;

#ifdef UNIX
 signal(SIGINT,(__sighandler_t *)s_ign); 
#endif
#ifndef UNIX
// signal(SIGINT,SIG_IGN);
#endif

 for (i = 0;i < MAXNETS;i++)
  Nets[i].active = 0;
#ifdef UNIX
  printf("%c[32m",0x1b);
#endif 
 printf("\n\t+---------------------------------------------------------+\n");
 printf("\t|Perceptron training system `Petras` by Povilas Daniushis |\n");
 printf("\t|                           %s                       |\n",VERSION);
 printf("\t+---------------------------------------------------------+\n");
#ifdef UNIX
  printf("%c[0m",0x1b);
#endif   
 printf("Neuro-console ready.Enter command or ? if you want help\n");

if (MINLAYERS<2 || MINNEURONS<1)
  exit(printf("Stop. Invalid  MINLAYERS || MINNEURONS\n"));

 while (1)
 {
  memset(cmd,0,sizeof(cmd));
  p = cmd;
  fflush(stdout);
#ifdef UNIX
 printf("%c[34m",0x1b);
#endif 
  printf("petra$ ");
#ifdef  UNIX
 printf("%c[0m",0x1b);
#endif 
  j = 0;
  failed = FALSE;

  while (!iscntrl((*p++ = getchar()))) j++;
   if (j>BUFLEN)
    {
     printf("Command to long as %d > %d \n",j,BUFLEN);
     continue;
    }
     else if (j == 0) continue;
    cmd[strlen(cmd)-1]='\0';
    memset(parsed,0,sizeof(parsed));
    parse(cmd,parsed);
/*
*   Suported commands : 
*
*
*/
  if (!strcmp(parsed[0],"exit"))
   {
    signal(SIGINT,SIG_DFL);
    printf("Bye!\n");
    exit(0);
   }
  else
  if (!strcmp(parsed[0],"shell"))
    system(SHELLCMD);
  else
  if (!strcmp(parsed[0],"show"))
  {
   j = 0;
    for (i = 0; i<MAXNETS; i++)
     if (Nets[i].active)
     {
      j++;
      printf("%d - th network active.Type : %s\n",i,Nettype(Nets[i].type));
     }
    if (j>0)
     printf("all other - non active \n");
     printf("total %d network(s) active\n",j);
  }
  else
  if (!strcmp(parsed[0],"help") || !strcmp(parsed[0],"?")) system(HELPCMD);
  else
  /* create multiplayer perceptron */
  if (!strcmp(parsed[0],"mp"))
   {
     failed = FALSE;
      if (( j = retindex()) == 0)
      {
       failed = TRUE;
       printf("No aviable indexes. Increase NETINDEX in interactive.cpp\nFailed.\n");
      }
     if (_atoi(parsed[1])<MINLAYERS || _atoi(parsed[1])>MAXLAYERS)
     {
      printf("Invalid layers : %s\nFailed.\n",parsed[1]);
      failed = TRUE;
     }
     else
     {
       neurons = new int[_atoi(parsed[1])];
       printf("Setting up %d-layer perceptron\n",_atoi(parsed[1]));
        for ( i = 0; i<_atoi(parsed[1]); i++)
         if (_atoi(parsed[i+2])>=MINNEURONS && _atoi(parsed[i+2])<=MAXNEURONS)
         {
           printf("Layer [%d] -> %d\n",i,_atoi(parsed[i+2]));
           neurons[i] = _atoi(parsed[i+2]);
         }
          else
           {
            printf("Invalid neuron number %s\nFailed.\n",parsed[i+2]);
            failed = TRUE;
            break;
           }
      }
       if (failed == FALSE)
          {
           CreateBPNet(&Nets[j].Net,neurons,_atoi(parsed[1]));
           Nets[j].active = TRUE;
       Nets[j].type = MLP;
           printf("Created.Index %d\n",j);
          }
   }
 /* set initial (random) weights.See in mlp.cpp */
 else if (!strcmp(parsed[0],"r"))
  {
   failed = FALSE;
  if (_atoi(parsed[1])  < 1 ) 
  {
   failed = 1;
   printf("net index %s lower than MAXNETS\n.Failed.\n",parsed[1]);
  }
  if (_atoi(parsed[1]) > MAXNETS)
   {
    failed = 1;
    printf("Net index %s greater than MAXNETS\n.Failed.\n",parsed[1]);
   } 
  if (Nets[_atoi(parsed[1])].active == FALSE)
   {
     failed = 1;
     printf("Net with index %s not active\n.Failed.\n",parsed[1]);
   }
   else
    {
     printf("Randomizing weights of NN[%s]\n",parsed[1]);
     RandomWeights(&Nets[_atoi(parsed[1])].Net);
    }
  }
   /* print weights */
  else if (!strcmp(parsed[0],"w"))
  {
     failed = 0;
  if (Nets[_atoi(parsed[1])].active == TRUE)
      PrintWeights(&Nets[_atoi(parsed[1])].Net);
  else
   {
    failed = 1;
    printf("Net with index %s not active\nFailed.\n",parsed[1]);
   }
  }
  /* propagate and print output */
 else if (!strcmp(parsed[0],"o"))
 {
     failed = 0;
  if (Nets[_atoi(parsed[1])].active == TRUE)
  {
      PropagateNetwork(&Nets[_atoi(parsed[1])].Net);
      PrintOutput(&Nets[_atoi(parsed[1])].Net);
  }
  else
   {
    failed = 1;
    printf("Net with index %s not active\nFailed.\n",parsed[1]);
   }
 }
 /* input from file */
 else if (!strcmp(parsed[0],"i"))
 {
     FILE *fp;
     failed = 0;
      if (Nets[_atoi(parsed[1])].active == FALSE)
      {
       failed = 1;
       printf("Net with index %s not active\nFailed.\n",parsed[1]);
      }
      if ((fp = fopen(parsed[2],"r")) == NULL)
      {
       failed = 1;
       perror("Reading data file");
      }
      if (failed == FALSE)
      {
        Net = (NET *)&Nets[_atoi(parsed[1])].Net;
        j = Net->InputLayer->Units;
//      printf("input layer units %d\n",j);
        float *_data = new float[j+1];
        printf("Loading input data from %s\n",parsed[2]);
         for (i = 0; i< j ; i++)
           fscanf(fp,"%f",&_data[i]);
         Input(&Nets[_atoi(parsed[1])].Net,_data);
         free(_data);
      }
  }
   /* simulate from file */
  else if (!strcmp(parsed[0],"sim"))
  {
      FILE *fp;
      failed = 0;

      if (Nets[_atoi(parsed[1])].active == FALSE)
      {
       failed = 1;
       printf("Net with index %s not active\nFailed.\n",parsed[1]);
      }
      else
      if ((fp = fopen(parsed[2],"r")) == NULL)
      {
       failed = 1;
       printf("Reading data file '%s':",parsed[2]);
       perror("");
      }
      if (failed == FALSE)
      {
        Net = (NET *)&Nets[_atoi(parsed[1])].Net;
        j = Net->InputLayer->Units;
        float *_data = new float[j+1];
        printf("Simulating net %s from file %s\n",parsed[1],parsed[2]);
    do
    {
         for (i = 0; i< j ; i++)
           fscanf(fp,"%f",&_data[i]);
        if (feof(fp)) break;
         Input(&Nets[_atoi(parsed[1])].Net,_data);
     PropagateNetwork(&Nets[_atoi(parsed[1])].Net);
     PrintOutput(&Nets[_atoi(parsed[1])].Net);
    } while (!feof(fp));
         free(_data);
    }

  }
  /* alive form file */
  else if (!strcmp(parsed[0],"a"))
  {
     failed = 0;
  if (( j = retindex()) == 0)
  {
   failed = 1;
   printf("No aviable indexes. Increase NETINDEX in interactive.cpp \nFailed.\n");
  }
  if (failed == FALSE)
      if ((AliveFromFile(&Nets[j].Net,parsed[1],1)) == 1)
       {
        Nets[j].active = TRUE;
        Nets[j].type = 2;
        printf("Alived. Index %d\n",j);
       }
       else
       {
        failed = TRUE;
           printf("File %s not found\nFailed.\n",parsed[1]);
       }
  }
    /*  Train from file */
  else if (!strcmp(parsed[0],"t"))
  {
     failed = 0;
 if (( j = retindex()) == 0)
  {
   failed = 1;
   printf("No aviable indexes. Increase NETINDEX in interactive.cpp\n.Failed.\n");
  }
  if (failed == FALSE)
  if ((TrainFromFile(&Nets[j].Net,parsed[1],1,0)) == 1)
   {
    Nets[j].active = TRUE;
    Nets[j].type = 2;
    printf("Trained. Index %d\n",j);
   }
    else
     {
      failed = TRUE;
      printf("(I/O or internal error)Failed.\n");
     }
  }
   else if (!strcmp(parsed[0],"d"))
 {
     failed = 0;
  if (Nets[_atoi(parsed[1])].active == TRUE)
  {
//        CIA!!!!
//       if (&Nets[_atoi(parsed[1])].Net != NULL)
//        free(&Nets[_atoi(parsed[1])].Net);
        Nets[_atoi(parsed[1])].active = 0;
        printf("Deleted\n");
  }
  else
   {
    failed = 1;
    printf("Net with index %s does not exist\n",parsed[1]);
   }
 }
  else if (!strcmp(parsed[0],"s"))
  {
     FILE *fp;
     failed = FALSE;
  if (Nets[_atoi(parsed[1])].active == FALSE)
  {
   failed = 1;
   printf("Net with index %s does not active \nFailed.\n",parsed[1]);
  }
  if (failed == FALSE)
  {
   if (strcmp(parsed[3],"f") && (fp = fopen(parsed[2],"r")) != NULL)
   {
    fclose(fp);
    printf("File %s exists. Use f as 4-th argument to overwrite\nFailed.\n",parsed[2]);
   } else
      if ((SaveNN(&Nets[_atoi(parsed[1])].Net,parsed[2])) == 1)
        printf("Saved.\n");
       else
       {
        failed = TRUE;
        perror("Input file\nFailed.\n");
       }
    }
  } 
    else if (!strcmp("cw",parsed[0])) //svoriu keitimas po viena.
   {
     int net,l,i,j;                  // neleidzia nuliniu !!!!
    if (
        ((_atoi(parsed[1])) == -1) ||
        ((_atoi(parsed[2])) == -1) ||
        ((_atoi(parsed[3])) == -1) ||
        ((_atoi(parsed[4])) == -1)
        )
       {
        printf("Not valid positive integer detected in parameters \n");
        failed = TRUE;
       } else
    {
     net = _atoi(parsed[1]);
     if (Nets[net].active == FALSE) { printf("Net %d not active \n",net); failed = TRUE; }
    }
    if (failed == FALSE)
    {
    l   = _atoi(parsed[2]);
    i   = _atoi(parsed[3]);
    j   = _atoi(parsed[4]);
    Net = (NET *)&Nets[net].Net;

    if (Net->Layers - 1 < l) { printf("To big layer \n"); failed = TRUE; }
    if (l < 0) { printf("To low layer \n"); failed = TRUE; }
    if (failed == FALSE)
    {
     if (Net->Layer[l]->Units < i) { printf("To big upper weight index \n"); failed = TRUE; }
     if (i< 1) { printf("To low upper weight index \n"); failed = TRUE; }

     if (Net->Layer[l-1]->Units < j) { printf("To big lower weight index \n"); failed = TRUE; }
     if (j < 1) { printf("To low lowe weight index \n"); failed = TRUE; }

     if (isfloat(parsed[5]) == 0) { printf("Weight value isnt float \n"); failed = TRUE; }
    }

    if (failed == FALSE)
     Net->Layer[l]->Weight[i][j] = atof(parsed[5]);
    else printf("failed.\n");
    }
   }
   /* default */
    else  printf("Unknown command (%s ... )\n",cmd);
 } // while (1)
}

