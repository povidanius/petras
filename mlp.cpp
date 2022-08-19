/*
Dar karta dekui Kartsen Kudzu,ir kurio programos as supratau
BP algoritma ir kuri kaip pvz. buvo naudojama rasant sita programa.

Author: Povilas Daniushis,paralax@freemail.lt,2003

TODO: Genetiniu algoritmu mokymas
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#include "log.cpp"
#include "bitmaps.cpp"


#define ETA_MUL_OK          1.07
#define ETA_MUL_SUX         0.79
#define ETA             0.25    /*mokymo greitis*/
#define siACTIVATION     0
#define thACTIVATION     1
#define NIU              0.9    /* Inercijos daugiklis - ijungiam intercija; rulezz */
#define ALFA             0.9    /* Sigmoidines funckijos priplojantis daugikliukas tiesinio regiono padidinimui */
#define SIGMOID(x) (float)1/(1+exp(-ALFA*x)) /*sigmoidine aktyvacijos f-ja,naudojam jei reikia tik + outputu */
#define TANH(x) (float)tanh(x) /* hiperbolinio tangento aktyvacijos f-ja ,naudojan jei reikia ir + ir - outputut */
#define BIAS            1
#define MAXDATA             1   /* normalise data to [0,MAXDATA] */
#define OUTMAX              1   /* output extension to (0,OUTMAX) */

static int patterns;
static float **datas,**targs; //,**outputs;
static int stop_training = 0; //controled by signal handler,kol kas only in unix systems

typedef struct
{
  int          Units;
  float*       Output;
  float*       Error;
  float**      Weight;
  float**      SavedWeight;
  float**      SavedDelta;
} LAYER;

typedef struct
{
  LAYER**        Layer;
  LAYER*         InputLayer;
  LAYER*         OutputLayer;
  float          Eta;
  float          Activation; //Aktyvacijos funkcijos tipas (sigmoid() - 0, tanh() - 1)
  float          Error;
  int            Layers;
  float          MX;
  float          Normalizer;
  float          Tau;
  float	         Momentum;
  int 		 graph;	
} NET;

typedef struct
{
  float*** weight;
  float    fitnes;
  float    prob;
} MEMB;

typedef struct
{
 MEMB**  member;
 float   mutation;
 int     size;
} POPULATION;

// Inicializavimas,I/O
void CreateBPNet(NET *Net,int *,int);
float RandomWeight(NET *);
void RandomWeights(NET *);
void SetEta(NET *,float);
void Input(NET *,float *);
void Output(NET *,float *);
//Tinklo outputo gavimas
void PropagateLayer(NET *,LAYER *,LAYER *);
void PropagateNetwork(NET *);
//Tinklo klaidos radimas ir BP algoritmas
void ComputeOutputError(NET *,float *);
void BackpropagateLayer(NET *,LAYER *,LAYER *);
void SaveWeights(NET *);
void RestoreWeights(NET *);
void BackpropagateNet(NET *);
void BpIteration(NET*, float* , float*);
int  BpTrain(NET *,float **,float **,int,int,float);
int  NetWeights(NET *);
//GA funckijos
void SetUpPopulation(POPULATION *,NET *,int);
void CreateInitPopulation(POPULATION *,NET *);
void FindFitness(POPULATION *,NET *,float **,float **,int);
void FindProbs(POPULATION *,NET *);
//Pagalbines f-jos
void PrintOutput(NET *);
void PrintWeights(NET *);
void PrintMemberWeights(MEMB *,NET *);
void PrintFitnes(POPULATION *);
int  NeedNormalize(float *,int);
void NormalizeMatrix(NET *,float **,int);
void NormalizeVector(NET *,float *);
int isLinearSeperable(NET* Net, float *input, float* Target);

