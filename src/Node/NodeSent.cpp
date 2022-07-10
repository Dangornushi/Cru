#include "Node.hpp"

#include <fstream>

string Node::sent() {
    string ret = funCall();
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

            valueName = word();

            tokNumCounter++;

            expect(":");

            tokNumCounter++;

            valueType = word();

            tokNumCounter++;

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
                    if (data == "class") {
                        ret = addIndent() + valueName + " = " + valueType + "()\n" + sent();
                    }
                    else {
                        ret = addIndent() + valueName + " = " + data + "\n" + sent();
                    }
                }
                if (langMode == CPP) {
                    if (valueType == "str") {
                        ret = addIndent() + "char " + valueName + " [] = " + data + ";\n" + sent();
                    } else if (valueType == "string") {
                        ret = addIndent() + "__Cru_string " + valueName + " =  _String(" + data + ");\n" + sent();

                    } else if (valueType == "vec") {
                        ret = addIndent() + "__Cru_Vec_String " + valueName + " =  {" + data + "};\n" + sent();

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
            ret += sentS;
            ret += "\n" + addIndent();
            if (langMode == CPP)
                ret += "}";

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
                ret += +"}";

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
                ret = addIndent() + "print(" + data + ")\n";
            }
            if (langMode == CPP) {
                ret = addIndent() + "__CRU_Strput(" + data + ", sizeof(" + data + "));\n";
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
                ret = addIndent() + "print(" + data + ")\n";
            }
            if (langMode == CPP) {
                ret = addIndent() + "__CRU_Stringput(&" + data + ");\n";
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

            return ret + "\n" + sent();

        } break;
        case IMPORT: {

            tokNumCounter++;
            string filedata;
            string ret;

            std::ifstream reading_file;
            reading_file.open(token[tokNumCounter].tokChar, std::ios::in);

            while (std::getline(reading_file, filedata))
                ret += filedata;

            tokNumCounter++;

            expect(";");
            return ret;

        } break;
        case SEMICORON: {
            tokNumCounter++;
            return addIndent() + ";";
        } break;
        default: {
            if (token[tokNumCounter].tokChar == "}" || token[tokNumCounter + 1].tokChar == "")
                return "";
            tokNumCounter++;
            tokNumCounter++;
            ret = addIndent() + ret + "\n" +  sent();
            return ret;
            /*
   string ret = "";

   if (token[tokNumCounter].tokNum != RRIPPLE) {
       ret = token[tokNumCounter].tokChar + ".";
       tokNumCounter+=2;
       ret += addSub();
       tokNumCounter+=2;
       ret +=sent();

       cout << ret << endl;
   }
   return ret;*/
        } break;
    }
    return ret;
}
