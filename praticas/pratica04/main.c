#include <stdio.h>
#include <stdlib.h>
#include "conectividade.h"
#include "planaridade.h"

int main() {
    printf("=== Testes da Pratica 04 ===\n\n");

    // Grafo 1: Teste de Articulações e Pontes (Grafo "Haltere")
    GrafoLista *g1 = criar_grafo_lista(6);
    inserir_aresta_lista(g1, 0, 1);
    inserir_aresta_lista(g1, 1, 2);
    inserir_aresta_lista(g1, 2, 0); // Triangulo 1
    
    inserir_aresta_lista(g1, 2, 3); // Ponte e articulações (2 e 3)
    
    inserir_aresta_lista(g1, 3, 4);
    inserir_aresta_lista(g1, 4, 5);
    inserir_aresta_lista(g1, 5, 3); // Triangulo 2

    printf("--- Grafo 1 (Conectividade) ---\n");
    encontrar_articulacoes(g1);
    detectar_pontes(g1);
    printf("\n");

    // Grafo 2: K5 - Não Planar
    GrafoLista *k5 = criar_grafo_lista(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            inserir_aresta_lista(k5, i, j);
        }
    }

    printf("--- Grafo 2 (K5 - Planaridade) ---\n");
    printf("Formula de Euler (m <= 3n - 6): %s\n", eh_planar_euler(k5) ? "Passou" : "Falhou");
    printf("Heuristica de Kuratowski: %s\n", verifica_kuratowski(k5) ? "Planar" : "Nao Planar");
    printf("\n");

    // Grafo 3: K3,3 - Não Planar
    GrafoLista *k33 = criar_grafo_lista(6);
    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            inserir_aresta_lista(k33, i, j);
        }
    }

    printf("--- Grafo 3 (K3,3 - Planaridade) ---\n");
    printf("Formula de Euler (m <= 3n - 6): %s (Nota: K3,3 passa em Euler, falha em Kuratowski)\n", eh_planar_euler(k33) ? "Passou" : "Falhou");
    printf("Heuristica de Kuratowski: %s\n", verifica_kuratowski(k33) ? "Planar" : "Nao Planar");
    
    liberar_grafo_lista(g1);
    liberar_grafo_lista(k5);
    liberar_grafo_lista(k33);

    return 0;
}