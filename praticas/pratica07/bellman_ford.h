#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

// Representacao por lista de adjacencia (mesmo modelo da Pratica 06).
typedef struct ArestaPonderada {
    int destino;
    int peso;
    struct ArestaPonderada *prox;
} ArestaPonderada;

typedef struct {
    int n;
    ArestaPonderada **adj;
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);

// Insere uma aresta DIRECIONADA u -> v, permitindo pesos negativos.
int inserir_aresta_ponderada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo_ponderado(GrafoListaPonderado *g);

// Preenche dist[] e pred[] a partir de origem.
// Retorno: 1 = sem ciclo negativo alcancavel; 0 = ciclo negativo alcancavel;
//         -1 = parametros invalidos, falha de memoria ou custo fora do tipo int.
// INT_MAX em dist[v] significa que o vertice v e inalcancavel.
// Se retornar 0, dist[] nao representa caminhos minimos validos.
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

// Segue n predecessores para entrar no ciclo e imprime-o na direcao das arestas.
void reconstruir_ciclo_negativo(int n, int vertice, const int *pred);

#endif
