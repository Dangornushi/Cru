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


int isDigit(string s1);

extern int langMode;
extern int debugMode;

struct tokens{
	int tokNum;
	string tokChar;
};


