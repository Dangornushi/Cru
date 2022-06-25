#include "Node.hpp"

string Node::sent() {
    switch (token[tokNumCounter].tokNum) {
        case LET: {
            int isMut;
            int isPointer;
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

            int hasType;
            hasType = False;

            for (vector<Type>::const_iterator i = valMemory.begin(); i != valMemory.end(); i++) {
                if (i->name == valueName) {
                    // もうすでに変数が存在する
                    hasType = True;
                    break;
                }
                // 初めて宣言される
            }

            valMemory.push_back({0, isMut, valueType, valueName});

            if (hasType == True && isMut == True) {
                cout << "Warning: value '" << valueName << "' has the @Mut option. " << endl;
                ret = addIndent() + valueName + " = " + data + ";\n" + sent();
            }
            if (hasType == True && isMut != True) {
                cout << "Error: value '" << valueName << "' has already been defined. " << endl;
                exit(0);
            } else {
                if (langMode == PYTHON) {
                        ret = addIndent() + valueName +
                              " = " + data + "\n" + sent();
                }
                if (langMode == CPP) {
                    if (valueType == "str") {
                        ret = addIndent() + "char " + valueName + " [] = " + data + ";\n" + sent();
                    }
                    else if (valueType == "string") {
                        ret = addIndent() + "char *" + valueName + " = " + data + ";\n" + sent();

                    } else
                        ret = addIndent() + valueType + " " + valueName + " = " + data + ";\n" +
                              sent();
                }
            }

            return ret;

        } break;
        case IF: {
            string ret;
            string evalS;
            string sentS;

            expect("if");
            tokNumCounter++;
            evalS = eval();
            if (langMode == PYTHON)
                ret = addIndent() + "if " + evalS + ":\n";
            if (langMode == CPP)
                ret = addIndent() + "if (" + evalS + ") {\n";
            expect("{");
            tokNumCounter++;
            indent++;
            sentS = sent();
            indent--;
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            ret += "\n" + addIndent();
            if (langMode == CPP)
                ret +=  + "}";

            return ret + "\n" + sent();
        } break;
        case FOR: {
            string ret;
            string loopS;
            string sentS;

            expect("for");
            tokNumCounter++;
            loopS = loop();
            if (langMode == PYTHON)
                ret = addIndent() + "for " + loopS + ":\n";
            if (langMode == CPP)
                ret = addIndent() + "for (" + loopS + ") {\n";
            expect("{");
            tokNumCounter++;
            indent++;
            sentS = sent();
            indent--;
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            ret += "\n" + addIndent();
            if (langMode == CPP)
                ret +=  + "}";

            return ret + "\n" + sent();
        } break;
        case PUT: {
            string ret;

            expect("put");
            tokNumCounter++;
            string data = addSub();
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            if (langMode == PYTHON) {
                ret = addIndent() + "print(" + addSub() + ")\n";
            }
            if (langMode == CPP) {
                ret = addIndent() + "__CRU_Strput(" + data + ", sizeof("+ data + "));\n";
            }

            return ret + sent();
        } break;
        case PRINT: {
            string ret;

            expect("print");
            tokNumCounter++;
            string data = addSub();
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            if (langMode == PYTHON) {
                ret = addIndent() + "print(" + addSub() + ")\n";
            }
            if (langMode == CPP) {
                ret = addIndent() + "__CRU_Stringput(" + data + ");\n";
            }

            return ret + sent();
        } break;
        case RETURN: {
            string ret;

            expect("return");
            tokNumCounter++;

            if (langMode == PYTHON) {
                ret = addIndent() + "return " + addSub();
            }
            if (langMode == CPP) {
                ret = addIndent() + "return " + addSub() + ";";
            }

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
        default: {
            if (token[tokNumCounter+1].tokChar == "")
                return "";
            cout << token[tokNumCounter].tokChar << endl;
            string ret = addSub();
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            return addIndent() + ret + ";\n" + sent();
        } break;
    }
    return "";
}
