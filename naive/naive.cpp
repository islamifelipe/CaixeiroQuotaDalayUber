#include "LKHParser.h"
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


const string LKH_PATH = "LKH-2.0.7/";
const string LK_FILES_PATH = "LK_FILES/";

int main(int argc, char *argv[])
{

	int s = 0; // vértice origem
	int n; // quantidade de vértices
	int l; // quantidade de passageiros
	int R; // capacidade de carro
	double K; // quota minima a ser coletada pelo caixeiro


	//////////////////// LEITURA DA INSTÂNCIA ////////////////////
	vector<int> set_vertices;
	vector< vector<double> > custos_vector;
	cin>>n;
	cin>>l;
	cin>>R;

	double **custos = new double*[n]; // matriz de custo das arestas
	double **tempo = new double*[n]; // matriz de tempo das arestas
	double **L = new double*[l]; // matriz lx4 de passageiros: tarifa maxxima, tempo maximo, origem e destino
	double **penalidades = new double*[l]; // matriz lxn de penalidades: penalidade[i][j] = penalidade de desembarcar i em j
	double **vertices = new double*[n]; // matriz nx2 do bonus e delay de cada vértice 


	for (int i=0; i<n; i++){ // inicializa e ler custos das arestas
		set_vertices.push_back(i);
		custos[i] = new double[n];
		vector<double> aulcv;
		for (int j=0; j<n; j++){
			cin>>custos[i][j];
			aulcv.push_back(custos[i][j]);
		}
		custos_vector.push_back(aulcv);
	
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
        L[i][2]--; /*a instância deve indicar a origem entre 1 e n*/
        L[i][3]--; /*idem*/
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
 	
 	env.set(GRB_IntParam_Method, 1); // 1 é mais rapido //primal
  	env.set(GRB_IntParam_SiftMethod,1);
    env.set(GRB_DoubleParam_TimeLimit, 10800); 
    env.set(GRB_DoubleParam_MIPGap,10);
    GRBModel model = GRBModel(env);; // cria modelo

	/****************** VARIAVEIS DO MODELO ******************/

	GRBVar *u; //usada pelas restricoes de subcilo
	GRBVar *q; // q[a] = 1 se o passageiro 0<=a<l embarcou
	GRBVar *p; // p[i] = 1 se o caixeiro apanha o bonus do vertice 0<=i<n
	int **x; //diz se o caixeiro passou pela aresta ij (VALORES FIXOS!!!!!!!!!!!)
	GRBVar **beta; // variavel de linearizaçao. Matriz nxn
	GRBVar **theta; // variavel de linearizaçao.  Matriz nxn
	GRBVar **f; //f[a][i] = 1 se o passageiro 0<=a<l desce no vértice 0<=i<n
	GRBVar ***psi; // variavel de linearizacao nxnxl
	GRBVar ***z; //z[i][j][a] 0<=i,j<n 0<=a<R
	GRBVar ***v; //v[i][j][a]=1 se o passageiro 'a' trafega pela aresta (i,j) 0<=i,j<n 0<=a<l 
    GRBVar ***mu; // linearizacao nxnxl
    GRBVar ***zeta; // linearizacao nxnxl


    GRBVar **taxaRateio = new GRBVar*[n];
	u = new GRBVar[n];
	q = new GRBVar[l];
	p = new GRBVar[n];
	x = new int*[n]; 
	f = new GRBVar*[l]; 
	beta = new GRBVar*[n];
	theta = new GRBVar*[n];
   	z = new GRBVar**[n];
   	v = new GRBVar**[n];  
    zeta = new GRBVar**[n];
    mu = new GRBVar**[n];
    psi = new GRBVar**[n];

    for (int i = 0; i < n; i++) {
        taxaRateio[i] = new GRBVar[n];
        for (int j = 0; j < n; j++) {
            taxaRateio[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "pcrtil" +std::to_string(i)+std::to_string(j));
        }
    }
   	for (int i=0; i<n;i++){
   		p[i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "p"+std::to_string(i));
        x[i] = new int[n];
        beta[i] = new GRBVar[n];
        theta[i] = new GRBVar[n];
        z[i] = new GRBVar*[n];
        v[i] = new GRBVar*[n];
        mu[i] = new GRBVar*[n];
        zeta[i] = new GRBVar*[n];
        psi[i] = new GRBVar*[n];
        for (int j=0; j<n; j++){
        	x[i][j] = 0;
        	z[i][j]   =  new GRBVar[R];
        	v[i][j]   =  new GRBVar[l];
            mu[i][j]  =  new GRBVar[l];
            zeta[i][j]=  new GRBVar[l];
            psi[i][j] =  new GRBVar[l];
           //if (i!=j){
        		for (int a=0; a<R; a++){
        			z[i][j][a] = model.addVar(0.0, 1, 0.0, GRB_CONTINUOUS, "z"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
        		}
        		for (int a=0; a<l; a++){
					v[i][j][a] = model.addVar(0, 1, 0.0, GRB_BINARY, "v"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
        		    mu[i][j][a] = model.addVar(0, 1, 0.0, GRB_BINARY, "mu"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
                    zeta[i][j][a] = model.addVar(0, 1, 0.0, GRB_BINARY, "zeta"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
                    psi[i][j][a] = model.addVar(0, 1, 0.0, GRB_CONTINUOUS, "psi"+std::to_string(i)+std::to_string(j)+std::to_string(a)); 
                    
                }
        	//}
        }
   	}

   	for (int a=0; a<l; a++){
   		q[a] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "q"+std::to_string(a));
   		f[a] = new GRBVar[n];
   		for (int i=0; i<n; i++){
   			f[a][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "f"+std::to_string(a)+std::to_string(i)); 
   		}
   	}

   	for (int i=0; i<n; i++){ // se o grafo for direcionado, exitira uma aresta ij e outra ji
		for (int j=i+1; j<n; j++){
			beta[i][j] = model.addVar(0.0, 100000, 0.0, GRB_CONTINUOUS, "beta"+std::to_string(i)+std::to_string(j)); 
			beta[j][i] = model.addVar(0.0, 100000, 0.0, GRB_CONTINUOUS, "beta"+std::to_string(j)+std::to_string(i));
			theta[i][j] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "theta"+std::to_string(i)+std::to_string(j)); 
			theta[j][i] = model.addVar(0.0, 1, 0.0, GRB_BINARY, "theta"+std::to_string(j)+std::to_string(i));
		}
	}

	for (int i=0; i<n; i++){
		u[i] = model.addVar(0.0, n-1, 0.0, GRB_CONTINUOUS, "u"+std::to_string(i));
	}

	model.update();

	/****************** EXECUCAO DO Lin-Kernighan ******************/


	cout<<"Rota do caixeiro dada pelo LK = ";
        LKHParser parser(LKH_PATH, LK_FILES_PATH, set_vertices, custos_vector, "augumacoisa", string("TSP"));
    vector<int> path = parser.LKHSolution();
        int custosdf = 0;
    for (int i=0; i<path.size()-1; i++){
        cout<<path[i]<<" ";
        custosdf +=  custos_vector[path[i]][path[i+1]];
    }
    cout<<path[path.size()-1]<<" ";
    custosdf +=  custos_vector[path[path.size()-1]][path[0]];
    cout<<"Custo TSP = "<<custosdf<<endl;

    /****************** PREENCHE A MATRIZ X[i][j] COM OS VALORES DE PATH ******************/

    for (int i=0; i<path.size()-1; i++){
    	x[path[i]][path[i+1]] = 1;
    }
    x[path[n-1]][path[0]] = 1;
	/****************** FUNCAO OBJETIVO DO MODELO ******************/

    GRBQuadExpr obj = 0;
    GRBQuadExpr xe = 0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i!=j){
                xe =  custos[i][j] * x[i][j] * taxaRateio[i][j];

                obj += xe ;
            }
        }
    }

    for (int ll = 0; ll < l; ll++) {
        for (int j = 0; j < n; j++) {

            xe = (penalidades[ll][j] * f[ll][j]);

            obj += xe;

        }
    }

    model.setObjective(obj, GRB_MINIMIZE);

    for (int i = 0; i < n; i++) {
        taxaRateio[i][i].set(GRB_DoubleAttr_UB, 0);
    }

    u[s].set(GRB_DoubleAttr_UB, 0);
    p[s].set(GRB_DoubleAttr_UB, 0);

    int constrCont = 0;

	/****************** RESTRICOES DO MODELO ******************/


	//Restricoes 1 e 2 e 6 - Estas restriçoes garantem que o tour do caixeiro se dá num subconjunto de vértices.
    
    for (int j=0; j<n; j++){
    	if (j!=s){ // para todo j diferente de 's'
    		 GRBLinExpr rest1 = 0;
    		 GRBLinExpr rest2 = 0;
    		 GRBLinExpr subtracao = 0; // RESTRICAO 6 que obriga que o caixeiro deve sempre sair de uma cidade que ele entrou (deferente de s)
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

    //Restricoes 4 e 5 - Estas restriçoes obrigam que o tour comece e termine em s.
    GRBLinExpr rest3 = 0;
   	GRBLinExpr rest4 = 0;
   	for (int i=0; i<n; i++){ // indice so somatorio 
	 	if (i!=s){ // indice do somatorio diferente de 's'
	 		rest3 = rest3 + x[s][i];
	 		rest4 = rest4 + x[i][s];
	 	}
	}
	model.addConstr(rest3, GRB_EQUAL, 1,std::to_string(constrCont++));
	model.addConstr(rest4, GRB_EQUAL, 1,std::to_string(constrCont++));


	//Restricao 7 - Restriçoes de subtour
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

     // Restricao 8 - LINEARIZADA

    // //RESTRICAO 9 // LINEARIZADA
    // for (int ll = 0; ll < l; ll++) {
    //     GRBQuadExpr quadConstraint = 0;
    //     for (int i = 0; i < n; i++) {

    //         for (int j = 0; j < n; j++) {
    //             if (j!=i)
    //                 quadConstraint += v[i][j][ll] * tempo[i][j] + p[i] * vertices[i][1] * v[i][j][ll];
    //         }
    //     }
    //     model.addQConstr(quadConstraint <= L[ll][1], "rest9_tempoLimitePassageiro" +std::to_string(constrCont++));
    // }

    // Restricao 10
    //TODO : verificar se é redundante: executar com esta restriçao desligada
    //acopla o trajeto dos passageiros ao do caixeiro, bem como garante que a capacidade R máxima do carro não seja ultrapassada. 
    for (int i=0; i<n; i++){
    	for (int j=0; j<n; j++){
    	 	if(i!=j){
    	 		GRBLinExpr sum1 = 0;
    	 		for (int ll=0; ll<l; ll++){
    	 			sum1 = sum1 + v[i][j][ll];
    	 		}
    	 		GRBLinExpr r2 = R*x[i][j];
    	 		model.addConstr(sum1, GRB_LESS_EQUAL, r2 ,std::to_string(constrCont++)); // L[ll][1] é o tempo maximo que o passageiro ll deseja ficar no carro
			}
    	}
    }
    // cout<<"Sem a restriçao 11\n"<<endl;
    //restricao 11
    for (int ll=0; ll<l; ll++){
    	GRBLinExpr sum1 = 0;
    	for (int j=0; j<n; j++){
    		sum1 = sum1 + f[ll][j];
    	}
    	model.addConstr(sum1, GRB_LESS_EQUAL, 1 ,std::to_string(constrCont++)); 
    }

    // //RESTRICAO 12 LINEARIZADA
    // for (int ll = 0; ll < l; ll++) {
    //     GRBQuadExpr  quadConstraint = 0;
    //     for (int i = 0; i < n; i++) {
    //         for (int j = 0; j < n; j++) {
    //             if (i!=j)
    //                 quadConstraint += v[i][j][ll] * custos[i][j] * taxaRateio[i][j];
    //         }
    //     }
    //     model.addQConstr(quadConstraint <= L[ll][0], std::to_string(constrCont++));

    // }


    /* ============================== Restriçoes linearizadas ============================== */

    // Restricao 8 - LINEARIZADA
    GRBLinExpr res5;
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            if(i!=j){
                GRBLinExpr res42, res43, res44; // aproveitamos esse laço pra calcular estas restricoes =)
                res42 = x[i][j] - beta[i][j];
                res43 = 1 - p[i];
                res44 = x[i][j] - p[i];
                
                model.addConstr(theta[i][j], GRB_EQUAL, res42 ,std::to_string(constrCont++)); 
                model.addConstr(beta[i][j], GRB_LESS_EQUAL, res43 ,std::to_string(constrCont++)); 
                model.addConstr(beta[i][j], GRB_GREATER_EQUAL, res44 ,std::to_string(constrCont++)); 
     
                res5 = res5 + theta[i][j]*vertices[i][0]; //vertices[i][0] é o bonus do vértice i
                
            }
        }
    }
    model.addConstr(res5, GRB_GREATER_EQUAL, K ,std::to_string(constrCont++)); 
    

    // RESTRICAO 9 LINEARIZADA
    for (int ll = 0; ll < l; ll++) {
        GRBLinExpr rest9 = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j!=i){
                    rest9 += v[i][j][ll] * tempo[i][j] + vertices[i][1] * mu[i][j][ll];
                    model.addConstr(mu[i][j][ll] == p[i] - zeta[i][j][ll] ,std::to_string(constrCont++));
                    model.addConstr(zeta[i][j][ll] <= 1 - v[i][j][ll] ,std::to_string(constrCont++));
                    model.addConstr(zeta[i][j][ll] >= p[i] - v[i][j][ll] ,std::to_string(constrCont++));
                }
            }
        }
        model.addConstr(rest9 <= L[ll][1], "rest9_tempoLimitePassageiro" +std::to_string(constrCont++));
    }


    //RESTRICAO 12  LINEARIZADA
    for (int ll = 0; ll < l; ll++) {
        GRBLinExpr  rest12 = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i!=j){
                    rest12 += custos[i][j] * psi[i][j][ll];
                    model.addConstr(psi[i][j][ll] <= v[i][j][ll],std::to_string(constrCont++));
                    model.addConstr(psi[i][j][ll] <=  taxaRateio[i][j] ,std::to_string(constrCont++));
                    model.addConstr(psi[i][j][ll] >= taxaRateio[i][j] - 1 + v[i][j][ll],std::to_string(constrCont++));
                    
                }
            }
        }
        model.addConstr(rest12 <= L[ll][0], std::to_string(constrCont++));

    }


    //RESTRICAO 13 A - LINEARIZADA - CALCULO DA TAXA DE RATEIO
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                GRBLinExpr kdjfkdf = taxaRateio[i][j];
                for (int ll = 0; ll < l; ll++) {
                    kdjfkdf += psi[i][j][ll];
                }
                model.addConstr(kdjfkdf == 1.0, "restA_quantidadeDePassageirosNaAresta1" + std::to_string(constrCont++));
            }
        }
    }

            //******************************************************** NOVAS **********************************************************

            // //RESTRICAO 13 A - LINEARIZADA - CALCULO DA TAXA DE RATEIO
            // for (int i = 0; i < n; i++) {
            //     for (int j = 0; j < n; j++) {
                    
            //         if (i != j) {
            //             GRBLinExpr constraint = 1;
            //             for (int ll = 0; ll < l; ll++) {
            //                 constraint += v[i][j][ll];
            //             }
            //             model.addQConstr((constraint)* taxaRateio[i][j] == 1.0, "restA_quantidadeDePassageirosNaAresta" + std::to_string(constrCont++));
            //         }

            //     }
            // }

            //RESTRICAO 14 B
            for (int ll = 0; ll < l; ll++) {
                int oriem =(int) L[ll][2];
                GRBLinExpr constraint = f[ll][oriem];
                model.addConstr(constraint == 0, "restB_passageiroNaoDesembarcaNaSuaOrigem" +  std::to_string(constrCont++));

            }

            //RESTRICAO 15 C
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        for (int ll = 0; ll < l; ll++) {
                            model.addConstr(v[i][j][ll] <= q[ll], "restC_soPassaPassageiroPorOndePassaVeiculo" +  std::to_string(constrCont++));
                            
                        }
                    }
                }
            }

            //RESTRICAO 16 D 
            for (int ll = 0; ll < l; ll++) {
                for (int j = 0; j < n; j++) {
                    if (L[ll][2]!=j){
                        GRBLinExpr constraint = 0;
                        GRBLinExpr constraint2 = 0;
                        for (int i = 0; i < n; i++) {
                            if (i != j) {
                                constraint += v[i][j][ll];
                                constraint2 += v[j][i][ll];
                            }
                        }
                        model.addConstr(constraint - constraint2 == f[ll][j], "restD_PassageiroDesembarcaConformeValorDeF" +  std::to_string(constrCont++));
                    }
                }
            }


            //RESTRICAO 17 E
            for (int ll = 0; ll < l; ll++) {

                GRBLinExpr constraint = 0;

                for (int j = 0; j < n; j++) {
                    constraint += f[ll][j];
                }

                model.addConstr(constraint - q[ll] == 0, "restE_SePassageiroEmbarcaEntaoForcaDesembarque" +  std::to_string(constrCont++));

            }


            //RESTRICAO 18 F
            for (int ll = 0; ll < l; ll++) {
                
                int i = L[ll][2];
                
                GRBLinExpr constraint = 0;
                
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        constraint += v[i][j][ll];
                    }
                }

                model.addConstr(constraint == q[ll], "restF_PassageiroEhEmbarcadoApenasNaOrigem" + std::to_string(constrCont++));
                //model.addConstr(constraint <= 1, "restF_PassageiroEhEmbarcadoApenasNaOrigem" + std::to_string(constrCont++));

            }

            //RESTRICAO 19
        for (int ll=0; ll<l; ll++){
            for (int i=0; i<n; i++){
                 GRBLinExpr sum1 = 0;
                 for (int j=0; j<n; j++){
                     if (i!=j){
                         sum1 = sum1 + v[i][j][ll];
                     }
                 }
                 sum1 = sum1 + f[ll][i];
                 model.addConstr(sum1, GRB_LESS_EQUAL, 1 ,std::to_string(constrCont++)); 
            }
        }




            //garante que na aresta ij, onde j é origem de l, nao trafega l
        //RESTRICAO 20
            for (int ll=0; ll<l; ll++){
                for (int j=0; j<n; j++){
                    if (L[ll][2] == j){
                        GRBLinExpr constraint = 0;
                        for (int i = 0; i < n; i++) {
                            if (i != j) {
                                constraint += v[i][j][ll];
                            }
                        }
                        model.addConstr(constraint == 0, "restG_passageiroNaoVolta" + std::to_string(constrCont++));
                
                    }
                }
            }

            //RESTRICAO 21
            for (int ll=0; ll<l; ll++){
                GRBLinExpr srs1;
                GRBLinExpr srs2;
                 srs1 = f[ll][s];
                 for (int j=0; j<n; j++){
                     if (j!=s){
                         srs2 = srs2 + v[j][s][ll];
                     }
                 }
                model.addConstr(srs1, GRB_EQUAL, srs2 ,std::to_string(constrCont++)); 
            
            }

            //garante que so sai da origem que estiver na origem (origem do caixero)
                
            //RESTRICAO 22
             GRBLinExpr sum1 = 0;
             GRBLinExpr sum2 = 0;
             for (int ll=0; ll<l; ll++){
                 if (L[ll][2] == s) {// origem do passageiro ll
                     sum1 = sum1 + q[ll];
                 }
                 for (int j=0; j<n; j++){
                     if (s!=j){
                         sum2 = sum2 + v[s][j][ll];
                     }
                 }
             }
             model.addConstr(sum2 <= sum1,std::to_string(constrCont++)); 
           


	//////////////////// FIM DA CONSTRUÇAO DO MODELO ////////////////////

    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);
	if (optimstatus != GRB_INFEASIBLE){
        cout << model.get(GRB_DoubleAttr_ObjVal)<<endl;
		for (int i=0; i<n; i++){
			for (int j=i+1; j<n; j++){
				try{
	        		if (x[j][i] == 1){
                        cout<<"("<<j+1<<","<<i+1<<") ";
                    } else if (x[i][j]==1){
	        			cout<<"("<<i+1<<","<<j+1<<") ";
	        		}

                    if (x[i][j]==1 && x[j][i] == 1){
                        cout<<"ERRO: ISSO NAO DEVERIA ACONTECER!!!"<<endl;
                    }
        		} catch(GRBException ex){

      			}
               

			}
		}
        for (int ll=0; ll<l; ll++){
            if (q[ll].get(GRB_DoubleAttr_X)>0){
                cout<<"Passageiro "<<ll<<" embarcou em "<<L[ll][2]+1<<" ";
                for (int i=0; i<n; i++){
                    if (f[ll][i].get(GRB_DoubleAttr_X)>0){
                        cout<<"e desembarcou em "<<i+1;
                    }
                }
                cout<<endl;
            }

        }
        for (int i=0; i<n; i++){
            for (int j=0; j<n; j++){
                if (i!=j){
                    cout<<"x_"<<i<<j<<" = "<<x[i][j]<<endl;
            
                }
            }
        }
        for (int ll=0; ll<l; ll++){
            for (int i=0; i<n; i++){
                cout<<"f_"<<ll<<i<<" = "<<f[ll][i].get(GRB_DoubleAttr_X)<<endl;
                
            }              
        }
        for (int ll=0; ll<l; ll++){
            cout<<"q_"<<ll<<" = "<<q[ll].get(GRB_DoubleAttr_X)<<endl;
                           
        }
        for (int i=0; i<n; i++){
            for (int j=0; j<n; j++){
                if (i!=j){
                    for (int ll=0; ll<l; ll++){
                        try{
                            //if (v[j][i][ll].get(GRB_DoubleAttr_X)==1){
                                cout<<"v_"<<i<<j<<"^"<<ll<<" = "<<v[i][j][ll].get(GRB_DoubleAttr_X)<<endl;
                            //}
                        }catch(GRBException ex){

                        }

                    }
                }
            }
        }
	}
    cout<<endl;
	return 0;
    
}
