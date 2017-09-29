#ifndef SOLUCAO_CPP
#define SOLUCAO_CPP

#include "rand/randomc.h"
#include "param.h"

#include <vector>
#include <iostream>
#include <climits>
using namespace std;
/*O comprimento do cromossomo é variável*/

// TODO: melhorar o carregamento

class Solucao {
	public:
	TRandomMersenne *rg;

	private:
	double fitness;
	double qutoa;
	std::vector<int> cidades; // sequência de cidades do caixeiro 
	std::vector<bool> bonus;// se true, entao o caixeiro pegou o bonus da cidade correspondente
	std::vector<int> embarques; // diz se o passageiro correspondente embarcou ou nao. Se embarque[l]=-1, entao "l" nao embarcou. Se embarque[l]=i, diz que o passageiro embarcou em cidades[i]
	std::vector<int> desembarques; // vetor com valores i=0...|C|-1, onde i é um index do vetor "cidades". diz onde o passageiro desembarcou
	//desembarques[l] = i diz que o passageiro "l" desembarcou em cidades[i]
	/* os vetores "cidades" e "bonus" têm o mesmo tamanho 
		os vetores "embarques" e "desembarques" têm o comprimento da quantidade de passageiros (total, de todos os vértices)
	*/
	std::vector<int> passageirosPorAresta; // inicia tudo com 1, por causa do motorista (caixeiro)
	// passageirosPorAresta será definido pela heuristica de carregamento
	//passageirosPorAresta.size() deve ser cidades.size()
	std::vector<double> tempoAteVerticeI; // aqui computamos o tempo gasto até chegar à cidade de index=i, considerando inclusive o tempo das arestas
	//tempoAteVerticeI.size() deve ser cidades.size()+1
		

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
  		 tempoAteVerticeI.push_back(0);
  		 qutoa=0;
	}
	Solucao(TRandomMersenne &r) {
		for(int i=0; i<l; i++){
	    	embarques.push_back(-1); // nao embarcou
	    	desembarques.push_back(-1); // nao desembarcou
		}
		tempoAteVerticeI.push_back(0);
		rg = &r;
		qutoa=0;
	}

	void reset(){
		cidades.clear();
		bonus.clear();
		passageirosPorAresta.clear();
		tempoAteVerticeI.clear();
		for(int i=0; i<l; i++){
	    	embarques[i] = -1; // nao embarcou
	    	desembarques[i]= -1; // nao desembarcou
		}
		tempoAteVerticeI.push_back(0);
		qutoa=0;

	}

	void addCidade(int city, bool bonus1){
		cidades.push_back(city);
    	bonus.push_back(bonus1); // city agora é a ultima cidade (index = cidades.size()-1)
    	passageirosPorAresta.push_back(1); // vai somar mais la na frente
    	int indexUltima = cidades.size()-1;
    	qutoa += vertices[cidades[indexUltima]][0]*bonus[indexUltima];
    	tempoAteVerticeI.push_back(0);
    	
	}

	double getQuota(){
		return this->qutoa;
	}

	void trocaCidades(int c1, int c2){
		int v_aux = cidades[c1];
		cidades[c1] = cidades[c2];
		cidades[c2] = v_aux;

		bool b_aux = bonus[c1];
		bonus[c1] = bonus[c2];
		bonus[c2] = b_aux;
	}

	int getSize(){
		return cidades.size();
	}
	

	// Este método assume que a soluçao é viavel
	void calcularFitiness(){
		fitness = 0;
		for (int i=0; i<l; i++){
			if (embarques[i]!=-1){
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

	//  0 <=  i < cidades.size()
	int getCidade(int i){ // retorna a i-ésima cidade da rota
		return cidades[i];
	}

	// ATANCAO: NAO CONFUNDIR TEMPO DA ARESTA (i,j) COM O CUSTO DA ARESTA (i,j)
	// retorna true se a soluçao atende a todas as restriçoes do problema
	// O delay do bonus é contabilizado para o passageiro apenas na origem
	// ATENCAO: pra chamar esta funçao, é necessario que passageirosPorAresta e tempoAteVerticeI estejam devidamente definidos e preenchidos
	bool isViavel(){
		if (cidades.size()==0) {
			cout<<"SIZE ZERO"<<endl;
			return false;
		}
		if(qutoa<K) {
			cout<<"PROBLEMA QUOTA ";
			return false;
		}

		//Verifica a capacidade do carro e se o embarque e o desembarque ocorre num vértice contido na rota
		// verifica o tempo maximo de permanência do passageiro (daley)
		double delayPassageiro=0;
		int embarcouOnde, desembarcouOnde; // index (do vetor "cidades")
		double sumCusto = 0;
		int c1, c2;
		int indexFim = 0;
		for (int i=0; i<l; i++){
			sumCusto = 0;
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
					cout<<"PROBLEMA EMBARQUE E DESEMBARQUE IGUAIS ";
					return false;
				} // se o passageiro embarcou e desembarcou no mesmo vértice
				embarcouOnde = embarques[i]; 
				desembarcouOnde = desembarques[i]; 
				delayPassageiro = tempoAteVerticeI[desembarcouOnde] - tempoAteVerticeI[embarcouOnde] + vertices[cidades[embarcouOnde]][1]*bonus[embarcouOnde] - vertices[cidades[desembarcouOnde]][1]*bonus[desembarcouOnde]; // adiciona o delay de origem e retira e do chegada
				indexFim = desembarques[i];
				if (desembarcouOnde==0) {
					desembarcouOnde = tempoAteVerticeI.size()-1; // desembarcou na origem, apos a volta completa do caixeiro
					delayPassageiro = tempoAteVerticeI[desembarcouOnde] - tempoAteVerticeI[embarcouOnde] + vertices[cidades[embarcouOnde]][1]*bonus[embarcouOnde] - vertices[cidades[0]][1]*bonus[0]; // adiciona o delay de origem e retira e do chegada
					indexFim = 0;
				} 

				// cout<<"delayPassageiro = "<<delayPassageiro<<endl;
				// verifica delay do passageiro
				if (delayPassageiro>L[i][1]) {
					cout<<"PROBLEMA DELAY = "<<delayPassageiro<<" X "<<L[i][1]<<endl;
					return false;
				}

				//verifica custo maximo do passageiro
				for (int j=embarcouOnde; j<desembarcouOnde-1; j++){
					c1 = cidades[j];
					c2 = cidades[j+1];
					sumCusto += custos[c1][c2]/(double)passageirosPorAresta[j];
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
		for (int j=0; j<passageirosPorAresta.size(); j++){
			if (passageirosPorAresta[j]>R+1) {
				//cout<<passageirosPorAresta[j]<<endl;
				cout<<"PROBLEMA CAPACIDADE DO CARRO "<<endl;;
				return false;
			} 
		}
		
		return true;
	}

	

	/*Heuristica essencialmente gulosa
	Ordena os passageiros aptos a embarcar em ordem decrescente, de acordo com a tarifa maxima
	Se o i-ésimo passageiro desta ordenaçao puder embarcar, embarca-o e verifica-se, para os vértices onde ele está apto a desembarcar, aquele que tem menor penalidade
	a soluçao, ao final deste método, deve continuar viável*/
	// este procedimento (re)define os vector's passageirosPorAresta e tempoAteVerticeI 
	void heuristicaDeCarregamento1(){
		for(int i=0; i<l; i++){
	    	embarques[i] = -1; // nao embarcou
	    	desembarques[i] = -1; // nao desembarcou
  		}
		tempoAteVerticeI[0] = vertices[cidades[0]][1]*bonus[0]; // Inicialmente, considera-se somente o tempo de pegar o bonus em 's = cidades[0]', se for o caso
		passageirosPorAresta[0] = 1;
		double acumulador=0;
		for (int i = 1; i<cidades.size(); i++){
			acumulador = tempoAteVerticeI[i-1]+vertices[cidades[i]][1]*bonus[i]+tempo[cidades[i-1]][cidades[i]];
			tempoAteVerticeI[i] = acumulador;
			passageirosPorAresta[i] = 1; // vai somar mais na frente
		}
		acumulador = tempoAteVerticeI[cidades.size()-1]+tempo[cidades[cidades.size()-1]][cidades[0]];
		tempoAteVerticeI[cidades.size()] = acumulador; //volta ao vértice inicial
		
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

		for (int i=0; i<ordemDecrescenteTarifas.size(); i++){ // tenta embacar o passageiro ordemDecrescenteTarifas[i] no vértice L[ordemDecrescenteTarifas[i]][2]
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
				delayPassageiro = tempoAteVerticeI[pppp] - tempoAteVerticeI[pir] + vertices[cidades[pir]][1]*bonus[pir] - vertices[cidades[0]][1]*bonus[pppp]; //TODO: *bonus[pppp]; está correto ? Nao seria *bonus[0]; ? // adiciona o delay de origem e retira e do chegada
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
		if (isViavel()) calcularFitiness(); // TODO: sair?
		if (isViavel() == false) cout<<"ERROR CARREGAMENTO"<<endl;
		// cout<<"fitness = "<<getFitness()<<endl;
		
	}




	void printSolucao(){ 
		cout<<"Sequência de cidades : ";
		for (int i=0; i<cidades.size(); i++){
			cout<<cidades[i]+1<<" ";
		}
		cout<<"\n Bonus : ";
		for (int i=0; i<cidades.size(); i++){
			cout<<bonus[i]<<" ";
		}
		int embarcouOnde, desembarcouOnde;
		for (int i=0; i<l; i++){
			if (embarques[i]!=-1) { 
				embarcouOnde = embarques[i]; 
				desembarcouOnde = desembarques[i];
				cout<<"Passageiro "<<i+1<<" embarcou em "<<cidades[embarcouOnde]+1<<" e desembarcou em "<<cidades[desembarcouOnde]+1<<endl;
			}
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
		passageirosPorAresta = s.passageirosPorAresta;
		tempoAteVerticeI = s.tempoAteVerticeI;
		qutoa = s.qutoa;
	}

	/*Recombinação SPLIT
	Sorteia, com probabilidade uniforme, um ponto de corte (alelo de divisao)
	O cromossomo filho será constituido da primeira parte do pai e os alelos da mae que nao estao na primeira parte do pai
	Aconcelha-se que o algoritmo memético chame crossover (pai, mae) e crossover(mae, pai) e tome o filho de melhor fitness
	ATENCAO : utilizado somente para grafos completos
	*/
	bool crossover(Solucao &pai, Solucao &mae) {
		this->qutoa = 0;
		int i_alelo = rg->IRandom(1, pai.getSize()-1);
		for (int i=0; i<i_alelo; i++) this->addCidade(pai.getCidade(i), pai.getBonus(i));
		bool thereis = false;
		for (int i=0; i<mae.getSize(); i++){
			thereis = false;
			for (int j=0; j<this->getSize() && thereis==false; j++){
				if (this->getCidade(j)==mae.getCidade(i)) thereis = true;
			}
			if (thereis == false){
				this->addCidade(mae.getCidade(i), mae.getBonus(i));
			}
		}
		if (this->qutoa>=K) {
			heuristicaDeCarregamento1();
			return true;
		}
		return false;

	}

	bool getBonus(int i){
		return this->bonus[i];
	}


	/* Sorteia-se dois bonus. Tenta-sa inverter um e depois o outro. 
	Aquele que reduzir o tempo consumido por capturar o bonus, é o escolhido
	*/
	bool mutacaoInverteBonus(Solucao *sol){
		reset();
		bool deuCerto = false;
		int tentativa=0;
		this->qutoa = 0; // Necessario, pois em addCidades, a quota é incrementada
		this->fitness = sol->getFitness();
		double newQuota1=0, newQuota2 = 0, novaQuotafinal;
		int i_alelo, al1, al2;
		int multiplicador1, multiplicador2;
		double tempoBonus = 0;
		for (int i=0; i<sol->getSize(); i++) {
			this->addCidade(sol->getCidade(i), sol->getBonus(i));
			tempoBonus += vertices[sol->getCidade(i)][1]*sol->getBonus(i);
		}
		do{
			al1 = rg->IRandom(1, sol->getSize()-1);
			al2 = rg->IRandom(1, sol->getSize()-1);
			multiplicador1 = bonus[al1]==true ? -1 : 1; // inverte
			multiplicador2 = bonus[al2]==true ? -1 : 1; // inverte
			newQuota1 = this->qutoa + vertices[cidades[al1]][0]*multiplicador1;
			newQuota2 = this->qutoa + vertices[cidades[al2]][0]*multiplicador2;
			if (newQuota1>=K && newQuota2>=K) deuCerto = true;
			tentativa++;
		}while(tentativa<5 && deuCerto==false);
		double tm1 = tempoBonus + vertices[cidades[al1]][1]*multiplicador1;
		double tm2 = tempoBonus + vertices[cidades[al2]][1]*multiplicador2;
		if (tm1<tm2) {
			novaQuotafinal = newQuota1;
			i_alelo = al1;
		}
		else {
			novaQuotafinal = newQuota2;
			i_alelo = al2;
		}
		if(deuCerto == true){
			this->qutoa = novaQuotafinal;
			bonus[i_alelo] = !sol->getBonus(i_alelo);
			heuristicaDeCarregamento1();
		} 
		return deuCerto;
	}

	/*REmove uma cidade da rota. A cidade é sorteada randomicamente*/
	bool mutacaoRemoveCidade(Solucao *sol){
		if (sol->getSize()<=2) return false;
		reset();
		this->qutoa = 0;
		int i_alelo = rg->IRandom(1, sol->getSize()-1);
		for (int i=0; i<i_alelo; i++) this->addCidade(sol->getCidade(i), sol->getBonus(i));
		for (int i=i_alelo+1; i<sol->getSize(); i++) this->addCidade(sol->getCidade(i), sol->getBonus(i));
		if (this->qutoa<K) return false;
		heuristicaDeCarregamento1();
		return true;
	}
	/* Adiciona cidade à rota (com bonus ativado) e faz carregamento
	Um ponto i do cromossomo é sortado e uma cidade c também é sortada. Adiciona-se a cidade c na posiçao i+1
	nao modifica a soluçao passada como argumento*/
	bool mutacaoAddCidade(Solucao *sol){
		if (sol->getSize()==n) return false;
		reset();
		this->qutoa = 0;
		int i_alelo = rg->IRandom(1, sol->getSize()-1);
		bool auxAmostral[MAX_N]; // 0 se a cidade nao estah na rota. 1 caso contrario
		std::vector<int> amostral;
		for (int i=0; i<n; i++) auxAmostral[i] = false;
		for (int i=0; i<sol->getSize(); i++) {
			auxAmostral[sol->getCidade(i)] = true;
			if (i<i_alelo) this->addCidade(sol->getCidade(i), sol->getBonus(i));
		}
		for (int i=0; i<n; i++){
			if (auxAmostral[i]==false) amostral.push_back(i);
		}
		// assert(amostra.size()>0)
		int newCidadeIndex = rg->IRandom(0, amostral.size()-1);
		this->addCidade(amostral[newCidadeIndex], false);
		for (int i=i_alelo; i<sol->getSize(); i++) this->addCidade(sol->getCidade(i), sol->getBonus(i));
		heuristicaDeCarregamento1();
		return true;

	}
	// bool operator==(Solucao &s) {
	// 	for (int i=0; i<NUMOBJETIVOS; i++){
	// 		if (fabs(s.getObj(i)-getObj(i)) >= EPS) return false;
	// 	}
	// 	return true;
	// }

};

#endif
