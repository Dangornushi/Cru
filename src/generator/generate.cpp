#include "../CRU.hpp"
#include "../Node/Node.hpp"

string move(string indent, Type r1, string r2) {
	string ret;

    ret = indent + r1.regName + " = alloca " + r1.type + ", align " + r1.len + "\n" ; 
    ret += indent + "store " + r1.type + " " + r2 + ", " + r1.type + "* " + r1.regName  + ", align " + r1.len + "\n";
	return ret;
}

/*
 * plan A: Create a register Class.
 * plan B: Take string data as an argument.
 * plan C: Join load to a class, And take a register as an argument.
 */
string load(string indent, Type r1, string r2) {
	string ret;
	ret = indent + r2 + " = load " + r1.type + ", " + r1.type + "* " + r1.regName + ", align " + r1.len + "\n";
	return ret;
}

string calc(int mode, reg r1, string r2, string r3) {
	string ret;

	/*
	switch (mode) {
		case 1: {

		} break;
	}
	*/
	return ret;
}


string argMove(string indent, ReturnArgumentAndMove Argument, int *regCounter, Register *Regs ) {
	string ret;
	string r1;
	int i =0;

	for (auto argument : Argument.argVars) {
		int &tmpI = *regCounter;
		r1 = "%" + std::to_string(tmpI);
		string r2;
		Regs->Reg[argument.name].regName = r1; 
		Regs->llirReg[r1] = argument.name; 

		ret += move(indent, Regs->Reg[argument.name], argument.regName);
		*regCounter += ++i;
	}
	*regCounter -= 1;
	return ret;
}

