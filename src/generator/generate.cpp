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

string load(string regName, string type, string typeSize) {
	return "load " + type +", " + type + "* " + regName + ", align " + typeSize + "\n";
}

string loadPointer(string regName, string type, string typeSize) {
	return "load " + type +"*, " + type + "** " + regName + ", align " + typeSize + "\n";
}

string store(string regName, string type, string typeSize, string varName) {
	return "store " + type + " " + varName + ", " + type + "* " + regName  + ", align " + typeSize + "\n";
}

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
	int i = *regCounter;

	for (auto argument : Argument.argVars) {
		int &tmpI = *regCounter;
		r1 = "%" + std::to_string(tmpI);
		string r2;
        Regs->llirReg[argument.name] = r1;
		Regs->Reg[argument.name].regName = r1; 
		Regs->llirReg[r1] = argument.name; 

		ret += move(indent, Regs->Reg[argument.name], argument.regName);
		*regCounter = ++i;
	}
	return ret;
}

string bitcast(string indent, Register Regs, string name) {
	string ret;
	string type = Regs.Reg[name].type;
	string vName = Regs.Reg[name].name;
	string len = Regs.Reg[name].len;
	string r2 = "%" + std::to_string(Regs.registerAmount++);
	string r3 = "%" + std::to_string(Regs.registerAmount++);

	ret =  indent + name + " = alloca " + type + ", align 1\n";
	ret += indent + r2 + " = bitcast " + type + "* " + name + " to i8*\n";
	ret += indent + "call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 " + r2 + ", i8* align 1 getelementptr inbounds (" + type + ", " + type + "* " + vName + ", i32 0, i32 0), i64 " + len + ", i1 false)\n";

	return ret;
}

string strDef(string indent, Register *Regs, string name, string value) {
	string ret;
	string r1 = "%" + std::to_string(Regs->registerAmount++);
	string len = std::to_string(value.size());

	Regs->Reg[r1] = {name, r1, "["+ len + " x i8]", len, "\%s"};

	ret = bitcast(indent, *Regs, r1);

	return ret;
}

