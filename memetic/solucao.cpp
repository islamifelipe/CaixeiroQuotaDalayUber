#ifndef SOLUCAO_CPP
#define SOLUCAO_CPP

#include "rand/randomc.h"
#include "param.h"

#include <vector>
#include <iostream>
#include <climits>
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

	private:
	bool static compara(int a1, int a2){ // passageiro a1,a2
		return L[a1][0]>L[a2][0];
	}

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
		fitness = 0;
		int embarcouOnde, desembarcouOnde; // index (do vetor "cidades")
		std::vector<int> passageirosPorAresta; // inicia tudo com 1, por causa do motorista (caixeiro)
		//passageirosPorAresta.size() deve ser cidades.size()
		for (int i = 0; i<cidades.size(); i++) passageirosPorAresta.push_back(1);

		for (int i=0; i<l; i++){
			if (embarques[i]!=-1){
				embarcouOnde = embarques[i]; 
				desembarcouOnde = desembarques[i]; 
				if (desembarcouOnde==0) desembarcouOnde = cidades.size();
				for (int j=embarcouOnde; j<desembarcouOnde; j++) passageirosPorAresta[j]++;
				fitness+=penalidades[i][cidades[desembarques[i]]]; // contabiliza as penalidades
			}
		}
		int c1, c2;
		for (int i = 0; i<cidades.size()-1; i++){
			c1 = cidades[i];
			c2 = cidades[i+1];
			fitness+=custos[c1][c2]/passageirosPorAresta[i];
		}
		c1 = cidades[cidades.size()-1];
		c2 = cidades[0];
		fitness+=custos[c1][c2]/passageirosPorAresta[cidades.size()-1];

	}

	double getFitness(){
		return fitness;
	}

	// ATANCAO: NAO CONFUNDIR TEMPO DA ARESTA (i,j) COM O CUSTO DA ARESTA (i,j)
	// retorna true se a soluçao atende a todas as restriçoes do problema
	// O delay do bonus é contabilizado para o passageiro apenas na origem
	bool isViavel(){
		if (cidades.size()==0) return false;

		std::vector<int> passageirosPorAresta; // inicia tudo com 1, por causa do motorista (caixeiro)
		//passageirosPorAresta.size() deve ser cidades.size()
		// verifica a quota minima
		double qutoa = vertices[cidades[0]][0]*bonus[0]; // contabiliza o bonus coletado (se for o caso) na cidade inicial 
		std::vector<double> tempoAteVerticeI; // aqui computamos o tempo gasto até chegar à cidade de index=i, considerando inclusive o tempo das arestas
		//tempoAteVerticeI.size() deve ser cidades.size()+1
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
		//tempoAteVerticeI[i] - tempoAteVerticeI[j] = chegada - origem = representa o tempo que passageiro permanecerá no carro ao se deslocar na cidade[j] à cidade[i],
		//mas tal valor contabiliza o tempo de tomar o bonus na chegada, mas nao na origem. É preciso, pois, retirar o tempo da chegada e adicionar o da origem 

		if(qutoa<K) {
			cout<<"PROBLEMA QUOTA ";
			return false;
		}

		//Verifica a capacidade do carro e se o embarque e o desembarque ocorre num vértice contido na rota
		// verifica o tempo maximo de permanência do passageiro (daley)
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
				embarcouOnde = embarques[i]; 
				desembarcouOnde = desembarques[i]; 
				delayPassageiro = tempoAteVerticeI[desembarcouOnde] - tempoAteVerticeI[embarcouOnde] + vertices[cidades[embarcouOnde]][1]*bonus[embarcouOnde] - vertices[cidades[desembarcouOnde]][1]*bonus[desembarcouOnde]; // adiciona o delay de origem e retira e do chegada
				// cout<<"Passageiro "<<i+1<<" Embarcou em "<<cidades[embarcouOnde]+1;
				if (desembarcouOnde==0) {
					desembarcouOnde = tempoAteVerticeI.size()-1; // desembarcou na origem, apos a volta completa do caixeiro
					delayPassageiro = tempoAteVerticeI[desembarcouOnde] - tempoAteVerticeI[embarcouOnde] + vertices[cidades[embarcouOnde]][1]*bonus[embarcouOnde] - vertices[cidades[0]][1]*bonus[0]; // adiciona o delay de origem e retira e do chegada
					// cout<<" e Desembarcou em "<<cidades[0]+1<<endl;
				} //else cout<<" e Desembarcou em "<<cidades[desembarcouOnde]+1<<endl;

				// cout<<"delayPassageiro = "<<delayPassageiro<<endl;
				if (delayPassageiro>L[i][1]) {
					cout<<"PROBLEMA DELAY = "<<delayPassageiro<<" X "<<L[i][1]<<endl;
					return false;
				}
				for (int j=embarcouOnde; j<desembarcouOnde; j++){
					passageirosPorAresta[j]++; // contabiliza passageiros por aresta
					if (passageirosPorAresta[j]>R+1) {
						cout<<passageirosPorAresta[j]<<endl;
						cout<<"PROBLEMA CAPACIDADE DO CARRO "<<endl;;
						return false;
					} 
				}
			}
		}
		
		// verifica o custo maximo pago pelo passageiro (nao tem como aproveitar o laço anterior!)
		double sumCusto = 0;
		int c1, c2;
		int indexFim = 0;
		for (int i=0; i<l; i++){
			sumCusto = 0;
			if (embarques[i]!=-1) { 
				embarcouOnde = embarques[i]; 
				desembarcouOnde = desembarques[i];
				indexFim = desembarques[i];
				if (desembarcouOnde==0) {
					desembarcouOnde = tempoAteVerticeI.size()-1; // desembarcou na origem, apos a volta completa do caixeiro
					indexFim = 0;
				}
				for (int j=embarcouOnde; j<desembarcouOnde-1; j++){
					c1 = cidades[j];
					c2 = cidades[j+1];
					sumCusto += custos[c1][c2]/passageirosPorAresta[j];
				}
				c1 = cidades[desembarcouOnde-1];
				c2 = cidades[indexFim];
				sumCusto += custos[c1][c2]/(double)passageirosPorAresta[desembarcouOnde-1];
				
				// cout<<"sumCusto = "<<sumCusto<<endl;
				if (sumCusto > L[i][0]) {
					cout<<"PROBLEMA CUSTO MAXIMO = "<<sumCusto <<" X "<< L[i][0]<<endl;
					return false;
				} 
			}
		}
		return true;
	}

	// tenta carregar i em algum vértice e descarregar em algum outro vértice na sequência do caminho
	// Se conseguir, retorna true
	// bool carrega(int i){

	// }

	/*Heuristica essencialmente gulosa
	Ordena os passageiros aptos a embarcar em ordem decrescente, de acordo com a tarifa maxima
	Se o i-ésimo passageiro desta ordenaçao puder embarcar, embarca-o e verifica-se, para os vértices onde ele está apto a desembarcar, aquele que tem menor penalidade
	a soluçao, ao final deste método, deve continuar viável*/
	void heuristicaDeCarregamento1(){
		for(int i=0; i<l; i++){
	    	embarques[i] = -1; // nao embarcou
	    	desembarques[i] = -1; // nao desembarcou
  		}
		std::vector<int> passageirosPorAresta; //passageirosPorAresta.size() deve ser cidades.size()
		std::vector<double> tempoAteVerticeI; //tempoAteVerticeI.size() deve ser cidades.size()+1
		tempoAteVerticeI.push_back(vertices[cidades[0]][1]*bonus[0]); // Inicialmente, considera-se somente o tempo de pegar o bonus em 's = cidades[0]', se for o caso
		double acumulador=0;
		for (int i = 1; i<cidades.size(); i++){
			acumulador = tempoAteVerticeI[i-1]+vertices[cidades[i]][1]*bonus[i]+tempo[cidades[i-1]][cidades[i]];
			tempoAteVerticeI.push_back(acumulador);
			passageirosPorAresta.push_back(1); // vai somar mais na frente
		}
		acumulador = tempoAteVerticeI[cidades.size()-1]+tempo[cidades[cidades.size()-1]][cidades[0]];
		tempoAteVerticeI.push_back(acumulador); //volta ao vértice inicial
		passageirosPorAresta.push_back(1);// vai somar mais na frente
		//tempoAteVerticeI[i] - tempoAteVerticeI[j] = chegada - origem = representa o tempo que passageiro permanecerá no carro ao se deslocar na cidade[j] à cidade[i],
		//mas tal valor contabiliza o tempo de tomar o bonus na chegada, mas nao na origem. É preciso, pois, retirar o tempo da chegada e adicionar o da origem 

		std::vector<int> ordemDecrescenteTarifas; //guarda as tarifas em ordem descrescente
		for (int i=0; i<cidades.size(); i++){
			for (int p=0; p<passageirosPorVertice[cidades[i]].size(); p++){
				ordemDecrescenteTarifas.push_back(passageirosPorVertice[cidades[i]][p]);
			}
		}
		sort(ordemDecrescenteTarifas.begin(), ordemDecrescenteTarifas.end(), compara);
		double tarifaMaxima, tempoMaximo;
		int passageiroP, embarcaEm;
		//bool embarca = false;
		for (int i=0; i<ordemDecrescenteTarifas.size(); i++){ // tenta embacar o passageiro ordemDecrescenteTarifas[i] no vértice L[ordemDecrescenteTarifas[i]][2]
			//embarca = false;
			passageiroP = ordemDecrescenteTarifas[i];
			tarifaMaxima = L[passageiroP][0];
			tempoMaximo = L[passageiroP][1];
			embarcaEm = L[passageiroP][2];
			double custoCompartilhado = 0, delayPassageiro;
			int pir = 0, pppp;  // pir e pppp, possiveis embarques e desembaques, respectivamente, sao indixes de "cidades"
			int c1, c2;
			for ( ; pir<cidades.size() && cidades[pir]!=embarcaEm; pir++);
			double minPenalidade = INT_MAX; // procura-se a penalidade minima entre os vértices onde o passageiro pode desembarcar
			int minPenalidadeDesembarque = -1; // item 
			for (pppp=pir+1; pppp<cidades.size(); pppp++){  
				delayPassageiro = tempoAteVerticeI[pppp] - tempoAteVerticeI[pir] + vertices[cidades[pir]][1]*bonus[pir] - vertices[cidades[pppp]][1]*bonus[pppp]; // adiciona o delay de origem e retira e do chegada
				c1 = cidades[pppp-1];
				c2 = cidades[pppp];
				custoCompartilhado+=custos[c1][c2]/(passageirosPorAresta[pppp-1]+1); // +1 por causa do novo passageiro que estamos prestes a embarcar
				if ((passageirosPorAresta[pppp-1]+1)<R+1 && delayPassageiro<tempoMaximo && custoCompartilhado<tarifaMaxima){
					//se entrar nesse if, significa dizer que o passageiro pode chegar ao vértice cidades[pppp].
					//devemos continuar pra saber se ele pode ir mais adiante
					if (penalidades[passageiroP][cidades[pppp]]<minPenalidade){
						minPenalidade = penalidades[passageiroP][cidades[pppp]];
						minPenalidadeDesembarque = pppp;
					}
				} else {
					pppp--;
					break;
				}
			}
			if (pppp==cidades.size()){ // testa o possivel desembarque na origem
				pppp = tempoAteVerticeI.size()-1;;
				delayPassageiro = tempoAteVerticeI[pppp] - tempoAteVerticeI[pir] + vertices[cidades[pir]][1]*bonus[pir] - vertices[cidades[0]][1]*bonus[pppp]; // adiciona o delay de origem e retira e do chegada
				c1 = cidades[cidades.size()-1];
				c2 = cidades[0];
				custoCompartilhado+=custos[c1][c2]/(passageirosPorAresta[cidades.size()-1]+1); // +1 por causa do novo passageiro que estamos prestes a embarcar
				if (passageirosPorAresta[cidades.size()-1]+1<R+1 && delayPassageiro<tempoMaximo && custoCompartilhado<tarifaMaxima){
					//se entrar nesse if, significa dizer que o passageiro pode chegar ao vértice cidades[0].
					//devemos continuar pra saber se ele pode ir mais adiante
					if (penalidades[passageiroP][cidades[0]]<minPenalidade){
						minPenalidade = penalidades[passageiroP][cidades[0]];
						minPenalidadeDesembarque = 0;
					}
				}
			}
			if (pir!=pppp && minPenalidadeDesembarque!=pir && minPenalidadeDesembarque!=-1){ // embarcou!!!
				calcularFitiness(); // TODO: pode melhorar a chamada a esse calculo do fitness. Pra nao ficar chamando toda vida
				double fitnessAnt =fitness;
				embarques[passageiroP] = pir;
				desembarques[passageiroP] = minPenalidadeDesembarque;
				int fimmmm = minPenalidadeDesembarque;
				if (minPenalidadeDesembarque==0) fimmmm = cidades.size();
				for (int poss=pir; poss<fimmmm; poss++) passageirosPorAresta[poss]++;
				calcularFitiness();
				double fitnessDepois = fitness;
				if (fitnessDepois>fitnessAnt){
					fitness = fitnessAnt;
					embarques[passageiroP] = -1;
					desembarques[passageiroP] = -1;
					fimmmm = minPenalidadeDesembarque;
					if (minPenalidadeDesembarque==0) fimmmm = cidades.size();
					for (int poss=pir; poss<fimmmm; poss++) passageirosPorAresta[poss]--;
				
				}
			}
		}
		// for (int i = 0; i<cidades.size(); i++){
		// 	cout<<passageirosPorAresta[i]<<" ";
		// }
		// cout<<endl;

		// 3055.7
		// embarques[30] = 5;
		// desembarques[30] = 0;

		// embarques[0] = 0;
		// desembarques[0] = 1;

		// embarques[1] = 0;
		// desembarques[1] = 2;

		// embarques[2] = 0;
		// desembarques[2] = 2;

		// embarques[5] = 0;
		// desembarques[5] = 2;

		// embarques[14] = 1;
		// desembarques[14] = 3;

		// cout<<"isViavel() = "<<isViavel() <<endl;
		if (isViavel()) calcularFitiness();
		else cout<<"ERROR CARREGAMENTO"<<endl;
		// cout<<"fitness = "<<getFitness()<<endl;
		
	}



	void printSolucao(){ // TODO: completar
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
