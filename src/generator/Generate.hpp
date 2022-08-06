#pragma once

#include "../CRU.hpp"
#include "../Node/Node.hpp"

string move(string indent, Type r1, string r2);
string load(string regName, string type, string typeSize);
string store(string regName, string type, string typeSize, string varName);
string calc(int mode, reg r1, string r2, string r3);
string argMove(string indent, ReturnArgumentAndMove Argument, int *regCounter, Register *Regs);
string strDef(string indent, Register *Regs, string name, string value);
string load(string indent, Type r1, string r2);

