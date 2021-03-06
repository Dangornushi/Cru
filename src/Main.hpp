#pragma once

#include "CRU.hpp"
#include "Lexer/Lexer.hpp"
#include "Node/Node.hpp"

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
    int isOS;

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
