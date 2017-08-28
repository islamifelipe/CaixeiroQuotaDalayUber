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
	vector<int> set;
	set.push_back(0);
	set.push_back(1);
	set.push_back(2);
	set.push_back(3);
	vector< vector<double> > custos;
	double ccuaux[4][4] = {{0,10,65,25}, 
	{100,0,15,32},
	{65,150,0,20},
	{25,32,200,0} };
	for (int i=0; i<4; i++){
		vector<double> l;
		for (int j=0; j<4; j++){
			l.push_back(ccuaux[i][j]);
		}
		custos.push_back(l);
	}
	LKHParser parser(LKH_PATH, LK_FILES_PATH, set, custos, "INST-E-4-3", string("ATSP"));
    vector<int> path = parser.LKHSolution();
    for (int i=0; i<path.size(); i++){
    	cout<<path[i]<<" ";
    }
    cout<<endl;
    
}
