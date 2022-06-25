#pragma once

#include "CRU.hpp"
#include "Lexer.hpp"
#include "Node.hpp"

class Main {
    int compileMode;
    int devMode;
    int doLink;
    int doRename;
    int doO2;

    int langMode;
    int debugMode;
    string fileName;
    string version;

    string runCode;
    string fileData;

    int isEV3;
    int isRun;

    void open();
    void write();
    void run();
    void cmdArg(int i, char *arg[], int Big);

    vector<tokens> token;

  public:
    Main(int n, char *arg[]);
    void cru();
};

void cmdArg(int i, char *arg[]);
