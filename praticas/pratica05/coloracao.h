#ifndef COLORACAO_H
#define COLORACAO_H

// Estruturas do Grafo autocontidas
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

// Gerenciamento do Grafo
GrafoLista* criar_grafo_lista(int n);
void inserir_aresta_lista(GrafoLista* g, int u, int v);
int grau_lista(GrafoLista* g, int u);
void liberar_grafo_lista(GrafoLista* g);

// Funções da Prática 05
int* coloracao_gulosa(GrafoLista *g, int *num_cores);
int* coloracao_welsh_powell(GrafoLista *g, int *num_cores);
int eh_bipartido(GrafoLista *g);

#endif