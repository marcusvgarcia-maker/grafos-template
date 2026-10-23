#ifndef DIJKSTRA_H
#define DIJKSTRA_H

// Grafo ponderado representado por lista de adjacencia
typedef struct ArestaPonderada {
    int destino;
    int peso;
    struct ArestaPonderada *prox;
} ArestaPonderada;

typedef struct {
    int n;
    ArestaPonderada **adj;
} GrafoListaPonderado;

typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; // vertice -> indice no heap; -1 = fora do heap
} MinHeap;

// Grafo nao direcionado
GrafoListaPonderado *criar_grafo_ponderado(int n);
int inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

// Fila de prioridade Min-Heap.
MinHeap *criar_min_heap(int capacidade);
int inserir_min_heap(MinHeap *heap, int vertice, int distancia);
NoHeap extrair_minimo(MinHeap *heap);
int diminuir_chave(MinHeap *heap, int vertice, int nova_distancia);
void liberar_min_heap(MinHeap *heap);

// Calcula distancias e predecessores a partir da origem
void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);

// Imprime a sequencia de vertices do caminho minimo
void reconstruir_caminho(int origem, int destino, int *pred);

#endif
