#include "Node.hpp"

string Node::sent() {
    switch (token[tokNumCounter].tokNum) {
        case LET: {
            int isMut;
            string valueName;
            string valueType;
            string data;
            string ret;

            expect("let");
            tokNumCounter++;
            if (token[tokNumCounter].tokNum == MUT) {
                isMut = True;
                tokNumCounter++;
            } else
                isMut = False;

            valueName = token[tokNumCounter++].tokChar;

            expect(":");

            tokNumCounter++;

            // if (valMemory.)
            valueType = token[tokNumCounter++].tokChar;

            expect("<-");

            tokNumCounter++;

            data = addSub();
		    tokNumCounter++;

            expect(";");

            tokNumCounter++;

            int hasType = True;

            for (vector<Type>::const_iterator i = valMemory.begin(); i != valMemory.end(); i++) {
                if (i->name == valueName)
                    // もうすでに変数が存在する
                    break;
                else
                    // 初めて宣言される
                    hasType = False;
            }

            valMemory.push_back({0, isMut, valueType, valueName});
            if (hasType == True && isMut == False) {
                cout << "let " << valueName << " : " << valueType << " <-" << endl
                     << "    ^ The variable is doubly defined!!" << endl;
                exit(1);
            }
            if (hasType == True && isMut == True) {
                cout << "Warning: value '" << valueName << "' has the @Mut option. " << endl;
                ret = addIndent() + valueName + " = " + data + ";\n" + sent();
            }
            else
                ret = addIndent() + valueType + " " + valueName + " = " + data + ";\n" + sent();

            return ret;

        } break;
        case IF: {
            string ret;
            ret = addIndent() + "if (";
            expect("if");
            tokNumCounter++;
            ret += eval();
            ret += ") {\n";
            expect("{");
            tokNumCounter++;
            indent++;
            ret += sent();
            indent--;
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            ret += "\n" + addIndent() + "}\n";
            return ret + sent();
        } break;
        case FOR: {
            string ret;
            ret += addIndent() + "for (";
            expect("for");
            tokNumCounter++;
            ret += loop();
            ret += ") {\n";
            expect("{");
            tokNumCounter++;
            cout << token[tokNumCounter].tokChar << endl;
            indent++;
            ret += sent();
            indent--;
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            ret += "\n" + addIndent() + "}\n";
            return ret + sent();
        }
        case RETURN: {
            expect("return");
            tokNumCounter++;

            string ret = addIndent() + "return " + addSub() + ";";

             // tokNumCounter++;
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            return ret + sent();

        } break;
        case SEMICORON: {
            tokNumCounter++;
            return addIndent() + ";";
        }
    }
    return "";
}
