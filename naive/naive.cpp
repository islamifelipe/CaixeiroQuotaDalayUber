#include "LKHParser.h"
#include <iostream>
#include <map> 
#include <string>
#include <climits>
#include <stack>   
#include <utility>      // std::pair
#include <cmath>
#include <list>
#include "gurobi_c++.h"
#include <sys/times.h>
#include <unistd.h>
using namespace std;


const string LKH_PATH = "LKH-2.0.7/";
const string LK_FILES_PATH = "LK_FILES/";

int main(int argc, char *argv[])
{

	int s = 0; // vértice origem
	int n; // quantidade de vértices
	int l; // quantidade de passageiros
	int R; // capacidade de carro
	double K; // quota minima a ser coletada pelo caixeiro


	//////////////////// LEITURA DA INSTÂNCIA ////////////////////
	vector<int> set_vertices;
	vector< vector<double> > custos_vector;
	cin>>n;
	cin>>l;
	cin>>R;

	double **custos = new double*[n]; // matriz de custo das arestas
	double **tempo = new double*[n]; // matriz de tempo das arestas
	double **L = new double*[l]; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
	double **penalidades = new double*[l]; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
	double **vertices = new double*[n]; // matriz nx2 do bonus e delay de cada vértice 


	for (int i=0; i<n; i++){ // inicializa e ler custos das arestas
		set_vertices.push_back(i);
		custos[i] = new double[n];
		vector<double> aulcv;
		for (int j=0; j<n; j++){
			cin>>custos[i][j];
			aulcv.push_back(custos[i][j]);
		}
		custos_vector.push_back(aulcv);
	
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
        L[i][2]--; /*a instância deve indicar a origem entre 1 e n*/
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


	LKHParser parser(LKH_PATH, LK_FILES_PATH, set_vertices, custos_vector, "augumacoisa", string("TSP"));
    vector<int> path = parser.LKHSolution();
    for (int i=0; i<path.size(); i++){
    	cout<<path[i]<<" ";
    }
    cout<<endl;
    
}
