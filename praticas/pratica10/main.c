#include "hamiltoniano.h"
#include "tps.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void imprimir_sequencia(const int *seq, int n, int fechar) {
    for (int i = 0; i < n; i++) printf("%s%d", i ? " -> " : "", seq[i]);
    if (fechar) printf(" -> %d", seq[0]);
}

// ------------------------------------------------- Caminho/Ciclo Hamiltoniano

// Alem de comparar com o esperado, confere a reducao para o TSP (n >= 3):
// existe ciclo hamiltoniano <=> custo otimo do TSP reduzido == n.
static int testar_hamiltoniano(const char *nome, int n, const int arestas[][2],
                               size_t m, int espera_caminho, int espera_ciclo) {
    GrafoLista *g = criar_grafo_lista(n);
    if (g == NULL) return 0;
    for (size_t i = 0; i < m; i++) {
        if (!inserir_aresta_lista(g, arestas[i][0], arestas[i][1])) {
            liberar_grafo_lista(g);
            return 0;
        }
    }

    int *caminho = calloc((size_t)n, sizeof(*caminho));
    int *ciclo = calloc((size_t)n, sizeof(*ciclo));
    int *rota = calloc((size_t)n, sizeof(*rota));
    if (caminho == NULL || ciclo == NULL || rota == NULL) {
        free(caminho);
        free(ciclo);
        free(rota);
        liberar_grafo_lista(g);
        return 0;
    }

    int achou_caminho = encontrar_caminho_hamiltoniano_bt(g, caminho);
    int achou_ciclo = encontrar_ciclo_hamiltoniano_bt(g, ciclo);
    int passou = achou_caminho == espera_caminho && achou_ciclo == espera_ciclo;
    if (achou_caminho == 1 && !eh_caminho_hamiltoniano(g, caminho)) passou = 0;
    if (achou_ciclo == 1 && !eh_ciclo_hamiltoniano(g, ciclo)) passou = 0;

    long long otimo = -1;
    if (n >= 3) {
        MatrizTSP *t = grafo_para_tsp(g);
        otimo = t == NULL ? -1 : tsp_forca_bruta(t, rota);
        if (otimo < 0 || (otimo == n) != (espera_ciclo == 1)) passou = 0;
        liberar_matriz_tsp(t);
    }

    printf("%-24s Caminho: %-3s", nome, achou_caminho == 1 ? "sim" : "nao");
    if (achou_caminho == 1) {
        printf(" [");
        imprimir_sequencia(caminho, n, 0);
        printf("]");
    }
    printf("\n%-24s Ciclo:   %-3s", "", achou_ciclo == 1 ? "sim" : "nao");
    if (achou_ciclo == 1) {
        printf(" [");
        imprimir_sequencia(ciclo, n, 1);
        printf("]");
    }
    if (n >= 3) printf("\n%-24s TSP reduzido: otimo = %lld (n = %d)", "", otimo, n);
    printf("  -> %s\n", passou ? "OK" : "FALHOU");

    free(caminho);
    free(ciclo);
    free(rota);
    liberar_grafo_lista(g);
    return passou;
}

// ------------------------------------------------------------------------ TSP

static int verificar_rota(const MatrizTSP *m, const int *rota, long long custo) {
    return custo >= 0 && custo_rota(m, rota) == custo;
}

static int testar_tsp_conhecido(void) {
    // Instancia classica de 4 cidades; o otimo e 80 (0-1-3-2-0).
    const int dist[4][4] = {{0, 10, 15, 20},
                            {10, 0, 35, 25},
                            {15, 35, 0, 30},
                            {20, 25, 30, 0}};
    MatrizTSP *m = criar_matriz_tsp(4);
    if (m == NULL) return 0;
    for (int u = 0; u < 4; u++)
        for (int v = u + 1; v < 4; v++) definir_distancia(m, u, v, dist[u][v]);

    int rb[4], rv[4], ri[4];
    long long cb = tsp_forca_bruta(m, rb);
    long long cv = tsp_vizinho_mais_proximo(m, 0, rv);
    long long ci = tsp_insercao(m, ri);
    int passou = cb == 80 && cv >= cb && ci >= cb &&
                 verificar_rota(m, rb, cb) && verificar_rota(m, rv, cv) &&
                 verificar_rota(m, ri, ci);

    printf("%-24s Bruta = %lld [", "4 cidades (classico)", cb);
    imprimir_sequencia(rb, 4, 1);
    printf("] | Vizinho = %lld | Insercao = %lld -> %s\n",
           cv, ci, passou ? "OK" : "FALHOU");
    liberar_matriz_tsp(m);
    return passou;
}