void CreateBPNet(NET *Net,int neurons[],int layers)
{
  int i,l;

 srand(time(NULL));

 Net->Layer = (LAYER**)calloc(layers,sizeof(LAYER*));

 for (l = 0; l<layers; l++)
 {
   Net->Layer[l] = (LAYER*) malloc (sizeof(LAYER));

   Net->Layer[l]->Units  = neurons[l];
   Net->Layer[l]->Output = (float*) calloc (neurons[l]+1,sizeof(float));
   Net->Layer[l]->Error  = (float*) calloc (neurons[l]+1,sizeof(float));
   Net->Layer[l]->Weight = (float**)calloc (neurons[l]+1,sizeof(float*));
   Net->Layer[l]->SavedWeight = (float **) calloc(neurons[l]+1,sizeof(float*));
   Net->Layer[l]->SavedDelta = (float **) calloc(neurons[l] + 1,sizeof(float *));
   Net->Layer[l]->Output[0] = BIAS;

   if (l>0)
    for (i = 1; i<=neurons[l];i++)
    {
     Net->Layer[l]->Weight[i] = (float*)calloc(neurons[l-1]+1,sizeof(float));
     Net->Layer[l]->SavedWeight[i] = (float *)calloc(neurons[l-1]+1,sizeof(float));
     Net->Layer[l]->SavedDelta[i] = (float *)calloc(neurons[l-1]+1,sizeof(float));
    }
}
  Net->InputLayer = Net->Layer[0];
  Net->OutputLayer = Net->Layer[layers-1];
  Net->Eta = ETA; // by default
  Net->Momentum = NIU;
  Net->Activation = siACTIVATION;
  Net->MX    = 0;
  Net->Layers = layers;
  Net->Normalizer = 1;
  Net->Tau = 1;

}


float RandomWeight(NET *Net)    /* Kaip parinkt pradini svori. Butinai pasprest !!!!!!! */
{
 if (Net->Activation == siACTIVATION)
  return SIGMOID(tan(rand()%10));
// if (Net->Activation == thACTIVATION)
//  return TANH(tan(rand()%10));               //experimentinis
  return SIGMOID(tan(rand()%10));
}

void RandomWeights(NET *Net)
{
 for (int l = 1; l < Net->Layers; l++)
   for (int i = 1; i <= Net->Layer[l]->Units; i++)
     for (int j = 0; j<=Net->Layer[l-1]->Units;j++)
      {
        Net->Layer[l]->Weight[i][j] = RandomWeight(Net);
        Net->Layer[l]->SavedDelta[i][j] = 0;
      }
}

void Input(NET *Net,float *data)
{
 register int i;

 for (i = 1; i<=Net->InputLayer->Units; i++ )
    Net->InputLayer->Output[i] = (float) data[i-1] / Net->Normalizer;
}

/*
 If target extension is used you must multiply
 each output by Net->Tau
*/
void Output(NET *Net,float *output)
{
  for (int i = 1; i<= Net->OutputLayer->Units; i++)
   output[i-1] = Net->OutputLayer->Output[i];
}

/*------------------------------------------------------
           Sklidimo (propagating ) f - jos
------------------------------------------------------*/

void PropagateLayer(NET* Net, LAYER* Lower, LAYER* Upper)
{
  int  i,j;
  float SUM;

   for (i = 1; i <= Upper->Units; i++)
   {
     SUM = 0;
       for ( j = 0; j <= Lower->Units; j++)
        SUM+=Upper->Weight[i][j]*Lower->Output[j];
     if (Net->Activation == siACTIVATION)
      {
       if (SUM<-45) Upper->Output[i] = 0;
        else if (SUM>45) Upper->Output[i] = 1;
        else Upper->Output[i] = SIGMOID(SUM);
      }
     else
     if (Net->Activation == thACTIVATION)
      {
       if (SUM<-45) Upper->Output[i] = -1;
        else if (SUM>45) Upper->Output[i] = 1;
        else Upper->Output[i] = TANH(SUM);
      }
  }
}

