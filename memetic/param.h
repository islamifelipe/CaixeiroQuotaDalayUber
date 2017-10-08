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
#define POPSIZE 100 //tamanho da populaçao de cromossomos
#define SIZEREPOSITORIO (POPSIZE*3)

#define ALFA 2.2//2.2 // valor para determinar a quantidade de vértices de uma soluçao inicial

/*Parâmetros do GA*/
#define TAXADECRUZAMENTO 0.98  // 98% por geraçao
#define TAXAMUTACAO_bonus 0.4 // 10% por geraçao
#define TAXAMUTACAO_addCity 0.22 // 10% por geraçao
#define TAXAMUTACAO_removeCtiy 0.33 // 10% por geraçao
#define QUANTGERACOES 30 // quantidade de iteracoes do M-GA

#define TAXAEXECUCAO_SA 0.9//0.9

/*Parâmetros para o Simulated Annealing*/
#define TEMPERATURA 5//6//11 // vai sendo dividido por FATORDECAIMENTO
#define FATORDECAIMENTO 1.3
#define FATORDECAIMENTOITERACAO 1.2
#define ITERACAO_K 9 //11 // vai sendo multiplicado por FATORDECAIMENTOITERACAO
#define ITARACAO_LS 15//15//
// O algoritmo para quando a temperatura for menor que 1
// quanto menor a temperatura, maior a quantidade de iteraçoes


/*MELHORES PARAMETROS PARA O SA:

#define TEMPERATURA 19 <-> 21 // vai sendo dividido por FATORDECAIMENTO
#define FATORDECAIMENTO 1.5
#define FATORDECAIMENTOITERACAO 1.5
#define ITERACAO_K 10

#define TEMPERATURA 6//11 // vai sendo dividido por FATORDECAIMENTO
#define FATORDECAIMENTO 1.2
#define FATORDECAIMENTOITERACAO 1.2
#define ITERACAO_K 9

#define TEMPERATURA 5//6//11 // vai sendo dividido por FATORDECAIMENTO
#define FATORDECAIMENTO 1.3
#define FATORDECAIMENTOITERACAO 1.2
#define ITERACAO_K 9 //11 // vai sendo multiplicado por FATORDECAIMENTOITERACAO
#define ITARACAO_LS 15//15//


*/


#endif


