/*
  ANN v2 script parser
		       # this is comment
  type=MLP;	       # one type of MLP is defined	  
  act=F;	       # F=(sigmoid,tanh).	
  neurons=x1,x2,..xl;  # xk - unsigned integer; k = 1,...,l
  eta=e;	       # e - float
  epsilon=ep;	       # ep - float
  maxiter=max;	       # maxiter - unsigned long integer
  patterns=p	       # p - unsigned log integer,how many patterns learn	
  data=filename;       # read training data from file filename.
  
  padaryt pradinius svorius config.
  padaryt  ' = ' mode.
  
*/  
  
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>

#include "config.h"
#include "numbers.cpp"

#define N_ZO 0
#define N_MX 1

typedef struct
{
  int layers;
  int *neurons;
  float eta;
  int   activation;
  float epsilon;
  float momentum;
  unsigned long int maxiter;
  int patterns;
  int normalizing;
  int graph;
  float **datas;
  float **targs;
} MLP;   

typedef
 struct
{
 char *name;
 char *value;
} COMMAND;

char err_msg[128];

void 
 print(char *str,int lenght)
{
  for (int i = 0; i<lenght; i++)
   printf("%c",str[i]);
   printf("\n");
}

int 
 mask(char c)
  {
    if (
       ((int) c == EOF) ||
       ((int) c == 10 ) ||
       ((int) c == 13 ) 	
       ) return 1;
       return 0;
   }
 
