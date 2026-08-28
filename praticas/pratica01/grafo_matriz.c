#include <stdio.h>
#include <stdlib.h>
#include "grafo_matriz.h"

GrafoMatriz* criar_grafo_matriz(int n) {
    GrafoMatriz* g = (GrafoMatriz*) malloc(sizeof(GrafoMatriz));
    g->n = n;
    g->adj = (int**) malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = (int*) calloc(n, sizeof(int)); 
    }
    return g;
}

void inserir_aresta_matriz(GrafoMatriz* g, int u, int v) {
    if (u >= 0 && u < g->n && v >= 0 && v < g->n) {
        g->adj[u][v] = 1;
        g->adj[v][u] = 1; 
    }
}

void remover_aresta_matriz(GrafoMatriz* g, int u, int v) {
    if (u >= 0 && u < g->n && v >= 0 && v < g->n) {
        g->adj[u][v] = 0;
        g->adj[v][u] = 0;
    }
}

int grau_matriz(GrafoMatriz* g, int u) {
    int grau = 0;
    if (u >= 0 && u < g->n) {
        for (int i = 0; i < g->n; i++) {
            if (g->adj[u][i] == 1) {
                grau++;
            }
        }
    }
    return grau;
}

int sao_adjacentes_matriz(GrafoMatriz* g, int u, int v) {
    if (u >= 0 && u < g->n && v >= 0 && v < g->n) {
        return g->adj[u][v];
    }
    return 0;
}

void liberar_grafo_matriz(GrafoMatriz* g) {
    if (g) {
        for (int i = 0; i < g->n; i++) {
            free(g->adj[i]);
        }
        free(g->adj);
        free(g);
    }
}