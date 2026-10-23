#include "dijkstra.h"
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
    if (g == NULL || u < 0 || v < 0 || u >= g->n || v >= g->n || peso < 0)
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

    for (int i = 0; i < g->n; i++) {
        ArestaPonderada *atual = g->adj[i];
        while (atual != NULL) {
            ArestaPonderada *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

// Toda troca precisa atualizar o mapeamento posicao[vertice].
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
        if (heap->dados[pai].distancia <= heap->dados[i].distancia) break;
        trocar(heap, pai, i);
        i = pai;
    }
}

static void descer(MinHeap *heap, int i) {
    while (1) {
        int menor = i;
        int esquerda = 2 * i + 1;
        int direita = 2 * i + 2;

        if (esquerda < heap->tamanho &&
            heap->dados[esquerda].distancia < heap->dados[menor].distancia)
            menor = esquerda;

        if (direita < heap->tamanho &&
            heap->dados[direita].distancia < heap->dados[menor].distancia)
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
    for (int i = 0; i < capacidade; i++) heap->posicao[i] = -1;
    return heap;
}

int inserir_min_heap(MinHeap *heap, int vertice, int distancia) {
    if (heap == NULL || heap->tamanho == heap->capacidade ||
        vertice < 0 || vertice >= heap->capacidade || heap->posicao[vertice] != -1)
        return 0;

    int i = heap->tamanho++;
    heap->dados[i] = (NoHeap){vertice, distancia};
    heap->posicao[vertice] = i;
    subir(heap, i);
    return 1;
}

NoHeap extrair_minimo(MinHeap *heap) {
    NoHeap vazio = {-1, INT_MAX};
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

int diminuir_chave(MinHeap *heap, int vertice, int nova_distancia) {
    if (heap == NULL || vertice < 0 || vertice >= heap->capacidade)
        return 0;

    int i = heap->posicao[vertice];
    if (i < 0 || nova_distancia > heap->dados[i].distancia)
        return 0;

    heap->dados[i].distancia = nova_distancia;
    subir(heap, i);
    return 1;
}

void liberar_min_heap(MinHeap *heap) {
    if (heap == NULL) return;
    free(heap->dados);
    free(heap->posicao);
    free(heap);
}

void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (g == NULL || dist == NULL || pred == NULL || origem < 0 || origem >= g->n) {
        fprintf(stderr, "Parametros invalidos para Dijkstra.\n");
        return;
    }

    MinHeap *heap = criar_min_heap(g->n);
    if (heap == NULL) {
        fprintf(stderr, "Falha ao alocar o Min-Heap.\n");
        return;
    }

    for (int v = 0; v < g->n; v++) {
        dist[v] = INT_MAX;
        pred[v] = -1;
        inserir_min_heap(heap, v, INT_MAX);
    }

    dist[origem] = 0;
    diminuir_chave(heap, origem, 0);

    while (heap->tamanho > 0) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;
        if (atual.distancia == INT_MAX) break; // Restaram apenas inalcancaveis.

        for (ArestaPonderada *aresta = g->adj[u]; aresta != NULL; aresta = aresta->prox) {
            int v = aresta->destino;
            int peso = aresta->peso;

            // Vertice finalizado, ou soma que excederia o limite do tipo int.
            if (heap->posicao[v] == -1 || peso < 0 || dist[u] > INT_MAX - peso)
                continue;

            int nova_distancia = dist[u] + peso;
            if (nova_distancia < dist[v]) {
                dist[v] = nova_distancia;
                pred[v] = u;
                diminuir_chave(heap, v, nova_distancia);
            }
        }
    }
    liberar_min_heap(heap);
}

void reconstruir_caminho(int origem, int destino, int *pred) {
    if (pred == NULL || origem < 0 || destino < 0) {
        printf("Caminho invalido.\n");
        return;
    }

    int capacidade = 8;
    int quantidade = 0;
    int *caminho = malloc((size_t)capacidade * sizeof(*caminho));
    if (caminho == NULL) {
        printf("Falha ao alocar memoria para o caminho.\n");
        return;
    }

    // Percorre de destino ate origem; -1 indica que nao existe caminho.
    int atual = destino;
    while (atual != -1) {
        if (quantidade == capacidade) {
            int nova_capacidade = capacidade * 2;
            int *novo = realloc(caminho, (size_t)nova_capacidade * sizeof(*caminho));
            if (novo == NULL) {
                free(caminho);
                printf("Falha ao ampliar memoria para o caminho.\n");
                return;
            }
            caminho = novo;
            capacidade = nova_capacidade;
        }
        caminho[quantidade++] = atual;
        if (atual == origem) break;
        atual = pred[atual];
    }

    if (quantidade == 0 || caminho[quantidade - 1] != origem) {
        printf("Nao existe caminho de %d ate %d.\n", origem, destino);
    } else {
        printf("Caminho minimo de %d ate %d: ", origem, destino);
        for (int i = quantidade - 1; i >= 0; i--)
            printf("%d%s", caminho[i], i == 0 ? "\n" : " -> ");
    }
    free(caminho);
}
