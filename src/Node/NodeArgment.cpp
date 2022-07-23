#include "Node.hpp"
#include "../generator/Generate.hpp"


string Node::funcCallArtgment() {
	string returnFunctionArgment = "";
	string oneArgment;
    int nowWord;

    if (token[tokNumCounter].tokNum == RBRACKET) {
        return "";
    }

	while (1) {
        nowWord = token[tokNumCounter-1].tokNum;

        if (nowWord == RBRACKET || nowWord == RRIPPLE) {
            tokNumCounter--;
            break;
        } else {
            string arg = addSub();

            switch (langMode) {
                case LLIR: {
                    string argmentReg;
                    string type   = regType[arg];
                    string newReg = "%" + std::to_string(registerAmount);

                    reg r1 = {arg, type, typeSize[type]};
                    reg r2 = {newReg, type, typeSize[type]};
                    loads += load(addIndent(), r2, r1);
                    oneArgment += type + " noundef " + r2.regName;

                    llirReg.insert_or_assign(token[tokNumCounter].tokChar, registerAmount++);
                    regType.insert_or_assign(r2.regName, r2.type);
                } break;
                default: {
                    if (token[tokNumCounter - 1].tokChar == "{")
                        arg = "{" + arg + "}";
                    else
                        ;

                    for (int i = 0; i < arg.length(); i++)
                        oneArgment.push_back(arg[i]);
                    tokNumCounter++;
                } break;
            }
            cout << ">> " << token[tokNumCounter].tokChar << endl;

            if (token[tokNumCounter].tokNum == CANMA) {
                oneArgment.push_back(',');
                //tokNumCounter++;
            }
            tokNumCounter++;
        }
    }

	return oneArgment;
}

string Node::funcDefArtgment() {
	string returnFunctionArgment = "";
	vector<string> oneArgment;
    string valueName;
    string valueType;

    registerAmount = 0;

	while (1) {
        if (token[tokNumCounter - 1].tokNum == CANMA || token[tokNumCounter].tokNum == RBRACKET) {
            switch (langMode) {
                case CPP:
                    returnFunctionArgment += oneArgment.at(2) + " " + oneArgment.at(0);
                    break;
                case PYTHON:
                    returnFunctionArgment += oneArgment.at(0);
                    break;
                case LLIR: {
                    string r1;
                    valueName = oneArgment[0];
                    if (oneArgment[2] == "int")
                        valueType = "i32";
                    r1 = "%" + std::to_string(registerAmount);
                    llirType.insert_or_assign(valueName, valueType);
                    llirReg.insert_or_assign(valueName, registerAmount);
                    regType.insert_or_assign(r1, valueType);
                    returnFunctionArgment += valueType + " noundef " + r1;
                    registerAmount++;
                } break;
                default:
                    break;
            }
            returnFunctionArgment += ", ";
			oneArgment = {};
        }
        if (token[tokNumCounter].tokChar[0] == ')') {
			returnFunctionArgment.erase(returnFunctionArgment.end()-2, returnFunctionArgment.end());
            break;
		}
        else {
            oneArgment.push_back(token[tokNumCounter].tokChar);
            tokNumCounter++;
        }
    }

	return returnFunctionArgment;
}