static int testar_tsp_pequeno(void) {
    int passou = 1;

    // n = 1: custo 0. n = 2: ida e volta.
    MatrizTSP *um = criar_matriz_tsp(1);
    MatrizTSP *dois = criar_matriz_tsp(2);
    if (um == NULL || dois == NULL) {
        liberar_matriz_tsp(um);
        liberar_matriz_tsp(dois);
        return 0;
    }
    definir_distancia(dois, 0, 1, 7);

    int r1[1], r2b[2], r2v[2], r2i[2];
    passou &= tsp_forca_bruta(um, r1) == 0;
    passou &= tsp_vizinho_mais_proximo(um, 0, r1) == 0;
    passou &= tsp_insercao(um, r1) == 0;
    passou &= tsp_forca_bruta(dois, r2b) == 14;
    passou &= tsp_vizinho_mais_proximo(dois, 0, r2v) == 14;
    passou &= tsp_insercao(dois, r2i) == 14;
    // Parametros invalidos.
    passou &= tsp_forca_bruta(NULL, r1) == -1;
    passou &= tsp_vizinho_mais_proximo(dois, 5, r2v) == -1;
    passou &= definir_distancia(dois, 0, 0, 1) == 0;
    passou &= definir_distancia(dois, 0, 1, -3) == 0;

    printf("%-24s n=1 -> custo 0; n=2 -> custo 14; entradas invalidas -> %s\n",
           "Casos de borda", passou ? "OK" : "FALHOU");
    liberar_matriz_tsp(um);
    liberar_matriz_tsp(dois);
    return passou;
}

// Gerador deterministico de numeros para repetir o mesmo experimento.
static uint32_t proximo_aleatorio(uint32_t *estado) {
    *estado = *estado * UINT32_C(1664525) + UINT32_C(1013904223);
    return *estado;
}

// n cidades em uma grade 100 x 100; distancia de Manhattan (inteira).
static MatrizTSP *gerar_instancia(int n) {
    MatrizTSP *m = criar_matriz_tsp(n);
    int *x = malloc((size_t)n * sizeof(*x));
    int *y = malloc((size_t)n * sizeof(*y));
    if (m == NULL || x == NULL || y == NULL) {
        liberar_matriz_tsp(m);
        free(x);
        free(y);
        return NULL;
    }

    uint32_t estado = UINT32_C(20261010) + (uint32_t)n;
    for (int i = 0; i < n; i++) {
        x[i] = (int)((proximo_aleatorio(&estado) >> 16) % 100u);
        y[i] = (int)((proximo_aleatorio(&estado) >> 16) % 100u);
    }
    for (int u = 0; u < n; u++)
        for (int v = u + 1; v < n; v++)
            definir_distancia(m, u, v, abs(x[u] - x[v]) + abs(y[u] - y[v]));

    free(x);
    free(y);
    return m;
}

typedef long long (*Algoritmo)(const MatrizTSP *, int *);

static long long executar_bruta(const MatrizTSP *m, int *rota) {
    return tsp_forca_bruta(m, rota);
}

static long long executar_vizinho(const MatrizTSP *m, int *rota) {
    return tsp_vizinho_mais_proximo(m, 0, rota);
}

static long long executar_insercao(const MatrizTSP *m, int *rota) {
    return tsp_insercao(m, rota);
}

// Media do tempo de CPU por execucao, em milissegundos.
// Repete ate acumular cerca de 30 ms: uma unica chamada seria curta demais.
static double medir_tempo(const MatrizTSP *m, Algoritmo algoritmo,
                          long long *custo) {
    int *rota = malloc((size_t)m->n * sizeof(*rota));
    if (rota == NULL) return -1.0;

    const double minimo_segundos = 0.03;
    const int max_repeticoes = 100000;
    int repeticoes = 0;
    long long resultado = -1;
    clock_t inicio = clock();
    if (inicio == (clock_t)-1) {
        free(rota);
        return -1.0;
    }
    double segundos = 0.0;
    do {
        resultado = algoritmo(m, rota);
        if (!verificar_rota(m, rota, resultado)) {
            free(rota);
            return -1.0;
        }
        repeticoes++;
        clock_t fim = clock();
        if (fim == (clock_t)-1) {
            free(rota);
            return -1.0;
        }
        segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;
    } while (segundos < minimo_segundos && repeticoes < max_repeticoes);

    *custo = resultado;
    free(rota);
    return (segundos * 1000.0) / repeticoes;
}

