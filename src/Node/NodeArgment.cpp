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

        if (token[tokNumCounter].tokNum == ADDRESS) {
            tokNumCounter++;
            arg                                   = addSub();
            Regs.Reg[Regs.llirReg[arg]].ownerShip = true;
        } else
            arg = addSub();
        switch (langMode) {
            case LLIR: {
                string argmentReg;
                string type                       = Regs.Reg[Regs.llirReg[arg]].type;
                string newReg                     = "%" + std::to_string(registerAmount);
                string init_outputFormatSpecifier = Regs.Reg[Regs.llirReg[arg]].outputFormatSpecifier;
                string argReg;

                if (arg[0] == ' ') {
                    argmentLoadSentS += arg;
                    type = Regs.Reg[Regs.nowVar].type;
                    newReg = Regs.nowVar;
                } else if (!isDigit(arg)){
                    newReg = arg;
                    type = "i32";
                } else {
                    // TODO : del
                    oneBeforeInstruction = "load";
                    newReg = arg;
                }

                if (newReg[0] == '@' && type[0] != '[') {
                    type = "i8*";
                    newReg = Regs.llirReg[Regs.llirReg[arg]];
                }

                determinationOfOwnership(&newReg);

                drop(newReg);

                string allocaReg = "%" + std::to_string(registerAmount++);
                string loadReg = "%" + std::to_string(registerAmount);

                if (Regs.Reg[Regs.llirReg[newReg]].regName.empty() && type != "i8*") {
                    argmentLoadSentS += addIndent() + allocaReg + " = alloca i32, align 8\n"; 
                    if (type[type.size()] != '*') {
                        argmentLoadSentS += addIndent() + "store  " + type + " " + newReg + ", " + type + "* "+ allocaReg + ", align 8\n"; 
                        type += "*";
                    }
                    else {
                        type = type.substr(0, type.size()-1);
                        argmentLoadSentS += addIndent() + "store  " + type + " " + newReg + ", " + type + " "+ allocaReg + ", align 8\n"; 
                    }
                }
                else if (type[0] == '[') {
                    newReg = Regs.llirReg[Regs.llirReg[arg]];

                    argmentLoadSentS += addIndent() + allocaReg + " = getelementptr inbounds " + type + ", " + type + "*" + newReg + ", i64 0, i64 0" + "\n";

                    type = "i8**";
                }
                else
                    argmentLoadSentS += addIndent() + allocaReg + " = load "+ type +", " + type + "* " +newReg + " , align " + typeSize[type] + "\n";

                oneArgment += type + " noundef " + allocaReg;

                Regs.llirReg[newReg] = token[tokNumCounter].tokChar;

                tokNumCounter++;
                break;
            }
            default: {

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

            valueName = oneArgment.at(0);
            valueType = oneArgment.at(2);
            switch (langMode) {
                case CPP: {
                    OFS = typeToPrint[oneArgment[index+2]];
                    argument.returnFunctionArgument += variableType(langMode, valueType) + " " + valueName;
                    Regs.Reg[valueName] = {valueName, valueName, valueType, typeSize[valueType], OFS};
                    Regs.Reg[valueName].isMut = mut;
                    break;
                }
                case PYTHON:
                    argument.returnFunctionArgument += valueName;
                    Regs.Reg[valueName] = {valueName, valueName, valueType, typeSize[valueType], OFS};
                    Regs.Reg[valueName].isMut = mut;
                    break;
                case LLIR: {
                    if (valueType == "vec") {
                        index+=4;
                        valueType = valueType+ "&" + oneArgment[index++];
                    }
                    else if (oneArgment[index] == "@mut") {
                        mut = true;
                        index++;
                        valueName = oneArgment[index];
                    }

                    valueType = variableType(langMode, valueType) + "*";
                    OFS = typeToOfs[oneArgment[index+2]];

                    Regs.llirReg["%" + std::to_string(registerAmount)] = valueName;

                    argument.returnFunctionArgument += valueType + " noundef %" + std::to_string(registerAmount);

                    string regName = "%" + std::to_string(registerAmount++);
                    string wordNameReg = "@.str." + valueName;

                    Regs.Reg[valueName] = {valueName, regName, valueType, typeSize[valueType], OFS};
                    Regs.Reg[valueName].isMut = mut;

                    Regs.llirReg[valueName] = regName;
                    Regs.llirReg[regName] = wordNameReg;
                    Regs.llirReg[wordNameReg] = regName;

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