void PropagateNetwork(NET *Net)
{
  for (int i = 0;i <Net->Layers-1; i++)
   PropagateLayer(Net,Net->Layer[i],Net->Layer[i+1]);
}

/*-----------------------------------------------------
           Klaidu radimas BP algorimtui
------------------------------------------------------ */

void ComputeOutputError(NET* Net, float* Target)
{
  int  i;
  float Out, Err;

  Net->Error = 0;
  for (i=1; i<=Net->OutputLayer->Units; i++)
  {
    Out = Net->OutputLayer->Output[i];
    Err = Target[i-1]-Out;  
    if (Net->Activation == siACTIVATION) Net->OutputLayer->Error[i] = Out*(1-Out) * Err;
    else if (Net->Activation == thACTIVATION) Net->OutputLayer->Error[i] = (1-Out*Out) * Err;
    Net->Error += 0.5*Err*Err;                             // MSE
  }
}

int isLinearSeparable(NET* Net, float *input, float* Target)
{
  int  i;
  float Out;
    
    Input(Net,input);
    PropagateNetwork(Net);
    
    for (i=1; i<=Net->OutputLayer->Units; i++)
      {
              Out = Net->OutputLayer->Output[i]; 
//              printf("output %f",Out);
//              printf("target %f \n",Target[i-1]);
              if (fabs(Target[i-1]-Out) >= 0.5) return 0;
      }  
  return 1;  
}


/*---------------------------------------------------------
            Atgalinio sklidimos f-jos: surandam zemesniu sluoksniu
     klaidas kai turim aukstesniu klaidas.
*----------------------------------------------------------*/

void BackpropagateLayer(NET *Net,LAYER* Upper, LAYER* Lower)
{
  int   i,j;
  float Out, Err;

  for (i=1; i<=Lower->Units; i++)
  {
    Out = Lower->Output[i];
    Err = 0;
    for (j=1; j<=Upper->Units; j++)
      Err += Upper->Weight[j][i] * Upper->Error[j];
   if (Net->Activation == siACTIVATION) Lower->Error[i] =  Out * (1-Out) * Err;
   else  if (Net->Activation == thACTIVATION) Lower->Error[i] =  (1-Out*Out) * Err;
  }
}

void BackpropagateNet(NET* Net)
{
  for (int l=Net->Layers-1; l>1; l--)     // BACK propagation
    BackpropagateLayer(Net,Net->Layer[l], Net->Layer[l-1]);
}

void SaveWeights(NET *Net)
{
 int l,n,j;

  for (l = 1; l<Net->Layers; l++)
   for (n = 1; n<=Net->Layer[l]->Units; n++)
    for (j = 0; j<=Net->Layer[l-1]->Units; j++)
     Net->Layer[l]->SavedWeight[n][j] = Net->Layer[l]->Weight[n][j];
}

void RestoreWeights(NET *Net)
{
 int l,n,j;

  for (l = 1; l<Net->Layers; l++)
   for (n = 1; n<=Net->Layer[l]->Units; n++)
    for (j = 0; j<=Net->Layer[l-1]->Units; j++)
     Net->Layer[l]->Weight[n][j] = Net->Layer[l]->SavedWeight[n][j];
}
/*----------------------------------------------------
    Sinapsiniu svoriu pakeitimas
------------------------------------------------------*/
void UpdateWeights(NET *Net)
{
  register int   l,i,j;
  float Out, Err,Delta;

  for (l=1; l<Net->Layers; l++)
    for (i=1; i<=Net->Layer[l]->Units; i++)
      for (j=0; j<=Net->Layer[l-1]->Units; j++)
       {
        
//        float little_rand = (float)1/(10000*log(rand()+2));
//	printf("l = %f \n",little_rand);
//        little_rand = 0;  
	
        Out = Net->Layer[l-1]->Output[j];
        Err = Net->Layer[l]->Error[i];
	
         Delta = Net->Momentum*Net->Layer[l]->SavedDelta[i][j] + (1-Net->Momentum)*Net->Eta *  Err * Out;   
         Net->Layer[l]->Weight[i][j] += Delta;
         Net->Layer[l]->SavedDelta[i][j] = Delta;
       }
}

