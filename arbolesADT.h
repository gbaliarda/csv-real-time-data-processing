#ifndef arbolesADT_h
#define arbolesADT_h
#include "stdlib.h"
typedef struct arbolesCDT *arbolesADT;

typedef struct tVectorQuery1
{
  char *barrio;
  float cantArbolesPorHab;
} tVectorQuery1;

// Crea un nuevo TAD
arbolesADT newArboles();

void addBarrio(arbolesADT arboles, char *nombreBarrio, int habitantes);

void addArbol(arbolesADT arboles, char *nombreBarrio, char *nombreCientifico);

void saveData(arbolesADT arboles);
// Devuelve la cantidad de comunas que hay en la ciudad.
unsigned int cantBarrios(arbolesADT arbol);

// Devuelve la cantidad de especies de arboles distintas que hay.
unsigned int cantEspecies(arbolesADT arbol);

// QUERY 1
tVectorQuery1 *totalArbolesHabitante(arbolesADT arbol);

// NO VA
// tVectorQuery1 *totalArbolesBarrio(arbolesADT arbol);

// NO VA
// Retorna el promedio del diametro del arbol a la altura del pecho
// tVectorQuery3 *diametroPromedioArbol(arbolesADT arbol);

// Libera la memoria ocupada por el TAD
void freeArboles(arbolesADT arbol);

#endif