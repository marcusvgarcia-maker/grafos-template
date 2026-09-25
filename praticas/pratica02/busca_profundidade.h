#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "busca_largura.h" // Herda as estruturas do GrafoLista definidas aqui

// Pilha (LIFO) para DFS iterativa
typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;

void dfs_recursiva(GrafoLista *g, int u, int *visitado);
int contar_componentes(GrafoLista *g);
int tem_ciclo(GrafoLista *g);

#endif