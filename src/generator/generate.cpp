#include "../CRU.hpp"

string move(string indent, reg r1, string var) {
	string ret;

    ret = indent + r1.regName + " = alloca " + r1.type + ", align " + r1.typeSize + "\n" ; 
    ret += indent + "store " + r1.type + " " + var+ ", " + r1.type + "* " + r1.regName  + ", align " + r1.typeSize + "\n";
	return ret;
}

/*
 * plan A: Create a register Class.
 * plan B: Take string data as an argument.
 * plan C: Join load to a class, And take a register as an argument.
 */
string load(string indent, reg r1, reg r2) {
	string ret;
	ret = indent + r1.regName + " = load " + r2.type + ", " + r2.type + "* " + r2.regName + ", align " + r2.typeSize + "\n";
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

