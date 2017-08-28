#include "FileHelper.h"

void FileHelper::createFile(string name, string content, string path) {
    ofstream file;
    string _path = path + name;
    file.open (_path.c_str());
    file << content;
    file.close();
}

string FileHelper::readFile(string path) {

    string line;
    string file = "";
    ifstream myfile (path.c_str());
    if (myfile.is_open()) {
        while ( getline (myfile, line) ) {
            file+= line + "\n";
        }
        myfile.close();
    } else {
        cout << "Unable to open file";
    }

    return file;
}

string FileHelper::getFileNameFromPath(string path) {
    int idx = path.find_last_of(".");
    string filename = "";
    if (idx != -1) {
        for (int i = idx - 1; path[i] != '/'; i--) {
            filename = path[i] + filename;
        }
    } else {
        filename = "Error: Invalid format path or file";
    }

    return filename;
}

void FileHelper::deleteFile(string path) {
    if( remove(path.c_str()) != 0 )
        perror("Error deleting file");
}
