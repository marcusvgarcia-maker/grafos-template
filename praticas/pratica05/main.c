#include <stdio.h>
#include <stdlib.h>
#include "coloracao.h"

void imprimir_cores(int *cores, int n, int num_cores, const char *metodo) {
    printf("--- %s ---\n", metodo);
    printf("Numero cromatico estimado: %d\n", num_cores);
    for (int i = 0; i < n; i++) {
        printf("Vertice %d: Cor %d\n", i, cores[i]);
    }
    printf("\n");
}

int main() {
    printf("=== Testes da Pratica 05 ===\n\n");

    // Grafo 1: Grafo Bipartido (Ciclo de 4 vértices)
    GrafoLista *g1 = criar_grafo_lista(4);
    inserir_aresta_lista(g1, 0, 1);
    inserir_aresta_lista(g1, 1, 2);
    inserir_aresta_lista(g1, 2, 3);
    inserir_aresta_lista(g1, 3, 0);

    printf("Testando Grafo 1 (Ciclo Par C4)\n");
    printf("Eh bipartido (2-coloracao)? %s\n\n", eh_bipartido(g1) ? "Sim" : "Nao");

    int num_cores_g1 = 0;
    int *cores_gulosa_g1 = coloracao_gulosa(g1, &num_cores_g1);
    imprimir_cores(cores_gulosa_g1, g1->n, num_cores_g1, "Coloracao Gulosa");

    int num_cores_wp_g1 = 0;
    int *cores_wp_g1 = coloracao_welsh_powell(g1, &num_cores_wp_g1);
    imprimir_cores(cores_wp_g1, g1->n, num_cores_wp_g1, "Coloracao Welsh-Powell");

    free(cores_gulosa_g1);
    free(cores_wp_g1);
    liberar_grafo_lista(g1);

    // Grafo 2: Grafo Estrela modificado (Diferença entre Guloso e WP)
    GrafoLista *g2 = criar_grafo_lista(6);
    inserir_aresta_lista(g2, 0, 1);
    inserir_aresta_lista(g2, 0, 2);
    inserir_aresta_lista(g2, 0, 3);
    inserir_aresta_lista(g2, 0, 4);
    inserir_aresta_lista(g2, 0, 5); // Vértice 0 tem grau máximo

    printf("Testando Grafo 2 (Estrela)\n");
    printf("Eh bipartido (2-coloracao)? %s\n\n", eh_bipartido(g2) ? "Sim" : "Nao");

    int num_cores_g2 = 0;
    int *cores_gulosa_g2 = coloracao_gulosa(g2, &num_cores_g2);
    imprimir_cores(cores_gulosa_g2, g2->n, num_cores_g2, "Coloracao Gulosa");

    int num_cores_wp_g2 = 0;
    int *cores_wp_g2 = coloracao_welsh_powell(g2, &num_cores_wp_g2);
    imprimir_cores(cores_wp_g2, g2->n, num_cores_wp_g2, "Coloracao Welsh-Powell");

    free(cores_gulosa_g2);
    free(cores_wp_g2);
    liberar_grafo_lista(g2);

    return 0;
}