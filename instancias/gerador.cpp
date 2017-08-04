/*
#=======================================================================
# Islame Felipe DA COSTA FERNANDES --- Copyright 2017
#-----------------------------------------------------------------------
# 
#=======================================================================
*/

/*Instâncias simétricas*/


#include <iostream>
#include <time.h>       /* time */
#include <stdlib.h>     /* srand, rand */
#include <utility>
#include <limits.h>
#include <vector>

using namespace std;
/* OK primeiro, geramos uma matriz de custos aleatórios (entre 100 e 999) associados a cada aresta do grafo*/

		/* Depois calculamos o bonus de cada vértice da seguinte maneira:
	 	* 	soma-se os custos das arestas incidentes num vértice i
	 	*	quanto maior for tal soma, maior deve ser o bonus. Assim, os vértices de melhores bonus, terao arestas com maiores custos associados
	OK 	*	sorteia-se o bonus (100 a 999) a partir de um fator de correlaçao
	 	*/
	 	
	//OK   /*A cota mínima deve ser sorteada entre s/3 e s/2, onde s é a soma dos bonus de todos os vértices*/

	//OK  /*sorteia-se randomicamente (e livremente) a origem e o destino de cada passageiro*/


	 //OK /* Gerar o custo (tarifa) maximo que cada passageiro está disposto a pagar : porcentagem do custo da AGM dos custos */



	 		/*monta-se uma nova matriz (grafo), nxn, com os valores do tempo (0 - 99) associado a cada aresta
	//OK  	* o tempo de uma aresta deve ser anticorrelacionado com o custo dessa aresta (aresta mais baratas, tomam mais tempo)*/

//	OK /*Para cada vértice, associar um valor de tempo (20 - 99). O bonus e o tempo do vértice sao correlacionados */


	 // OK /* Como gerar o tempo maximo que cada passageiro está disposto a ficar dentro do carro? Tree */


	 /*para cada passageiro i e vértice j, associa-se um valor (? - ?) wij que representa o valor que será acrescido no custo final da funcao objetivo. */



/* TODO: 
	quantidade de passageiros
	penalidade
	assimétrica

*/

//esta funçao implementa um algorimo de PRIM, bem de simples e retorna o peso da arvore resultante
int getPesoAGM(int **pesos, int n){
	int custo_aux[1000];
	int fixo[1000];

	int total = 0;

	for(int i=0; i<n; i++) {
		fixo[i] = 0;
	  	custo_aux[i] = INT_MAX;
	}
	custo_aux[0] = 0;

	for(int faltam = n; faltam>0; faltam--) {
		int no = -1;
	  	for(int i=0; i<n; i++)
	    	if(!fixo[i] && (no==-1 || custo_aux[i] < custo_aux[no]))
	      		no = i;
		fixo[no] = 1;

	  	if(custo_aux[no] == INT_MAX) {
	   	 	total = INT_MAX;
	    	break;
	  	}
	  	total += custo_aux[no];

	  	for(int i=0; i<n; i++)
	    	if(custo_aux[i] > pesos[no][i])
	      		custo_aux[i] = pesos[no][i];
	}
	return total;
}
/*Se o alfa for negativo, é anticorrelaçao. Positivo, correlaçao*/
pair<double, double> getGamaBeta(double alpha){
	double betha, gama;
	betha = 0.5*((double)(1-alpha)); // Correçao do Chen et al (2007)
	if (alpha>=0){
		gama = betha;
	} else{
		gama = 1 - betha;
	}
	return make_pair(gama,betha);
}

double randd() {
  return (double)rand() / ((double)RAND_MAX + 1);
}