static double desvio_percentual(long long custo, long long otimo) {
    if (otimo <= 0) return 0.0;
    return (double)(custo - otimo) * 100.0 / (double)otimo;
}

static int experimento(void) {
    const int tamanhos[] = {5, 8, 10, 12};
    printf("\n=== Experimento: custo e tempo medio por execucao ===\n");
    printf("Cidades aleatorias (semente fixa) em grade 100x100, distancia de Manhattan\n");
    printf("%3s | %6s %6s %6s | %7s %7s | %12s %12s %12s\n",
           "n", "otimo", "vizinho", "inserc.", "viz.(%)", "ins.(%)",
           "Bruta (ms)", "Vizinho (ms)", "Insercao (ms)");

    for (size_t i = 0; i < sizeof(tamanhos) / sizeof(tamanhos[0]); i++) {
        MatrizTSP *m = gerar_instancia(tamanhos[i]);
        if (m == NULL) return 0;

        long long custo_bruta = -1, custo_vizinho = -1, custo_insercao = -1;
        double t_bruta = medir_tempo(m, executar_bruta, &custo_bruta);
        double t_vizinho = medir_tempo(m, executar_vizinho, &custo_vizinho);
        double t_insercao = medir_tempo(m, executar_insercao, &custo_insercao);

        // O exato nunca pode perder para uma heuristica.
        if (t_bruta < 0 || t_vizinho < 0 || t_insercao < 0 ||
            custo_bruta > custo_vizinho || custo_bruta > custo_insercao) {
            liberar_matriz_tsp(m);
            return 0;
        }
        printf("%3d | %6lld %6lld %6lld | %7.1f %7.1f | %12.6f %12.6f %12.6f\n",
               m->n, custo_bruta, custo_vizinho, custo_insercao,
               desvio_percentual(custo_vizinho, custo_bruta),
               desvio_percentual(custo_insercao, custo_bruta),
               t_bruta, t_vizinho, t_insercao);
        liberar_matriz_tsp(m);
    }
    printf("Obs.: tempos variam conforme o computador; a forca bruta cresce como (n-1)!.\n");
    return 1;
}

int main(void) {
    const int ciclo5[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}};
    const int caminho5[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}};
    const int estrela4[][2] = {{0, 1}, {0, 2}, {0, 3}};
    const int k4[][2] = {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};
    const int dois_triangulos[][2] = {{0, 1}, {1, 2}, {2, 0},
                                      {3, 4}, {4, 5}, {5, 3}};
    const int aresta[][2] = {{0, 1}};
    // Grafo de Petersen: tem caminho hamiltoniano, mas nao tem ciclo.
    const int petersen[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0},
                               {0, 5}, {1, 6}, {2, 7}, {3, 8}, {4, 9},
                               {5, 7}, {7, 9}, {9, 6}, {6, 8}, {8, 5}};

    printf("=== Pratica 10: Caminho Hamiltoniano e TSP ===\n");
    printf("\n--- Caminho e Ciclo Hamiltoniano (backtracking) ---\n");
    int ok = 1;
    ok &= testar_hamiltoniano("Ciclo C5", 5, ciclo5, 5, 1, 1);
    ok &= testar_hamiltoniano("Caminho de 5 vertices", 5, caminho5, 4, 1, 0);
    ok &= testar_hamiltoniano("Estrela K1,3", 4, estrela4, 3, 0, 0);
    ok &= testar_hamiltoniano("Completo K4", 4, k4, 6, 1, 1);
    ok &= testar_hamiltoniano("Dois triangulos", 6, dois_triangulos, 6, 0, 0);
    ok &= testar_hamiltoniano("Grafo de Petersen", 10, petersen, 15, 1, 0);
    ok &= testar_hamiltoniano("Uma aresta (n=2)", 2, aresta, 1, 1, 0);
    ok &= testar_hamiltoniano("Vertice unico", 1, NULL, 0, 1, 0);

    printf("\n--- TSP ---\n");
    ok &= testar_tsp_conhecido();
    ok &= testar_tsp_pequeno();

    if (ok) ok = experimento();
    printf("\nResultado final: %s\n", ok ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHA");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
