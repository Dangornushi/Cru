#pragma once

#include "../CRU.hpp"
#include "../Lexer/Lexer.hpp"

#include <cctype>

#define INFUNC 0
#define INCLASS 1
#define INSUBANDMAINCLASS 2
#define INFILE 256

#define INT 0
#define STR 1
#define AUTO 2

#define PY_D_C 0
#define C_DEF 1
#define C_CALL 2

#define i8 1
#define I32 4
#define I64 8

struct defArg {
    string type;
    string name;
};

typedef struct {
    string name;
    string regName;
    string type;
    string len;
    string outputFormatSpecifier;
} Type;

typedef struct {
    map<string, Type> Reg;
    map<string, string> llirReg;
    int registerAmount;
} Register;

typedef struct {
    vector<Type> argVars;
    string argMove;
    string returnFunctionArgument;
} ReturnArgumentAndMove;

struct Node {

    Node(int langMode);

    vector<tokens> token;
    Register Regs;

    vector<string> menberFunctions;
    map<string, string> classAndInstance;

    map<string, string> typeSize;
    map<string, string> llirType;
    map<string, string> regType;

    int tokNumCounter;
    int indent;
    int langMode;
    int classEnabled;
    int enumEnabled;
    int registerAmount;
    int strAmount;
    int putDefExists;
    int funcDefQuantity;

    void expect(string str);
    string nowClassName;
    string nowFuncName;
    string nowInstanceName;
    string nowType;
    string LLIRnowVar;
    string strDefine;
    string loads;
    string functionDefine;
    string addIndent();
    string parse(vector<tokens> geToken);

    string eval();
    string loop();
    string comparison(int i, string ret);
    ReturnArgumentAndMove funcDefArgument();
    string funcCallArtgment();

    string functionDefinition();
    string funCall(string instanceName);
    string sent();
    string expr();
    string mulDiv();
    string addSub();
    string word();
};
