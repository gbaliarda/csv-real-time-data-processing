#include "csvParser.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#define LONGITUD_MAX 300
#define BLOQUE 15

static FILE *revisarArchivo(char *ruta, int cantColumnas)
{
    FILE *file = fopen(ruta, "r");

    // Si no existe el archivo o no logra abrirlo
    if (!file)
        return NULL;

    char c[LONGITUD_MAX];
    char *filaAux;

    filaAux = fgets(c, LONGITUD_MAX, file);

    // Si la primer fila es vacia
    if (filaAux == NULL)
        return NULL;
    // Si la cantidad de columnas es erronea
    else
    {
        int cantidad = 0;
        while (*filaAux != 0)
        {
            if (*filaAux == ';')
                cantidad++;
            filaAux++;
        }
        if (cantidad != cantColumnas - 1)
            return NULL;
    }

    return file;
}

static void leerBarrios(arbolesADT arboles, FILE *file)
{
    if (!file)
    {
        errno = ENOENT;
        return;
    }
    char c[LONGITUD_MAX];

    char *s, *nombreBarrio;

    // Recorremos cada fila del archivo y para cada fila, ignoramos la primer fila porque ya la utilizamos,tomamos la informacion de cada columna que nos sirve.
    while (fgets(c, LONGITUD_MAX, file) != NULL)
    {
        s = strtok(c, ";");
        nombreBarrio = malloc(sizeof(char) * strlen(s) + 1);
        if (nombreBarrio == NULL)
        {
            errno = ENOMEM;
            return;
        }
        strcpy(nombreBarrio, s);
        unsigned int habitantes = atol(strtok(NULL, ";"));

        agregarBarrio(arboles, nombreBarrio, habitantes);
    }
    fclose(file);
}

static void leerArboles(arbolesADT arboles, FILE *fileArboles, int cantColumnas, int columnaComuna, int columnaNombre)
{
    if (!fileArboles)
    {
        errno = ENOENT;
        return;
    }

    // Si la cantidad de barrios es 0 no agrega ningun arbol.
    if (cantBarrios(arboles) == 0)
        return;

    char c[LONGITUD_MAX];

    // Extraemos los datos que son utiles para las querys para cada ejemplar de arbol, la primer fila ya es utilizada previamente.
    while (fgets(c, LONGITUD_MAX, fileArboles) != NULL)
    {
        strtok(c, ";");
        char *nombreCientifico, *s, *nombreBarrio;

        for (int i = 2; i <= cantColumnas; i++)
        {
            s = strtok(NULL, ";");
            if (i == columnaComuna)
            {
                nombreBarrio = malloc(sizeof(char) * strlen(s) + 1);
                if (nombreBarrio == NULL)
                {
                    errno = ENOMEM;
                    return;
                }
                strcpy(nombreBarrio, s);
            }
            else if (i == columnaNombre)
            {
                nombreCientifico = malloc(sizeof(char) * (strlen(s) + 1));
                if (nombreCientifico == NULL)
                {
                    errno = ENOMEM;
                    return;
                }
                strcpy(nombreCientifico, s);
            }
        }

        agregarArbol(arboles, nombreBarrio, nombreCientifico);
    }
    fclose(fileArboles);
}

void leerCSV(arbolesADT arboles, char *barriosPATH, char *arbolesPATH, int cantColumnasBarriosCSV, int cantColumnasArbolesCSV, int columnaComuna, int columnaNombre)
{
    // Checkea el archivo de barrios
    FILE *barriosCSV = revisarArchivo(barriosPATH, cantColumnasBarriosCSV);

    // Checkea el archivo de arboles
    FILE *arbolesCSV = revisarArchivo(arbolesPATH, cantColumnasArbolesCSV);

    if (barriosCSV == NULL || arbolesCSV == NULL)
    {
        errno = EINVAL;
        return;
    }

    leerBarrios(arboles, barriosCSV);
    if (errno != 0)
        return;

    leerArboles(arboles, arbolesCSV, cantColumnasArbolesCSV, columnaComuna, columnaNombre);
    if (errno != 0)
        return;

    guardarData(arboles);
}