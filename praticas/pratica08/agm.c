#include "agm.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

// Criacao, insercao e liberacao seguem a lista ponderada da Pratica 06.
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
    if (g == NULL || g->adj == NULL ||
        u < 0 || v < 0 || u >= g->n || v >= g->n)
        return 0;

    ArestaPonderada *ida = malloc(sizeof(*ida));
    if (ida == NULL) return 0;

    ArestaPonderada *volta = NULL;
    if (u != v) {
        volta = malloc(sizeof(*volta));
        if (volta == NULL) {
            free(ida);
            return 0;
        }
    }

    ida->destino = v;
    ida->peso = peso;
    ida->prox = g->adj[u];
    g->adj[u] = ida;

    if (volta != NULL) {
        volta->destino = u;
        volta->peso = peso;
        volta->prox = g->adj[v];
        g->adj[v] = volta;
    }
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

DSU *criar_dsu(int n) {
    if (n <= 0) return NULL;

    DSU *d = malloc(sizeof(*d));
    if (d == NULL) return NULL;

    d->pai = malloc((size_t)n * sizeof(*d->pai));
    d->rank = calloc((size_t)n, sizeof(*d->rank));
    if (d->pai == NULL || d->rank == NULL) {
        free(d->pai);
        free(d->rank);
        free(d);
        return NULL;
    }

    d->n = n;
    for (int x = 0; x < n; x++) d->pai[x] = x;
    return d;
}

int find(DSU *d, int x) {
    if (d == NULL || x < 0 || x >= d->n) return -1;

    int raiz = x;
    while (d->pai[raiz] != raiz) raiz = d->pai[raiz];

    // Todos os vertices do caminho passam a apontar diretamente para a raiz.
    while (d->pai[x] != x) {
        int proximo = d->pai[x];
        d->pai[x] = raiz;
        x = proximo;
    }
    return raiz;
}

int unir(DSU *d, int x, int y) {
    int raiz_x = find(d, x);
    int raiz_y = find(d, y);
    if (raiz_x == -1 || raiz_y == -1) return -1;
    if (raiz_x == raiz_y) return 0;

    // A arvore de menor rank fica sob a de maior rank.
    if (d->rank[raiz_x] < d->rank[raiz_y]) {
        d->pai[raiz_x] = raiz_y;
    } else if (d->rank[raiz_x] > d->rank[raiz_y]) {
        d->pai[raiz_y] = raiz_x;
    } else {
        d->pai[raiz_y] = raiz_x;
        d->rank[raiz_x]++;
    }
    return 1;
}

void liberar_dsu(DSU *d) {
    if (d == NULL) return;
    free(d->pai);
    free(d->rank);
    free(d);
}

// Heap adaptado de dijkstra.c (Pratica 06): distancia foi trocada por chave.
static void trocar(MinHeap *heap, int i, int j) {
    NoHeap temporario = heap->dados[i];
    heap->dados[i] = heap->dados[j];
    heap->dados[j] = temporario;
    heap->posicao[heap->dados[i].vertice] = i;
    heap->posicao[heap->dados[j].vertice] = j;
}

static void subir(MinHeap *heap, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (heap->dados[pai].chave <= heap->dados[i].chave) break;
        trocar(heap, pai, i);
        i = pai;
    }
}

static void descer(MinHeap *heap, int i) {
    // Apenas nos internos possuem filhos; evita calcular filhos de uma folha.
    while (i < heap->tamanho / 2) {
        int menor = i;
        int esquerda = 2 * i + 1;
        int direita = esquerda + 1;

        if (heap->dados[esquerda].chave < heap->dados[menor].chave)
            menor = esquerda;
        if (direita < heap->tamanho &&
            heap->dados[direita].chave < heap->dados[menor].chave)
            menor = direita;

        if (menor == i) break;
        trocar(heap, i, menor);
        i = menor;
    }
}

MinHeap *criar_min_heap(int capacidade) {
    if (capacidade <= 0) return NULL;

    MinHeap *heap = malloc(sizeof(*heap));
    if (heap == NULL) return NULL;

    heap->dados = malloc((size_t)capacidade * sizeof(*heap->dados));
    heap->posicao = malloc((size_t)capacidade * sizeof(*heap->posicao));
    if (heap->dados == NULL || heap->posicao == NULL) {
        free(heap->dados);
        free(heap->posicao);
        free(heap);
        return NULL;
    }

    heap->capacidade = capacidade;
    heap->tamanho = 0;
    for (int v = 0; v < capacidade; v++) heap->posicao[v] = -1;
    return heap;
}

int inserir_min_heap(MinHeap *heap, int vertice, long long chave) {
    if (heap == NULL || heap->tamanho == heap->capacidade ||
        vertice < 0 || vertice >= heap->capacidade || heap->posicao[vertice] != -1)
        return 0;

    int i = heap->tamanho++;
    heap->dados[i] = (NoHeap){vertice, chave};
    heap->posicao[vertice] = i;
    subir(heap, i);
    return 1;
}

