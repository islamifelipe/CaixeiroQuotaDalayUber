#ifndef SOLUCAO_CPP
#define SOLUCAO_CPP

#include "rand/randomc.h"
#include "param.h"

#include <vector>

/*O comprimento do cromossomo é variável*/

class Solucao {
	public:
	TRandomMersenne *rg;
	float fitness;

	std::vector<int> cidades; // sequência de cidades do caixeiro 
	std::vector<bool> bonus;// se true, entao o caixeiro pegou o bonus da cidade correspondente
	std::vector<int> embarques; // diz se o passageiro correspondente embarcou ou nao
	std::vector<int> desembarques; // diz onde o passageiro desembarcou
	/* os vetores "cidades" e "bonus" têm o mesmo tamanho 
		os vetores "embarques" e "desembarques" têm o comprimento da quantidade de passageiros (total, de todos os vértices)

	*/

	public:
	Solucao(){}
	Solucao(TRandomMersenne &r) {
		rg = &r;
	}


	void crossover(Solucao &pai, Solucao &mae) {

	}

	void mutacao(Solucao &sol){

	}

	void operator=(Solucao &s) { // assign s to this object 
		rg = s.rg;
		cidades = s.cidades;
		bonus = s.bonus;
		embarques = s.embarques;
		desembarques = s.desembarques;
		fitness = s.fitness;
	}
	// bool operator==(Solucao &s) {
	// 	for (int i=0; i<NUMOBJETIVOS; i++){
	// 		if (fabs(s.getObj(i)-getObj(i)) >= EPS) return false;
	// 	}
	// 	return true;
	// }
};

#endif
