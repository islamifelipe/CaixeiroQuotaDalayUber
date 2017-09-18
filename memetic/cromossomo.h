#ifndef CROMOSSOMO_H
#define CROMOSSOMO_H

//////////////////////// DEPRECIATED

#include <vector>

// representaçao do cromossomo (de tamanho variado)
// a sequência de cidades deve começar no vértice 0 (s = 0)
typedef struct $
{
	std::vector<int> cidades; // sequência de cidades do caixeiro 
	std::vector<bool> bonus;// se true, entao o caixeiro pegou o bonus da cidade correspondente
	std::vector<int> embarques; // diz se o passageiro correspondente embarcou ou nao
	std::vector<int> desembarques; // diz onde o passageiro desembarcou
	/* os vetores "cidades" e "bonus" têm o mesmo tamanho 
		os vetores "embarques" e "desembarques" têm o mesmo tamanho
	*/
}Cromossomo;


#endif
