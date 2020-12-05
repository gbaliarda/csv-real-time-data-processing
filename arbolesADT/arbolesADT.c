#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "arbolesADT.h"
#define BLOCK 15

typedef struct arbol
{
  char *nombreCientifico;  // String que almacena el nombre cientifico del arbol.
  unsigned int cantTotal;  // La cantidad de arboles de la misma especie.
  struct arbol *proxArbol; // Puntero que apunta al siguiente arbol en la lista.
} arbol;

typedef arbol *TArbol;

typedef struct barrio
{
  char *barrio;                    // Nombre del barrio.
  unsigned int habitantes;         // Cantidad de habitantes del barrio.
  unsigned int cantArboles;        // Cantidad de arboles del barrio.
  struct barrio *proxBarrioPorHab; // Puntero que apunta al siguiente barrio en el orden de la Query 1.
  struct barrio *proxBarrioAlf;    // Puntero que apunta al siguiente barrio en el orden de la Query 2.
  TArbol primerArbol;              // Puntero que apunta al primer elemento de la lista de los arboles de la ciudad.
} barrio;

typedef barrio *TBarrio;

typedef struct arbolesCDT
{
  TBarrio primerBarrioPorHab; // Puntero que apunta al primer barrio en el orden de la Query 1.
  TBarrio primerBarrioAlf;    // Puntero que apunta al primer barrio en el orden de la Query 2.
  barrio *vectorBarrios;      // Vector auxiliar
  unsigned int cantBarrios;   // Cantidad de barrios que hay.
} arbolesCDT;

void agregarBarrio(arbolesADT arboles, char *nombreBarrio, int habitantes)
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
  arboles->vectorBarrios[arboles->cantBarrios].proxBarrioPorHab = NULL;
  arboles->vectorBarrios[arboles->cantBarrios].proxBarrioAlf = NULL;
  arboles->vectorBarrios[arboles->cantBarrios].primerArbol = NULL;

  arboles->cantBarrios += 1;
}

static TArbol agregarArbolesRecAlf(TArbol listaArboles, char *nombreCientifico)
{
  int c;

  // Si el arbol a agregar no tiene ningun ejemplar ya agregado o la lista esta vacia, lo agrego.
  if (listaArboles == NULL || (c = strcmp(listaArboles->nombreCientifico, nombreCientifico)) > 0)
  {
    TArbol nuevoArbol = malloc(sizeof(arbol));

    if (nuevoArbol == NULL)
    {
      errno = ENOMEM;
      return listaArboles;
    }

    nuevoArbol->nombreCientifico = nombreCientifico;
    nuevoArbol->cantTotal = 1;
    nuevoArbol->proxArbol = listaArboles;
    return nuevoArbol;
  }

  if (c == 0)
  {
    listaArboles->cantTotal += 1;
    // Si encuentro un barrio y un arbol con el mismo nombre, libero la memoria reservada al no volver a usarlos.
    free(nombreCientifico);
  }
  else
    // Si c < 0 sigo recorriendo la lista.
    listaArboles->proxArbol = agregarArbolesRecAlf(listaArboles->proxArbol, nombreCientifico);

  return listaArboles;
}

void agregarArbol(arbolesADT arboles, char *nombreBarrio, char *nombreCientifico)
{
  int flag = 0;
  for (int i = 0; i < arboles->cantBarrios && flag == 0; i++)
    if (strcmp(arboles->vectorBarrios[i].barrio, nombreBarrio) == 0)
    {
      flag = 1;
      arboles->vectorBarrios[i].cantArboles += 1;
      // Creo el nodo y lo agrego a la lista de arboles
      arboles->vectorBarrios[i].primerArbol = agregarArbolesRecAlf(arboles->vectorBarrios[i].primerArbol, nombreCientifico);
      free(nombreBarrio);
    }
}

static TBarrio ordenarBarrioPorHab(TBarrio primerBarrioPorHab, TBarrio barrio)
{
  int c;
  if (primerBarrioPorHab == NULL || (c = (int)(100 * (primerBarrioPorHab->cantArboles / (float)primerBarrioPorHab->habitantes)) - (int)(100 * (barrio->cantArboles / (float)barrio->habitantes))) < 0 || (c == 0 && strcmp(barrio->barrio, primerBarrioPorHab->barrio) < 0))
  {
    barrio->proxBarrioPorHab = primerBarrioPorHab;
    return barrio;
  }

  primerBarrioPorHab->proxBarrioPorHab = ordenarBarrioPorHab(primerBarrioPorHab->proxBarrioPorHab, barrio);
  return primerBarrioPorHab;
}

