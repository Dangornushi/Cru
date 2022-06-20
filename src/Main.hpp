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

  string version;
  string runCode;
  string fileData;
  string fileName;

  void open();
  void write();
  void run();

  vector<tokens> token;

 public:
  Main(int n, char *arg[]);
  void cru();
};

Lexer lexer;
Node node;
