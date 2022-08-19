/*

       Source Code of Petras (Perceptron TRAining System)
                          2 0 0 3
            by Povilas Daniushis,paralax@freemail.lt

History:
 2003.06.21-23 v 1.00 - simplest BP network
 2003.08.19    v 1.03 - added alpha,fixed some bugs.
 2003.08.20    v 1.10 - added simple interactive menu.
 2003.09.06    v 1.11 - added data normalization routines.
 2003.09.10    v 1.15 - added simple *.ann synax test funcion.
 2003.09.10    v 1.20 alpha - added sigle layer perceptron unit.
 2003.09.23    v 1.20 beta - fixed some bugs and added <sim> command to console
 2003.09.29    v 1.20 added BMP picture database with smile/non smile faces.
 2003.11.05    v 1.21 added tanh() activation and some improvements.
 2003.11.28    v 1.22 added momentum,fixed bug ( program crashed if MAXITER < 100 )
 2004.01.27    v 1.41 added parser for ANN 2 script (not compatible with old ANN)
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
//#include <process.h>  //uzkomentuot unixam sita daikta

#include "mlp.cpp"
#include "iofunct.cpp"
#include "interactive.cpp"
#include "config.h"


// ir nieko butu padaryt ka nors panasaus i testsyntax
// (is mlp.cpp)
extern int patterns;
extern float **datas,**targs; //,**outputs;
//You are not alowed to delete or modify this line
char *Author_Signature=
"Author of this program (C) Povilas Daniuis,paralax@freemail.lt";

void Help(char *name)
{
#ifdef UNIX
  printf("%c[32m",0x1b);
#endif 
  printf(" +--------------+ Petras %s +------------+\n",VERSION);
  printf(" |            perceptron training system      |\n");
  printf(" +--------------------------------------------+\n");
#ifdef UNIX
  printf("%c[0m",0x1b);
#endif   
  printf("Usage: %s [-fslupvih] [file1 (file2)]\n",name);
  printf("Type %s -h for more detailed help or see docs\n",name);
  printf("Author: Povilas Daniushis, pdaniusis@ik.su.lt\n");
#ifdef UNIX
  printf("%c[34m",0x1b);
#endif 
   printf("http://ik.su.lt/~pdaniusis (in lithuanian)\n");
#ifdef UNIX
  printf("%c[0m",0x1b);
#endif   
   
}


int
 main(int argc,char **argv)
 {
  NET Net;
  int  i;
  char c;
  int p_file  = 0; //process *.ann script
  int s_file  = 0; //save weights to file
  int l_file = 0;  //load neural network weights form file
  int u_network = 0; //only use network,whed alived
  int print_output = 0; //print output
  int verbose = 0; //verbose mode
  int interactive = 0;
  int test = 1;
  pid_t t = getpid();
  char msg[128];

  srand(time(NULL));

  memset(msg,0,sizeof(msg));
  sprintf(msg,"-- Petras session started (pid = %d) --- \n",t);

  if (LOG_LEVEL) WriteToLog(msg);

#ifndef UNIX
 if (LOG_LEVEL) WriteToLog("Going Interactive() \n");
 Interactive();
 if (LOG_LEVEL) WriteToLog("Terminating sesion \n***\n\n");
 return 0;
#endif

#ifdef UNIX
/*
All problems are in getopt().If you have one form Windows,you easy
can fix it.If you ported Petras onto Windows,please let me know.
*/
  if (argc < 2)
  {
   Help(argv[0]);
   return 0;
  }
    while ((c = getopt(argc,argv,"fslupvih")) != EOF)
     switch (c)
     {
     case 'h':
           system("less intercom.txt");
	   return 0;
	   break;	   
     case 'f':
          p_file = 1;
          break;
    case 's':
          s_file = 1;
          break;
   case 'l':
          l_file = 1;
          break;
   case 'u':
          u_network = 1;
          break;
   case  'p':
          print_output = 1;
          break;
   case 'v':
          verbose = 1;
          break;
   case 'i':
          interactive = 1;
          break;
   default:
          Help(argv[0]);
          return 0;
          break;
   }


 if (interactive) Interactive();

/* Pagal prioriteta pirmas TrainFromFile() */
 if (p_file)
 {
  for (i = 1; i<argc; i++)
  {
   FILE *fp = fopen(argv[i],"r");
   if (fp)
   {
    fclose(fp);
    if (TrainFromFile(&Net,argv[i],verbose,0) != 1)
    {
     printf("STOP\n");
     return -1;
    }
    test = 0;
    break;
   }
 }
 if (test) exit(printf("File (1) not found !\n"));
 if (print_output)
 {
 if (verbose)
  printf("Output of neural network after training:\n");
 for (int j = 0; j < patterns; j++)
 {
  Input(&Net,datas[j]);
  PropagateNetwork(&Net);
  PrintOutput(&Net);
 }
}
 // Toliau ieskom ar reikia issaugoti outputa
 if (s_file)
 {
  if (!SaveNN(&Net,argv[i+1])) perror("Can't save NN");
  else if (verbose)  printf("NN saved to %s\n",argv[i+1]);
 }
 return 0;
}
 // Toliau,jeigu turimas apmokytas tinklas,ji galima
 // atgaivinti ir naudoti.
if (l_file)
 {
 for (i = 1; i<argc; i++)
  {
   FILE *fp = fopen(argv[i],"r");
   if (fp)
   {
    fclose(fp);
    if (!AliveFromFile(&Net,argv[i],verbose))
      exit(printf("File not found\n"));

    if (verbose) printf("Network alived !\n");
    break;
   }
  }
  if (u_network)
    if (!UseFromFile(&Net,argv[i+1],verbose))
     exit(printf("File not found \n"));
   return 0;
 }
#endif
  return 0;
 }
