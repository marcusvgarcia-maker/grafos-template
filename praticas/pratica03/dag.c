#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

// --- Funções Auxiliares do Grafo ---

GrafoLista* criar_grafo_lista(int n) {
    GrafoLista* g = (GrafoLista*) malloc(sizeof(GrafoLista));
    g->n = n;
    g->adj = (No**) malloc(n * sizeof(No*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
    return g;
}

void inserir_aresta_direcionada(GrafoLista* g, int u, int v) {
    // Insere APENAS de u para v (Grafo Direcionado)
    No* novo = (No*) malloc(sizeof(No));
    novo->destino = v;
    novo->prox = g->adj[u];
    g->adj[u] = novo;
}

void liberar_grafo_lista(GrafoLista* g) {
    for (int i = 0; i < g->n; i++) {
        No* atual = g->adj[i];
        while (atual != NULL) {
            No* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(g->adj);
    free(g);
}

// --- Funções da Prática 03 ---

int* ordenacao_topologica_kahn(GrafoLista *g, int *tamanho) {
    int n = g->n;
    int* grau_entrada = (int*) calloc(n, sizeof(int));
    int* ordem = (int*) malloc(n * sizeof(int));
    int count = 0;

    // 1. Calcula o grau de entrada de todos os vértices
    for (int i = 0; i < n; i++) {
        No* atual = g->adj[i];
        while (atual != NULL) {
            grau_entrada[atual->destino]++;
            atual = atual->prox;
        }
    }

    // 2. Inicializa a "Fila" (usaremos um array simulando fila)
    int* fila = (int*) malloc(n * sizeof(int));
    int inicio = 0, fim = 0;

    for (int i = 0; i < n; i++) {
        if (grau_entrada[i] == 0) {
            fila[fim++] = i; // Enfileira
        }
    }

    // 3. Processa a Fila
    while (inicio < fim) {
        int u = fila[inicio++]; // Desenfileira
        ordem[count++] = u;

        No* atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            grau_entrada[v]--;
            if (grau_entrada[v] == 0) {
                fila[fim++] = v; // Enfileira
            }
            atual = atual->prox;
        }
    }

    free(grau_entrada);
    free(fila);

    // 4. Se processou todos os vértices, é DAG. Senão, tem ciclo.
    if (count == n) {
        *tamanho = count;
        return ordem;
    } else {
        free(ordem);
        *tamanho = 0;
        return NULL; // Há ciclo
    }
}

// Função auxiliar recursiva para o DFS
// visitado[i]: 0 (não visitado), 1 (visitando - pilha de recursão), 2 (visitado/finalizado)
int dfs_visita(GrafoLista *g, int u, int *visitado, int *ordem, int *idx) {
    visitado[u] = 1; 

    No* atual = g->adj[u];
    while (atual != NULL) {
        int v = atual->destino;
        if (visitado[v] == 1) {
            return 0; // Encontrou um ciclo (aresta de retorno)
        }
        if (visitado[v] == 0) {
            if (!dfs_visita(g, v, visitado, ordem, idx)) {
                return 0;
            }
        }
        atual = atual->prox;
    }

    visitado[u] = 2; // Finalizado
    ordem[(*idx)--] = u; // Adiciona na ordem topológica de trás pra frente (simula o empilhamento)
    return 1;
}

int* ordenacao_topologica_dfs(GrafoLista *g, int *tamanho) {
    int n = g->n;
    int* visitado = (int*) calloc(n, sizeof(int));
    int* ordem = (int*) malloc(n * sizeof(int));
    int idx = n - 1; // Posição para inserir do final para o início

    for (int i = 0; i < n; i++) {
        if (visitado[i] == 0) {
            if (!dfs_visita(g, i, visitado, ordem, &idx)) {
                // Ciclo detectado
                free(visitado);
                free(ordem);
                *tamanho = 0;
                return NULL;
            }
        }
    }

    free(visitado);
    *tamanho = n;
    return ordem;
}

int eh_dag(GrafoLista *g) {
    int tamanho = 0;
    int* ordem = ordenacao_topologica_kahn(g, &tamanho);
    if (ordem != NULL) {
        free(ordem);
        return 1; // É acíclico
    }
    return 0; // Possui ciclo
}