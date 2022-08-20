#include "Node.hpp"
#include "../generator/Generate.hpp"


string Node::funcCallArtgment() {
	string oneArgment;
    int nowWord;

    argmentLoadSentS = "";

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
                string argReg;


                if (arg[0] == ' ') {
                    loads += arg;
                    type = llirType[Regs.nowVar];
                    newReg = Regs.nowVar;
                } else if (!isDigit(arg)){
                    newReg = arg;
                    type = "i32";
                } else {
                    // TODO : del
                    oneBeforeInstruction = "load";
                    newReg = arg;
                }

                if (newReg[0] == '@') {
                    type = "i8*";
                    newReg = Regs.llirReg[Regs.llirReg[arg]];
                }

                argReg = "%" + std::to_string(registerAmount++);

                argmentLoadSentS += argReg + " = " + load(newReg, type, typeSize[type]) + addIndent();

                oneArgment += type + " noundef " + argReg;

                if (token[tokNumCounter+1].tokChar == ",")
                    tokNumCounter++;

                Regs.llirReg[newReg] = token[tokNumCounter].tokChar;

                determinationOfOwnership(&newReg);
                drop(newReg);
                break;
            }
            default: {
                arg = addSub();

                if (token[tokNumCounter - 1].tokChar == "{")
                    arg = "{" + arg + "}";

                for (int i = 0; i < arg.length(); i++)
                    oneArgment.push_back(arg[i]);
                tokNumCounter++;

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
    bool   mut;
    int index = 0;
    ReturnArgumentAndMove argument;

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
                    valueName = oneArgment[index];
                    if (oneArgment[index] == "@mut") {
                        mut = true;
                        index++;
                        valueName = oneArgment[index];
                    }

                    if (oneArgment[index+2] == "int") {
                        valueType = "i32*";
                        OFS       = "\%d";
                    }
                    else if (oneArgment[index+2] == "string") {
                        valueType = "i8*";
                        OFS       = "\%s";
                    }

                    llirType[valueName]                                = valueType;
                    Regs.llirReg["%" + std::to_string(registerAmount)] = valueName;

                    argument.returnFunctionArgument += valueType + " noundef %" + std::to_string(registerAmount);

                    string regName = "%" + std::to_string(registerAmount++);
                    string wordNameReg = "@.str." + valueName;

                    Regs.Reg[valueName] = {valueName, regName, valueType, typeSize[valueType], OFS};
                    Regs.Reg[valueName].isMut = mut;

                    give(valueName);

                    Regs.llirReg[regName] = wordNameReg;
                    Regs.llirReg[wordNameReg] = valueName;

                    argument.argVars.push_back(Regs.Reg[valueName]);

                    index = 0;
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
