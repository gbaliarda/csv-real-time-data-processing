#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "arbolesADT.h"
#define MAX_LENGTH 240
#define CANTIDAD_COLUMNAS_ARBOLESCSV 19
#define COLUMNA_BARRIO 13
#define COLUMNA_NOMBRE_CIENTIFICO 7
#define BLOCK 15

typedef struct arbol
{
  char *nombreCientifico;  // String que almacena el nombre cientifico del arbol.
  unsigned int cantTotal;  // La cantidad de arboles de la misma especie.
  struct arbol *nextArbol; // Puntero que apunta al siguiente arbol en la lista.
} arbol;

typedef arbol *TArbol;

typedef struct barrio
{
  char *barrio;                    // Nombre del barrio.
  unsigned int habitantes;         // Cantidad de habitantes del barrio.
  unsigned int cantArboles;        // Cantidad de arboles del barrio.
  struct barrio *nextBarrioPorHab; // Puntero que apunta al siguiente barrio en el orden de la Query 1.
  struct barrio *nextBarrioAlf;    // Puntero que apunta al siguiente barrio en el orden de la Query 2.
  TArbol firstArbol;               // Puntero que apunta al primer elemento de la lista de los arboles de la ciudad.
} barrio;

typedef barrio *TBarrio;

typedef struct arbolesCDT
{
  TBarrio firstBarrioPorHab; // Puntero que apunta al primer barrio en el orden de la Query 1.
  TBarrio firstBarrioAlf;    // Puntero que apunta al primer barrio en el orden de la Query 2.
  barrio *vectorBarrios;     // Vector auxiliar
  unsigned int cantBarrios;  // Cantidad de barrios que hay.
} arbolesCDT;

void addBarrio(arbolesADT arboles, char *nombreBarrio, int habitantes)
{
  if (arboles->cantBarrios % BLOCK == 0)
  {
    arboles->vectorBarrios = realloc(arboles->vectorBarrios, sizeof(barrio) * (arboles->cantBarrios + BLOCK));
    if (arboles->vectorBarrios == NULL)
    {
      errno = ENOMEM;
      return;
    }
  }

  arboles->vectorBarrios[arboles->cantBarrios].barrio = nombreBarrio;
  arboles->vectorBarrios[arboles->cantBarrios].habitantes = habitantes;
  arboles->vectorBarrios[arboles->cantBarrios].cantArboles = 0;
  arboles->vectorBarrios[arboles->cantBarrios].nextBarrioPorHab = NULL;
  arboles->vectorBarrios[arboles->cantBarrios].nextBarrioAlf = NULL;
  arboles->vectorBarrios[arboles->cantBarrios].firstArbol = NULL;

  arboles->cantBarrios += 1;
}

static TArbol addArbolesRecAlf(TArbol listaArboles, char *nombreCientifico)
{
  int c;

  // Si el arbol a agregar no tiene ningun ejemplar ya agregado o la lista esta vacia, lo agrego.
  if (listaArboles == NULL || (c = strcmp(listaArboles->nombreCientifico, nombreCientifico)) > 0)
  {
    TArbol newArbol = malloc(sizeof(arbol));

    if (newArbol == NULL)
    {
      errno = ENOMEM;
      return listaArboles;
    }

    newArbol->nombreCientifico = nombreCientifico;
    newArbol->cantTotal = 1;
    newArbol->nextArbol = listaArboles;
    return newArbol;
  }

  if (c == 0)
  {
    listaArboles->cantTotal += 1;
    // Si encuentro un barrio y un arbol con el mismo nombre, libero la memoria reservada al no volver a usarlos.
    free(nombreCientifico);
  }
  else
    // Si c < 0 sigo recorriendo la lista.
    listaArboles->nextArbol = addArbolesRecAlf(listaArboles->nextArbol, nombreCientifico);

  return listaArboles;
}

void addArbol(arbolesADT arboles, char *nombreBarrio, char *nombreCientifico)
{
  int flag = 0;
  for (int i = 0; i < arboles->cantBarrios && flag == 0; i++)
    if (strcmp(arboles->vectorBarrios[i].barrio, nombreBarrio) == 0)
    {
      flag = 1;
      arboles->vectorBarrios[i].cantArboles += 1;
      // Creo el nodo y lo agrego a la lista de arboles
      arboles->vectorBarrios[i].firstArbol = addArbolesRecAlf(arboles->vectorBarrios[i].firstArbol, nombreCientifico);
      free(nombreBarrio);
    }
}

static TBarrio ordenarBarrioPorHab(TBarrio firstBarrioPorHab, TBarrio barrio)
{
  int c;
  if (firstBarrioPorHab == NULL || (c = (int)(100 * (firstBarrioPorHab->cantArboles / (float)firstBarrioPorHab->habitantes)) - (int)(100 * (barrio->cantArboles / (float)barrio->habitantes))) < 0 || (c == 0 && strcmp(barrio->barrio, firstBarrioPorHab->barrio) < 0))
  {
    barrio->nextBarrioPorHab = firstBarrioPorHab;
    return barrio;
  }

  firstBarrioPorHab->nextBarrioPorHab = ordenarBarrioPorHab(firstBarrioPorHab->nextBarrioPorHab, barrio);
  return firstBarrioPorHab;
}

