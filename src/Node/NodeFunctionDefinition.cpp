#include "Node.hpp"

string Node::functionDefinition() {
    if (token[tokNumCounter].tokNum == FN) {
        string argment = "";
        string functionName;
        string functionType;
		string functionData;
		string ret;

		indent++;

        tokNumCounter++; // fn

        functionName = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
			if (token[tokNumCounter].tokNum != RBRACKET)
            	argment = funcDefArtgment();
            tokNumCounter++; // )
	
        }

        expect(":");

        tokNumCounter++;

        functionType = word();

		string Fsent = "fn " + functionName + "(" + argment + ") : ";  


        if (functionType == "auto") {
			cout << Fsent + functionType << endl;;
            for (int i= 0; i < Fsent.length();i++)
                cout << " " << std::flush;
            cout << "^ Can't use 'auto' for function return value." << endl;
        }

        tokNumCounter++;

        expect("{");

        tokNumCounter++;

		functionData = sent();

        expect("}");

        tokNumCounter++;

		indent--;

		valMemory.push_back({0, False, functionType, functionName});

		ret = functionType + " " + functionName + " (" + argment + ") {\n" + functionData + "\n}" + functionDefinition();
		

        return ret;
    }
    return "";
}