int **floydWarshall(int n, int **W){
	/*W nao é modificada*/
	int **D;
	D = new int*[n]; // D
	for (int i=0; i<n; i++){
		D[i] = new int[n];
		for (int j=0; j<n; j++){
			D[i][j] = W[i][j]; 
		}
	}

	for (int k = 0; k<n; k++){
		for (int i = 0; i<n; i++){
			for (int j = 0; j<n; j++){
				if (D[i][j]<=D[i][k] + D[k][j]){
					D[i][j] = D[i][j];	
				} else {
					D[i][j] = D[i][k] + D[k][j];
				}
			}
		}
	}
	return D;

}
/* ./gerador quantidade_de_vertices fator_de_correlaçao fato_de_anticorrelaçao*/
int main (int argc, char *argv[]){
	srand (time(NULL));
	int n; // quantidade de vértices do grafo 
	int capacidade; // capacidade do veículo 
	int cota; // cota minima de bonus a ser coletada
	int num_passageiros = 0; // quantidade TOTAL de passageiros
	int **custos; // matriz nxn de custos
	int **tempoArestas; // matriz nxn de tempos das arestas
	int capacidades[3] = {3,4,5};
	int porcentagens[3] = {30, 40, 50}; // porcentagem utilizada para calular a tarifa maxima de cada passageiro (a partir do valor da agm)
	int porcentagens2[3] = {5, 10, 20}; // porcentagens para a quantidade de passageiros por vértices
	int tetoPassageiros[3] = {30, 40, 50}; // tetos para a quantidade de passageiros
	int **vertices;
	//int maxTimesPassageiros;
	double *somas; // usado para normalizar as somas dos custos das arestas incidentes em cada vértices
	double alpha_corr, alpha_anticorr;
	vector<int> *passageiros;

	n = atoi(argv[1]); // quantidade de vertices
	//maxTimesPassageiros = 20; // no maximo, a quantidade de passageiros será 20 vezes mais que a quantidade de vértices 
	
	alpha_corr =  atof(argv[2]);
	alpha_anticorr = atof(argv[3]);
	capacidade = capacidades[rand()%3]; // sorteia a capacidade do veiculo
	


	pair<double, double> correlacao = getGamaBeta(alpha_corr);
	pair<double, double> anticorrelacao = getGamaBeta(alpha_anticorr);

	custos = new int*[n];
	vertices = new int*[n];
	tempoArestas = new int*[n];
	somas = new double[n];
	passageiros = new vector<int>[n];;
	for (int i=0; i<n;i++){
		custos[i] = new int[n];
		tempoArestas[i] = new int[n];
		vertices[i] = new int[2]; // bonus e tempo
	}

	
	for (int i=0; i<n; i++){
		int min = (int) n*((double)porcentagens2[(rand()%3)]/100);
		if (min<1) min=1;
		int max = tetoPassageiros[rand()%3];
		int quant = min;
		if (quant>max) quant = max;
		num_passageiros += quant;
		for (int j=0; j<quant; j++){
			int v= rand()%n;
			if (v==i){
				j--;
				continue;
			}
			passageiros[i].push_back(v); 
		}
	}

	cout<<n<<" ";
	cout<<num_passageiros<<" ";
	cout<<capacidade<<endl;
	cout<<endl;

	for (int i=0; i<n; i++){
		custos[i][i] = 0;
		for (int j=i+1; j<n; j++){

			double custo01 = randd();

			int sinal = 1;
			if ((rand()%2) == 1) sinal = -1; // se 0 for sorteado, entao usa-se -1
			double ra = randd();
			double tempo = alpha_anticorr*custo01 + anticorrelacao.second + anticorrelacao.first*ra*sinal;
			
			custos[i][j] = (int) (custo01*1000); //  rand()%(999 - 100 + 1) + 100;
			custos[j][i] = custos[i][j];

			tempoArestas[i][j] = (int) (tempo*1000);
			tempoArestas[j][i] = tempoArestas[i][j];

		}
	}
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			cout<<custos[i][j]<<" ";
		}
		cout<<endl;
	}

	int** pahts = floydWarshall(n, tempoArestas);

	cout<<endl;
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			cout<<tempoArestas[i][j]<<" ";
		}
		cout<<endl;
	}

	cout<<endl;

	int s = 0;
	double max = -1;
	double min = INT_MAX;
	for (int i=0; i<n; i++){
		int sum_peso = 0;
		for (int j=0; j<n; j++){
			sum_peso += custos[i][j];
		}
		if (sum_peso>max){
			max = sum_peso;
		}
		if (sum_peso<min){
			min = sum_peso;
		}
		somas[i] = sum_peso;
	}

	//normaliza as somas
	for (int i=0; i<n; i++){
		somas[i] = (somas[i] - min)/(max-min+1);
	}
	
	//finalmente, apos a normalizaçao, calculamos os bonus
	for (int i=0; i<n; i++){
		int sinal = 1;
		if ((rand()%2) == 1) sinal = -1; // se 0 for sorteado, entao usa-se -1
		double ra = randd();
		double bonus = alpha_corr*somas[i] + correlacao.second + correlacao.first*ra*sinal;
		vertices[i][0]=(int)(bonus*1000);
			
		// aproveitar esse laço para gerar os tempos dos vértices
		if (vertices[i][0]==0) vertices[i][1] = 0;
		else {
			sinal = 1;
			if ((rand()%2) == 1) sinal = -1; // se 0 for sorteado, entao usa-se -1
			ra = randd();
			double tempo = alpha_corr*vertices[i][0] + correlacao.second + correlacao.first*ra*sinal;
			vertices[i][1]=(int)(bonus*100); // 1000 porque o valor do bonus deve ser entre 100 e 999
		}

		s+=vertices[i][0]; // soma o bonus
	}
	cota = rand()%(s/2 - s/3 + 1) + s/3; ///

	cout<<cota<<endl;
	for (int i=0; i<n; i++){

		cout<<i<<" "<<vertices[i][0]<<" "<<vertices[i][1]<<endl;
	}


	cout<<endl;

	// antiga maneira como eu calculava os passageiros nos vértices
	// for (int i=0; i<n; i++){
	// 	int v= rand()%n;
	// 	if (v==i){
	// 		i--;
	// 		continue;
	// 	}
	// 	passageiros[i].push_back(v);  
	// }
	// // for (int cont=0; cont<num_passageiros-n; cont++){
	// // 	int origem = rand()%n;
	// // 	int destino = rand()%n;
	// // 	if (origem==destino){
	// // 		cont--;
	// // 		continue;
	// // 	}
	// // 	passageiros[origem].push_back(destino); 
	// // }

	int treeCustos = getPesoAGM(custos, n); //define custo maximo que o passageiro quer pagar


	
	int pas_id= 0; //id passageiro 
	for (int i=0; i<n; i++){
		for (int j=0; j<passageiros[i].size(); j++){
			int pos = porcentagens[rand()%3];
			int treeTempo = pahts[i][passageiros[i][j]]*3; // pega o caminho mais curto entre a origem e o destino e multiplica por 3
			cout<<pas_id<<" "<<i<<" "<<passageiros[i][j]<<" "<<(double)treeCustos*((double) pos/100)<<"   "<<treeTempo<<endl;
			pas_id++;
		}
	}

	return 0;
}