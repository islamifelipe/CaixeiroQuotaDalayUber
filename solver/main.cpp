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
	int s = 0; // vértice origem
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
	GRBVar *q; // q[a] = 1 se o passageiro 0<=a<l embarcou
	GRBVar *p; // p[i] = 1 se o caixeiro apanha o bonus do vertice 0<=i<n
	GRBVar **x; //diz se o caixeiro passou pela aresta ij
	GRBVar **beta; // variavel de linearizaçao. Matriz nxn
	GRBVar **alfa; // variavel de linearizaçao.  Matriz nxn
	GRBVar **f; //f[a][i] = 1 se o passageiro 0<=a<l desce no vértice 0<=i<n
	GRBVar **phi; // variavel de linearizaçao. Matriz lxn
	GRBVar **psi; // variavel de linearizaçao. Matriz lxn
	GRBVar **delta; // variavel de linearizaçao. Matriz lxn
	GRBVar ***z; //z[i][j][a] 0<=i,j<n 0<=a<R
	GRBVar ***v; //v[i][j][a]=1 se o passageiro 'a' trafega pela aresta (i,j) 0<=i,j<n 0<=a<l 



	u = new GRBVar[n];
	q = new GRBVar[l];
	p = new GRBVar[n];
	x = new GRBVar*[n]; 
	f = new GRBVar*[l]; 
	psi = new GRBVar*[l]; 
	phi = new GRBVar*[l]; 
	delta = new GRBVar*[l]; 
	beta = new GRBVar*[n];
	alfa = new GRBVar*[n];
   	z = new GRBVar**[n];
   	v = new GRBVar**[n];  

   	for (int i=0; i<n;i++){
   		p[i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "p"+std::to_string(i));
        x[i] = new GRBVar[n];
        beta[i] = new GRBVar[n];
        alfa[i] = new GRBVar[n];
        z[i] = new GRBVar*[n];
        v[i] = new GRBVar*[n];
        for (int j=0; j<n; j++){
        	z[i][j] =  new GRBVar[R];
        	v[i][j] =  new GRBVar[l];
        	if (i!=j){
        		for (int a=0; a<R; a++){
        			z[i][j][a] = model.addVar(0.0, 1, 0.0, GRB_CONTINUOUS, "z"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
        		}
        		for (int a=0; a<l; a++){
					v[i][j][a] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "v"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
        		}
        	}
        }
   	}

   	for (int a=0; a<l; a++){
   		q[a] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "q"+std::to_string(a));
   		f[a] = new GRBVar[n];
   		psi[a] = new GRBVar[n];
   		phi[a] = new GRBVar[n];
   		delta[a] = new GRBVar[n];
   		for (int i=0; i<n; i++){
   			f[a][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "f"+std::to_string(a)+std::to_string(i)); 
   			psi[a][i] = model.addVar(0.0,100000,0.0, GRB_CONTINUOUS, "psi"+std::to_string(a)+std::to_string(i));
   			phi[a][i] = model.addVar(0.0,1,0.0, GRB_BINARY, "phi"+std::to_string(a)+std::to_string(i));
   			delta[a][i] = model.addVar(0.0,100000,0.0, GRB_CONTINUOUS, "delta"+std::to_string(a)+std::to_string(i));
   			
   		}
   	}

   	for (int i=0; i<n; i++){ // se o grafo for direcionado, exitira uma aresta ij e outra ji
		for (int j=i+1; j<n; j++){
			x[i][j] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "x"+std::to_string(i)+std::to_string(j)); 
			x[j][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "x"+std::to_string(j)+std::to_string(i));
			beta[i][j] = model.addVar(0.0, 100000, 0.0, GRB_CONTINUOUS, "beta"+std::to_string(i)+std::to_string(j)); 
			beta[j][i] = model.addVar(0.0, 100000, 0.0, GRB_CONTINUOUS, "beta"+std::to_string(j)+std::to_string(i));
			alfa[i][j] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "alfa"+std::to_string(i)+std::to_string(j)); 
			alfa[j][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "alfa"+std::to_string(j)+std::to_string(i));
		}
	}

	for (int i=1; i<n; i++){
		u[i] = model.addVar(0.0, n-1, 0.0, GRB_CONTINUOUS, "u"+std::to_string(i));
	}

	model.update();

	/****************** FUNCAO OBJETIVO DO MODELO ******************/

   	int constrCont = 0;

    GRBLinExpr exprObjet;
    double um = 1;
    double menos1=-1;

    for (int i=0; i<n; i++){
    	for (int j=i+1; j<n; j++){
    		exprObjet = exprObjet + custos[i][j]*x[i][j];
    		//exprObjet.addTerms(&custos[i][j], &x[i][j],1);  // sentido direto (conceitual) da aresta
   			GRBLinExpr newsum;
   			for (int aa=0; aa<R; aa++){
   				int a = aa+1; // aqui fazemos de 0...R-1. Mas o modelo pede de 1...R
   				double coef = custos[i][j]/(a*(a+1));
   				newsum = newsum + coef*z[i][j][aa];
				//newsum.addTerms(&coef, &z[i][j][aa],1);  // sentido direto (conceitual) da aresta
   			}
   			exprObjet = exprObjet - newsum;
    	}
    }
    GRBLinExpr secondPartObj;
    for (int ll=0; ll<l; ll++){
    	for (int j=0; j<n; j++){
    		secondPartObj = secondPartObj + penalidades[ll][j]*f[ll][j];
    	}
    }

    exprObjet = secondPartObj + exprObjet;
	model.setObjective(exprObjet,GRB_MINIMIZE); 
   	
	/****************** RESTRICOES DO MODELO ******************/


	//Estas restriçoes garantem que o tour do caixeiro se dá num subconjunto de vértices.
    for (int j=0; j<n; j++){
    	if (j!=s){ // para todo j diferente de 's'
    		 GRBLinExpr rest1;
    		 GRBLinExpr rest2;
    		 GRBLinExpr subtracao; // RESTRICAO que obriga que o caixeiro deve sempre sair de uma cidade que ele entrou (deferente de s)
    		 for (int i=0; i<n; i++){
    		 	if (i!=j){ // indice do somatorio diferente do j
    		 		rest1 = rest1 + x[i][j];
    		 		rest2 = rest2 + x[j][i];
    		 	}
    		 }
    		 subtracao = rest1 - rest2;
    		 model.addConstr(rest1, GRB_LESS_EQUAL, 1,std::to_string(constrCont++));
    		 model.addConstr(rest2, GRB_LESS_EQUAL, 1,std::to_string(constrCont++));
    		 model.addConstr(subtracao, GRB_EQUAL, 0,std::to_string(constrCont++));		
    	}
    }

    //Estas restriçoes obrigam que o tour comece e termine em s.
    GRBLinExpr rest3;
   	GRBLinExpr rest4;
   	for (int i=0; i<n; i++){ // indice so somatorio 
	 	if (i!=s){ // indice do somatorio diferente de 's'
	 		rest3 = rest3 + x[s][i];
	 		rest4 = rest4 + x[i][s];
	 	}
	}
	model.addConstr(rest3, GRB_EQUAL, 1,std::to_string(constrCont++));
	model.addConstr(rest4, GRB_EQUAL, 1,std::to_string(constrCont++));


	//Restriçoes de subtour
	double pp = n-1;
    for (int i=1; i<n; i++){
    	for (int j=1; j<n; j++){
    	 	if(i!=j){
    	 		GRBLinExpr rest5;
    	 		rest5 = u[i] - u[j] + (n-1)*x[i][j];
    			model.addConstr(rest5, GRB_LESS_EQUAL, pp-1,std::to_string(constrCont++));
          	}
    	}
    }

    //garantem que cada passageiro permaneça no carro no maaximo L[ll][1] unidades de tempo.
    for (int ll=0; ll<l; ll++){
    	GRBLinExpr sum1;
    	GRBLinExpr sum2;
    	for (int i=0; i<n; i++){
    		sum2 = sum2 + p[i]*vertices[i][1]; // vertices[i][1] delay associado ao vértice i
    		for (int j=0; j<n; j++){
    			if (i!=j)
    				sum1 = sum1 + v[i][j][ll]*tempo[i][j];
    		}
    	}
    	GRBLinExpr restr = sum1 + sum2;
    	model.addConstr(restr, GRB_LESS_EQUAL, L[ll][1] ,std::to_string(constrCont++)); // L[ll][1] é o tempo maximo que o passageiro ll deseja ficar no carro

    }

    //TODO : verificar se é redundante: executar com esta restriçao desligada
    //acopla o trajeto dos passageiros ao do caixeiro, bem como garante que a capacidade R máxima do carro não seja ultrapassada. 
    for (int i=0; i<n; i++){
    	for (int j=0; j<n; j++){
    	 	if(i!=j){
    	 		GRBLinExpr sum1;
    	 		for (int ll=0; ll<l; ll++){
    	 			sum1 = sum1 + v[i][j][ll];
    	 		}
    	 		GRBLinExpr r2 = R*x[i][j];
    	 		model.addConstr(sum1, GRB_LESS_EQUAL, r2 ,std::to_string(constrCont++)); // L[ll][1] é o tempo maximo que o passageiro ll deseja ficar no carro
			}
    	}
    }

    // realiza o acoplamento entre as variáveis do ciclo hamiltoniano, variáveis de carregamento e transporte de passageiros
    for (int i=0; i<n; i++){
    	for (int j=0; j<n; j++){
    	 	if(i!=j){
    	 		GRBLinExpr sum1;
    	 		GRBLinExpr sum2;
    	 		GRBLinExpr sum3;
    	 		GRBLinExpr dir;
    	 		GRBLinExpr res;
    	 		for (int ll=0; ll<l; ll++){
    	 			if (L[ll][2] == i) {// origem do passageiro ll
    	 				sum1 = sum1 + q[ll];
    	 			}
    	 			sum3 = sum3 + f[ll][i];
    	 		}
    	 		for (int aa = 0; aa<n; aa++){
    	 			if (aa!=i){
    	 				for (int ll=0; ll<l; ll++){
    	 					sum2 = sum2 + v[aa][i][ll];
    	 				}
    	 			}
    	 		}
    	 		res = sum1 + sum2 - sum3;
    	 		dir = R*x[i][j];
    	 		model.addConstr(res, GRB_LESS_EQUAL, dir ,std::to_string(constrCont++)); // L[ll][1] é o tempo maximo que o passageiro ll deseja ficar no carro
			}
    	}
    }


    // TODO: verificar se é redundante
    for (int ll=0; ll<l; ll++){
    	for (int i=0; i<n; i++){
    		if (L[ll][2] == i) {// origem do passageiro ll
    			GRBLinExpr sum1;
    			for (int j=0; j<n; j++){
    				if (i!=j){
    					sum1 = sum1 + v[i][j][ll];
    				}
    			}
    			model.addConstr(sum1, GRB_GREATER_EQUAL, q[ll] ,std::to_string(constrCont++)); 
				model.addConstr(sum1, GRB_LESS_EQUAL, 1 ,std::to_string(constrCont++)); 
			}
    	}
    }

 	for (int ll=0; ll<l; ll++){
    	for (int i=0; i<n; i++){
    		GRBLinExpr sum1;
			for (int j=0; j<n; j++){
				if (i!=j){
					sum1 = sum1 + v[i][j][ll];
				}
			}
			sum1 = sum1 + f[ll][i];
			model.addConstr(sum1, GRB_LESS_EQUAL, 1 ,std::to_string(constrCont++)); 
		}
    }

    
    // TODO: verificar se é redundante
    for (int ll=0; ll<l; ll++){
    	GRBLinExpr sum1;
    	for (int j=0; j<n; j++){
    		sum1 = sum1 + f[ll][j];
    	}
    	model.addConstr(sum1, GRB_LESS_EQUAL, 1 ,std::to_string(constrCont++)); 
    }

    for (int i=0; i<n; i++){
    	GRBLinExpr sum1;
    	GRBLinExpr sum2;
    	for (int ll=0; ll<l; ll++){
    		if (L[ll][2] == i) {// origem do passageiro ll
    			sum1 = sum1 + q[ll];
    		}
    		for (int j=0; j<n; j++){
    			if (i!=j){
    				sum2 = sum2 + v[i][j][ll];
    			}
    		}
    	}
    	model.addConstr(sum1, GRB_LESS_EQUAL, sum2 ,std::to_string(constrCont++)); 
    }


    GRBLinExpr srs1;
    GRBLinExpr srs2;
    for (int ll=0; ll<l; ll++){
    	srs1 = srs1 + f[l][s];
    	for (int j=0; j<n; j++){
    		if (j!=s){
    			srs2 = srs2 + v[j][s][ll];
    		}
    	}
    }
    // TODO: GRB_EQUAL or GRB_LESS_EQUAL?????
    model.addConstr(srs1, GRB_EQUAL, srs2 ,std::to_string(constrCont++)); 
    

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