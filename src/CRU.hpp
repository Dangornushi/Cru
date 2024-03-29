#pragma once

#include <iostream>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;

#define True 1
#define False 0

#define PYTHON 0
#define CPP 1
#define RUST 2
#define LLIR 3

int            isDigit(string s1);
string         execDir();
string         variableType(int langMode, string typeName);
vector<string> pathSplit(string p);

extern int langMode;
extern int debugMode;

struct tokens{
	int tokNum;
	string tokChar;
};

struct reg {
	string regName;
	string type;
	string typeSize;
};

