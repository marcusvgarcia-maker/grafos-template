#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"

// --- Implementação do Grafo (Autocontido) ---
GrafoLista* criar_grafo_lista(int n) {
    GrafoLista* g = (GrafoLista*) malloc(sizeof(GrafoLista));
    g->n = n;
    g->adj = (No**) malloc(n * sizeof(No*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

static void inserir_no(GrafoLista* g, int u, int v) {
    No* novo = (No*) malloc(sizeof(No));
    novo->destino = v;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
}

void inserir_aresta_lista(GrafoLista* g, int u, int v) {
    if (u >= 0 && u < g->n && v >= 0 && v < g->n) {
        inserir_no(g, u, v);
        if (u != v) {
            inserir_no(g, v, u);
        }
    }
}

void liberar_grafo_lista(GrafoLista* g) {
    if (g) {
        for (int i = 0; i < g->n; i++) {
            No* atual = g->adj[i];
            while (atual != NULL) {
                No* prox = atual->prox;
                free(atual);
                atual = prox;
            }
        }
        free(g->adj);
        free(g);
    }
}

// --- Algoritmo de Tarjan para Articulações e Pontes ---
void dfs_articulacoes(GrafoLista *g, int u, int *visitado, int *descoberta, int *low, int *pai, int *articulacao, int *tempo) {
    visitado[u] = 1;
    descoberta[u] = low[u] = ++(*tempo);
    int filhos = 0;
    
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        
        if (!visitado[v]) {
            filhos++;
            pai[v] = u;
            dfs_articulacoes(g, v, visitado, descoberta, low, pai, articulacao, tempo);
            
            // Atualiza o valor low de u
            if (low[v] < low[u]) {
                low[u] = low[v];
            }
            
            // Condição 1: u é raiz e tem mais de um filho
            if (pai[u] == -1 && filhos > 1) {
                articulacao[u] = 1;
            }
            
            // Condição 2: u não é raiz e o valor low do filho v é >= tempo de descoberta de u
            if (pai[u] != -1 && low[v] >= descoberta[u]) {
                articulacao[u] = 1;
            }
            
        } else if (v != pai[u]) {
            if (descoberta[v] < low[u]) {
                low[u] = descoberta[v];
            }
        }
        atual = atual->prox;
    }
}

void encontrar_articulacoes(GrafoLista *g) {
    int n = g->n;
    int *visitado = (int*) calloc(n, sizeof(int));
    int *descoberta = (int*) calloc(n, sizeof(int));
    int *low = (int*) calloc(n, sizeof(int));
    int *pai = (int*) malloc(n * sizeof(int));
    int *articulacao = (int*) calloc(n, sizeof(int));
    int tempo = 0;

    for (int i = 0; i < n; i++) {
        pai[i] = -1;
    }

    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_articulacoes(g, i, visitado, descoberta, low, pai, articulacao, &tempo);
        }
    }

    printf("Vertices de corte (Articulacoes): ");
    int achou = 0;
    for (int i = 0; i < n; i++) {
        if (articulacao[i]) {
            printf("%d ", i);
            achou = 1;
        }
    }
    if (!achou) printf("Nenhum");
    printf("\n");

    free(visitado); free(descoberta); free(low); free(pai); free(articulacao);
}

// Reutiliza a lógica do Tarjan para focar apenas nas arestas de corte
static void dfs_pontes(GrafoLista *g, int u, int *visitado, int *descoberta, int *low, int *pai, int *tempo) {
    visitado[u] = 1;
    descoberta[u] = low[u] = ++(*tempo);
    
    No *atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        
        if (!visitado[v]) {
            pai[v] = u;
            dfs_pontes(g, v, visitado, descoberta, low, pai, tempo);
            
            if (low[v] < low[u]) low[u] = low[v];
            
            // Condição de Ponte
            if (low[v] > descoberta[u]) {
                printf("(%d, %d) ", u, v);
            }
        } else if (v != pai[u]) {
            if (descoberta[v] < low[u]) low[u] = descoberta[v];
        }
        atual = atual->prox;
    }
}

void detectar_pontes(GrafoLista *g) {
    int n = g->n;
    int *visitado = (int*) calloc(n, sizeof(int));
    int *descoberta = (int*) calloc(n, sizeof(int));
    int *low = (int*) calloc(n, sizeof(int));
    int *pai = (int*) malloc(n * sizeof(int));
    int tempo = 0;

    for (int i = 0; i < n; i++) pai[i] = -1;

    printf("Pontes (Arestas de corte): ");
    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            dfs_pontes(g, i, visitado, descoberta, low, pai, &tempo);
        }
    }
    printf("\n");

    free(visitado); free(descoberta); free(low); free(pai);
}