#ifndef AGM_H
#define AGM_H

// Estruturas exigidas pelo enunciado.
typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

// Lista ponderada adaptada da Pratica 06; o grafo e nao direcionado.
typedef struct ArestaPonderada {
    int destino;
    int peso;
    struct ArestaPonderada *prox;
} ArestaPonderada;

typedef struct {
    int n;
    ArestaPonderada **adj;
} GrafoListaPonderado;

// No Prim, chave e o peso da melhor aresta que liga o vertice a arvore.
// long long permite reservar LLONG_MAX sem excluir arestas de peso INT_MAX.
typedef struct {
    int vertice;
    long long chave;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao; // vertice -> indice no heap; -1 = fora do heap
} MinHeap;

GrafoListaPonderado *criar_grafo_ponderado(int n);
// Insere ida e volta; permite pesos negativos, paralelas e lacos.
int inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

DSU *criar_dsu(int n);
int find(DSU *d, int x); // Compressao de caminho; -1 para indice invalido.
// Retorna 1 se uniu, 0 se ja estavam unidos ou -1 para parametro invalido.
int unir(DSU *d, int x, int y);
void liberar_dsu(DSU *d);

MinHeap *criar_min_heap(int capacidade);
int inserir_min_heap(MinHeap *heap, int vertice, long long chave);
NoHeap extrair_minimo(MinHeap *heap);
int diminuir_chave(MinHeap *heap, int vertice, long long nova_chave);
void liberar_min_heap(MinHeap *heap);

// Os dois algoritmos devolvem a quantidade de arestas selecionadas, ou -1
// para parametros invalidos/falha de memoria. Em caso de erro, descarte a saida.
// resultado deve ter espaco para n-1 arestas (pode ser NULL quando n == 1).
// custo_total recebe a soma em long long. Os vertices sao numerados de 0 a n-1.
// Em grafo desconexo, produzem uma FLORESTA geradora minima:
// componentes = n - quantidade_de_arestas. Existe AGM quando a quantidade e n-1.

// A lista de entrada contem cada aresta nao direcionada uma unica vez.
// A ordenacao usa uma copia, preservando o vetor original. O(E log E).
int kruskal(int n, const Aresta *arestas, int m,
            Aresta *resultado, long long *custo_total);

// Comeca em origem e continua nas demais componentes, se houver.
// Lista de adjacencia + heap com decrease-key: O((V+E) log V).
int prim(const GrafoListaPonderado *g, int origem,
         Aresta *resultado, long long *custo_total);

#endif
