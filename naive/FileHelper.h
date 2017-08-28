#ifndef FILE_HELPER_H
#define FILE_HELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class FileHelper {
public:
    void createFile(string name, string content, string path);
    string readFile(string path);
    string getFileNameFromPath(string path);
    void deleteFile(string path);
};

#endif // FILE_HELPER_H