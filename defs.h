
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
  float              MX;
  float          Normalizer;
  float              Tau;
} NET;

