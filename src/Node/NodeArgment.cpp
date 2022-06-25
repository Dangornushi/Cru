#include "Node.hpp"


string Node::funcCallArtgment() {
	string returnFunctionArgment = "";
	string oneArgment;

	while (1) {
        if (token[tokNumCounter].tokNum== RBRACKET) {
            break;
		}
        if (token[tokNumCounter].tokNum == CANMA) {
            oneArgment.push_back(',');
            tokNumCounter++;
        }
        else {
            string arg = addSub();
            for (int i = 0; i < arg.length(); i++)
                oneArgment.push_back(arg[i]);
            if (token[tokNumCounter].tokNum == RBRACKET) {
                break;
            }
            tokNumCounter++;
        }
    }

	return oneArgment;
}

string Node::funcDefArtgment() {
	string returnFunctionArgment = "";
	vector<string> oneArgment;

	while (1) {
        if (token[tokNumCounter - 1].tokChar[0] == ',' || token[tokNumCounter].tokChar[0] == ')') {
            returnFunctionArgment += oneArgment[2] + " " + oneArgment[0];
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
