/*
This file contains Program configuration
Author : Povilas Daniushis,paralax@freemail.lt"
*/

/*
 Coment this line if you dont have UNIX
 Warning: This program is written for UNIX.
 In Windows you can't use some important features,
 like command line interface.
*/
// #define UNIX

/* Version 
 If you are not project coordinator you ar NOT permited to
 change program version string
*/

#define VERSION "v 1.42e"
/*
   Method of normalizing
*/
#define SIMPLE_NORMALIZING
//#define MINUS_MX_NORMALIZING

#define LOG_LEVEL     1
#define DEFAULT_LOG "./log.txt"

#define MINLAYERS 2
#define MAXLAYERS 100
#define MINNEURONS 1
#define MAXNEURONS 2000
#define MAXNETS   128
