#ifndef SOLUCAO_CPP
#define SOLUCAO_CPP

#include "rand/randomc.h"
#include "param.h"

#include <vector>
#include <iostream>
using namespace std;
/*O comprimento do cromossomo é variável*/

class Solucao {
	public:
	TRandomMersenne *rg;
	double fitness;

	std::vector<int> cidades; // sequência de cidades do caixeiro 
	std::vector<bool> bonus;// se true, entao o caixeiro pegou o bonus da cidade correspondente
	std::vector<int> embarques; // diz se o passageiro correspondente embarcou ou nao. Se embarque[l]=-1, entao "l" nao embarcou. Se embarque[l]=i, diz que o passageiro embarcou em cidades[i]
	std::vector<int> desembarques; // vetor com valores i=0...|C|-1, onde i é um index do vetor "cidades". diz onde o passageiro desembarcou
	//desembarques[l] = i diz que o passageiro "l" desembarcou em cidades[i]
	/* os vetores "cidades" e "bonus" têm o mesmo tamanho 
		os vetores "embarques" e "desembarques" têm o comprimento da quantidade de passageiros (total, de todos os vértices)

	*/

	public:
	Solucao(){
		 for(int i=0; i<l; i++){
	    	embarques.push_back(-1); // nao embarcou
	    	desembarques.push_back(-1); // nao desembarcou
  		 }
	}
	Solucao(TRandomMersenne &r) {
		for(int i=0; i<l; i++){
	    	embarques.push_back(-1); // nao embarcou
	    	desembarques.push_back(-1); // nao desembarcou
		}
		rg = &r;
	}

	// Este método assume que a soluçao é viavel
	void calcularFitiness(){

	}

	double getFitness(){
		return fitness;
	}

	// ATANCAO: NAO CONFUNDIR TEMPO DA ARESTA (i,j) COM O CUSTO DA ARESTA (i,j)
	// retorna true se a soluçao atende a todas as restriçoes do problema
	// TUDO: saber se o passageiro desce antes da coleta do bonus. Se descer antes, é preciso subtrair a coleta do bonus
	// TUDO: falta testar
	bool isViavel(){
		if (cidades.size()==0) return false;

		std::vector<int> passageirosPorAresta; // inicia tudo com 1, por causa do motorista (caixeiro)
		// verifica a quota minima
		double qutoa = vertices[cidades[0]][0]*bonus[0]; // contabiliza o bonus coletado (se for o caso) na cidade inicial 
		std::vector<double> tempoAteVerticeI; // aqui computamos o tempo gasto até chegar à cidade de index=i, considerando inclusive o tempo das arestas
		tempoAteVerticeI.push_back(vertices[cidades[0]][1]*bonus[0]); // Inicialmente, considera-se somente o tempo de pegar o bonus em 's = cidades[0]', se for o caso
		double acumulador=0;
		for (int i = 1; i<cidades.size(); i++){
			qutoa+=vertices[cidades[i]][0]*bonus[i];
			acumulador = tempoAteVerticeI[i-1]+vertices[cidades[i]][1]*bonus[i]+tempo[cidades[i-1]][cidades[i]];
			tempoAteVerticeI.push_back(acumulador);
			passageirosPorAresta.push_back(1); // vai somar mais na frente
		}
		acumulador = tempoAteVerticeI[cidades.size()-1]+tempo[cidades[cidades.size()-1]][cidades[0]];
		tempoAteVerticeI.push_back(acumulador); //volta ao vértice inicial
		passageirosPorAresta.push_back(1);// vai somar mais na frente
		if(qutoa<K) {
			cout<<"PROBLEMA QUOTA ";
			return false;
		}

		//Verifica a capacidade do carro e se o embarque e o desembarque ocorre num vértice contido na rota
		// verifica o tempo maximo de permanência do passageiro (daley)
		int contCapacidade = 0;
		double delayPassageiro=0;
		int embarcouOnde, desembarcouOnde; // index (do vetor "cidades")
		for (int i=0; i<l; i++){
			if (embarques[i]!=-1) { // se o passageiro embarcou...
				delayPassageiro=0;
				if ((desembarques[i]<cidades.size() && desembarques[i]>=0)==false) {
					cout<<"PROBLEMA DESEMBARQUE ";
					return false; // entao o passageiro i deve ter desembarcado. desembarques[i] deve ser um index de "cidades"
				}
				if ((embarques[i]<cidades.size() && embarques[i]>=0 && cidades[embarques[i]] == L[i][2]) == false) {
					cout<<"PROBLEMA EMBARQUE ";
					return false;
				}
				if ((embarques[i]!=desembarques[i])==false)  {
					cout<<"PROBLEMA IGUAIS ";
					return false;
				} // se o passageiro embarcou e desembarcou no mesmo vértice
				embarcouOnde = embarques[i]; //cidades[embarques[i]]?
				desembarcouOnde = desembarques[i]; //cidades[desembarques[i]]?
				if (desembarcouOnde==0) desembarcouOnde = tempoAteVerticeI.size()-1; // desembarcou na origem, apos a volta completa do caixeiro
				delayPassageiro = tempoAteVerticeI[desembarcouOnde] - tempoAteVerticeI[embarcouOnde]; // TODO: verificar se o tempo aqui está ok. Verificar questao da coleta do bonus nas extremidades
				cout<<"delayPassageiro = "<<delayPassageiro<<endl;
				if (delayPassageiro>L[i][1]) {
					cout<<"PROBLEMA DELAY = "<<delayPassageiro<<" X "<<L[i][1]<<" ";
					return false;
				}
				contCapacidade++;
				for (int j=embarcouOnde; j<desembarcouOnde; j++){
					passageirosPorAresta[j]++; // contabiliza passageiros por aresta
				}
			}
		}
		if (contCapacidade>R) {
			cout<<"PROBLEMA CAPACIDADE DO CARRO ";
			return false;
		} 

		// verifica o custo maximo pago pelo passageiro (nao tem como aproveitar o laço anterior!)
		double sumCusto = 0;
		int c1, c2;
		for (int i=0; i<l; i++){
			sumCusto = 0;
			if (embarques[i]!=-1) { 
				embarcouOnde = embarques[i]; //cidades[embarques[i]]?
				desembarcouOnde = desembarques[i]; //cidades[desembarques[i]]?
				if (desembarcouOnde==0) desembarcouOnde = tempoAteVerticeI.size()-1; // desembarcou na origem, apos a volta completa do caixeiro
				for (int j=embarcouOnde; j<desembarcouOnde; j++){
					c1 = cidades[j];
					c2 = cidades[j+1];
					sumCusto += custos[c1][c2]/passageirosPorAresta[j];
				}
				//TUDO : PROBLEMA AQUI
				if (desembarcouOnde == tempoAteVerticeI.size()-1){ // desembarcou na origem, apos a volta completa do caixeiro
					c1 = cidades[desembarcouOnde-1];
					c2 = cidades[0];
					cout<<c1<<" "<<c2<<endl;
					sumCusto += custos[c1][c2]/(double)passageirosPorAresta[desembarcouOnde-1];
				}
				cout<<"sumCusto = "<<sumCusto<<endl;
				if (sumCusto > L[i][0]) {
					cout<<"PROBLEMA CUSTO MAXIMO = "<<sumCusto <<" X "<< L[i][0]<<endl;
					return false;
				} 
			}
		}
		return true;
	}

