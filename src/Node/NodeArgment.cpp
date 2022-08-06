#include "Node.hpp"
#include "../generator/Generate.hpp"


string Node::funcCallArtgment() {
	string oneArgment;
    int nowWord;

    if (token[tokNumCounter].tokNum == RBRACKET)
        return "";

    while (1) {
        nowWord = token[tokNumCounter].tokNum;

        if (nowWord == SEMICORON || token[tokNumCounter-1].tokNum == RBRACKET)  {
            tokNumCounter--;
            break;
        }

        else if (nowWord == RBRACKET || nowWord == RRIPPLE) {
            break;
        }
        //else 
        string arg;

        switch (langMode) {
            case LLIR: {
                arg = addSub();

                string argmentReg;
                string type                       = Regs.Reg[Regs.llirReg[arg]].type;
                string newReg                     = "%" + std::to_string(registerAmount);
                string init_outputFormatSpecifier = Regs.Reg[Regs.llirReg[arg]].outputFormatSpecifier;

                if (arg[0] == ' ') {
                    loads += arg;
                    type = llirType[Regs.nowVar];
                    newReg = Regs.nowVar;
                } else if (!isDigit(arg)){
                    newReg = arg;
                    type = "i32";
                } else {
                    Type r1 = {"", arg, type, typeSize[type], init_outputFormatSpecifier};
                    loads += load(addIndent(), r1, newReg);
                    registerAmount++;
                }

                oneArgment += type + " noundef " + newReg;

                Regs.llirReg[newReg] = token[tokNumCounter].tokChar;

                break;
            }
            default: {
                arg = addSub();

                if (token[tokNumCounter - 1].tokChar == "{")
                    arg = "{" + arg + "}";

                for (int i = 0; i < arg.length(); i++)
                    oneArgment.push_back(arg[i]);
                tokNumCounter ++;

                break;
            }
        }

        if (token[tokNumCounter].tokNum == CANMA) {
            oneArgment.push_back(',');
            tokNumCounter ++;
            continue;
        }

        tokNumCounter++;
    }

    return oneArgment;
}

ReturnArgumentAndMove Node::funcDefArgument() {
	vector<string> oneArgment;
    string valueName;
    string valueType;
    string OFS;
    ReturnArgumentAndMove argument;

    registerAmount = 0;

	while (1) {
        if (token[tokNumCounter - 1].tokNum == CANMA || token[tokNumCounter].tokNum == RBRACKET) {
            switch (langMode) {
                case CPP:
                    argument.returnFunctionArgument += oneArgment.at(2) + " " + oneArgment.at(0);
                    break;
                case PYTHON:
                    argument.returnFunctionArgument += oneArgment.at(0);
                    break;
                case LLIR: {
                    valueName = oneArgment[0];
                    if (oneArgment[2] == "int") {
                        valueType = "i32";
                        OFS = "\%d";
                    }
                    llirType[valueName] = valueType;

                    Regs.llirReg["%" + std::to_string(registerAmount)] = valueName;

                    argument.returnFunctionArgument += valueType + " noundef %" + std::to_string(registerAmount);

                    Regs.Reg[valueName] = {valueName, "%"+std::to_string(registerAmount), valueType, typeSize[valueType], OFS};
                    registerAmount++;
                    argument.argVars.push_back(Regs.Reg[valueName]);
                    break;
                } 
                default:
                    break;
            }
            argument.returnFunctionArgument += ", ";
			oneArgment = {};
        }

        if (token[tokNumCounter].tokNum == RBRACKET) {
			argument.returnFunctionArgument.erase(argument.returnFunctionArgument.end()-2, argument.returnFunctionArgument.end());
            break;
		}
        else {
            oneArgment.push_back(token[tokNumCounter].tokChar);
            tokNumCounter++;
        }
    }

	return argument;
}
