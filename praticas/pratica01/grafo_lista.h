#ifndef GRAFO_LISTA_H
#define GRAFO_LISTA_H

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
void remover_aresta_lista(GrafoLista* g, int u, int v);
int grau_lista(GrafoLista* g, int u);
int sao_adjacentes_lista(GrafoLista* g, int u, int v);
void liberar_grafo_lista(GrafoLista* g);

#endif