/*----------------------------------------------------------
    Ivairios f-jos
*----------------------------------------------------------*/

void BpIteration(NET* Net, float* InputData, float* Target)
{
  Input(Net, InputData);
  PropagateNetwork(Net);
  ComputeOutputError(Net, Target);
  BackpropagateNet(Net);
  UpdateWeights(Net);
}

int
BpTrain(NET *Net,float *datas[],float *targs[],
            int patterns,int maxiter,float epsilon)
{
register int i,j, l;
 int Trained;
 float SUPErr,           //Klaidu supremumas
       MErr,             //Klaidu vidurkis
       NetErr=0,         //Paskutines iteracijos klaida (Net->Eta keitimui)
       max;
 char str[64];
 int  percent = 0,
      neednorm = 0,
      lin_sep = 1;

 float *mse_data_array = (float *)calloc(maxiter / 100 , sizeof(float));
 int    mse_data_index = 0;
 
 /*
    Firstly we determine if normalization is needed.
    Of course if we need to normalize data we then
    do this thing.
 */
  SaveWeights(Net);
 for (i = 0; i<patterns; i++)
  if (NeedNormalize(datas[i],Net->InputLayer->Units)) { neednorm = 1; break; }
 if (neednorm)
 {
  printf("Input data normalization needed\n");

  max = fabs(datas[0][0]);
  for (i = 0; i<patterns; i++)
   for (j = 0; j<Net->InputLayer->Units; j++)
    if (datas[i][j] > max) max = datas[i][j];
   Net->Normalizer = max;
 }

   printf("Net->Normalizer = %f\n",Net->Normalizer);

 //NormalizeMatrix(Net,datas,patterns);
/*
    Second step - compress targets to (0,1).Of course,
    if you take data from output layer you have to multiply
    all values by Net->Tau
*/
 neednorm = 0;
  for (i = 0; i< patterns; i++)
   if (NeedNormalize(targs[i],Net->OutputLayer->Units)) { neednorm = 1; break; }
  if (neednorm)
   {
     printf("Output data compresion needed\n");
     max = targs[0][0];
     for (i = 0; i< patterns; i++)
      for (j = 0; j<Net->OutputLayer->Units; j++)
       if (targs[i][j] > max ) max = targs[i][j];
        Net->Tau = max;
         for (i = 0; i< patterns; i++)
          for (j = 0; j<Net->OutputLayer->Units; j++)
           targs[i][j]/=Net->Tau;
   }
      printf("Net->Tau = %f\n",Net->Tau);

/*
    There we will train our network.
*/
 for (i = 0; i<maxiter; i++)
  {
  Trained = 1;
  MErr = SUPErr = 0;
  for (j = 0; j < patterns ; j++)
  {
    BpIteration(Net,datas[j],targs[j]);
    Input(Net,datas[j]);
    PropagateNetwork(Net);
    if (stop_training == 1) //called SIGINT
    {
     stop_training = 0;
     return 0;
    }
    MErr+=Net->Error;                          //MSE vidurkis
  }
    MErr/=patterns;

  //Ar isejo moxla ?
   if (MErr > epsilon) Trained = 0;

/*
 Optimizaciju skyrelis.Panaudotos ivairios idejos.Pagrindiniai resursai -
 prof. S.Raudzio konspektai, ir SSA saitas kazkokiu daktaru subadytas,kurio
 adreso dabar neprisimenu (is vgtu lygtaijs).
*/

// Kas iteraciju procenta ziurim ar klaidos vidurkis dideja ar mazeja ir
// keictiam Net->Eta (mokymo greiti).Jeigu dideja,tai POP'inam
// senus svorius,su kuriais klaida buvo mazesne.Jeigu mazeja - didinam.

  if (maxiter < 100  || i%(maxiter/100) == 0) // jei maxiter < 100 be sito bus negerove
  {
   if (MErr > NetErr)
   {
    Net->Eta*=ETA_MUL_SUX;			// imest dinamines pseudokonstantas :)
    RestoreWeights(Net);
   }
   else
    if (MErr <=  NetErr)
    {
      Net->Eta*=ETA_MUL_OK;
      SaveWeights(Net);
    }
   NetErr = MErr;
  }

  /*
    Output some data to stdout
  */
  fflush(stdout);
  if (maxiter < 100  || i%(maxiter/100) == 0)     //Kas  %
   {
//    system("clear");
    percent++;

    printf("%d%% of maxiter \n",percent);
    printf("Net->Eta = %f\n",Net->Eta);
    printf("M{Err} = %f\n",MErr);


    mse_data_array[mse_data_index] = MErr;
    mse_data_index++;

    memset(str,0,sizeof(str));
    sprintf(str,"MSE[%d] = %f \n",i,MErr);
    WriteToLog(str);
    
    printf("Needed error = %f\n",epsilon);
   }
  if (Trained)
    {
     if (Net->Layers == 2)
     {
     lin_sep = 1;
     for (l = 0; l < patterns; l++)
       {        
          lin_sep *= isLinearSeparable(Net,datas[l],targs[l]);
       } 
     printf("Linear separability = %d \n", lin_sep);       
    } 
     printf("Sucessefully exiting on interation %d\n",i);
    if (Net->graph)
      MSE_graph(mse_data_array,mse_data_index);
     return Trained;
    }
 }

   MSE_graph(mse_data_array,mse_data_index);
   return Trained;
}

