#include <stdio.h>
#include <stdlib.h>
#include "planaridade.h"

int contar_arestas(GrafoLista *g) {
    int m = 0;
    for (int i = 0; i < g->n; i++) {
        No *atual = g->adj[i];
        while (atual != NULL) {
            m++;
            atual = atual->prox;
        }
    }
    return m / 2; // Grafo não direcionado conta em dobro
}

int eh_planar_euler(GrafoLista *g) {
    if (g->n < 3) return 1; // Grafos pequenos são planares
    
    int m = contar_arestas(g);
    // Fórmula de Euler: m <= 3n - 6
    if (m > 3 * g->n - 6) {
        return 0; // Viola a desigualdade
    }
    return 1;
}

static int sao_adjacentes(GrafoLista *g, int u, int v) {
    No *atual = g->adj[u];
    while (atual != NULL) {
        if (atual->destino == v) return 1;
        atual = atual->prox;
    }
    return 0;
}

// Heurística força bruta simplificada para subgrafos K5 ou K3,3 em n <= 10
int verifica_kuratowski(GrafoLista *g) {
    if (g->n > 10) return 1; // Apenas heurística para grafos pequenos
    
    int n = g->n;
    
    // Testa possível K5 (5 vértices totalmente conectados entre si)
    if (n >= 5) {
        for(int a=0; a<n-4; a++)
        for(int b=a+1; b<n-3; b++)
        for(int c=b+1; c<n-2; c++)
        for(int d=c+1; d<n-1; d++)
        for(int e=d+1; e<n; e++) {
            if(sao_adjacentes(g,a,b) && sao_adjacentes(g,a,c) && sao_adjacentes(g,a,d) && sao_adjacentes(g,a,e) &&
               sao_adjacentes(g,b,c) && sao_adjacentes(g,b,d) && sao_adjacentes(g,b,e) &&
               sao_adjacentes(g,c,d) && sao_adjacentes(g,c,e) &&
               sao_adjacentes(g,d,e)) {
                return 0; // Contém um K5, não planar
            }
        }
    }
    
    // Testa possível K3,3 (Bipartido completo 3x3)
    if (n >= 6) {
        for(int a=0; a<n-5; a++)
        for(int b=a+1; b<n-4; b++)
        for(int c=b+1; c<n-3; c++)
        for(int x=c+1; x<n-2; x++)
        for(int y=x+1; y<n-1; y++)
        for(int z=y+1; z<n; z++) {
            if(sao_adjacentes(g,a,x) && sao_adjacentes(g,a,y) && sao_adjacentes(g,a,z) &&
               sao_adjacentes(g,b,x) && sao_adjacentes(g,b,y) && sao_adjacentes(g,b,z) &&
               sao_adjacentes(g,c,x) && sao_adjacentes(g,c,y) && sao_adjacentes(g,c,z)) {
                return 0; // Contém um K3,3, não planar
            }
        }
    }
    
    return 1; // Passou na heurística
}