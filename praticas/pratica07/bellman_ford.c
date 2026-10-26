#include "bellman_ford.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) return NULL;

    GrafoListaPonderado *g = malloc(sizeof(*g));
    if (g == NULL) return NULL;

    g->n = n;
    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (g->adj == NULL) {
        free(g);
        return NULL;
    }
    return g;
}

int inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso) {
    if (g == NULL || u < 0 || v < 0 || u >= g->n || v >= g->n)
        return 0;

    ArestaPonderada *nova = malloc(sizeof(*nova));
    if (nova == NULL) return 0;

    nova->destino = v;
    nova->peso = peso;
    nova->prox = g->adj[u];
    g->adj[u] = nova; // Nao cria automaticamente a aresta de volta.
    return 1;
}

void liberar_grafo_ponderado(GrafoListaPonderado *g) {
    if (g == NULL) return;

    for (int u = 0; u < g->n; u++) {
        ArestaPonderada *atual = g->adj[u];
        while (atual != NULL) {
            ArestaPonderada *proxima = atual->prox;
            free(atual);
            atual = proxima;
        }
    }
    free(g->adj);
    free(g);
}

void reconstruir_ciclo_negativo(int n, int vertice, const int *pred) {
    if (n <= 0 || vertice < 0 || vertice >= n || pred == NULL) {
        printf("Nao foi possivel reconstruir o ciclo.\n");
        return;
    }

    // Mesmo que vertice esteja fora do ciclo, n passos garantem entrar nele.
    int inicio = vertice;
    for (int i = 0; i < n; i++) {
        if (inicio < 0 || inicio >= n || pred[inicio] < 0 || pred[inicio] >= n) {
            printf("Nao foi possivel reconstruir o ciclo.\n");
            return;
        }
        inicio = pred[inicio];
    }

    int *ciclo = malloc((size_t)n * sizeof(*ciclo));
    if (ciclo == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para reconstruir o ciclo.\n");
        return;
    }

    // Os predecessores percorrem as arestas no sentido inverso.
    int atual = inicio;
    int tamanho = 0;
    do {
        if (atual < 0 || atual >= n || tamanho >= n) {
            printf("Nao foi possivel reconstruir o ciclo.\n");
            free(ciclo);
            return;
        }
        ciclo[tamanho++] = atual;
        atual = pred[atual];
    } while (atual != inicio);

    // Imprime na direcao correta: u -> v, e fecha o ciclo.
    printf("Ciclo negativo: %d", ciclo[0]);
    for (int i = tamanho - 1; i >= 1; i--)
        printf(" -> %d", ciclo[i]);
    printf(" -> %d\n", ciclo[0]);

    free(ciclo);
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (g == NULL || g->adj == NULL || g->n <= 0 || dist == NULL ||
        pred == NULL || origem < 0 || origem >= g->n) {
        fprintf(stderr, "Parametros invalidos para Bellman-Ford.\n");
        return -1;
    }

    // Usa long long internamente para evitar overflow em somas de pesos int.
    // A interface dist[] permanece int, assim como na Pratica 06.
    const long long infinito = LLONG_MAX / 4;
    long long *d = malloc((size_t)g->n * sizeof(*d));
    if (d == NULL) {
        fprintf(stderr, "Erro ao alocar distancias.\n");
        return -1;
    }

    for (int v = 0; v < g->n; v++) {
        d[v] = infinito;
        dist[v] = INT_MAX;
        pred[v] = -1;
    }
    d[origem] = 0;

    // Passo 1: no maximo n-1 iteracoes de relaxacao de todas as arestas.
    for (int iteracao = 1; iteracao < g->n; iteracao++) {
        int alterou = 0;
        for (int u = 0; u < g->n; u++) {
            if (d[u] == infinito) continue; // Nao relaxa a partir de inalcancaveis.
            for (ArestaPonderada *a = g->adj[u]; a != NULL; a = a->prox) {
                long long candidata = d[u] + (long long)a->peso;
                if (candidata < d[a->destino]) {
                    d[a->destino] = candidata;
                    pred[a->destino] = u;
                    alterou = 1;
                }
            }
        }
        if (!alterou) break; // Nenhuma melhoria: convergiu.
    }

    // Passo 2: a n-esima iteracao identifica ciclos negativos alcancaveis.
    int vertice_no_ciclo = -1;
    for (int u = 0; u < g->n; u++) {
        if (d[u] == infinito) continue;
        for (ArestaPonderada *a = g->adj[u]; a != NULL; a = a->prox) {
            long long candidata = d[u] + (long long)a->peso;
            if (candidata < d[a->destino]) {
                d[a->destino] = candidata;
                pred[a->destino] = u;
                vertice_no_ciclo = a->destino;
            }
        }
    }

    if (vertice_no_ciclo != -1) {
        printf("Foi encontrado um ciclo negativo alcancavel da origem %d.\n", origem);
        reconstruir_ciclo_negativo(g->n, vertice_no_ciclo, pred);
        free(d);
        return 0;
    }

    // Converte os resultados para o vetor int exigido pela atividade.
    for (int v = 0; v < g->n; v++) {
        if (d[v] == infinito) continue;
        if (d[v] < INT_MIN || d[v] >= INT_MAX) {
            fprintf(stderr, "Distancia para o vertice %d nao cabe em int.\n", v);
            free(d);
            return -1;
        }
        dist[v] = (int)d[v];
    }

    free(d);
    return 1;
}
