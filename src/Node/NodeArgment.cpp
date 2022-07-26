#include "Node.hpp"
#include "../generator/Generate.hpp"


string Node::funcCallArtgment() {
	string oneArgment;
    int nowWord;

    if (token[tokNumCounter].tokNum == RBRACKET) {
        return "";
    }

	while (1) {
        nowWord = token[tokNumCounter].tokNum;


        if (nowWord == RBRACKET || nowWord == RRIPPLE)
            //tokNumCounter--;
            break;
        else if (token[tokNumCounter].tokNum == CANMA) {
            oneArgment.push_back(',');
            tokNumCounter++;

        } else {
            string var = token[tokNumCounter].tokChar;
            string arg = addSub();

            switch (langMode) {
                case LLIR: {
                    string argmentReg;
                    string type   = Regs.Reg[Regs.llirReg[arg]].type;
                    string newReg = "%" + std::to_string(registerAmount);

                    Type r1 = {"", arg, type, typeSize[type]};
                    reg r2 = {newReg, type, typeSize[type]};
                    loads += load(addIndent(), r1, newReg);

                    oneArgment += type + " noundef " + r2.regName;

                    Regs.llirReg.insert_or_assign(r2.regName, token[tokNumCounter].tokChar);
                    //regType.insert_or_assign(r2.regName, r2.type);
                    //tokNumCounter++;
                    registerAmount++;

                    break;
                }
                default: {

                    if (token[tokNumCounter - 1].tokChar == "{")
                        arg = "{" + arg + "}";
                    else
                        ;

                    for (int i = 0; i < arg.length(); i++)
                        oneArgment.push_back(arg[i]);
                    break;  
                }
            }

            tokNumCounter++;
        }
    }

	return oneArgment;
}

ReturnArgumentAndMove Node::funcDefArgument() {
	vector<string> oneArgment;
    ReturnArgumentAndMove argument;
	//string returnFunctionArgment = "";
    string valueName;
    string valueType;

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
                    if (oneArgment[2] == "int")
                        valueType = "i32";
                    reg r1 = {"%" + std::to_string(registerAmount), valueType, typeSize[valueType]};
llirType.insert_or_assign(valueName, r1.type);
                    Regs.llirReg.insert_or_assign("%" + std::to_string(registerAmount), valueName);
                    regType.insert_or_assign(r1.regName, r1.type);

                    argument.returnFunctionArgument += valueType + " noundef " + r1.regName;
                    Regs.Reg[valueName] = {valueName, "%"+std::to_string(registerAmount), valueType, typeSize[valueType]};
                    registerAmount++;
                    argument.argVars.push_back(Regs.Reg[valueName]);

                } break;
                default:
                    break;
            }
            argument.returnFunctionArgument += ", ";
			oneArgment = {};
        }
        if (token[tokNumCounter].tokChar[0] == ')') {
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