static TBarrio ordenarBarrioAlf(TBarrio firstBarrioAlf, TBarrio barrio)
{
  if (firstBarrioAlf == NULL || strcmp(barrio->barrio, firstBarrioAlf->barrio) < 0)
  {
    barrio->nextBarrioAlf = firstBarrioAlf;
    return barrio;
  }

  firstBarrioAlf->nextBarrioAlf = ordenarBarrioAlf(firstBarrioAlf->nextBarrioAlf, barrio);
  return firstBarrioAlf;
}

void saveData(arbolesADT arboles)
{
  for (int i = 0; i < arboles->cantBarrios; i++)
  {
    TBarrio newBarrio = malloc(sizeof(barrio));
    if (newBarrio == NULL)
    {
      errno = ENOMEM;
      // Si falla al reservar memoria, libero la memoria que ocupaba el vector.
      free(arboles->vectorBarrios);
      return;
    }
    newBarrio->barrio = arboles->vectorBarrios[i].barrio;
    newBarrio->habitantes = arboles->vectorBarrios[i].habitantes;
    newBarrio->cantArboles = arboles->vectorBarrios[i].cantArboles;
    newBarrio->nextBarrioPorHab = NULL;
    newBarrio->nextBarrioAlf = NULL;
    newBarrio->firstArbol = arboles->vectorBarrios[i].firstArbol;

    // Ordeno la tail que apunta al siguiente en orden descendente del cociente entre la cantidad de arboles y los habitantes por comuna, con el cociente truncado a 2 decimales y tambien luego ordenado alfabeticamente.
    arboles->firstBarrioPorHab = ordenarBarrioPorHab(arboles->firstBarrioPorHab, newBarrio);

    arboles->firstBarrioAlf = ordenarBarrioAlf(arboles->firstBarrioAlf, newBarrio);
  }

  free(arboles->vectorBarrios);
}

arbolesADT newArboles()
{
  arbolesADT newArbol = calloc(1, sizeof(arbolesCDT));
  if (newArbol == NULL)
  {
    errno = ENOMEM;
    return NULL;
  }
  return newArbol;
}

unsigned int cantBarrios(arbolesADT arbol)
{
  return arbol->cantBarrios;
}

tVectorQuery1 *totalArbolesHabitante(arbolesADT arboles)
{
  // Armo un vector de una estructura que guarde el nombre del barrio y el cociente entre la cantidad de arboles y los habitantes de esta comuna.
  tVectorQuery1 *arbolesPorHabitante = malloc(sizeof(tVectorQuery1) * arboles->cantBarrios);
  if (arbolesPorHabitante == NULL)
  {
    errno = ENOMEM;
    return NULL;
  }
  int i = 0;
  TBarrio aux = arboles->firstBarrioPorHab;
  while (aux != NULL)
  {
    arbolesPorHabitante[i].barrio = aux->barrio;
    arbolesPorHabitante[i++].cantArbolesPorHab = (unsigned int)(100 * (aux->cantArboles / (float)aux->habitantes)) / 100.0;
    aux = aux->nextBarrioPorHab;
  }
  return arbolesPorHabitante;
}

static char *especieMasPopular(TArbol firstArbol)
{
  TArbol arbolMasPopular = firstArbol;

  TArbol aux = firstArbol->nextArbol;
  while (aux != NULL)
  {
    if (arbolMasPopular->cantTotal < aux->cantTotal)
      arbolMasPopular = aux;

    aux = aux->nextArbol;
  }

  return arbolMasPopular->nombreCientifico;
}

tVectorQuery2 *especieMasPopularPorBarrio(arbolesADT arboles)
{
  tVectorQuery2 *vectorQuery2 = malloc(sizeof(tVectorQuery2) * arboles->cantBarrios);
  if (vectorQuery2 == NULL)
  {
    errno = ENOMEM;
    return NULL;
  }
  int i = 0;
  TBarrio aux = arboles->firstBarrioAlf;
  while (aux != NULL)
  {
    vectorQuery2[i].barrio = aux->barrio;
    vectorQuery2[i++].nombreCientifico = aux->firstArbol != NULL ? especieMasPopular(aux->firstArbol) : "No existen arboles";
    aux = aux->nextBarrioAlf;
  }
  return vectorQuery2;
}

static void freeArbolesRec(TArbol firstArbol)
{
  if (firstArbol == NULL)
    return;

  freeArbolesRec(firstArbol->nextArbol);

  free(firstArbol->nombreCientifico);

  free(firstArbol);
}

static void freeBarriosRec(TBarrio firstBarrio)
{
  if (firstBarrio == NULL)
    return;

  freeBarriosRec(firstBarrio->nextBarrioPorHab);

  free(firstBarrio->barrio);

  freeArbolesRec(firstBarrio->firstArbol);

  free(firstBarrio);
}

void freeArboles(arbolesADT arboles)
{
  // Liberar lista barrios
  freeBarriosRec(arboles->firstBarrioPorHab);

  free(arboles);
}