int NetWeights(NET *Net)
{
 int l;
 int W = 0;
  for (l = 1; l<Net->Layers; l++)
  W+=Net->Layer[l]->Units*(Net->Layer[l-1]->Units + 1);

 return W;
}

/*-------------------------------------------------------------------
            Genetiniu algoritmu f-jos NN
---------------------------------------------------------------------*/
void SetupPopulation(POPULATION *P,NET *Net,int p_size)
{
 int i,l,n,s;

 P->member = (MEMB**)calloc(p_size,sizeof(MEMB*));
 P->size   = p_size;
 P->mutation = 0.05; //kol kas

  for (i = 0; i<p_size; i++)
  {
    P->member[i] = (MEMB*) malloc (sizeof(MEMB));
    P->member[i]->weight = (float***) calloc (NetWeights(Net),sizeof(float**));
     for (l = 1; l<Net->Layers; l++)
      {
        s = Net->Layer[l]->Units * (Net->Layer[l-1]->Units + 1 );
        P->member[i]->weight[l] = (float **) calloc(s,sizeof(float *));
         for (n = 0; n<Net->Layers; n++)
          P->member[i]->weight[l][n] = (float *)calloc(Net->Layer[l-1]->Units+1,sizeof(float));
      }
    P->member[i]->fitnes = 0;
    P->member[i]->prob = 0;
  }
}

void CreateInitPopulation(POPULATION *P,NET *Net)
{
 int i,j,l,k;

 for (k = 0; k<P->size; k++)
  for (l = 1;l < Net->Layers; l++)
   for (j = 1;j <= Net->Layer[l]->Units; j++)
    for (i = 0;i <= Net->Layer[l-1]->Units; i++)
     P->member[k]->weight[l][j][i] = RandomWeight(Net);
}

