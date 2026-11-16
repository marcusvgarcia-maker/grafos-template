#ifndef TPS_H
#define TPS_H

#include "hamiltoniano.h"

// Instancia do Problema do Caixeiro Viajante (TSP) simetrico.
// d guarda a matriz n x n em um vetor: distancia(u, v) = d[u * n + v].
typedef struct {
    int n;
    int *d;
} MatrizTSP;

// Todas as distancias comecam em 0.
MatrizTSP *criar_matriz_tsp(int n);
// Define d(u,v) = d(v,u) = dist. Exige u != v e dist >= 0; retorna 1 se ok.
int definir_distancia(MatrizTSP *m, int u, int v, int dist);
// Retorna -1 para indices invalidos.
int obter_distancia(const MatrizTSP *m, int u, int v);
void liberar_matriz_tsp(MatrizTSP *m);

// Reducao Ciclo Hamiltoniano -> TSP: peso 1 nas arestas existentes e 2 nas
// inexistentes. Para n >= 3, o grafo tem ciclo hamiltoniano se e somente se o
// TSP otimo custa exatamente n.
MatrizTSP *grafo_para_tsp(const GrafoLista *g);

// Custo do ciclo que percorre a rota (permutacao de 0..n-1) e volta ao inicio.
// Retorna -1 se a rota nao for uma permutacao valida.
long long custo_rota(const MatrizTSP *m, const int *rota);

// Os algoritmos abaixo gravam em rota (capacidade n) a permutacao encontrada,
// comecando na origem, e retornam o custo do ciclo ou -1 em caso de erro.

// Exato: fixa a origem no vertice 0 e testa as (n-1)! permutacoes. O(n!).
long long tsp_forca_bruta(const MatrizTSP *m, int *rota);

// Heuristica construtiva: parte da origem e vai sempre ao vizinho nao visitado
// mais proximo. O(n^2); nao garante otimalidade.
long long tsp_vizinho_mais_proximo(const MatrizTSP *m, int origem, int *rota);

// Heuristica de insercao do mais proximo: comeca em {0}, escolhe o vertice
// fora do ciclo mais proximo dele e o insere onde aumenta menos o custo.
// O(n^3); nao garante otimalidade.
long long tsp_insercao(const MatrizTSP *m, int *rota);

#endif
