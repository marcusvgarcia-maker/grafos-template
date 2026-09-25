#include <stdio.h>
#include <stdlib.h>
#include "busca_largura.h"
#include "busca_profundidade.h"

int main() {
    printf("=== Testes da Pratica 02 ===\n\n");
    
    // Grafo 1: Componentes, Biparticao e BFS
    GrafoLista *g1 = criar_grafo_lista(5);
    inserir_aresta_lista(g1, 0, 1);
    inserir_aresta_lista(g1, 1, 2);
    inserir_aresta_lista(g1, 3, 4); // Grafo desconexo (2 componentes)
    
    printf("1. Testando Componentes Conexos:\n");
    printf("Numero de componentes: %d (Esperado: 2)\n\n", contar_componentes(g1));
    
    printf("2. Testando Ciclos:\n");
    printf("Tem ciclo? %s (Esperado: Nao)\n\n", tem_ciclo(g1) ? "Sim" : "Nao");
    
    printf("3. Testando Biparticao:\n");
    printf("Eh bipartido? %s (Esperado: Sim)\n\n", eh_bipartido(g1) ? "Sim" : "Nao");
    
    printf("4. Testando BFS a partir do vertice 0:\n");
    int dist[5], pred[5];
    bfs(g1, 0, dist, pred);
    for (int i = 0; i < 3; i++) {
        printf("Vertice %d -> Distancia: %d, Predecessor: %d\n", i, dist[i], pred[i]);
    }
    
    liberar_grafo_lista(g1);
    
    // Grafo 2: Com Ciclo e Nao Bipartido
    printf("\n--- Testes com Grafo Ciclico ---\n");
    GrafoLista *g2 = criar_grafo_lista(3);
    inserir_aresta_lista(g2, 0, 1);
    inserir_aresta_lista(g2, 1, 2);
    inserir_aresta_lista(g2, 2, 0); 
    
    printf("Tem ciclo? %s (Esperado: Sim)\n", tem_ciclo(g2) ? "Sim" : "Nao");
    printf("Eh bipartido? %s (Esperado: Nao)\n", eh_bipartido(g2) ? "Sim" : "Nao");
    
    liberar_grafo_lista(g2);
    
    return 0;
}