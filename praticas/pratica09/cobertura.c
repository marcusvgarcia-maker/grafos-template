#include "cobertura.h"
#include <stdlib.h>
#include <string.h>

GrafoLista *criar_grafo_lista(int n) {
    if (n <= 0) return NULL;

    GrafoLista *g = malloc(sizeof(*g));
    if (g == NULL) return NULL;
    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (g->adj == NULL) {
        free(g);
        return NULL;
    }
    g->n = n;
    g->m = 0;
    return g;
}

int inserir_aresta_lista(GrafoLista *g, int u, int v) {
    if (g == NULL || u < 0 || v < 0 || u >= g->n || v >= g->n)
        return 0;

    // O mesmo par nao precisa aparecer duas vezes na lista.
    for (No *a = g->adj[u]; a != NULL; a = a->prox)
        if (a->destino == v) return 1;

    No *ida = malloc(sizeof(*ida));
    if (ida == NULL) return 0;
    No *volta = NULL;
    if (u != v) {
        volta = malloc(sizeof(*volta));
        if (volta == NULL) {
            free(ida);
            return 0;
        }
    }

    ida->destino = v;
    ida->prox = g->adj[u];
    g->adj[u] = ida;
    if (volta != NULL) {
        volta->destino = u;
        volta->prox = g->adj[v];
        g->adj[v] = volta;
    }
    g->m++;
    return 1;
}

void liberar_grafo_lista(GrafoLista *g) {
    if (g == NULL) return;
    for (int v = 0; v < g->n; v++) {
        No *atual = g->adj[v];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

int eh_cobertura(const GrafoLista *g, const int *selecionado) {
    if (g == NULL || selecionado == NULL) return 0;
    for (int u = 0; u < g->n; u++)
        for (const No *a = g->adj[u]; a != NULL; a = a->prox)
            if (!selecionado[u] && !selecionado[a->destino])
                return 0;
    return 1;
}

// Grau residual = quantidade de arestas incidentes ainda nao cobertas.
static int grau_residual(const GrafoLista *g, const int *selecionado, int u) {
    if (selecionado[u]) return 0;
    int grau = 0;
    for (const No *a = g->adj[u]; a != NULL; a = a->prox)
        if (!selecionado[a->destino]) grau++;
    return grau;
}

int cobertura_gulosa(const GrafoLista *g, int *selecionado) {
    if (g == NULL || selecionado == NULL) return -1;
    memset(selecionado, 0, (size_t)g->n * sizeof(*selecionado));

    int quantidade = 0;
    for (;;) {
        int melhor_vertice = -1;
        int maior_grau = 0;
        for (int v = 0; v < g->n; v++) {
            int grau = grau_residual(g, selecionado, v);
            if (grau > maior_grau) {
                maior_grau = grau;
                melhor_vertice = v;
            }
        }
        // Nenhum grau residual positivo: todas as arestas estao cobertas.
        if (melhor_vertice == -1) break;
        selecionado[melhor_vertice] = 1;
        quantidade++;
    }
    return quantidade;
}

// Aresta (u,v) descoberta: nenhum dos extremos faz parte da cobertura.
static int encontrar_aresta_descoberta(const GrafoLista *g,
                                      const int *atual, int *u, int *v) {
    for (int i = 0; i < g->n; i++) {
        if (atual[i]) continue;
        for (const No *a = g->adj[i]; a != NULL; a = a->prox) {
            if (!atual[a->destino]) {
                *u = i;
                *v = a->destino;
                return 1;
            }
        }
    }
    return 0;
}

// Em qualquer cobertura de uma aresta descoberta (u,v), u ou v deve entrar.
// Assim, os dois ramos cobrem todas as possibilidades sem gerar 2^n mascaras.
static void busca_bt(const GrafoLista *g, int *atual, int quantidade,
                     int *melhor_solucao, int *melhor_quantidade) {
    // Poda por limite: nao vale continuar se ja nao podemos melhorar.
    if (quantidade >= *melhor_quantidade) return;

    int u, v;
    if (!encontrar_aresta_descoberta(g, atual, &u, &v)) {
        *melhor_quantidade = quantidade;
        memcpy(melhor_solucao, atual, (size_t)g->n * sizeof(*atual));
        return;
    }

    // Tenta primeiro a extremidade com maior grau residual.
    if (grau_residual(g, atual, v) > grau_residual(g, atual, u)) {
        int temporario = u;
        u = v;
        v = temporario;
    }

    atual[u] = 1;
    busca_bt(g, atual, quantidade + 1, melhor_solucao, melhor_quantidade);
    atual[u] = 0;

    if (u != v) {  // Em um laco (u,u), basta o unico extremo.
        atual[v] = 1;
        busca_bt(g, atual, quantidade + 1, melhor_solucao, melhor_quantidade);
        atual[v] = 0;
    }
}

int cobertura_minima_bt(const GrafoLista *g, int *selecionado) {
    if (g == NULL || selecionado == NULL) return -1;

    int *atual = calloc((size_t)g->n, sizeof(*atual));
    int *melhor_solucao = malloc((size_t)g->n * sizeof(*melhor_solucao));
    if (atual == NULL || melhor_solucao == NULL) {
        free(atual);
        free(melhor_solucao);
        return -1;
    }

    // A gulosa fornece uma solucao valida para iniciar a poda.
    int melhor_quantidade = cobertura_gulosa(g, melhor_solucao);
    if (melhor_quantidade >= 0)
        busca_bt(g, atual, 0, melhor_solucao, &melhor_quantidade);
    if (melhor_quantidade >= 0)
        memcpy(selecionado, melhor_solucao,
               (size_t)g->n * sizeof(*selecionado));

    free(atual);
    free(melhor_solucao);
    return melhor_quantidade;
}
