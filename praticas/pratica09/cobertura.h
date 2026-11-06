#ifndef COBERTURA_H
#define COBERTURA_H

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

// selecionado[v] = 1 indica que v pertence a cobertura; 0 caso contrario.
// Retorna 1 se cada aresta possui pelo menos uma extremidade selecionada.
int eh_cobertura(const GrafoLista *g, const int *selecionado);

// Retornam a quantidade de vertices selecionados ou -1 em caso de erro.
// O vetor selecionado deve ter capacidade para g->n inteiros.
// Backtracking exato com poda por melhor limite conhecido (exponencial).
int cobertura_minima_bt(const GrafoLista *g, int *selecionado);

// Heuristica: maior grau residual primeiro; nao garante otimalidade.
int cobertura_gulosa(const GrafoLista *g, int *selecionado);

#endif