static TBarrio ordenarBarrioAlf(TBarrio primerBarrioAlf, TBarrio barrio)
{
  if (primerBarrioAlf == NULL || strcmp(barrio->barrio, primerBarrioAlf->barrio) < 0)
  {
    barrio->proxBarrioAlf = primerBarrioAlf;
    return barrio;
  }

  primerBarrioAlf->proxBarrioAlf = ordenarBarrioAlf(primerBarrioAlf->proxBarrioAlf, barrio);
  return primerBarrioAlf;
}

void guardarData(arbolesADT arboles)
{
  for (int i = 0; i < arboles->cantBarrios; i++)
  {
    TBarrio nuevoBarrio = malloc(sizeof(barrio));
    if (nuevoBarrio == NULL)
    {
      errno = ENOMEM;
      // Si falla al reservar memoria, libero la memoria que ocupaba el vector.
      free(arboles->vectorBarrios);
      return;
    }
    nuevoBarrio->barrio = arboles->vectorBarrios[i].barrio;
    nuevoBarrio->habitantes = arboles->vectorBarrios[i].habitantes;
    nuevoBarrio->cantArboles = arboles->vectorBarrios[i].cantArboles;
    nuevoBarrio->proxBarrioPorHab = NULL;
    nuevoBarrio->proxBarrioAlf = NULL;
    nuevoBarrio->primerArbol = arboles->vectorBarrios[i].primerArbol;

    // Ordeno la tail que apunta al siguiente en orden descendente del cociente entre la cantidad de arboles y los habitantes por comuna, con el cociente truncado a 2 decimales y tambien luego ordenado alfabeticamente.
    arboles->primerBarrioPorHab = ordenarBarrioPorHab(arboles->primerBarrioPorHab, nuevoBarrio);

    arboles->primerBarrioAlf = ordenarBarrioAlf(arboles->primerBarrioAlf, nuevoBarrio);
  }

  free(arboles->vectorBarrios);
}

arbolesADT nuevoArboles()
{
  arbolesADT nuevoArbol = calloc(1, sizeof(arbolesCDT));
  if (nuevoArbol == NULL)
  {
    errno = ENOMEM;
    return NULL;
  }
  return nuevoArbol;
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
  TBarrio aux = arboles->primerBarrioPorHab;
  while (aux != NULL)
  {
    arbolesPorHabitante[i].barrio = aux->barrio;
    arbolesPorHabitante[i++].cantArbolesPorHab = (unsigned int)(100 * (aux->cantArboles / (float)aux->habitantes)) / 100.0;
    aux = aux->proxBarrioPorHab;
  }
  return arbolesPorHabitante;
}

static char *especieMasPopular(TArbol primerArbol)
{
  TArbol arbolMasPopular = primerArbol;

  TArbol aux = primerArbol->proxArbol;
  while (aux != NULL)
  {
    if (arbolMasPopular->cantTotal < aux->cantTotal)
      arbolMasPopular = aux;

    aux = aux->proxArbol;
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
  TBarrio aux = arboles->primerBarrioAlf;
  while (aux != NULL)
  {
    vectorQuery2[i].barrio = aux->barrio;
    vectorQuery2[i++].nombreCientifico = aux->primerArbol != NULL ? especieMasPopular(aux->primerArbol) : "No existen arboles";
    aux = aux->proxBarrioAlf;
  }
  return vectorQuery2;
}

static void freeArbolesRec(TArbol primerArbol)
{
  if (primerArbol == NULL)
    return;

  freeArbolesRec(primerArbol->proxArbol);

  free(primerArbol->nombreCientifico);

  free(primerArbol);
}

static void freeBarriosRec(TBarrio firstBarrio)
{
  if (firstBarrio == NULL)
    return;

  freeBarriosRec(firstBarrio->proxBarrioPorHab);

  free(firstBarrio->barrio);

  freeArbolesRec(firstBarrio->primerArbol);

  free(firstBarrio);
}

void freeArboles(arbolesADT arboles)
{
  // Liberar lista barrios
  freeBarriosRec(arboles->primerBarrioPorHab);

  free(arboles);
}