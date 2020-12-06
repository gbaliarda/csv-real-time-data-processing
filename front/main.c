#include "./../arbolesADT/arbolesADT.h"
#include "csvParser.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#ifdef BUE
#define CANTIDAD_COLUMNAS_ARBOLESCSV 13
#define CANTIDAD_COLUMNAS_BARRIOCSV 2
#define COLUMNA_BARRIO 3
#define COLUMNA_NOMBRE_CIENTIFICO 8
#else
#define CANTIDAD_COLUMNAS_ARBOLESCSV 19
#define CANTIDAD_COLUMNAS_BARRIOCSV 2
#define COLUMNA_BARRIO 13
#define COLUMNA_NOMBRE_CIENTIFICO 7
#endif

int main(int argc, char *argv[])
{
  errno = 0;

  if (argc != 3)
  {
    // Argumento invalido
    errno = EINVAL;
    fprintf(stderr, "Error de argumentos: Numero de argumentos incorrecto\n");
    return errno;
  }

  // Crea el TAD
  arbolesADT arboles = nuevoArboles();

  if (errno != 0)
  {
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return errno;
  }

  // Lee los archivos CSV y carga los datos en arboles
  leerCSV(arboles, argv[1], argv[2], CANTIDAD_COLUMNAS_BARRIOCSV, CANTIDAD_COLUMNAS_ARBOLESCSV, COLUMNA_BARRIO, COLUMNA_NOMBRE_CIENTIFICO);

  if (errno != 0)
  {
    fprintf(stderr, "Error en la lectura de los archivos CSV.\n");
    free(arboles);
    return errno;
  }

  // QUERIES

  FILE *fPtr;

  // QUERY 1

  tVectorQuery1 *vector1 = totalArbolesHabitante(arboles);

  if (errno != 0)
  {
    fprintf(stderr, "Error en la Query 1: %s\n", strerror(errno));
    errno = 0;
  }
  else
  {
    fPtr = fopen("./queries/query1.csv", "w");

    if (fPtr == NULL)
    {
      errno = ENOENT;
      fprintf(stderr, "Error al crear query1.csv: %s\n", strerror(errno));
    }
    else
    {
      fprintf(fPtr, "BARRIO;ARBOLES_POR_HABITANTE\n");
      for (int i = 0; i < cantBarrios(arboles); i++)
        fprintf(fPtr, "%s;%.2f\n", vector1[i].barrio, vector1[i].cantArbolesPorHab);
      printf("Archivo query1.csv creado con exito.\n");
      fclose(fPtr);
    }
  }

  // END QUERY 1

  errno = 0;

  // QUERY 2

  tVectorQuery2 *vector2 = especieMasPopularPorBarrio(arboles);

  if (errno != 0)
  {
    fprintf(stderr, "Error en la Query 2: %s\n", strerror(errno));
    errno = 0;
  }
  else
  {
    fPtr = fopen("./queries/query2.csv", "w");

    if (fPtr == NULL)
    {
      errno = ENOENT;
      fprintf(stderr, "Error al crear query2.csv %s\n", strerror(errno));
    }
    else
    {
      fprintf(fPtr, "BARRIO;NOMBRE_CIENTIFICO\n");
      for (int i = 0; i < cantBarrios(arboles); i++)
        fprintf(fPtr, "%s;%s\n", vector2[i].barrio, vector2[i].nombreCientifico);
      printf("Archivo query2.csv creado con exito.\n");
      fclose(fPtr);
    }
  }

  // END QUERY 2

  free(vector1);
  free(vector2);
  freeArboles(arboles);
  return 0;
}
