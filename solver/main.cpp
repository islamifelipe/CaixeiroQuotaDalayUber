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
	




	/// TODO: precisa ser revisado TSP
	GRBEnv env = GRBEnv();;
 	env.set("OutputFlag","0");
	//env.set(GRB_DoubleParam_IterationLimit,MAXITERACOES);
 	//env.set(GRB_DoubleParam_Heuristics, 0); // 0% do tempo aplicado a heuristica do root
  

  	//env.set(GRB_IntParam_NodeMethod, 0);
 	//env.set(GRB_IntParam_Presolve, 0);
  	env.set(GRB_IntParam_Method, 1); // 1 é mais rapido //primal
  	env.set(GRB_IntParam_SiftMethod,1);
	GRBModel model = GRBModel(env);;

	//inicializa variaveis;
	for (int i=0; i<g->getQuantArestas(); i++){ // se o grafo for direcionado, exitira uma aresta ij e outra ji
		int origem = g->getArestas(i)->getOrigem();
		int destino = g->getArestas(i)->getDestino();
		x[origem][destino] = model.addVar(0.0, 1, 0.0, GRB_CONTINUOUS, "x"+std::to_string(origem)+std::to_string(destino)); // relaxacao linear
		x[destino][origem] = model.addVar(0.0, 1, 0.0, GRB_CONTINUOUS, "x"+std::to_string(destino)+std::to_string(origem));
	}///GRB_BINARY
	for (int i=1; i<g->getQuantVertices(); i++){
		u[i] = model.addVar(0.0, 10000000, 0.0, GRB_CONTINUOUS, "u"+std::to_string(i));
	}

   model.update();
   int constrCont = 0;
   GRBLinExpr exprObjet;
   GRBLinExpr rest1,nova;
   double um = 1;
   for (int i=0; i<g->getQuantArestas(); i++){
   		double peso = g->getArestas(i)->getPeso();
   		exprObjet.addTerms(&peso, &x[g->getArestas(i)->getOrigem()][g->getArestas(i)->getDestino()],1); 
   		exprObjet.addTerms(&peso, &x[g->getArestas(i)->getDestino()][g->getArestas(i)->getOrigem()],1); //destino origem também ??????
   		
      if (g->getArestas(i)->getDestino()==0 || g->getArestas(i)->getOrigem()==0){
   			rest1.addTerms(&um, &x[g->getArestas(i)->getOrigem()][g->getArestas(i)->getDestino()],1);
   		//	rest1.addTerms(&um, &x[g->getArestas(i)->getDestino()][g->getArestas(i)->getOrigem()],1);
   		
      }
   }
 
   model.setObjective(exprObjet,GRB_MINIMIZE); 
  model.addConstr(rest1, GRB_EQUAL, 1,std::to_string(constrCont++));
   //model.update();

   for (int j=1; j<g->getQuantVertices(); j++){
  		GRBLinExpr rest2;
  		for (int i=0; i<g->getQuantVertices(); i++){
        if (i!=j){ 
          if (g->isAdjacente(j,i)!=-1){// sai de j
            rest2.addTerms(&um,&x[j][i],1);
          } 
        }
      }
      model.addConstr(rest2, GRB_EQUAL, 1,std::to_string(constrCont++));

    }
    //cout<<"\n\n"<<endl;
    for (int i=1; i<g->getQuantVertices(); i++){
      GRBLinExpr rest3;
      for (int j=0; j<g->getQuantVertices(); j++){
        if (i!=j){ 
          if (g->isAdjacente(j,i)!=-1){// sai de j
            rest3.addTerms(&um,&x[j][i],1);
           // cout<<"+1x"<<j<<i<<" ";
          } 
        }
      }
       model.addConstr(rest3, GRB_EQUAL, 1,std::to_string(constrCont++));
     
    }
  	
   double menos1=-1;
   double p = g->getQuantVertices()-1;
    for (int i=1; i<g->getQuantVertices(); i++){
    	 for (int j=1; j<g->getQuantVertices(); j++){
    	 	if(i!=j && g->isAdjacente(i,j)!=-1){
    	 		GRBLinExpr rest4;
          //cout<<"+ "<<um<<"u"<<i<<" ";
    			rest4.addTerms(&um,&u[i],1);
    	 		rest4.addTerms(&menos1,&u[j],1);
         // cout<<"- 1"<<"u"<<j<<" ";
    	 		rest4.addTerms(&p,&x[i][j],1); // grafo nao completo
         // cout<<"+ "<<p<<"x"<<i<<j<<" ";
    	 		model.addConstr(rest4, GRB_LESS_EQUAL, p-1,std::to_string(constrCont++));
  			  //cout<<"<= "<<p-1<<endl;
          }
    	 }
    }
     model.optimize();
      // model.update();
	int optimstatus = model.get(GRB_IntAttr_Status);
	vector<Aresta*> caminho;
	Informacao ret = {caminho,0};
	if (optimstatus != GRB_INFEASIBLE){
		for (int i=0; i<g->getQuantArestas(); i++){
			try{
        if (x[g->getArestas(i)->getDestino()][g->getArestas(i)->getOrigem()].get(GRB_DoubleAttr_X)>=0.1 || x[g->getArestas(i)->getOrigem()][g->getArestas(i)->getDestino()].get(GRB_DoubleAttr_X)>=0.1){
         //  cout<<"x"<<g->getArestas(i)->getOrigem()<<"_"<<g->getArestas(i)->getDestino()<<" = "<<x[g->getArestas(i)->getOrigem()][g->getArestas(i)->getDestino()].get(GRB_DoubleAttr_X)<<endl;
         // cout<<"x"<<g->getArestas(i)->getDestino()<<"_"<<g->getArestas(i)->getOrigem()<<" = "<<x[g->getArestas(i)->getDestino()][g->getArestas(i)->getOrigem()].get(GRB_DoubleAttr_X)<<endl;
          
          ret.caminho.push_back(g->getArestas(i));
          ret.custo+=g->getArestas(i)->getPeso();
        }
      } catch(GRBException ex){

      }
		}
	} 

	return ret;
	return 0;
}