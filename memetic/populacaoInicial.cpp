#ifndef POPULACAOINICIAL
#define POPULACAOINICIAL


#include "rand/randomc.h"
#include "param.h"
#include "Solucao.cpp"
#include "simulatedannealing.cpp"

#include "LK_FILES/LKHParser.h"
#include <vector>
#include <iostream>
using namespace std;


const string LKH_PATH = "LK_FILES/LKH-2.0.7/";
const string LK_FILES_PATH = "LK_FILES2/";

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
	do {
		std::vector<int> amostral;
		vert[0] = s;
		for (int i=0; i<n; i++) 
			if (i!=s) amostral.push_back(i);
		sum = vertices[s][0]; // soma o bonus
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
		//cout<<"Individuo tamanho "<<nv<<" gerado aleatoriamente com "<<tentativaCont<<" tentativas"<<endl;
		return true;
	}
}


/*LRC = seja v o vértice que se liga ao ultimo e que bonus(v) seja maximo 
Entao, LRC é composto pelos vértices v' tais que custo[ultima][v']<custo[ultima][v] e o bonus[v'] é no maximo custo[ultima][v]*(1+alfa_LRC)*/
int constroiLRC2(int LRC[MAX_N], bool marcado[MAX_N], int ultima){
	int size = 0;
	int indexMin = 0;
	for(; indexMin<n && marcado[indexMin]==true; indexMin++);
	double minCusto = custos[ultima][indexMin];
	for (int i=0; i<n; i++){
		if (marcado[i]==false){
			if (custos[ultima][i]<minCusto){
				minCusto = custos[ultima][i];
				indexMin = i;
			}
		}
	}
	LRC[size++] = indexMin;
	for (int i=0; i<n; i++){
		if (marcado[i]==false){
			if (vertices[i][0]>vertices[indexMin][0]){
				if (custos[ultima][i]<minCusto*(1+alfa_lrc)){
					LRC[size++] = i;
				}
			}
		}
	}
	return size;
}	

/*LRC = seja v o vértice tal que custo[ultima][v] seja minimo. 
Entao, LRC é composto pelos vértices v' tais que bonus(v')>bonus(v) e custo[ultima][v'] é no maximo custo[ultima][v]*(1+alfa_LRC)*/
int constroiLRC(int LRC[MAX_N], bool marcado[MAX_N], int ultima){
	int size = 0;
	int indexMin = 0;
	for(; indexMin<n && marcado[indexMin]==true; indexMin++);
	double minCusto = custos[ultima][indexMin];
	for (int i=0; i<n; i++){
		if (marcado[i]==false){
			if (custos[ultima][i]<minCusto){
				minCusto = custos[ultima][i];
				indexMin = i;
			}
		}
	}
	LRC[size++] = indexMin;
	for (int i=0; i<n; i++){
		if (marcado[i]==false){
			if (vertices[i][0]>vertices[indexMin][0]){
				if (custos[ultima][i]<=minCusto*(1.0+alfa_lrc)){
					LRC[size++] = i;
				}
			}
		}
	}
	return size;
}	

void getIndividuo1(double alfa, TRandomMersenne &rg, Solucao *sol){
	std::vector<int> verticesOrdenados; // gurda os vértices ordenados por bonus
	for (int i=0; i<n; i++)verticesOrdenados.push_back(i); // o i-ésimo vértice
	sort(verticesOrdenados.begin(),verticesOrdenados.end(), compare);
	comprimentoMinimo=0; // a quantidade minima de vértices (com os maiores bonus), tal que é possivel atingir K
	double sumK=0;
	for (int i=0; i<n && sumK<K; i++) {
		sumK+=vertices[verticesOrdenados[i]][0];
		comprimentoMinimo++;
	}
	comprimentoMinimo++;
	int mmmmax = comprimentoMinimo*alfa;
	if (mmmmax>=n) mmmmax = n;
	int nv = rg.IRandom(comprimentoMinimo, mmmmax);
	sol->addCidade(s, true);
	bool marcado[MAX_N]; // 0 se a cidade nao estah em sol. 1 caso contrario
	for (int i=0; i<n; i++) marcado[i] = false;
	marcado[s] = true;
	for (int i=1; i<nv; i++){
		int LRC[MAX_N]; // indices dos vértices que podem ser sortados
		/*LRC = seja v o vértice tal que custo[ultima][v] seja minimo. 
		Entao, LRC é composto pelos vértices v' tais que bonus(v')>bonus(v) e custo[ultima][v'] é no maximo custo[ultima][v]*(1+alfa_LRC)*/
		int ultima = sol->getCidade(sol->getSize()-1);
		int sizeLRC = constroiLRC(LRC, marcado, ultima);
		int index = rg.IRandom(0, sizeLRC-1);
		sol->addCidade(LRC[index], true);
		marcado[LRC[index]] = true;
	}
	while (sol->getQuota()<K) {
		double bonusMaximo = INT_MIN;
		int index = -1;
		for (int i=0; i<n; i++){
			if (marcado[i]==false && bonusMaximo<vertices[i][0]){
				bonusMaximo = vertices[i][0];
				index = i;
			}
		}
		sol->addCidade(index, true);
		marcado[index] = true;
	}

	sol->heuristicaDeCarregamento1();
	buscalocal(*sol,rg);

}