void FindFitness(POPULATION *P,NET *Net,float datas[][4],float targs[][4],int patterns)
{
 int l,n,i,m,k,j;
 float Err,max_Err;

for (m = 0; m<P->size; m++)
 {
 for (l = 1; l<Net->Layers; l++)
  for (n = 1; n<=Net->Layer[l]->Units; n++)
   for (i = 0; i<=Net->Layer[l-1]->Units; i++)
    Net->Layer[l]->Weight[n][i] = P->member[m]->weight[l][n][i];
    max_Err = 0;
    for (k = 0;k<patterns;k++)
    {
     Input(Net,datas[k]);
     PropagateNetwork(Net);
      for (j = 1; j<=Net->OutputLayer->Units; j++)
      {
        Err = fabs(Net->OutputLayer->Output[j] - targs[k][j-1]);
        if (Err > max_Err ) max_Err = Err;
      }
    }
   P->member[m]->fitnes = max_Err;
  }
}

void FindProbs(POPULATION *P,NET *Net)
{
// int i;
}

void f1()
{
 NET Net;
 POPULATION P;
 int nrns[]={2,2,1};
 float dta[][4]={{0,0},{0,1},{1,0},{1,1}};
 float trg[][4]={{0},{1},{1},{0}};
  CreateBPNet(&Net,nrns,3);
  RandomWeights(&Net);
  SetupPopulation(&P,&Net,10);
  CreateInitPopulation(&P,&Net);
  FindFitness(&P,&Net,dta,trg,4);
  PrintFitnes(&P);
  FindProbs(&P,&Net);
}

/*------------------------------------------------------------------*
                        Pagalbines  f - jos
--------------------------------------------------------------------*/
// BP
void PrintWeights(NET *Net)
{
  for (int l = 1; l<Net->Layers; l++)
    for (int n = 1; n<=Net->Layer[l]->Units; n++)
     for (int i = 0; i<=Net->Layer[l-1]->Units; i++)
       printf("Layer = %d Weight(%d,%d) = %f\n",l,n,i,Net->Layer[l]->Weight[n][i]);
}

void PrintOutput(NET *Net)
{
 for (int i = 1; i<=Net->OutputLayer->Units; i++)
  printf("%f\n",Net->OutputLayer->Output[i] * Net->Tau);
}

void PrintOError(NET *Net)
{
 for (int i = 1; i<= Net->OutputLayer->Units; i++)
   printf("%f",Net->OutputLayer->Error[i]);
}

//GA
void PrintMemberWeights(MEMB *M,NET *Net)
{
 int l,n,i;

 for (l = 1; l<Net->Layers; l++)
  for (n = 1; n<=Net->Layer[l]->Units; n++)
   for (i = 0; i<=Net->Layer[l-1]->Units; i++)
    printf("w(%d,%d,%d) = %f\n",l,n,i,M->weight[l][n][i]);
}

void PrintFitnes(POPULATION *P)
{
 int i;

  for (i = 0; i<P->size; i++)
   printf("F(%d) = %f\n",i,P->member[i]->fitnes);
}
      
int
NeedNormalize(float *data,int length)
{
 int i;
  for (i = 0;i < length; i++)
   if (fabs(data[i]) > MAXDATA) return 1;


 return 0;
}

void NormalizeVector(NET *Net,float *data)
{
  register int i;
  float S,MX,DX;

  S = 0;
   for (i = 0; i<Net->InputLayer->Units; i++)
      S+=data[i];
    MX = (float)S/Net->InputLayer->Units;
     S = 0;
      for (i = 0; i<Net->InputLayer->Units; i++)
       S+= (data[i] - MX) * (data[i] - MX);
    DX = (float)S/Net->InputLayer->Units;
 //    Net->Normalizer = (float)S/(Net->InputLayer->Units * patterns);

     for (i = 0;i < Net->InputLayer->Units; i++)
      data[i] = (float) (data[i] - MX) / sqrt(DX);
}

void
 NormalizeMatrix(NET *Net,float *data[],int patterns)
  {
   register int i;
   for (i = 0; i<patterns; i++)
    NormalizeVector(Net,data[i]);
  }

