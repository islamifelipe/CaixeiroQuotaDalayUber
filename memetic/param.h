#ifndef PARAM_H
#define PARAM_H

#include "rand/randomc.h"
#include <vector>


extern int s; // vértice origem
extern int n; // quantidade de vértices
extern int l; // quantidade de passageiros
extern int R; // capacidade de carro
extern double K; // quota minima a ser coletada pelo caixeiro


extern double **custos; // matriz de custo das arestas
extern double **tempo; // matriz de tempo das arestas
extern double **L; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
extern double **penalidades; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
extern double **vertices; // matriz nx2 do bonus e delay de cada vértice 
extern std::vector<int> *passageirosPorVertice; // para cada i (vértice i), diz (a lista de) os passageiros que esparam embarcar em i


#define MAX_N 1000 // quantidade maxima de vértices

#define POPSIZE 1 //tamanho da populaçao de cromossomos

/*parâmetros dos cromossomos*/


#endif


