#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

// Grafo nao direcionado, baseado na lista de adjacencia da Pratica 01.
// Aceita lacos; arestas repetidas sao ignoradas.
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;               // Numero de vertices (0 a n-1).
    int m;               // Numero de arestas nao direcionadas.
    No **adj;
} GrafoLista;

GrafoLista *criar_grafo_lista(int n);
int inserir_aresta_lista(GrafoLista *g, int u, int v);
void liberar_grafo_lista(GrafoLista *g);

// Verificadores: sequencia de n vertices distintos, com arestas consecutivas.
// O ciclo tambem precisa da aresta entre o ultimo e o primeiro vertice.
// Retornam 1 se valido e 0 caso contrario.
int eh_caminho_hamiltoniano(const GrafoLista *g, const int *caminho);
int eh_ciclo_hamiltoniano(const GrafoLista *g, const int *ciclo);

// Backtracking com permutacoes parciais: so estende a sequencia com vertices
// ainda nao usados e adjacentes ao ultimo (poda por adjacencia). Exponencial.
// O vetor de saida deve ter capacidade para g->n inteiros.
// Retornam 1 se encontrou (sequencia gravada na saida), 0 se nao existe
// ou -1 em caso de erro.
int encontrar_caminho_hamiltoniano_bt(const GrafoLista *g, int *caminho);

// Convencoes: n == 1 so tem ciclo se houver laco; n == 2 nunca tem ciclo
// (grafo simples). O ciclo comeca no vertice 0 e o retorno ao inicio e implicito.
int encontrar_ciclo_hamiltoniano_bt(const GrafoLista *g, int *ciclo);

#endif
