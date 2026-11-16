#include "hamiltoniano.h"
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

static int existe_aresta(const GrafoLista *g, int u, int v) {
    for (const No *a = g->adj[u]; a != NULL; a = a->prox)
        if (a->destino == v) return 1;
    return 0;
}

// Valida a sequencia; "fechar" exige tambem a aresta ultimo -> primeiro.
static int validar_sequencia(const GrafoLista *g, const int *seq, int fechar) {
    if (g == NULL || seq == NULL) return 0;
    int n = g->n;
    if (fechar && n == 2) return 0;  // Grafo simples nao tem ciclo de 2.

    unsigned char *visto = calloc((size_t)n, sizeof(*visto));
    if (visto == NULL) return 0;

    int ok = 1;
    for (int i = 0; i < n && ok; i++) {
        int v = seq[i];
        if (v < 0 || v >= n || visto[v]) {
            ok = 0;
        } else {
            visto[v] = 1;
            if (i > 0 && !existe_aresta(g, seq[i - 1], v)) ok = 0;
        }
    }
    if (ok && fechar && !existe_aresta(g, seq[n - 1], seq[0])) ok = 0;

    free(visto);
    return ok;
}

int eh_caminho_hamiltoniano(const GrafoLista *g, const int *caminho) {
    return validar_sequencia(g, caminho, 0);
}

int eh_ciclo_hamiltoniano(const GrafoLista *g, const int *ciclo) {
    return validar_sequencia(g, ciclo, 1);
}

// Estado da busca. A matriz de adjacencia deixa o teste de aresta em O(1).
typedef struct {
    int n;
    const unsigned char *matriz;
    unsigned char *usado;
    int *sequencia;
    int exigir_ciclo;
} Busca;

// Preenche a posicao pos da permutacao. Retorna 1 ao achar uma solucao.
static int estender(Busca *b, int pos) {
    int n = b->n;
    if (pos == n) {
        if (!b->exigir_ciclo) return 1;
        return b->matriz[b->sequencia[n - 1] * n + b->sequencia[0]];
    }

    int anterior = b->sequencia[pos - 1];
    for (int v = 0; v < n; v++) {
        // Poda: vertice repetido ou sem aresta a partir do anterior.
        if (b->usado[v] || !b->matriz[anterior * n + v]) continue;
        b->usado[v] = 1;
        b->sequencia[pos] = v;
        if (estender(b, pos + 1)) return 1;
        b->usado[v] = 0;
    }
    return 0;
}

static int buscar(const GrafoLista *g, int *saida, int exigir_ciclo) {
    if (g == NULL || saida == NULL) return -1;
    int n = g->n;

    unsigned char *matriz = calloc((size_t)n * (size_t)n, sizeof(*matriz));
    unsigned char *usado = calloc((size_t)n, sizeof(*usado));
    int *sequencia = malloc((size_t)n * sizeof(*sequencia));
    if (matriz == NULL || usado == NULL || sequencia == NULL) {
        free(matriz);
        free(usado);
        free(sequencia);
        return -1;
    }

    for (int u = 0; u < n; u++)
        for (const No *a = g->adj[u]; a != NULL; a = a->prox)
            matriz[u * n + a->destino] = 1;

    Busca b = {n, matriz, usado, sequencia, exigir_ciclo};
    int achou = 0;

    if (exigir_ciclo) {
        if (n == 1) {
            // Ciclo trivial: so existe com laco.
            if (matriz[0]) {
                sequencia[0] = 0;
                achou = 1;
            }
        } else if (n >= 3) {
            // O ciclo e rotacionavel: basta fixar o vertice 0 como inicio.
            sequencia[0] = 0;
            usado[0] = 1;
            achou = estender(&b, 1);
        }
    } else {
        // O caminho pode comecar em qualquer vertice.
        for (int inicio = 0; inicio < n && !achou; inicio++) {
            memset(usado, 0, (size_t)n * sizeof(*usado));
            sequencia[0] = inicio;
            usado[inicio] = 1;
            achou = estender(&b, 1);
        }
    }

    if (achou) memcpy(saida, sequencia, (size_t)n * sizeof(*saida));

    free(matriz);
    free(usado);
    free(sequencia);
    return achou;
}

int encontrar_caminho_hamiltoniano_bt(const GrafoLista *g, int *caminho) {
    return buscar(g, caminho, 0);
}

int encontrar_ciclo_hamiltoniano_bt(const GrafoLista *g, int *ciclo) {
    return buscar(g, ciclo, 1);
}