int 
 parse(MLP *Mlp,char *cmd)
 {
   char buff[512];
   char *p;
   COMMAND *pcmd;

   memset(err_msg,0,sizeof(err_msg));
   strcpy(buff,cmd);

   /* remove commented */
   buff[strcspn(buff,"#")] = '\0'; 	   
   if (!strlen(buff)) return 0;
   
   /* ';' must be at end of any command */
   buff[strcspn(buff,";") + 1] = '\0';   

   /* test command syntax */
   if (!strchr(buff,'='))
    {
      strcpy(err_msg,"not valid command syntax: '=' not detected \n");
      return -1;
    }
   if ((strchr(buff,'=')) != (strrchr(buff,'=')))
    {
     strcpy(err_msg," '=' must be used only once in one command \n");
     return -1;
    } 
   if (buff[strlen(buff) - 1] != ';')       
    {
     strcpy(err_msg,"command must end with ';'\n");
     return -1;
    }
   if ((strchr(buff,';')) != (strrchr(buff,';')))    
   {
    strcpy(err_msg,"';' you must use ony at the end of command \n");
    return -1;
   }
   if (buff[0] == '=')
    {
     strcpy(err_msg,"null command name detected\n");
     return -1;
    }
   if (buff[strlen(buff) - 2] == '=')
    {
     strcpy(err_msg,"null command value detected\n");
     return -1;
    } 
       
  /* validate command and set MLP parameters (if passed) */
   p = strtok(buff,"=");
   pcmd = (COMMAND *)  calloc(1,sizeof(COMMAND));
   pcmd->name = (char *) calloc(strlen(p),sizeof(char));
   pcmd->name = p;
   p = strtok(NULL,";");
   pcmd->value = (char *) calloc(strlen(p),sizeof(char));
   pcmd->value = p;
   
   // o kas jei nera gale ; ar shiaip neteisinga syntax

   /* Set network type */
   if (!strcmp(pcmd->name,"type")) 
    {
     if (strcmp(pcmd->value,"mlp"))  
      {
       strcpy(err_msg,"type must be 'mlp'\n");
       return -1;
      }
      return 0;
    }
   /* Set activation function */
   if (!strcmp(pcmd->name,"act"))
    {
      if ((strcmp(pcmd->value,"sigmoid")) && (strcmp(pcmd->value,"tanh")))
       {
        strcpy(err_msg,"activation function must be sigmoid or tanh\n");
	return -1;
       }   
      if (!strcmp(pcmd->value,"sigmoid")) 
       {
        Mlp->activation = 0; //sigmoid
	return 0;
       }	
      else if (!strcmp(pcmd->value,"tanh"))
       {
        Mlp->activation = 1; //hyperbolic tangent
	return 0;
       } 
    }
     /* set neuron quantity in each layer */
      if (!strcmp(pcmd->name,"neurons")) 
       {
    	    char *q;
	    int   n,l = 0;
	    int   neurons[MAXLAYERS];
	    
	    q = strtok(pcmd->value,",");
    
	    while (q)
	     {
	       n = _atoi(q);  
	       if (n == -1)
	        {
		  strcpy(err_msg,"Neuron quantity must bee unsigned integer\n");
		  return -1;
		}
	       if (n < MINNEURONS || n > MAXNEURONS)	
	        {
		  strcpy(err_msg,"Neuron quantity not fits in [MINNEURONS,MAXNEURONS] (defined in config.h) \n");
		  return -1;
  		}
	       neurons[l] = n;	
	       l++;	
	       q = strtok(NULL,",");
	     } 
          if (l < MINLAYERS || l > MAXLAYERS)
            {
		  strcpy(err_msg,"layer number not fits in [MINLAYERS,MAXLAYERS] (defined in config.h) \n");
		  return -1;
	    }

	      Mlp->layers = l;
	      Mlp->neurons = (int *) calloc(l,sizeof(int));
	      memcpy(Mlp->neurons,neurons,l*sizeof(int));

#ifdef	DEBUG      
	      printf("%d \n",Mlp->layers);
	       for (n = 0; n < l; n++)
	        printf("->%d \n",Mlp->neurons[n]);
#endif		
	      return 0;
      }
      /* set eta */
       if (!strcmp(pcmd->name,"eta"))
        {
	 float e;
	 if (isfloat(pcmd->value) == 1)
	  e = atof(pcmd->value);
	 else if (isfloat(pcmd->value) == -1)
	  {
	   strcpy(err_msg,"Eta must be floating point number \n");
	   return -1;
	  }	  
	 if (e <= 0)
	  {
	   strcpy(err_msg,"Eta must have positive real value (in most cases recomended ~ 0.25) \n");
	   return -1;    
	  }
	  Mlp->eta = e;
	  return 0;
	}  
     /* set epsilon */
      if (!strcmp(pcmd->name,"epsilon") == 1)
       {
        float ep;
         if (isfloat(pcmd->value) == 1)
	  ep = atof(pcmd->value);
	 else if (isfloat(pcmd->value) == -1)
	  {
	   strcpy(err_msg,"Epsilon must be floating point number \n");
	   return -1;
	  }	  
	if (ep <= 0)
	 {
	  strcpy(err_msg,"Epsilon must have small positive value \n");
	  return -1;
	 }
	if (ep > 0.5) 
	 {
	  strcpy(err_msg,"You must be joking :). MSE > 0.5 ????? (if you are seriuos - edit parser.cpp and set what you want where if (ep > 0.5) .. ) \n");
	  return -1;
	 }
	 Mlp->epsilon = ep;
	 return 0;
       }
       /* set MAXITER */
       if (!strcmp(pcmd->name,"maxiter"))
	{
	 long int max;
	 max = _atoi(pcmd->value);
	 if (max < 0)
	  {
	   strcpy(err_msg,"MAXITER must be unsigned integer \n");
	   return -1;
	  }
	 Mlp->maxiter = max;
	 return 0;
	}       
       /* set patterns */
       if (!strcmp(pcmd->name,"patterns"))
	{
	 long int patterns;
	 patterns = _atoi(pcmd->value);
	 if (patterns <= 0)
	  {
	   strcpy(err_msg,"patterns must be positive unsigned integer \n");
	   return -1;
	  }
	 Mlp->patterns = patterns;
	 return 0;
	}       
       /* set defined momentum constant */    	
       if (!strcmp(pcmd->name,"momentum"))    	
       {
        float momentum;
	if (!isfloat(pcmd->value))
	 {
	  strcpy(err_msg,"momentum value must be floating point number \n");
	  return -1;
	 }
	momentum = atof(pcmd->value);
	if (momentum <= 0 || momentum >=1) 
	 {
	  strcpy(err_msg,"momentum must be from open interval (0,1) \n");
	  return -1;
	 }
        Mlp->momentum = momentum;	 
        return 0; /* momentum by default is defined in mlp.cpp so we will not chek before loading data*/
       }	
      /* normalizing method */ 
      if (!strcmp(pcmd->name,"normalizing"))
       {
	if (strcmp(pcmd->value,"N_MX") && strcmp(pcmd->value,"N_ZO"))
	 {
	  strcpy(err_msg,"unknown normalization methos: use N_MX to do MX=0 and DX=1 or use N_ZO to compress data into [0,1] \n");
	  return -1;
	 }
        Mlp->normalizing = (strcmp(pcmd->value,"N_MX") ? 0 : 1 );
        return 0; 
       }	
       /* read training data */
        if (!strcmp(pcmd->name,"data"))
	 {
	   FILE *fp = fopen(pcmd->value,"r");
	   int  i,j;
	   if (fp == NULL)
	    {
	      strcpy(err_msg,"cant read data file");
	      return -1;
	    }	 
          if (!Mlp->layers || 		
	      !Mlp->neurons ||
	      !Mlp->eta ||
	      !Mlp->epsilon ||
	      !Mlp->patterns ||
	      !Mlp->maxiter)    /* momentum by default is defined */
	      {
	       strcpy(err_msg,"you must set ALL network parameters befora reading data \n");
	       return -1;
	      }  
	    //allocate memory and read data directly to Mlp.
	    Mlp->datas = (float **) calloc(Mlp->patterns,sizeof(float *));
	    Mlp->targs = (float **) calloc(Mlp->patterns,sizeof(float *));
	     for (i = 0; i<Mlp->patterns; i++)
	     {
	       Mlp->datas[i] = (float *)calloc(Mlp->neurons[0],sizeof(float));
	       Mlp->targs[i] = (float *)calloc(Mlp->neurons[Mlp->layers - 1],sizeof(float));
	     }  	   
	     for (i = 0; i<Mlp->patterns; i++)
	     {
	      for (j = 0; j<Mlp->neurons[0]; j++)
	       if ((fscanf(fp,"%f",&Mlp->datas[i][j])) == 0)
	        {
	         strcpy(err_msg,"Invalid Mlp->datas \n");
	         return -1;
	        }
	      for (j = 0; j<Mlp->neurons[Mlp->layers - 1]; j++)
	       if ((fscanf(fp,"%f",&Mlp->targs[i][j])) == 0)
	        {
    		 strcpy(err_msg,"Invalid Mlp->targs");
	         return -1;
	        }
  	      }
	       /*
	       printf("%d \n",Mlp->patterns);
	       printf("%d \n",Mlp->neurons[0]);
	       printf("%d \n",Mlp->neurons[Mlp->layers - 1]); 
	       */
	       return 0;
          }	
		   if (!strcmp(pcmd->name,"graph"))
		    {
		     if (!strcmp(pcmd->value,"on")) Mlp->graph = 1;
		      else if (!strcmp(pcmd->value,"off")) Mlp->graph = 0;
    		        else  
			{
			 strcpy(err_msg,"value for \"graph\" must be \"on\" of \"off\" (default - \"on\")\n");
			 return -1;
			}        
		  return 0;
		  }
      strcpy(err_msg,"parser function end reached (not valid command words) \n");
      return -1;
 }


