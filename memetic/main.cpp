/*
#=======================================================================
# Islame Felipe DA COSTA FERNANDES --- Copyright 2017
#-----------------------------------------------------------------------
# This code implements a memetic algorithm for the traveling salesman problem with
# multi-rideshare, Quota, delay and incomplete landing (title not official)
# THIS CODE WORKS ONLY FOR COMPLETE GRAPHS
#=======================================================================
*/



#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sys/times.h>
#include <sys/types.h>
#include <iostream>

#include "cromossomo.h"
#include "param.h"

using namespace std;

int s; // vértice origem
int n; // quantidade de vértices
int l; // quantidade de passageiros
int R; // capacidade de carro
double K; // quota minima a ser coletada pelo caixeiro


double **custos; // matriz de custo das arestas
double **tempo; // matriz de tempo das arestas
double **L; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
double **penalidades; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
double **vertices; // matriz nx2 do bonus e delay de cada vértice 


void leituraDaInstancia(){
	//////////////////// LEITURA DA INSTÂNCIA ////////////////////
	
	s = 0; // vértice origem

	
	cin>>n;
	cin>>l;
	cin>>R;

	custos = new double*[n]; // matriz de custo das arestas
	tempo = new double*[n]; // matriz de tempo das arestas
	L = new double*[l]; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
	penalidades = new double*[l]; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
	vertices = new double*[n]; // matriz nx2 do bonus e delay de cada vértice 


	for (int i=0; i<n; i++){ // inicializa e ler custos das arestas
		custos[i] = new double[n];
		for (int j=0; j<n; j++){
			cin>>custos[i][j];
		}
	}
	for (int i=0; i<n; i++){ // inicializa e ler tempos das arestas
		tempo[i] = new double[n];
		for (int j=0; j<n; j++){
			cin>>tempo[i][j];
		}
	}
	for (int i=0; i<l; i++){
		L[i] = new double[4];
		for (int j=0; j<4; j++){
			cin>>L[i][j];
		}
        L[i][2]--; /*a instância fornecida indica a origem entre 1 e n, mas o probrama numera os vértices de 0 a n*/
        L[i][3]--; /*idem*/
	}

	for (int i=0; i<l; i++){
		penalidades[i] = new double[n];
		for (int j=0; j<n; j++){
			cin>>penalidades[i][j];
		}
	}

	cin>>K;
	int lixo;
	for (int i=0; i<n; i++){
		vertices[i] = new double[2];
		cin>>lixo; // somente para tirar o index do ultima matriz da instância
		for (int j=0; j<2; j++){
			cin>>vertices[i][j];
		}
	}

	//////////////////// FIM DA LEITURA DA INSTÂNCIA ////////////////////

}




int main(){




	return 0;
}