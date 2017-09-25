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


#define EPS 1e-9 // quanquer coisa menor que esse valor, é considerado 0

#define MAX_N 1000 // quantidade maxima de vértices

/*parâmetros dos cromossomos*/
#define POPSIZE 20 //tamanho da populaçao de cromossomos

/*Parâmetros do GA*/
#define TAXADECRUZAMENTO 0.97  // 98% por geraçao
#define TAXADEMUTACAO 0.1 // 10% por geraçao
#define QUANTGERACOES 30 // quantidade de iteracoes do M-GA

/*Parâmetros para o Simulated Annealing*/
#define TEMPERATURA 30 // vai sendo dividido por FATORDECAIMENTO
#define FATORDECAIMENTO 1.8
#define FATORDECAIMENTOITERACAO 1.8
#define ITERACAO_K 15 // vai sendo multiplicado por FATORDECAIMENTOITERACAO
// O algoritmo para quando a temperatura for menor que 1
// quanto menor a temperatura, maior a quantidade de iteraçoes



#endif


