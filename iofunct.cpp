#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//#include "numbers.cpp"
#include "parser.cpp"

int SaveNN(NET *,char *);
int AliveFromFile(NET *,char *,int);
int UseFromFile(NET *,char *,int);
int TrainFromFile(NET *,char *,int,int);
int testannsyntax(MLP *,char *);
void PrintRezult(MLP *);


int SaveNN(NET *Net,char *filename)
{
  FILE *fp = fopen(filename,"w");
  if (!fp)
  {
    printf("I/O error !\n");
    return -1;
  }
  printf("Saving to file %s\n",filename);
  fprintf(fp,"%d\n",Net->Layers);
//  printf("L = %d\n",Net->Layers);
  for (int l = 0; l<Net->Layers; l++)
   fprintf(fp,"%d\n",Net->Layer[l]->Units);

   fprintf(fp,"%f\n",Net->Eta);
   fprintf(fp,"%f\n",Net->Normalizer);
   fprintf(fp,"%f\n",Net->Tau);
   fprintf(fp,"%f\n",Net->Activation);  //

 for (int l = 1; l<Net->Layers; l++)
   for (int j = 0; j<=Net->Layer[l-1]->Units; j++)
    for (int n = 1; n<=Net->Layer[l]->Units; n++)
     fprintf(fp,"%f\n",Net->Layer[l]->Weight[n][j]);
  fclose(fp);
  return 1;
}

int AliveFromFile(NET *Net,char *filename,int verbose)
{
FILE *fp = fopen(filename,"r");
int layers;
int *neurons;

if (!fp) return -1;


 fscanf(fp,"%d",&layers);
 neurons = new int [layers+1];
 for (int l = 0; l<layers; l++)
   fscanf(fp,"%d",&neurons[l]);
  CreateBPNet(Net,neurons,layers);
  fscanf(fp,"%f",&Net->Eta);
  fscanf(fp,"%f",&Net->Normalizer);
  fscanf(fp,"%f",&Net->Tau);
  fscanf(fp,"%f",&Net->Activation);

  for (int l = 1; l<Net->Layers; l++)
   for (int j = 0; j<=Net->Layer[l-1]->Units; j++)
    for (int n = 1; n<=Net->Layer[l]->Units; n++)
     fscanf(fp,"%f",&Net->Layer[l]->Weight[n][j]);
  fclose(fp);
  return 1;
}

int UseFromFile(NET *Net,char *filename,int verbose)
{
  FILE *fp = fopen(filename,"r");
  float *input;
  int   n;
  char cmd[128];

 if (!fp) return 0;
 memset(cmd,0,sizeof(cmd));
 if (verbose) printf("Scanning how many patterns .. \n");
 fscanf(fp,"%s",cmd);
 patterns = _atoi(cmd);
  if (patterns < 1)
  {
   printf("Error: '%s' not valid patterns \n",cmd);
   return -1;
  }

 if (verbose) printf("Allocating memory of %d patterns \n",_atoi(cmd));
 input   = new float [patterns];
 if (verbose) printf("Loading data .. \n");
 for (n=0;n<patterns;n++)
  {
  for (int i = 1; i<=Net->InputLayer->Units; i++)
   fscanf(fp,"%f",&input[i-1]);
  Input(Net,input);
  PropagateNetwork(Net);
  PrintOutput(Net);
 }
  fclose(fp);
 return 1;
}

int TrainFromFile (NET *Net,char  *filename,int verbose,int mode)
{
 int   Sucess = 1;
 int  i,j;
 MLP   Mlp;

 time_t t = time(NULL);
 fflush(stdout);
 printf("Parsing %s .. ",filename);
 if (read_file(&Mlp,filename) == -1)
    return -1;
   printf(" ok \n");
//   PrintRezult(&Mlp);
   fflush(stdout);
   printf("Setting up %d - layer perceptron:\n",Mlp.layers);
   CreateBPNet(Net,Mlp.neurons,Mlp.layers);
   for (i = 0; i<Mlp.layers;i++)
    printf("%d neuron(s) in %d - th layer \n",Mlp.neurons[i],i);
   printf("Randomizing weights \n");
   RandomWeights(Net);
   printf("Constants: \n");
   printf("\t Net->Eta = %f \n",Mlp.eta);
   printf("\t Net->Epsilon = %f \n",Mlp.epsilon);
   printf("\t Net->Maxiter = %d \n",Mlp.maxiter);
   printf("\t MSE graph generation = %d \n",Mlp.graph);
   if (Mlp.momentum) 
    printf("\t Net->Momentum = %f \n",Mlp.momentum);
   Net->Eta = Mlp.eta;
   Net->Activation = Mlp.activation;
   Net->graph = Mlp.graph;
  if (Mlp.momentum) Net->Momentum = Mlp.momentum; 
   printf("Activation function = %d \n",Mlp.activation);
   printf("Allocating memory for training data arrays.. (%d patterns)\n",Mlp.patterns);
   datas = (float **)calloc(Mlp.patterns,sizeof(float *));
    for (i = 0; i<Mlp.patterns;i++)
     datas[i] = (float *)calloc(Mlp.neurons[0],sizeof(float));
   targs = (float **)calloc(Mlp.patterns,sizeof(float *));
     for (i = 0; i<Mlp.patterns;i++)
      targs[i] = (float *)calloc(Mlp.neurons[Mlp.layers - 1],sizeof(float));

 for (i = 0; i<Mlp.patterns; i++)
  for (j = 0; j<Net->InputLayer->Units; j++)
    datas[i][j] = Mlp.datas[i][j];

 for (i = 0; i<Mlp.patterns; i++)
  for (j = 0; j<Net->OutputLayer->Units; j++)
    targs[i][j] = Mlp.targs[i][j];
  patterns = Mlp.patterns;
  printf("Training perceptron (please by patient)\n");
 if (!BpTrain(Net,datas,targs,Mlp.patterns,Mlp.maxiter,Mlp.epsilon))
       Sucess = 0;
if (verbose && Sucess)
 printf("Sucess! (M{Err} is lower than epsilon = %f)\n",Mlp.epsilon);
 else if (verbose && !Sucess)
 printf("Failture ! (M{Err} is higher then epsilon = %f)\n",Mlp.epsilon);
 if (verbose) printf("Training complete in aprox. %d second(s) \n",time(NULL) - t );
 return 1;
}


void
 PrintRezult(MLP *Mlp)
 {
  int i,j;
  printf("Mlp->layers = %d \n",Mlp->layers);
  printf("Mlp->neurons = { ");
  for ( i = 0; i<Mlp->layers; i++)
   printf("%d ",Mlp->neurons[i]);
  printf(" } \n");
 printf("Mlp->eta = %f \n",Mlp->eta);
  printf("Mlp->epsilon = %f \n",Mlp->epsilon);
  printf("Mlp->maxiter = %d \n",Mlp->maxiter);
  printf("Mlp->patterns = %d \n",Mlp->patterns);
  for (i = 0; i<Mlp->patterns; i++)
  {
   printf("Mlp->datas[%d] = { ",i);
    for (j = 0; j<Mlp->neurons[0]; j++)
     printf("%f ",Mlp->datas[i][j]);
    printf(" } \n");
     printf("Mlp->targs[%d] = {",i);
     for (j = 0; j<Mlp->neurons[Mlp->layers-1]; j++)
      printf("%f ",Mlp->targs[i][j]);
     printf(" } \n");
  }
 }

    