int
  read_file(MLP *Mlp,char *filename)
{
 char cmd[512];
 char *p;
 int   bytes_read = 1;
 FILE *fp;
 int   l;
 int   line = 0;
 int   i;

 Mlp->activation = 0;
 Mlp->patterns = 0;
 Mlp->neurons = NULL;
 Mlp->eta = 0;
 Mlp->epsilon = 0;
 Mlp->maxiter = 0;
 Mlp->momentum = 0;
 Mlp->graph = 1;
 Mlp->normalizing = N_ZO;	// by default normalize into [0,1] 
 strcpy(err_msg,""); 
 
 if ((fp = fopen(filename,"r")) == NULL) return -1;
 p = cmd;
 memset(cmd,0,sizeof(cmd));
 while (!feof(fp))
 {
   bytes_read = 0;
   while (!mask(*p++ = fgetc(fp)))  bytes_read++;
   line++;
//   printf("%d %d\n",line,bytes_read);
   if (!bytes_read) 
    { 
     p = cmd;
     memset(cmd,0,sizeof(cmd));
     continue; 
    }
    for (l = 0; l<bytes_read; l++)
     cmd[l] = tolower(cmd[l]); 
   i = 0;     
   while (!isalpha(cmd[i]) && cmd[i] != '#') i++;
    for (l = 0; l<bytes_read - i; l++)
     cmd[l] = cmd[l + i];
     if (parse(Mlp,cmd) == -1) 
      {
       fclose(fp);
       printf("error at line %d : \n",line);
       printf("%s",err_msg);
       return -1;
      }
     p = cmd;
     memset(cmd,0,sizeof(cmd));
   if (feof(fp)) break;
 } 
  fclose(fp);
  return 0;
}