	/*tenta carregar i em algum vértice e descarregar em algum outro vértice na sequência do caminho
	Se conseguir, retorna true*/
	bool carrega(int i){

	}

	// determina embarques e desembarques aleatórios, mas sem violar as restriçoes do problema
	// Nao se preocupa se o fitness é bom ou nao
	// a soluçao, ao final deste método, deve continuar viável
	void carregamentoAleatorio(){
		int contPassageiros=0; // deve ser menor que R
		std::vector<int> amostral;
		//int index = 0;
		embarques[30] = 5;
		desembarques[30] = 6;
		cout<<"isViavel() = "<<isViavel()<<endl;;
		// for (int i=0; i<cidades.size() && contPassageiros<R; i++){
		// 	index = rg->IRandom(0, passageirosPorVertice[i].size()-1); // sorteia um passageiro aleatorio

		// }
	}



	void printSolucao(){ // TODO : completar
		cout<<"Sequência de cidades : ";
		for (int i=0; i<cidades.size(); i++){
			cout<<cidades[i]<<" ";
		}
		cout<<"\n Bonus : ";
		for (int i=0; i<cidades.size(); i++){
			cout<<bonus[i]<<" ";
		}
		cout<<endl;
	}

	void operator=(Solucao &s) { // assign s to this object 
		rg = s.rg;
		cidades = s.cidades;
		bonus = s.bonus;
		embarques = s.embarques;
		desembarques = s.desembarques;
		fitness = s.fitness;
	}

	void crossover(Solucao &pai, Solucao &mae) {

	}

	void mutacao(){

	}
	// bool operator==(Solucao &s) {
	// 	for (int i=0; i<NUMOBJETIVOS; i++){
	// 		if (fabs(s.getObj(i)-getObj(i)) >= EPS) return false;
	// 	}
	// 	return true;
	// }
};

#endif
