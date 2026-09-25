#include <stdio.h>
#include <stdlib.h>
#include "busca_profundidade.h"

void dfs_recursiva(GrafoLista *g, int u, int *visitado) {
    visitado[u] = 1;
    No *atual = g->adj[u];
    
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            dfs_recursiva(g, v, visitado);
        }
        atual = atual->prox;
    }
}

int contar_componentes(GrafoLista *g) {
    int *visitado = (int*) calloc(g->n, sizeof(int));
    int componentes = 0;
    
    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            componentes++;
            dfs_recursiva(g, i, visitado);
        }
    }
    
    free(visitado);
    return componentes;
}

static int dfs_ciclo(GrafoLista *g, int u, int pai, int *visitado) {
    visitado[u] = 1;
    No *atual = g->adj[u];
    
    while (atual != NULL) {
        int v = atual->destino;
        if (!visitado[v]) {
            if (dfs_ciclo(g, v, u, visitado)) return 1;
        } else if (v != pai) {
            return 1; 
        }
        atual = atual->prox;
    }
    return 0;
}

int tem_ciclo(GrafoLista *g) {
    int *visitado = (int*) calloc(g->n, sizeof(int));
    
    for (int i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            if (dfs_ciclo(g, i, -1, visitado)) {
                free(visitado);
                return 1;
            }
        }
    }
    
    free(visitado);
    return 0;
}