NoHeap extrair_minimo(MinHeap *heap) {
    NoHeap vazio = {-1, LLONG_MAX};
    if (heap == NULL || heap->tamanho == 0) return vazio;

    NoHeap minimo = heap->dados[0];
    heap->posicao[minimo.vertice] = -1;
    heap->tamanho--;
    if (heap->tamanho > 0) {
        heap->dados[0] = heap->dados[heap->tamanho];
        heap->posicao[heap->dados[0].vertice] = 0;
        descer(heap, 0);
    }
    return minimo;
}

int diminuir_chave(MinHeap *heap, int vertice, long long nova_chave) {
    if (heap == NULL || vertice < 0 || vertice >= heap->capacidade)
        return 0;

    int i = heap->posicao[vertice];
    if (i < 0 || nova_chave > heap->dados[i].chave) return 0;

    heap->dados[i].chave = nova_chave;
    subir(heap, i);
    return 1;
}

void liberar_min_heap(MinHeap *heap) {
    if (heap == NULL) return;
    free(heap->dados);
    free(heap->posicao);
    free(heap);
}

static int comparar_arestas(const void *a, const void *b) {
    const Aresta *x = a;
    const Aresta *y = b;

    // Evita subtrair pesos: INT_MAX - INT_MIN causaria overflow em int.
    if (x->peso != y->peso) return (x->peso > y->peso) ? 1 : -1;
    if (x->u != y->u) return (x->u > y->u) ? 1 : -1;
    return (x->v > y->v) - (x->v < y->v);
}

int kruskal(int n, const Aresta *arestas, int m,
            Aresta *resultado, long long *custo_total) {
    if (n <= 0 || m < 0 || custo_total == NULL ||
        (n > 1 && resultado == NULL) || (m > 0 && arestas == NULL))
        return -1;

    for (int i = 0; i < m; i++) {
        if (arestas[i].u < 0 || arestas[i].v < 0 ||
            arestas[i].u >= n || arestas[i].v >= n)
            return -1;
    }

    *custo_total = 0;
    if (m == 0 || n == 1) return 0;

    Aresta *ordenadas = malloc((size_t)m * sizeof(*ordenadas));
    DSU *d = criar_dsu(n);
    if (ordenadas == NULL || d == NULL) {
        free(ordenadas);
        liberar_dsu(d);
        return -1;
    }
    memcpy(ordenadas, arestas, (size_t)m * sizeof(*ordenadas));
    qsort(ordenadas, (size_t)m, sizeof(*ordenadas), comparar_arestas);

    int quantidade = 0;
    for (int i = 0; i < m && quantidade < n - 1; i++) {
        Aresta a = ordenadas[i];
        // Se as extremidades ja estao unidas, a aresta criaria um ciclo.
        // Isso tambem descarta lacos (u == v).
        if (unir(d, a.u, a.v) == 1) {
            resultado[quantidade++] = a;
            *custo_total += (long long)a.peso;
        }
    }

    free(ordenadas);
    liberar_dsu(d);
    return quantidade;
}

int prim(const GrafoListaPonderado *g, int origem,
         Aresta *resultado, long long *custo_total) {
    if (g == NULL || g->adj == NULL || g->n <= 0 ||
        origem < 0 || origem >= g->n || custo_total == NULL ||
        (g->n > 1 && resultado == NULL))
        return -1;

    MinHeap *heap = criar_min_heap(g->n);
    int *pred = malloc((size_t)g->n * sizeof(*pred));
    if (heap == NULL || pred == NULL) {
        liberar_min_heap(heap);
        free(pred);
        return -1;
    }

    for (int v = 0; v < g->n; v++) {
        pred[v] = -1;
        inserir_min_heap(heap, v, LLONG_MAX);
    }
    diminuir_chave(heap, origem, 0);
    *custo_total = 0;

    int quantidade = 0;
    while (heap->tamanho > 0) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;

        if (pred[u] != -1) {
            resultado[quantidade++] = (Aresta){pred[u], u, (int)atual.chave};
            *custo_total += atual.chave;
        }
        // Sem predecessor, u inicia uma componente (inclusive a primeira).
        // Diferente do Dijkstra, nao paramos ao extrair uma chave infinita.

        for (ArestaPonderada *a = g->adj[u]; a != NULL; a = a->prox) {
            int v = a->destino;
            int i = heap->posicao[v];
            if (i != -1 && (long long)a->peso < heap->dados[i].chave) {
                pred[v] = u;
                // Prim usa so o peso da aresta, sem somar o custo ate u.
                diminuir_chave(heap, v, a->peso);
            }
        }
    }

    liberar_min_heap(heap);
    free(pred);
    return quantidade;
}
