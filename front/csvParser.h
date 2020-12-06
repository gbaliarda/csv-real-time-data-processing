#ifndef csvParser_h
#define csvParser_h
#include "./../arbolesADT/arbolesADT.h"

// Lee los archivos CSV
void leerCSV(arbolesADT arboles, char *barriosPATH, char *arbolesPATH, int cantColumnasBarriosCSV, int cantColumnasArbolesCSV, int columnaComuna, int columnaNombre);

#endif