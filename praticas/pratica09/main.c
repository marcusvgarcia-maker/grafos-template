#include "cobertura.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void imprimir_vertices(const int *selecionado, int n) {
    printf("{");
    int primeiro = 1;
    for (int i = 0; i < n; i++) {
        if (selecionado[i]) {
            printf("%s%d", primeiro ? "" : ", ", i);
            primeiro = 0;
        }
    }
    printf("}");
}

static int testar_caso(const char *nome, int n, const int arestas[][2],
                       size_t m, int otimo_esperado) {
    GrafoLista *g = criar_grafo_lista(n);
    if (g == NULL) return 0;
    for (size_t i = 0; i < m; i++) {
        if (!inserir_aresta_lista(g, arestas[i][0], arestas[i][1])) {
            liberar_grafo_lista(g);
            return 0;
        }
    }

    int *exata = calloc((size_t)n, sizeof(*exata));
    int *gulosa = calloc((size_t)n, sizeof(*gulosa));
    if (exata == NULL || gulosa == NULL) {
        free(exata);
        free(gulosa);
        liberar_grafo_lista(g);
        return 0;
    }
    int qtd_exata = cobertura_minima_bt(g, exata);
    int qtd_gulosa = cobertura_gulosa(g, gulosa);
    int passou = qtd_exata == otimo_esperado && qtd_gulosa >= qtd_exata &&
                 eh_cobertura(g, exata) && eh_cobertura(g, gulosa);

    printf("%-27s Exata = %d ", nome, qtd_exata);
    imprimir_vertices(exata, n);
    printf(" | Gulosa = %d ", qtd_gulosa);
    imprimir_vertices(gulosa, n);
    printf(" | %s\n", passou ? "OK" : "FALHOU");

    free(exata);
    free(gulosa);
    liberar_grafo_lista(g);
    return passou;
}

// Gerador deterministico de numeros para repetir o mesmo experimento.
static uint32_t proximo_aleatorio(uint32_t *estado) {
    *estado = *estado * UINT32_C(1664525) + UINT32_C(1013904223);
    return *estado;
}

static GrafoLista *gerar_grafo_experimento(int n) {
    GrafoLista *g = criar_grafo_lista(n);
    if (g == NULL) return NULL;
    // Um ciclo assegura uma base conectada. Demais arestas sao reproduziveis.
    for (int v = 0; v < n; v++) {
        if (!inserir_aresta_lista(g, v, (v + 1) % n)) {
            liberar_grafo_lista(g);
            return NULL;
        }
    }
    uint32_t estado = UINT32_C(20260909) + (uint32_t)n;
    for (int u = 0; u < n; u++) {
        for (int v = u + 1; v < n; v++) {
            if (proximo_aleatorio(&estado) % 100u < 30u &&
                !inserir_aresta_lista(g, u, v)) {
                liberar_grafo_lista(g);
                return NULL;
            }
        }
    }
    return g;
}

// Media do tempo de CPU por execucao, em milissegundos.
// Repete ate acumular cerca de 30 ms: uma unica chamada seria curta demais.
static double medir_tempo(const GrafoLista *g, int usar_backtracking,
                          int *resultado) {
    int *selecionado = malloc((size_t)g->n * sizeof(*selecionado));
    if (selecionado == NULL) return -1.0;

    const double minimo_segundos = 0.03;
    const int max_repeticoes = 100000;
    int repeticoes = 0;
    int quantidade = -1;
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) {
        free(selecionado);
        return -1.0;
    }
    double segundos = 0.0;
    do {
        quantidade = usar_backtracking
            ? cobertura_minima_bt(g, selecionado)
            : cobertura_gulosa(g, selecionado);
        if (quantidade < 0 || !eh_cobertura(g, selecionado)) {
            free(selecionado);
            return -1.0;
        }
        repeticoes++;
        clock_t fim = clock();
        if (fim == (clock_t)-1) {
            free(selecionado);
            return -1.0;
        }
        segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;
    } while (segundos < minimo_segundos && repeticoes < max_repeticoes);

    *resultado = quantidade;
    free(selecionado);
    return (segundos * 1000.0) / repeticoes;
}

static int experimento(void) {
    const int tamanhos[] = {5, 8, 10, 12, 15};
    printf("\n=== Experimento: tempo medio por execucao ===\n");
    printf("Grafo: ciclo + arestas adicionais pseudoaleatorias (semente fixa)\n");
    printf("%5s %6s %8s %8s %14s %14s\n",
           "n", "arestas", "otimo", "gulosa", "BT (ms)", "Gulosa (ms)");

    for (size_t i = 0; i < sizeof(tamanhos) / sizeof(tamanhos[0]); i++) {
        GrafoLista *g = gerar_grafo_experimento(tamanhos[i]);
        if (g == NULL) return 0;
        int qtd_exata = -1, qtd_gulosa = -1;
        double tempo_bt = medir_tempo(g, 1, &qtd_exata);
        double tempo_gulosa = medir_tempo(g, 0, &qtd_gulosa);
        if (tempo_bt < 0 || tempo_gulosa < 0 || qtd_exata > qtd_gulosa) {
            liberar_grafo_lista(g);
            return 0;
        }
        printf("%5d %6d %8d %8d %14.6f %14.6f\n",
               g->n, g->m, qtd_exata, qtd_gulosa, tempo_bt, tempo_gulosa);
        liberar_grafo_lista(g);
    }
    printf("Obs.: tempos variam conforme o computador e os grafos.\n");
    return 1;
}

int main(void) {
    const int estrela[][2] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}};
    const int triangulo[][2] = {{0, 1}, {1, 2}, {2, 0}};
    const int caminho[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    const int ciclo5[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}};
    const int com_laco[][2] = {{0, 0}, {1, 2}};
    const int exemplo_gulosa[][2] = {{0, 1}, {0, 2}, {1, 4}, {2, 3}};

    printf("=== Pratica 09: Cobertura de Vertices ===\n\n");
    int ok = 1;
    ok &= testar_caso("Estrela", 6, estrela, 5, 1);
    ok &= testar_caso("Triangulo", 3, triangulo, 3, 2);
    ok &= testar_caso("Caminho de 5 vertices", 5, caminho, 4, 2);
    ok &= testar_caso("Ciclo impar C5", 5, ciclo5, 5, 3);
    ok &= testar_caso("Sem arestas", 5, NULL, 0, 0);
    ok &= testar_caso("Laco e aresta separada", 3, com_laco, 2, 2);
    ok &= testar_caso("Gulosa pode ser pior", 5, exemplo_gulosa, 4, 2);

    if (ok) ok = experimento();
    printf("\nResultado final: %s\n", ok ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHA");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
