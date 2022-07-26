#pragma once

#include "../CRU.hpp"
#include "../Node/Node.hpp"

string move(string indent, Type r1, string r2);
string load(string indent, Type r1, string r2);
string calc(int mode, reg r1, string r2, string r3);
string argMove(string indent, ReturnArgumentAndMove Argument, int *regCounter, Register *Regs);
