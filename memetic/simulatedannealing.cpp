#ifndef SIMULATEDANNEALING  
#define SIMULATEDANNEALING

#include "param.h"
#include "Solucao.cpp"
#include "rand/randomc.h"
#include <math.h>       /* exp */

#include <iostream>
using namespace std;

/* Vizinhança utilizada: 2-opt na lista de vértices*/
/*Sorteia-se dois vértices, e troca-os de posiçao na sequência de cidades
DÁ CERTO SEMPRE PARA GRAFOS COMPLETOS*/


// baseado no modelo classico de SA (notas de aula de Profa. Silvia e Beth)
// a soluçao sol será modificada no final
void SA(Solucao &sol,TRandomMersenne &rg){
	//cout<<"INICIO do SA = "<<sol.getFitness()<<endl;
	double temperature = TEMPERATURA;
	double iteracaoK = ITERACAO_K/1.0;
	int quantIteracaoK = (int) iteracaoK;
	int v1, v2;
	double probabilidade;
	double deltaC;
	Solucao aux(rg);
	Solucao xk(rg);
	xk = sol;
	aux = xk;
	do {
		for (int i=0; i<quantIteracaoK; i++){
			//gera nova soluçao

			v1 = rg.IRandom(1,aux.getSize()-1);
			while ((v2 = rg.IRandom(1,aux.getSize()-1)) == v1);

			aux.trocaCidades(v1, v2);
			aux.heuristicaDeCarregamento1();
			
			deltaC = aux.getFitness() - xk.getFitness();
			if (deltaC<=0){ // aceita soluçao
				xk = aux;
				if (xk.getFitness()<sol.getFitness()){ //descida
					sol = xk;
				}
			} else{ // aplica probabilidade da temperatura
				probabilidade = exp((-1)*deltaC/temperature);
				double pp = rg.Random ();
				if (pp<probabilidade){
					xk = aux;
				} else { // volta o aux ao que era antes (ou seja, ao xk - anterior)
					aux.trocaCidades(v1, v2);
				}
			}
		}
		temperature/=FATORDECAIMENTO;
		iteracaoK*=FATORDECAIMENTOITERACAO;
		// cout<<"temperature = "<<temperature<<endl;
		// cout<<"iteracaoK = "<<iteracaoK<<endl;
		quantIteracaoK = (int) iteracaoK;
		if (xk.getFitness()<sol.getFitness()){ //descida
			sol = xk;
		}
	} while ((temperature-1.0)>EPS);
	//cout<<"FIM do SA = "<<sol.getFitness()<<endl;

}

#endif
