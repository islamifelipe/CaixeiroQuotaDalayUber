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

#include "param.h"
#include "populacaoInicial.cpp"
#include "Solucao.cpp"
#include "simulatedannealing.cpp"

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
std::vector<int> *passageirosPorVertice; // para cada i (vértice i), diz (a lista de) os passageiros que esparam embarcar em i

Solucao *populacao[POPSIZE]; // populaçao 
Solucao *repositorio[SIZEREPOSITORIO];
int quantRepositorio;

struct tms tempoAntes, tempoDepois, tempoDepois222;



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
	passageirosPorVertice = new std::vector<int>[n];

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
        int vert_origem = (int) L[i][2];
        passageirosPorVertice[vert_origem].push_back(i); // passageiro i deseja embarcar em L[i][2]
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
	cout<<"Instância lida com sucesso!"<<endl;
}

void setOtimo(Solucao * otimo){
	double valorOtimo = otimo->getFitness();
	int index = -1;
	for (int i=0; i<POPSIZE; i++){
		if (populacao[i]->getFitness()<valorOtimo){
			valorOtimo = populacao[i]->getFitness();
			index = i;
		}
	}
	if (index!=-1) *otimo = *populacao[index];
}


bool comparae2 (Solucao *s1, Solucao *s2) { 
	return s1->getFitness()<s2->getFitness(); 
} 
 
void elitimo(Solucao *newPop[POPSIZE], int sizeNovas){
	std::vector<Solucao *> uniao;
	for (int oeir=0; oeir<POPSIZE; oeir++) uniao.push_back(populacao[oeir]);
	for (int oeir=0; oeir<sizeNovas; oeir++) uniao.push_back(newPop[oeir]);
		

	std::sort (uniao.begin(), uniao.end(), comparae2);
	
	*populacao[0]  = *uniao[0];
	int  cont=1;
	for (int i=1; cont<POPSIZE && i<uniao.size(); i++) {
		if (abs(populacao[cont-1]->getFitness()-uniao[i]->getFitness())>EPS){
			*populacao[cont++]  = *uniao[i];
		}
	}
	for (int i=0; cont<POPSIZE && i<uniao.size(); i++){
		*populacao[cont++]  = *uniao[i];
	}
	

} 

void addRepositorio(Solucao *sol, TRandomMersenne &rg){
	if (quantRepositorio < SIZEREPOSITORIO){
		*repositorio[quantRepositorio++] = *sol;
	} else {
		int index = rg.IRandom(0, quantRepositorio-1);
		// int index = 0;
		// double max = INT_MIN;
		// for (int i=0; i<quantRepositorio; i++){
		// 	if (repositorio[i]->getFitness()>max){
		// 		max = repositorio[i]->getFitness();
		// 		index = i;
		// 	}
		// } 
		*repositorio[index] = *sol;
	}
}

/* TODO: aparentemente tem alguma coisa errada com o repositorio. Testar INST-B-40-3.txt e INST-A-40-4.txt  com e sem o repositorio*/

