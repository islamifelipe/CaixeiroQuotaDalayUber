#ifndef POPULACAOINICIAL
#define POPULACAOINICIAL


#include "rand/randomc.h"
#include "param.h"

#include <vector>
#include <iostream>
using namespace std;


int comprimentoMinimo; // diz o comprimento minimo do cromossomo, tal que cromossomos menores que isso violam a restriçao da quota
/*Usado em outros procedimentos para verticiar a condiçao básica de cromossmo inválido*/

/*O comprimento do cromossomo é variável*/
/*O comprimento do cromossomo |C| é sorteado, 
onde |C| deve ser maior ou igual ao mínimo de vértices necessarios para adquirir a quota mínima K*/

bool compare(int v1, int v2){
	return vertices[v1][0]>vertices[v2][0]; // ordena por bonus
}

/*retorna um subconjunto randômico de nv vértices 
Mas este subconnjunto deve conter o vértice inicial s*/
bool getSubsetKVertices(int nv, int vert[MAX_N], TRandomMersenne &rg){
	double sum = 0;
	int tentativaCont = 0, index, v;
	std::vector<int> amostral;
	vert[0] = s;
	do {
		for (int i=0; i<n; i++) 
			if (i!=s) amostral.push_back(i);
		sum = 0;
		for (int i=1; i<nv; i++){ // sorteia-se nv-1 (o vértice s ja está no vetor final) vértices
			index = rg.IRandom(0,amostral.size()-1);
			v = amostral[index];//sorteia um vértice;
			sum += vertices[v][0];
			vert[i] = v;
			amostral.erase(amostral.begin()+index);
		}
		if (sum>=K) break;
		tentativaCont++;
	}while (tentativaCont<3); // tenta-se, no maximo 3 vezes conseguir um conjunto com nv vertices cuja soma de bonus é maior ou igual a K
	
	if (sum<K){ return false;
	} else {
		cout<<"Individuo tamanho "<<nv<<" gerado aleatoriamente com "<<tentativaCont<<" tentativas"<<endl;
		return true;
	}
}

void getKliqueK(){

}

void populacaoInicial(TRandomMersenne &rg){
	std::vector<int> verticesOrdenados; // gurda os vértices ordenados por bonus
	for (int i=0; i<n; i++)verticesOrdenados.push_back(i); // o i-ésimo vértice
	sort(verticesOrdenados.begin(),verticesOrdenados.end(), compare);
	comprimentoMinimo=0;
	double sumK=0;
	for (int i=0; i<n && sumK<K; i++) {
		sumK+=vertices[verticesOrdenados[i]][0];
		comprimentoMinimo++;
	}
	int vert[MAX_N];
	int nv = rg.IRandom(comprimentoMinimo, n);
	if(getSubsetKVertices(nv,vert, rg)==false){
		vert[0] = s;
		for(int i=1; i<nv; i++) vert[i] = verticesOrdenados[i-1];
	}

	for (int i=0; i<nv; i++) cout<<vert[i]<<" ";
	cout<<endl;
	for (int i=0; i<nv; i++) cout<<vertices[vert[i]][0]<<" + ";
	cout<<endl;


	// cout<<"comprimentoMinimo = "<<comprimentoMinimo<<endl;
	// cout<<"K = "<<K<<endl;
	// cout<<"sumK = "<<sumK<<endl;
	
}

#endif
