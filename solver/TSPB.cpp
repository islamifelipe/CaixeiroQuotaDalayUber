/*
#=======================================================================
# Islame Felipe DA COSTA FERNANDES --- Copyright 2017
#-----------------------------------------------------------------------
# TSP with quotes
#=======================================================================

*/


#include <iostream>
#include <map> 
#include <string>
#include <climits>
#include <stack>   
#include <utility>      // std::pair
#include <cmath>
#include <list>
#include "gurobi_c++.h"
#include <sys/times.h>
#include <unistd.h>
#include <sstream>
using namespace std;


template <typename T>
    std::string to_string(T value)
    {
      //create an output string stream
      std::ostringstream os ;

      //throw the value into the string stream
      os << value ;

      //convert the string stream into a string and return
      return os.str() ;
    }





int main(){
	
	int n; // quantidade de vértices
	int l; // quantidade de passageiros
	int R; // capacidade de carro
	double K; // quota minima a ser coletada pelo caixeiro
	int s = 0;

	//////////////////// LEITURA DA INSTÂNCIA ////////////////////

	cin>>n;
	cin>>l;
	cin>>R;

	double **custos = new double*[n]; // matriz de custo das arestas
	double **tempo = new double*[n]; // matriz de tempo das arestas
	double **L = new double*[l]; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
	double **penalidades = new double*[l]; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
	double **vertices = new double*[n]; // matriz nx2 do bonus e delay de cada vértice 


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

	//////////////////// CONSTRUINDO MODELO ////////////////////

	GRBEnv env = GRBEnv();; 
 	//env.set("OutputFlag","0"); // desliga o output do solver
 	GRBModel model = GRBModel(env);; // cria modelo
 	env.set(GRB_DoubleParam_TimeLimit, 80000); 
 	env.set(GRB_IntParam_Method, 1); // 1 é mais rapido //primal
  	env.set(GRB_IntParam_SiftMethod,1);
	
	/****************** VARIAVEIS DO MODELO ******************/
  	int i,j;
	GRBVar *u =  new GRBVar[n]; //usada pelas restricoes de subcilo
	GRBVar **x = new GRBVar*[n];; //diz se o caixeiro passou pela aresta ij
	GRBVar *p = new GRBVar[n];

	
			for (int i = 0; i < n; i++) {

				u[i] = model.addVar(0.0, n - 1, 0.0, GRB_INTEGER, "u" +std::to_string(i));
				p[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "p" +std::to_string(i));

			}

			for (i = 0; i < n; i++) {
				x[i] = new GRBVar[n];
				for (j = 0; j < n; j++) {

					x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x" + std::to_string(i)+std::to_string(j));

				}
			}

	/****************** FUNCAO OBJETIVO E RESTRICOES DO MODELO ******************/


	model.update();
   
   	int constrCont = 0;

    GRBLinExpr exprObjet;
    GRBLinExpr rest1,nova;
    double um = 1;
    for (int i=0; i<n; i++){
    	for (int j=0; j<n; j++){
    		exprObjet.addTerms(&custos[i][j], &x[i][j],1);  // sentido direto (conceitual) da aresta
   		
    	}
   	}
   	model.setObjective(exprObjet,GRB_MINIMIZE); 
   	

   	///////////////////////////RESTRICOES////////////////////////////////////


   			GRBLinExpr c = 0;
			GRBLinExpr c1 = 0;
			GRBQuadExpr qc = 0;
			GRBQuadExpr qc1 = 0;

			for (i = 0; i < n; i++) {
				x[i][i].set(GRB_DoubleAttr_UB, 0);
			}

			u[s].set(GRB_DoubleAttr_UB, 0);
			p[s].set(GRB_DoubleAttr_UB, 0);

			//RESTRICAO 2
			for (j = 0; j < n; j++) {
				if (j != s) {
					GRBLinExpr c = 0;
					for (i = 0; i < n; i++) {
						if (i != j) {
							c += x[i][j];
						}
					}
					model.addConstr(c <= 1, "rest2_cicloIda" + std::to_string(constrCont++));
				}
			}

			//RESTRICAO 3
			for (j = 0; j < n; j++) {
				if (j != s) {
					GRBLinExpr c = 0;
					for (i = 0; i < n; i++) {
						if (i != j) {
							c += x[j][i];
						}
					}
					model.addConstr(c <= 1, "rest3_cicloVolta" + std::to_string(constrCont++));
				}
			}

			//RESTRICAO 4
			GRBLinExpr o1 = 0;
			for (i = 0; i < n; i++)
				if (i != s)
					o1 += x[s][i];
			model.addConstr(o1 == 1, std::to_string(constrCont++));

			//RESTRICAO 5
			GRBLinExpr d1 = 0;
			for (i = 0; i < n; i++)
				if (i != s)
					d1 += x[i][s];
			model.addConstr(d1 == 1, std::to_string(constrCont++));

			//RESTRICAO 6
			for (j = 0; j < n; j++) {
				if (j != s) {
					c = 0;
					c1 = 0;

					for (i = 0; i < n; i++)
						if (i != j)
							c += x[i][j];

					for (i = 0; i < n; i++)
						if (i != j)
							c1 += x[j][i];

					model.addConstr(c - c1 == 0, "rest6_idaVolta" + std::to_string(constrCont++));

				}
			}

			//RESTRICAO 7

			for (i = 1; i < n; i++) {
				for (j = 1; j < n; j++) {

					if (j != i) {
						c = 0;
						c += u[i] - u[j] + x[i][j] * (n - 1);
						model.addConstr(c <= n - 2, "rest7_evitarSubCiclo" + std::to_string(constrCont++));
					}

				}
			}

			//RESTRICAO 8
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					qc += vertices[i][0] * p[i] * x[i][j];
				}
			}

			model.addQConstr(qc >= K, "rest8_bonusMinimo");

			//***************************** RESOLUCAO *****************************************//
			

	//////////////////// FIM DA CONSTRUÇAO DO MODELO ////////////////////

    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);
	if (optimstatus != GRB_INFEASIBLE){
		cout << model.get(GRB_DoubleAttr_ObjVal)<<endl;
		for (int i=0; i<n; i++){
			for (int j=i+1; j<n; j++){
				try{
	        		if (x[j][i].get(GRB_DoubleAttr_X)>0){
	        			cout<<"("<<j<<","<<i<<") ";
	        		}
	        		else if(x[i][j].get(GRB_DoubleAttr_X)>0){
	        			cout<<"("<<i<<","<<j<<") ";
	        		}
        		} catch(GRBException ex){

      			}
			}
		}
	}
    cout<<endl;
	return 0;
}