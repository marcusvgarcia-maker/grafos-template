#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

// Estruturas do Grafo (incorporadas para evitar dependências externas)
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

// Fila (FIFO) obrigatória para BFS
typedef struct {
    int *dados;
    int capacidade, inicio, fim, tamanho;
} Fila;

void bfs(GrafoLista *g, int origem, int *dist, int *pred);
int eh_bipartido(GrafoLista *g);

#endif