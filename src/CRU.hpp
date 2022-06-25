#pragma once

#include <iostream>
#include <vector>
#include <map>

#define PYTHON 0
#define CPP 1

#define True 1
#define False 0

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;

int isDigit(string s1);

extern int langMode;
extern int debugMode;

struct tokens{
	int tokNum;
	string tokChar;
};

/* Natural tokens */
enum RESERV {
	/* RESERVs */
	ENTRY,
	FN,
	LET,
	RETURN,
	PUT,
	PRINT,
	IF,
	ELSE,
	ELIF,
	WHILE,
	LOOP,
	CALL,
	MUT,
	FOR,
	IN,
	APPEND,

	/* TYPE AND VARs */
	WORD,
	NUMBER,
	STR,
	VAR,
	AUTO,

	/* OPs */
	LBRACKET,
	RBRACKET,
	LRIPPLE,
	RRIPPLE,
	CORON,
	SEMICORON,
	SQ,
	DQ,
	CANMA,
	PERIOD,
	BIG,
	MINI,
	DOUBLEQUOT,
	SINGLEQUOT,
	PLUS,
	MIN,
	MUL,
	DIV,
	EQ,
	TILDE,
	ADDRESS,
};

