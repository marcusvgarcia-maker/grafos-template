#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

GrafoLista* criar_grafo_lista(int n);
void inserir_aresta_lista(GrafoLista* g, int u, int v);
void liberar_grafo_lista(GrafoLista* g);

void dfs_articulacoes(GrafoLista *g, int u, int *visitado, int *descoberta, int *low, int *pai, int *articulacao, int *tempo);
void encontrar_articulacoes(GrafoLista *g);
void detectar_pontes(GrafoLista *g);

#endif