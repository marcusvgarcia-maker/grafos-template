#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

// --- Implementação do Grafo ---
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

int grau_lista(GrafoLista* g, int u) {
    int grau = 0;
    No* atual = g->adj[u];
    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }
    return grau;
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

// --- Funções de Coloração[cite: 3] ---

int* coloracao_gulosa(GrafoLista *g, int *num_cores) {
    int n = g->n;
    int *cor = (int*) malloc(n * sizeof(int));
    int *disponivel = (int*) malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        cor[i] = -1; 
        disponivel[i] = 1; 
    }

    cor[0] = 0; // Colore o primeiro vértice com a primeira cor (0)
    int max_cor = 0;

    for (int u = 1; u < n; u++) {
        No *atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (cor[v] != -1) {
                disponivel[cor[v]] = 0; // Cor indisponível
            }
            atual = atual->prox;
        }

        int cr;
        for (cr = 0; cr < n; cr++) {
            if (disponivel[cr]) break;
        }

        cor[u] = cr;
        if (cr > max_cor) max_cor = cr;

        atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->destino;
            if (cor[v] != -1) {
                disponivel[cor[v]] = 1; // Reseta disponibilidade
            }
            atual = atual->prox;
        }
    }

    *num_cores = max_cor + 1;
    free(disponivel);
    return cor;
}

// Estrutura auxiliar para ordenação de Welsh-Powell
typedef struct {
    int vertice;
    int grau;
} VerticeGrau;

int comparar_grau_desc(const void *a, const void *b) {
    VerticeGrau *v1 = (VerticeGrau *)a;
    VerticeGrau *v2 = (VerticeGrau *)b;
    return v2->grau - v1->grau; // Ordem decrescente
}

int* coloracao_welsh_powell(GrafoLista *g, int *num_cores) {
    int n = g->n;
    VerticeGrau *vg = (VerticeGrau*) malloc(n * sizeof(VerticeGrau));
    
    for (int i = 0; i < n; i++) {
        vg[i].vertice = i;
        vg[i].grau = grau_lista(g, i);
    }

    qsort(vg, n, sizeof(VerticeGrau), comparar_grau_desc);

    int *cor = (int*) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) cor[i] = -1;

    int cor_atual = 0;
    int coloridos = 0;

    while (coloridos < n) {
        for (int i = 0; i < n; i++) {
            int u = vg[i].vertice;
            if (cor[u] == -1) {
                // Verifica se há vizinhos com a cor_atual
                int pode_colorir = 1;
                No *atual = g->adj[u];
                while (atual != NULL) {
                    if (cor[atual->destino] == cor_atual) {
                        pode_colorir = 0;
                        break;
                    }
                    atual = atual->prox;
                }

                if (pode_colorir) {
                    cor[u] = cor_atual;
                    coloridos++;
                }
            }
        }
        cor_atual++;
    }

    *num_cores = cor_atual;
    free(vg);
    return cor;
}

int eh_bipartido(GrafoLista *g) {
    int n = g->n;
    int *cor = (int*) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) cor[i] = -1;

    int *fila = (int*) malloc(n * sizeof(int));
    
    for (int start = 0; start < n; start++) {
        if (cor[start] == -1) {
            int inicio = 0, fim = 0;
            cor[start] = 0;
            fila[fim++] = start;

            while (inicio < fim) {
                int u = fila[inicio++];
                No *atual = g->adj[u];
                
                while (atual != NULL) {
                    int v = atual->destino;
                    if (cor[v] == -1) {
                        cor[v] = 1 - cor[u];
                        fila[fim++] = v;
                    } else if (cor[v] == cor[u]) {
                        free(cor);
                        free(fila);
                        return 0; // Não é bipartido
                    }
                    atual = atual->prox;
                }
            }
        }
    }
    
    free(cor);
    free(fila);
    return 1; // É bipartido
}