#include "csvParser.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#define MAX_LENGTH 240
#define BLOCK 15

static void readBarrios(arbolesADT arboles, char *barriosCSV)
{
    FILE *file = fopen(barriosCSV, "r");
    if (!file)
    {
        errno = ENOENT;
        return;
    }
    char c[MAX_LENGTH];
    // Descartamos la primera fila que contiene los nombres de cada columna
    fgets(c, MAX_LENGTH, file);

    char *s, *nombreBarrio;

    // Recorremos cada fila del archivo y para cada fila, tomamos la informacion de cada columna que nos sirve.
    while (fgets(c, MAX_LENGTH, file) != NULL)
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

        addBarrio(arboles, nombreBarrio, habitantes);
    }
}

static void readArboles(arbolesADT arboles, char *arbolesPATH, int cantColumnas, int columnaComuna, int columnaNombre)
{
    FILE *fileArboles = fopen(arbolesPATH, "r");
    if (!fileArboles)
    {
        errno = ENOENT;
        return;
    }

    char c[MAX_LENGTH];

    // Descartamos la primer lista ya que contiene los nombres de cada columna.
    fgets(c, MAX_LENGTH, fileArboles);

    // Extraemos los datos que son utiles para las querys para cada ejemplar de arbol
    while (fgets(c, MAX_LENGTH, fileArboles) != NULL)
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

        addArbol(arboles, nombreBarrio, nombreCientifico);
    }
}

void readCSV(arbolesADT arboles, char *barriosPATH, char *arbolesPATH, int cantColumnas, int columnaComuna, int columnaNombre)
{
    readBarrios(arboles, barriosPATH);

    readArboles(arboles, arbolesPATH, cantColumnas, columnaComuna, columnaNombre);

    saveData(arboles);
}