#include <stdio.h>
#include <stdlib.h>
#include "dag.h"

void imprimir_array(int* arr, int tamanho, const char* nome_metodo) {
    if (arr == NULL) {
        printf("[%s] Erro: Grafo possui ciclo! Nao e possivel ordenar.\n", nome_metodo);
        return;
    }
    printf("[%s] Ordenacao Topologica: ", nome_metodo);
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    printf("=== Teste 1: Grafo Aciclico (DAG) ===\n");
    GrafoLista* dag = criar_grafo_lista(6);
    // Exemplo clássico (0 e 1 sem dependências)
    inserir_aresta_direcionada(dag, 5, 2);
    inserir_aresta_direcionada(dag, 5, 0);
    inserir_aresta_direcionada(dag, 4, 0);
    inserir_aresta_direcionada(dag, 4, 1);
    inserir_aresta_direcionada(dag, 2, 3);
    inserir_aresta_direcionada(dag, 3, 1);

    if (eh_dag(dag)) {
        printf("eh_dag: O grafo 1 e um DAG.\n");
    }

    int tam_kahn = 0;
    int* ord_kahn = ordenacao_topologica_kahn(dag, &tam_kahn);
    imprimir_array(ord_kahn, tam_kahn, "KAHN");

    int tam_dfs = 0;
    int* ord_dfs = ordenacao_topologica_dfs(dag, &tam_dfs);
    imprimir_array(ord_dfs, tam_dfs, "DFS ");

    free(ord_kahn);
    free(ord_dfs);
    liberar_grafo_lista(dag);

    printf("\n=== Teste 2: Grafo com Ciclo ===\n");
    GrafoLista* ciclico = criar_grafo_lista(3);
    inserir_aresta_direcionada(ciclico, 0, 1);
    inserir_aresta_direcionada(ciclico, 1, 2);
    inserir_aresta_direcionada(ciclico, 2, 0); // Fechou o ciclo (0->1->2->0)

    if (!eh_dag(ciclico)) {
        printf("eh_dag: O grafo 2 possui ciclo.\n");
    }

    tam_kahn = 0;
    ord_kahn = ordenacao_topologica_kahn(ciclico, &tam_kahn);
    imprimir_array(ord_kahn, tam_kahn, "KAHN");

    tam_dfs = 0;
    ord_dfs = ordenacao_topologica_dfs(ciclico, &tam_dfs);
    imprimir_array(ord_dfs, tam_dfs, "DFS ");

    liberar_grafo_lista(ciclico);

    return 0;
}