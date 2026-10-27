#include "agm.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static void imprimir_resultado(const char *algoritmo, int n,
                               const Aresta *arestas, int quantidade,
                               long long custo) {
    printf("%s:\n", algoritmo);
    for (int i = 0; i < quantidade; i++)
        printf("  %d -- %d (peso %d)\n",
               arestas[i].u, arestas[i].v, arestas[i].peso);

    printf("  Arestas selecionadas: %d\n", quantidade);
    printf("  Custo total: %lld\n", custo);
    if (quantidade == n - 1)
        printf("  Arvore geradora minima.\n");
    else
        printf("  Grafo desconexo: floresta geradora minima com %d componentes.\n",
               n - quantidade);
}

// Mesmo modelo de testes com valores esperados usado na Pratica 07.
static int testar_grafo(const char *titulo, int n, const Aresta *arestas, int m,
                       int origem, int quantidade_esperada,
                       long long custo_esperado) {
    GrafoListaPonderado *g = criar_grafo_ponderado(n);
    if (g == NULL) {
        fprintf(stderr, "Erro ao criar grafo.\n");
        return 0;
    }

    for (int i = 0; i < m; i++) {
        if (!inserir_aresta_ponderada(g, arestas[i].u, arestas[i].v, arestas[i].peso)) {
            fprintf(stderr, "Erro ao inserir aresta.\n");
            liberar_grafo_ponderado(g);
            return 0;
        }
    }

    // Um vertice nao precisa de arestas, mas evitamos uma alocacao de tamanho 0.
    size_t capacidade = (size_t)(n > 1 ? n - 1 : 1);
    Aresta *agm_kruskal = malloc(capacidade * sizeof(*agm_kruskal));
    Aresta *agm_prim = malloc(capacidade * sizeof(*agm_prim));
    if (agm_kruskal == NULL || agm_prim == NULL) {
        fprintf(stderr, "Erro ao alocar resultados.\n");
        free(agm_kruskal);
        free(agm_prim);
        liberar_grafo_ponderado(g);
        return 0;
    }

    long long custo_kruskal = 0;
    long long custo_prim = 0;
    int qk = kruskal(n, arestas, m, agm_kruskal, &custo_kruskal);
    int qp = prim(g, origem, agm_prim, &custo_prim);

    printf("=== %s ===\n", titulo);
    if (qk >= 0) imprimir_resultado("Kruskal", n, agm_kruskal, qk, custo_kruskal);
    if (qp >= 0) imprimir_resultado("Prim", n, agm_prim, qp, custo_prim);

    int passou = qk == quantidade_esperada && qp == quantidade_esperada &&
                 custo_kruskal == custo_esperado && custo_prim == custo_esperado;
    printf("Teste: %s\n\n", passou ? "OK" : "FALHOU");

    free(agm_kruskal);
    free(agm_prim);
    liberar_grafo_ponderado(g);
    return passou;
}

int main(void) {
    // Mesmas arestas da Pratica 06. Primeiro, somente os 6 vertices conectados.
    const Aresta arestas1[] = {
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5},
        {2, 3, 8}, {2, 4, 10}, {3, 4, 2}, {3, 5, 6}, {4, 5, 3}
    };
    const int m1 = (int)(sizeof(arestas1) / sizeof(arestas1[0]));

    // Lacos nao pertencem a AGM; entre paralelas, pode entrar a mais barata.
    const Aresta arestas2[] = {
        {0, 0, -100}, {0, 1, 10}, {0, 1, -2}, {1, 2, 0},
        {0, 2, 5}, {2, 3, 3}, {1, 3, 8}, {3, 3, -50}
    };
    const int m2 = (int)(sizeof(arestas2) / sizeof(arestas2[0]));

    // Com empates, os algoritmos podem escolher arvores diferentes de mesmo custo.
    const Aresta arestas3[] = {
        {0, 1, 1}, {0, 2, 1}, {0, 3, 1}, {1, 2, 1}, {1, 3, 1}, {2, 3, 1}
    };
    const int m3 = (int)(sizeof(arestas3) / sizeof(arestas3[0]));

    // Confere pesos nos extremos de int e somas que precisam de long long.
    const Aresta arestas4[] = {
        {0, 1, INT_MAX}, {1, 2, INT_MIN}, {0, 2, INT_MAX}
    };
    const Aresta arestas5[] = {{0, 1, INT_MAX}, {1, 2, INT_MAX}};

    printf("=== Pratica 08: Arvore Geradora Minima ===\n\n");
    int ok = 1;
    ok &= testar_grafo("Grafo conectado da Pratica 06", 6, arestas1, m1, 0, 5, 13);
    ok &= testar_grafo("Vertice 6 isolado; Prim comeca nele", 7,
                       arestas1, m1, 6, 5, 13);
    ok &= testar_grafo("Pesos negativos, zero, paralelas e lacos", 4,
                       arestas2, m2, 0, 3, 1);
    ok &= testar_grafo("Pesos empatados", 4, arestas3, m3, 2, 3, 3);
    ok &= testar_grafo("Um unico vertice", 1, NULL, 0, 0, 0, 0);
    ok &= testar_grafo("Quatro vertices sem arestas", 4, NULL, 0, 0, 0, 0);
    ok &= testar_grafo("Pesos INT_MIN e INT_MAX", 3, arestas4, 3, 0, 2, -1);
    ok &= testar_grafo("Custo total maior que INT_MAX", 3, arestas5, 2,
                       0, 2, 2LL * INT_MAX);

    printf("Resultado final: %s\n", ok ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHA");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
