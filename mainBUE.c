#include "arbolesADT.h"
#include "csvParser.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"
#define CANTIDAD_COLUMNAS_ARBOLESCSV 13
#define COLUMNA_COMUNA 3
#define COLUMNA_NOMBRE_CIENTIFICO 8

int main(int argc, char *argv[])
{
  errno = 0;

  if (argc != 3)
  {
    // Argumento invalido
    errno = EINVAL;
    fprintf(stderr, "Argument Error: %s\n", strerror(errno));
    return 0;
  }

  arbolesADT arbolesBUE = newArboles();

  if (errno != 0)
  {
    // strerror retorna un string de error dependiendo del numero de error que se le pase
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return 0;
  }

  readCSV(arbolesBUE, argv[1], argv[2], CANTIDAD_COLUMNAS_ARBOLESCSV, COLUMNA_COMUNA, COLUMNA_NOMBRE_CIENTIFICO);

  // QUERIES

  FILE *fPtr;

  // QUERY 1

  tVectorQuery1 *vector1 = totalArbolesHabitante(arbolesBUE);

  if (errno != 0)
  {
    fprintf(stderr, "Error en la Query 1: %s\n", strerror(errno));
    errno = 0;
  }
  else
  {
    fPtr = fopen("./dataset/query1.csv", "w");

    if (fPtr == NULL)
    {
      errno = ENOENT;
      fprintf(stderr, "File Error al crear query1.csv: %s\n", strerror(errno));
    }
    else
    {
      fprintf(fPtr, "BARRIO;ARBOLES_POR_HABITANTE\n");
      for (int i = 0; i < cantBarrios(arbolesBUE); i++)
        fprintf(fPtr, "%s;%.2f\n", vector1[i].barrio, vector1[i].cantArbolesPorHab);
      printf("Archivo query1.csv creado con exito.\n");
      fclose(fPtr);
    }
  }

  // END QUERY 1
  free(vector1);
  freeArboles(arbolesBUE);
  return 0;
}
