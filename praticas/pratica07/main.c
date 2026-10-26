#include "bellman_ford.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int testar_grafo(const char *titulo, int n, const int arestas[][3],
                       size_t quantidade_arestas, int origem,
                       int resultado_esperado, const int *distancias_esperadas) {
    GrafoListaPonderado *g = criar_grafo_ponderado(n);
    if (g == NULL) {
        fprintf(stderr, "Erro ao criar grafo.\n");
        return 0;
    }

    for (size_t i = 0; i < quantidade_arestas; i++) {
        if (!inserir_aresta_ponderada(g, arestas[i][0], arestas[i][1], arestas[i][2])) {
            fprintf(stderr, "Erro ao inserir aresta.\n");
            liberar_grafo_ponderado(g);
            return 0;
        }
    }

    int *dist = malloc((size_t)n * sizeof(*dist));
    int *pred = malloc((size_t)n * sizeof(*pred));
    if (dist == NULL || pred == NULL) {
        fprintf(stderr, "Erro ao alocar vetores.\n");
        free(dist);
        free(pred);
        liberar_grafo_ponderado(g);
        return 0;
    }

    printf("=== %s ===\n", titulo);
    printf("Origem: %d\n", origem);
    int resultado = bellman_ford(g, origem, dist, pred);

    if (resultado == 1) {
        printf("Nenhum ciclo negativo alcancavel.\n");
        for (int v = 0; v < n; v++) {
            if (dist[v] == INT_MAX)
                printf("Vertice %d: inalcancavel\n", v);
            else
                printf("Vertice %d: distancia %d, predecessor %d\n",
                       v, dist[v], pred[v]);
        }
    } else if (resultado == -1) {
        fprintf(stderr, "Erro ao executar Bellman-Ford.\n");
    }

    int passou = (resultado == resultado_esperado);
    if (passou && resultado == 1 && distancias_esperadas != NULL) {
        for (int v = 0; v < n; v++) {
            if (dist[v] != distancias_esperadas[v]) {
                passou = 0;
                break;
            }
        }
    }
    printf("Teste: %s\n\n", passou ? "OK" : "FALHOU");

    free(dist);
    free(pred);
    liberar_grafo_ponderado(g);
    return passou;
}

int main(void) {
    // Caso 1: pesos negativos permitidos, mas nenhum ciclo negativo.
    // O vertice 5 permanece isolado.
    const int arestas1[][3] = {
        {0, 1, 6}, {0, 2, 7}, {1, 2, 8}, {1, 3, 5}, {1, 4, -4},
        {2, 3, -3}, {2, 4, 9}, {3, 1, -2}, {4, 0, 2}, {4, 3, 7}
    };
    const int esperado1[] = {0, 2, 7, 4, -2, INT_MAX};

    // Caso 2: ciclo negativo 1 -> 2 -> 1, custo -6 + 1 = -5.
    const int arestas2[][3] = {
        {0, 1, 4}, {1, 2, -6}, {2, 1, 1}, {2, 3, 2}
    };

    // Caso 3: ciclo negativo existente, mas inalcancavel a partir de 0.
    const int arestas3[][3] = {
        {0, 1, 3}, {2, 3, -5}, {3, 2, 1}
    };
    const int esperado3[] = {0, 3, INT_MAX, INT_MAX};

    printf("=== Pratica 07: Bellman-Ford ===\n\n");
    int ok = 1;
    ok &= testar_grafo("Grafo com pesos negativos, sem ciclo negativo", 6,
                       arestas1, sizeof(arestas1) / sizeof(arestas1[0]),
                       0, 1, esperado1);
    ok &= testar_grafo("Grafo com ciclo negativo alcancavel", 4,
                       arestas2, sizeof(arestas2) / sizeof(arestas2[0]),
                       0, 0, NULL);
    ok &= testar_grafo("Ciclo negativo inalcancavel", 4,
                       arestas3, sizeof(arestas3) / sizeof(arestas3[0]),
                       0, 1, esperado3);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
