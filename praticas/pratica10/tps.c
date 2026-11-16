#include "tps.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

MatrizTSP *criar_matriz_tsp(int n) {
    if (n <= 0) return NULL;

    MatrizTSP *m = malloc(sizeof(*m));
    if (m == NULL) return NULL;
    m->d = calloc((size_t)n * (size_t)n, sizeof(*m->d));
    if (m->d == NULL) {
        free(m);
        return NULL;
    }
    m->n = n;
    return m;
}

int definir_distancia(MatrizTSP *m, int u, int v, int dist) {
    if (m == NULL || u < 0 || v < 0 || u >= m->n || v >= m->n) return 0;
    if (u == v || dist < 0) return 0;
    m->d[u * m->n + v] = dist;
    m->d[v * m->n + u] = dist;
    return 1;
}

int obter_distancia(const MatrizTSP *m, int u, int v) {
    if (m == NULL || u < 0 || v < 0 || u >= m->n || v >= m->n) return -1;
    return m->d[u * m->n + v];
}

void liberar_matriz_tsp(MatrizTSP *m) {
    if (m == NULL) return;
    free(m->d);
    free(m);
}

MatrizTSP *grafo_para_tsp(const GrafoLista *g) {
    if (g == NULL) return NULL;
    MatrizTSP *m = criar_matriz_tsp(g->n);
    if (m == NULL) return NULL;

    // Comeca com peso 2 (aresta inexistente) e rebaixa para 1 onde ha aresta.
    for (int u = 0; u < g->n; u++)
        for (int v = u + 1; v < g->n; v++)
            definir_distancia(m, u, v, 2);

    for (int u = 0; u < g->n; u++)
        for (const No *a = g->adj[u]; a != NULL; a = a->prox)
            if (a->destino != u) definir_distancia(m, u, a->destino, 1);

    return m;
}

long long custo_rota(const MatrizTSP *m, const int *rota) {
    if (m == NULL || rota == NULL) return -1;
    int n = m->n;

    unsigned char *visto = calloc((size_t)n, sizeof(*visto));
    if (visto == NULL) return -1;

    long long custo = 0;
    int valida = 1;
    for (int i = 0; i < n && valida; i++) {
        int v = rota[i];
        if (v < 0 || v >= n || visto[v]) {
            valida = 0;
        } else {
            visto[v] = 1;
            if (i > 0) custo += m->d[rota[i - 1] * n + v];
        }
    }
    if (valida) custo += m->d[rota[n - 1] * n + rota[0]];

    free(visto);
    return valida ? custo : -1;
}

// ---------------------------------------------------------------- forca bruta

typedef struct {
    const MatrizTSP *m;
    int *atual;
    int *melhor_rota;
    long long melhor_custo;
} ForcaBruta;

static void trocar(int *v, int i, int j) {
    int t = v[i];
    v[i] = v[j];
    v[j] = t;
}

// Gera permutacoes trocando cada candidato para a posicao pos. O custo do
// trecho ja montado e acumulado, entao cada folha custa O(1) para avaliar.
// Nao ha poda: todas as (n-1)! rotas sao examinadas, como pede a atividade.
static void permutar(ForcaBruta *fb, int pos, long long parcial) {
    const MatrizTSP *m = fb->m;
    int n = m->n;
    int *atual = fb->atual;

    if (pos == n) {
        long long total = parcial + m->d[atual[n - 1] * n + atual[0]];
        if (total < fb->melhor_custo) {
            fb->melhor_custo = total;
            memcpy(fb->melhor_rota, atual, (size_t)n * sizeof(*atual));
        }
        return;
    }

    for (int i = pos; i < n; i++) {
        trocar(atual, pos, i);
        permutar(fb, pos + 1, parcial + m->d[atual[pos - 1] * n + atual[pos]]);
        trocar(atual, pos, i);
    }
}

long long tsp_forca_bruta(const MatrizTSP *m, int *rota) {
    if (m == NULL || rota == NULL) return -1;
    int n = m->n;

    int *atual = malloc((size_t)n * sizeof(*atual));
    if (atual == NULL) return -1;
    for (int i = 0; i < n; i++) atual[i] = i;

    // Origem fixa em 0: o ciclo e o mesmo para qualquer rotacao.
    ForcaBruta fb = {m, atual, rota, LLONG_MAX};
    permutar(&fb, 1, 0);

    free(atual);
    return fb.melhor_custo;
}

// ------------------------------------------------------ vizinho mais proximo

long long tsp_vizinho_mais_proximo(const MatrizTSP *m, int origem, int *rota) {
    if (m == NULL || rota == NULL || origem < 0 || origem >= m->n) return -1;
    int n = m->n;

    unsigned char *usado = calloc((size_t)n, sizeof(*usado));
    if (usado == NULL) return -1;

    long long custo = 0;
    int atual = origem;
    rota[0] = origem;
    usado[origem] = 1;

    for (int pos = 1; pos < n; pos++) {
        int proximo = -1;
        for (int v = 0; v < n; v++) {
            if (usado[v]) continue;
            if (proximo == -1 ||
                m->d[atual * n + v] < m->d[atual * n + proximo])
                proximo = v;
        }
        rota[pos] = proximo;
        usado[proximo] = 1;
        custo += m->d[atual * n + proximo];
        atual = proximo;
    }

    custo += m->d[atual * n + origem];  // Fecha o ciclo.
    free(usado);
    return custo;
}

// ------------------------------------------------------------------- insercao

long long tsp_insercao(const MatrizTSP *m, int *rota) {
    if (m == NULL || rota == NULL) return -1;
    int n = m->n;

    unsigned char *usado = calloc((size_t)n, sizeof(*usado));
    if (usado == NULL) return -1;

    rota[0] = 0;
    usado[0] = 1;
    int tamanho = 1;

    while (tamanho < n) {
        // 1) Vertice fora do ciclo mais proximo de algum vertice do ciclo.
        int escolhido = -1;
        int menor_distancia = 0;
        for (int v = 0; v < n; v++) {
            if (usado[v]) continue;
            int dist = INT_MAX;
            for (int i = 0; i < tamanho; i++) {
                int d = m->d[rota[i] * n + v];
                if (d < dist) dist = d;
            }
            if (escolhido == -1 || dist < menor_distancia) {
                escolhido = v;
                menor_distancia = dist;
            }
        }

        // 2) Aresta (a,b) do ciclo cuja substituicao por a->k->b custa menos.
        int melhor_pos = 1;
        long long menor_aumento = LLONG_MAX;
        for (int i = 0; i < tamanho; i++) {
            int a = rota[i];
            int b = rota[(i + 1) % tamanho];
            long long aumento = (long long)m->d[a * n + escolhido] +
                                m->d[escolhido * n + b] - m->d[a * n + b];
            if (aumento < menor_aumento) {
                menor_aumento = aumento;
                melhor_pos = i + 1;
            }
        }

        // 3) Abre espaco e insere o vertice apos a posicao escolhida.
        for (int j = tamanho; j > melhor_pos; j--) rota[j] = rota[j - 1];
        rota[melhor_pos] = escolhido;
        usado[escolhido] = 1;
        tamanho++;
    }

    free(usado);
    return custo_rota(m, rota);
}
