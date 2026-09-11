#ifndef DAG_H
#define DAG_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;       // Número de vértices
    No **adj;    // Lista de adjacência
} GrafoLista;

// Funções básicas de gerenciamento do grafo
GrafoLista* criar_grafo_lista(int n);
void inserir_aresta_direcionada(GrafoLista* g, int u, int v);
void liberar_grafo_lista(GrafoLista* g);

// Funções da Prática 03
int* ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
int* ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);
int eh_dag(GrafoLista *g);

#endif