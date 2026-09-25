#ifndef PLANARIDADE_H
#define PLANARIDADE_H

#include "conectividade.h"

int contar_arestas(GrafoLista *g);
int eh_planar_euler(GrafoLista *g);
int verifica_kuratowski(GrafoLista *g);

#endif