Solucao * memetic(TRandomMersenne &rg){
	quantRepositorio=0;
	for (int i=0; i<SIZEREPOSITORIO; i++) repositorio[i] = new Solucao(rg);
	populacaoInicial(ALFA, rg,populacao);
	times(&tempoDepois);

	fprintf(stdout,"Tempo(s) para calcular populaçao inicial = %.2lf\n", (double) (tempoDepois.tms_utime - tempoAntes.tms_utime) / 100.0 );
	
	Solucao *filho1 = new Solucao(rg);
	Solucao *filho2 = new Solucao(rg);
	Solucao *filhoEscolhido = new Solucao(rg);
	Solucao *otimo = new Solucao(rg);
	int pai, mae; // indexes
	int p1,p2,p3,p4;
	*otimo = *populacao[0];
	int contSemMudanca = 0;
	double otimoAntes = otimo->getFitness();
	Solucao *newPop[POPSIZE]; // nova populaçao
	int sizeNovas = 0;
	for (int i=0; i<POPSIZE; i++)  newPop[i] = new Solucao(rg);
	for (int i=0; i<QUANTGERACOES; i++){
		setOtimo(otimo);
	
		if (otimoAntes == otimo->getFitness()){
			contSemMudanca++;
		} else {
			otimoAntes = otimo->getFitness();
			contSemMudanca=0;
		}
		if (contSemMudanca==5){
			//contaRenovacao++;
			cout<<"RENOVA"<<endl;
			for (int cnontg = 0; cnontg<POPSIZE/2; cnontg++){
				int ijf = rg.IRandom(0, POPSIZE-1);
				// int index = rg.IRandom(0, quantRepositorio-1);
				// *populacao[ijf] = *repositorio[index];
				populacao[ijf]->reset();
				getIndividuo(1.35, rg, populacao[ijf]); // antes: 1.4
				populacao[ijf]->heuristicaDeCarregamento1();
				buscalocal(*populacao[ijf],rg);
				
			}
			contSemMudanca=0;
		}
		sizeNovas = 0;
		// for (int aap = 0; aap<POPSIZE; aap++){
		// 	cout<<populacao[aap]->getFitness()<<" "<<populacao[aap]->getSize()<<endl;
		// }
		cout<<"Geracao "<<i+1<<" Otimo = "<<otimo->getFitness()<<endl;
		for (int aap = 0; aap<POPSIZE; aap++){
			/*SORTEIA 4 individuos*/
			/*Faz-se o torneio binario entre eles*/
			p1 = rg.IRandom(0, POPSIZE-1);
			p2 = rg.IRandom(0, POPSIZE-1);
			p3 = rg.IRandom(0, POPSIZE-1);
			p4 = rg.IRandom(0, POPSIZE-1);
			if (populacao[p1]->getFitness()<populacao[p2]->getFitness()){
				pai = p1;
			} else {
				pai = p2;
			}

			if (populacao[p3]->getFitness()<populacao[p4]->getFitness()){
				mae = p3;
			} else {
				mae = p4;
			}
			filho1->reset(); 
			filho2->reset();
			filhoEscolhido->reset(); 
			double p = rg.Random();
			if (p<TAXADECRUZAMENTO){
				//cout<<"crossover"<<endl;
				bool f1 = filho1->crossover(*populacao[pai], *populacao[mae]);
				bool f2 = filho2->crossover(*populacao[mae], *populacao[pai]);
				if (f1==true && f2==true){
					if (filho1->getFitness()<filho2->getFitness()){
						*filhoEscolhido = *filho1;
						//addRepositorio(filho2,rg);
					} else{
						*filhoEscolhido = *filho2;
						//addRepositorio(filho1,rg);
					}
				} else {
					if (f1==true){
						*filhoEscolhido = *filho1;
					} else if (f2==true){
						*filhoEscolhido = *filho2;
					} else {
						if (populacao[pai]->getFitness()<populacao[mae]->getFitness()){
							*filhoEscolhido = *populacao[pai];
						} else{
							*filhoEscolhido = *populacao[mae];
						}
					}
				}
			} else {
				if (populacao[pai]->getFitness()<populacao[mae]->getFitness()){
					*filhoEscolhido = *populacao[pai];
				} else{
					*filhoEscolhido = *populacao[mae];
				}
			}
			if (filhoEscolhido->getFitness()<otimo->getFitness()) *otimo = *filhoEscolhido;
			p = rg.Random();
			if (p<TAXAMUTACAO_bonus){
				if (newPop[sizeNovas]->mutacaoInverteBonus(filhoEscolhido)==true){
						//addRepositorio(filhoEscolhido,rg);
						*filhoEscolhido = *newPop[sizeNovas];
				}
				if (newPop[sizeNovas]->getFitness()<otimo->getFitness()) *otimo = *filhoEscolhido;
				
			}
			p = rg.Random();
			if (p<TAXAMUTACAO_removeCtiy){
				if (newPop[sizeNovas]->mutacaoRemoveCidade2(filhoEscolhido)==true){
					//addRepositorio(filhoEscolhido,rg);
					*filhoEscolhido = *newPop[sizeNovas];
				}
				if (filhoEscolhido->getFitness()<otimo->getFitness()) *otimo = *filhoEscolhido;
			}
			p = rg.Random();
			if (p<TAXAMUTACAO_addCity){
				if (newPop[sizeNovas]->mutacaoAddCidade2(filhoEscolhido)==true){
					//addRepositorio(filhoEscolhido,rg);
					*filhoEscolhido = *newPop[sizeNovas];
				}
				if (filhoEscolhido->getFitness()<otimo->getFitness()) *otimo = *filhoEscolhido;	
			}
			*newPop[sizeNovas++] = *filhoEscolhido;
			p = rg.Random();
			if (p<TAXAEXECUCAO_SA){
				//addRepositorio(newPop[sizeNovas-1],rg);
				SA(*newPop[sizeNovas-1],rg); 
			}	
		}
		elitimo(newPop, sizeNovas);
	}
	return otimo;
}
/*TODO: Manter repositorio da populaçao (SA) pra renovaçao */

