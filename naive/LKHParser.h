#ifndef LKH_PARSER_H
#define LKH_PARSER_H

#include <iostream>
#include "FileHelper.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class LKHParser {
private:

    vector<int> set;
    vector<int> bestPath;
    string nameInstance;
    // TSP or ATSP (assymmetric)
    string tspType;
    vector < vector<double> > costs;

    string LKH_PATH;
    string LK_FILES_PATH;

    void createTSPInstance();

public:

    LKHParser(string LKH_PATH, string LK_FILES_PATH, vector<int> set, vector < vector<double> > costs, string nameInstance, string tspType);
    vector<int> LKHSolution();

};

#endif //LKH_PARSER_H