void getIndividuo(double alfa, TRandomMersenne &rg, Solucao *sol){
	//Solucao *sol = new Solucao(rg);
	std::vector<int> verticesOrdenados; // gurda os vértices ordenados por bonus
	for (int i=0; i<n; i++)verticesOrdenados.push_back(i); // o i-ésimo vértice
	sort(verticesOrdenados.begin(),verticesOrdenados.end(), compare);
	comprimentoMinimo=0; // a quantidade minima de vértices (com os maiores bonus), tal que é possivel atingir K
	double sumK=0;
	for (int i=0; i<n && sumK<K; i++) {
		sumK+=vertices[verticesOrdenados[i]][0];
		comprimentoMinimo++;
	}
	comprimentoMinimo++;
	int vert[MAX_N]; // guarda os vértices da clique
	std::vector<int> vert2;
	vector< vector<double> > custos_vector;
	int mmmmax = comprimentoMinimo*alfa;
	if (mmmmax>=n) mmmmax = n;
	int nv = rg.IRandom(comprimentoMinimo, mmmmax);
	if(getSubsetKVertices(nv,vert, rg)==false){
		vert[0] = s;
		for(int i=1; i<nv; i++) vert[i] = verticesOrdenados[i-1];
	}
	for (int i=0; i<nv; i++) { // toma klique e prepara os argumentos pro LK
		vector<double> aulcv;
		for (int j=0; j<nv; j++) {
			aulcv.push_back(custos[vert[i]][vert[j]]);
		}
		custos_vector.push_back(aulcv);
		vert2.push_back(i); 
		// O vértice em vert[i] é o id do vértice escolhido, conforme a instância. 
		// vert[i] é transformado em i para o LK
	}

	// for (int i=0; i<nv; i++) cout<<vert[i]<<" ";
	// cout<<endl;
	// for (int i=0; i<nv; i++) cout<<vertices[vert[i]][0]<<" + ";
	// cout<<endl;
	
	LKHParser parser(LKH_PATH, LK_FILES_PATH, vert2, custos_vector, "augumacoisa", string("TSP"));
    vector<int> path = parser.LKHSolution(); // path tem os valores do vetor vert2, que correspondem ao index em vert
   // double custosdf = 0;
    for (int i=0; i<path.size(); i++){
    	sol->addCidade(vert[path[i]], true);
    	// sol->cidades.push_back(vert[path[i]]);
    	// sol->bonus.push_back(true);
    	//cout<<vert[path[i]]<<" ";
       // custosdf +=  custos_vector[path[i]][path[i+1]];
    }
   // cout<<endl;
    // cout<<path[path.size()-1]<<" ";
    // custosdf +=  custos_vector[path[path.size()-1]][path[0]];
    // cout<<"\nCusto TSP = "<<custosdf<<endl;
    // cout<<endl;

	// cout<<"comprimentoMinimo = "<<comprimentoMinimo<<endl;
	// cout<<"K = "<<K<<endl;
	// cout<<"sumK = "<<sumK<<endl;
}

void populacaoInicial(double alfa, TRandomMersenne &rg, Solucao *populacao[POPSIZE]){
	cout<<"Gerando populaçao inicial"<<endl;
	for (int i=0; i<POPSIZE; i++){
		populacao[i] = new Solucao(rg);
		getIndividuo(alfa, rg, populacao[i]);
		//populacao[i]->printSolucao();
		populacao[i]->heuristicaDeCarregamento1();
		buscalocal(*populacao[i],rg);
		//cout<<"Fintness = "<<populacao[i]->getFitness()<<endl;
		//cout<<endl;
		// populacao[i]->calcularFitiness();
		//
	}
}

void populacaoInicial1(double alfa, TRandomMersenne &rg, Solucao *populacao[POPSIZE]){
	cout<<"Gerando populaçao inicial (com grasp)"<<endl;
	for (int i=0; i<POPSIZE; i++){
		populacao[i] = new Solucao(rg);
		getIndividuo1(alfa, rg, populacao[i]);
		//populacao[i]->printSolucao();
		populacao[i]->heuristicaDeCarregamento1();
		//buscalocal(*populacao[i],rg);
		//cout<<"Fintness = "<<populacao[i]->getFitness()<<endl;
		//cout<<endl;
		// populacao[i]->calcularFitiness();
		//
	}
}

#endif
