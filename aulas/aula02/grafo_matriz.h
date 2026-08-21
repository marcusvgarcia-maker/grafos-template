#ifndef GRAFO_MATRIZ_H
#define GRAFO_MATRIZ_H

#define LIMITE 10

typedef struct {
    int adjacencia[LIMITE][LIMITE];
    int num_vertices;
} GrafoMatriz;

void inicializar(GrafoMatriz *grafo, int numero);
void inserir_aresta(GrafoMatriz *grafo, int u, int v);
void inserir_arco(GrafoMatriz *grafo, int u, int v);
void exibir_matriz(GrafoMatriz *grafo);

#endif