#include <stdio.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"

int main() {
    int n = 4; 

    printf("=== Testando Matriz de Adjacencia ===\n");
    GrafoMatriz* g_matriz = criar_grafo_matriz(n);
    inserir_aresta_matriz(g_matriz, 0, 1);
    inserir_aresta_matriz(g_matriz, 1, 2);
    inserir_aresta_matriz(g_matriz, 2, 3);
    
    printf("Grau do vertice 1: %d\n", grau_matriz(g_matriz, 1));
    printf("0 e 1 sao adjacentes? %d\n", sao_adjacentes_matriz(g_matriz, 0, 1));
    printf("0 e 2 sao adjacentes? %d\n", sao_adjacentes_matriz(g_matriz, 0, 2));
    
    remover_aresta_matriz(g_matriz, 0, 1);
    printf("Apos remocao, 0 e 1 sao adjacentes? %d\n", sao_adjacentes_matriz(g_matriz, 0, 1));
    
    liberar_grafo_matriz(g_matriz);


    printf("\n=== Testando Lista de Adjacencia ===\n");
    GrafoLista* g_lista = criar_grafo_lista(n);
    inserir_aresta_lista(g_lista, 0, 1);
    inserir_aresta_lista(g_lista, 1, 2);
    inserir_aresta_lista(g_lista, 2, 3);
    
    printf("Grau do vertice 1: %d\n", grau_lista(g_lista, 1));
    printf("0 e 1 sao adjacentes? %d\n", sao_adjacentes_lista(g_lista, 0, 1));
    printf("0 e 2 sao adjacentes? %d\n", sao_adjacentes_lista(g_lista, 0, 2));
    
    remover_aresta_lista(g_lista, 0, 1);
    printf("Apos remocao, 0 e 1 sao adjacentes? %d\n", sao_adjacentes_lista(g_lista, 0, 1));
    
    liberar_grafo_lista(g_lista);

    return 0;
}