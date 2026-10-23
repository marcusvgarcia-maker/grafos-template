#include "dijkstra.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Grafo nao direcionado: 6 vertices conectados e 1 isolado (vertice 6).
    GrafoListaPonderado *g = criar_grafo_ponderado(7);
    if (g == NULL) {
        fprintf(stderr, "Erro ao criar grafo.\n");
        return EXIT_FAILURE;
    }

    const int arestas[][3] = {
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5},
        {2, 3, 8}, {2, 4, 10}, {3, 4, 2}, {3, 5, 6}, {4, 5, 3}
    };
    const size_t quantidade_arestas = sizeof(arestas) / sizeof(arestas[0]);
    for (size_t i = 0; i < quantidade_arestas; i++) {
        if (!inserir_aresta_ponderada(g, arestas[i][0], arestas[i][1], arestas[i][2])) {
            fprintf(stderr, "Erro ao inserir aresta.\n");
            liberar_grafo_ponderado(g);
            return EXIT_FAILURE;
        }
    }

    int dist[7];
    int pred[7];
    int origem = 0;
    dijkstra(g, origem, dist, pred);

    printf("=== Pratica 06: Dijkstra com Min-Heap ===\n");
    printf("Origem: %d\n\n", origem);
    for (int v = 0; v < g->n; v++) {
        if (dist[v] == INT_MAX)
            printf("Vertice %d: inalcancavel\n", v);
        else
            printf("Vertice %d: distancia %d, predecessor %d\n", v, dist[v], pred[v]);
    }

    printf("\n");
    reconstruir_caminho(origem, 5, pred); // Custo esperado: 13.
    reconstruir_caminho(origem, 6, pred); // Vertice isolado.

    liberar_grafo_ponderado(g);
    return EXIT_SUCCESS;
}