int main(int argc, char *argv[]){
	TRandomMersenne rg(atoi(argv[1])); // semente
	leituraDaInstancia(); 
	//populacaoInicial(rg,populacao);
	
	times(&tempoAntes);

	Solucao *otimo =  memetic(rg);

	cout<<"Solucao encontrada"<<endl;
	otimo->printSolucao();
	cout<<"Fitness = "<<otimo->getFitness()<<endl;

	times(&tempoDepois);

	
	/*========= Estatistica ========= */
	fprintf(stdout,"Tempo(s) Final = %.2lf\n", (double) (tempoDepois.tms_utime - tempoAntes.tms_utime) / 100.0 );
	


	

	// for (int cont = 0; cont<30; cont++){
	// 	for (int i=0; i<POPSIZE; i++){
	// 		//if (populacao[i]->getFitness()!=956.5) continue;
	// 		//cout<<"SOLUCAO POP: \n\t";
	// 		//cout<<"============ TESTE SOLUCAO "<<i<< " ============"<<endl;
	// 		//populacao[i]->printSolucao();
	// 		//cout<<"Fitness POP = "<<populacao[i]->getFitness()<<endl;;
	// 		//SA(*populacao[i],rg);
	// 		//cout<<endl;
	// 		Solucao *nova = new Solucao(rg);
	// 		if (nova->mutacaoInverteBonus(populacao[i])){
	// 			//cout<<"SOLUCAO MUTANTE: \n\t";
	// 			//nova->printSolucao();
	// 			//cout<<"SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			SA(*nova,rg);
	// 			//cout<<"NOVA SA SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			//nova->printSolucao();
	// 		}
	// 		*populacao[i] = *nova;
	// 		cout<<endl;
	// 	}
	// }

	// for (int i=0; i<10; i++){

	// 	int pai = rg.IRandom(0, POPSIZE-1);
	// 	int mae = rg.IRandom(0, POPSIZE-1);
	// 	//cout<<"================================================== TESTE  "<<i<< "=================================================="<<endl;
	// 	// cout<<"SOLUCAO PAI: \n\t";
	// 	// populacao[pai]->printSolucao();
	// 	// cout<<"Fitness = "<<populacao[pai]->getFitness()<<endl;;

	// 	// cout<<"SOLUCAO MAE: \n\t";
	// 	// populacao[mae]->printSolucao();
	// 	// cout<<"Fitness = "<<populacao[mae]->getFitness()<<endl;;

	// 	Solucao *filho1 = new Solucao(rg);
	// 	Solucao *filho2 = new Solucao(rg);

	// 	if (filho1->crossover(*populacao[pai], *populacao[mae])==true){
	// 		// cout<<"SOLUCAO FILHO: \n\t";
	// 		// filho1->printSolucao();
	// 		// cout<<"Fitness antes filho1 = "<<filho1->getFitness()<<endl;;
	// 		SA(*filho1,rg);
	// 		// cout<<"Fitness depois filho1 = "<<filho1->getFitness()<<endl;;
	// 	}

	// 	if (filho2->crossover(*populacao[mae], *populacao[pai])==true){
	// 		// cout<<"SOLUCAO FILHO: \n\t";
	// 		// filho2->printSolucao();
	// 		// cout<<"Fitness antes filho2= "<<filho2->getFitness()<<endl;;
	// 		SA(*filho2,rg);
	// 		// cout<<"Fitness depois filho2= "<<filho2->getFitness()<<endl;;
	// 	}
	// }

	// for (int cont = 0; cont<30; cont++){
	// 	cout<<"EOFKROFKROKFORKFORKFORKFORKFOKRFOKROFKROKFORKFORKFORKFORKFORKFORKFOKRFOKROFKROFKORKFORKFORKFROFKROFKORFKOR"<<endl;
	// 	for (int i=0; i<POPSIZE; i++){
	// 		//if (populacao[i]->getFitness()!=956.5) continue;
	// 		//cout<<"SOLUCAO POP: \n\t";
	// 		//cout<<"============ TESTE SOLUCAO "<<i<< " ============"<<endl;
	// 		//populacao[i]->printSolucao();
	// 		//cout<<"Fitness POP = "<<populacao[i]->getFitness()<<endl;;
	// 		//SA(*populacao[i],rg);
	// 		//cout<<endl;
	// 		Solucao *nova = new Solucao(rg);
	// 		if (nova->mutacaoInverteBonus(populacao[i])){
	// 			//cout<<"SOLUCAO MUTANTE: \n\t";
	// 			//nova->printSolucao();
	// 			//cout<<"SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			SA(*nova,rg);
	// 			//cout<<"NOVA SA SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			//nova->printSolucao();
	// 		}
	// 		*populacao[i] = *nova;
	// 		cout<<endl;
	// 	}
	// }

	// for (int i=0; i<POPSIZE; i++){
	// 		//if (populacao[i]->getFitness()!=956.5) continue;
	// 		//cout<<"SOLUCAO POP: \n\t";
	// 		cout<<"============ TESTE SOLUCAO "<<i<< " ============"<<endl;
	// 		populacao[i]->printSolucao();
	// 		cout<<"Fitness POP = "<<populacao[i]->getFitness()<<endl;;
	// 		//SA(*populacao[i],rg);
	// 		//cout<<endl;
	// 		Solucao *nova = new Solucao(rg);
	// 		if (nova->mutacaoInverteBonus(populacao[i])){
	// 			cout<<"SOLUCAO MUTANTE: \n\t";
	// 			nova->printSolucao();
	// 			cout<<"SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			SA(*nova,rg);
	// 			cout<<"NOVA SA SOLUCAO MUTANTE Fitness = "<<nova->getFitness()<<endl;;
	// 			nova->printSolucao();
	// 		}
	// 		*populacao[i] = *nova;
	// 		cout<<endl;
	// 	}

	// for (int i=0; i<n; i++){
	// 	cout<<"No vértice "<<i+1<<" desejam embrcar os passageiros : ";
	// 	for (int p=0; p<passageirosPorVertice[i].size(); p++){
	// 		cout<<passageirosPorVertice[i][p]<<" ";
	// 	}
	// 	cout<<endl;
	// }

	return 0;
}