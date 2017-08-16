/*
#=======================================================================
# Islame Felipe DA COSTA FERNANDES --- Copyright 2017
#-----------------------------------------------------------------------
# ...
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
using namespace std;


int main(){
	
	int n; // quantidade de vértices
	int l; // quantidade de passageiros
	int R; // capacidade de carro
	double K; // quota minima a ser coletada pelo caixeiro


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
 	env.set("OutputFlag","0"); // desliga o output do solver
 	GRBModel model = GRBModel(env);; // cria modelo

 	env.set(GRB_IntParam_Method, 1); // 1 é mais rapido //primal
  	env.set(GRB_IntParam_SiftMethod,1);

	/****************** VARIAVEIS DO MODELO ******************/

	GRBVar *u; //usada pelas restricoes de subcilo
	GRBVar **x; //diz se o caixeiro passou pela aresta ij

	x = new GRBVar*[n]; 
   	u = new GRBVar[n];

   	for (int i=0; i<n;i++){
        x[i] = new GRBVar[n];
   	}

	/****************** FUNCAO OBJETIVO E RESTRICOES DO MODELO ******************/

   	for (int i=0; i<n; i++){ // se o grafo for direcionado, exitira uma aresta ij e outra ji
		for (int j=i+1; j<n; j++){
			x[i][j] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "x"+std::to_string(i)+std::to_string(j)); 
			x[j][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "x"+std::to_string(j)+std::to_string(i));
		}
	}

	for (int i=1; i<n; i++){
		u[i] = model.addVar(0.0, 10000000, 0.0, GRB_CONTINUOUS, "u"+std::to_string(i));
	}

	model.update();
   
   	int constrCont = 0;

    GRBLinExpr exprObjet;
    GRBLinExpr rest1,nova;
    double um = 1;
    for (int i=0; i<n; i++){
    	for (int j=i+1; j<n; j++){
    		exprObjet.addTerms(&custos[i][j], &x[i][j],1);  // sentido direto (conceitual) da aresta
   			exprObjet.addTerms(&custos[i][j], &x[j][i],1); // sentido inverso (conceitual) da aresta
   		
    	}
   	}
   	model.setObjective(exprObjet,GRB_MINIMIZE); 
   	for (int i=1; i<n; i++){
   		rest1.addTerms(&um, &x[i][0],1); 
   		
   	}
  	model.addConstr(rest1, GRB_EQUAL, 1,std::to_string(constrCont++));
	  
  	for (int j=1; j<n; j++){ // para todo j
  		GRBLinExpr rest2;
  		for (int i=0; i<n; i++){
	        if (i!=j){ 
	         	rest2.addTerms(&um,&x[j][i],1);
	        }
      	}
      	model.addConstr(rest2, GRB_EQUAL, 1,std::to_string(constrCont++));
	}

	for (int i=1; i<n; i++){ // para todo i
    	GRBLinExpr rest3;
  		for (int j=0; j<n; j++){
    		if (i!=j){ 
    			rest3.addTerms(&um,&x[j][i],1); 
    		}
  		}
       	model.addConstr(rest3, GRB_EQUAL, 1,std::to_string(constrCont++));
    }


    double menos1=-1;
    double p = n-1;
    for (int i=1; i<n; i++){
    	for (int j=1; j<n; j++){
    	 	if(i!=j){
    	 		GRBLinExpr rest4;
    			rest4.addTerms(&um,&u[i],1);
    	 		rest4.addTerms(&menos1,&u[j],1);
    	 		rest4.addTerms(&p,&x[i][j],1); // grafo nao completo
         		model.addConstr(rest4, GRB_LESS_EQUAL, p-1,std::to_string(constrCont++));
  			 
          	}
    	}
    }

	//////////////////// FIM DA CONSTRUÇAO DO MODELO ////////////////////

    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);
	if (optimstatus != GRB_INFEASIBLE){
		for (int i=0; i<n; i++){
			for (int j=i+1; j<n; j++){
				try{
	        		if (x[j][i].get(GRB_DoubleAttr_X)==1 || x[i][j].get(GRB_DoubleAttr_X)==1){
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