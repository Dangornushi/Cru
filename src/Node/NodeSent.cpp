#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>

string Node::sent() {
    string ret;
    ret = funCall("");

    switch (token[tokNumCounter].tokNum) {
        case LET: {
            int isMut;
            int isPointer;
            string valueName;
            string valueType;
            string data;
            string ret;
            string vecType;

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

            if (token[tokNumCounter].tokChar == "<") {
                tokNumCounter++;
                vecType = token[tokNumCounter].tokChar;
                tokNumCounter++;
                expect(">");
                tokNumCounter++;
            }

            if (valueType == "int" && langMode == LLIR) {
                valueType = "i32";
            }
            
            llirType.insert_or_assign(valueName, valueType);

            if (token[tokNumCounter].tokChar == "<-") {
                expect("<-");

                tokNumCounter++;

                if (token[tokNumCounter].tokChar == "{") {
                    tokNumCounter++;

                    data = "{" + funcCallArtgment() + "}";

                    expect("}");
                    tokNumCounter++;
                }

                else {
                    data = addSub();
                    tokNumCounter++;
                }
            } else {
                string regNum = std::to_string(registerAmount);
                string size   = typeSize[valueType];
                ret = addIndent() + "%" + regNum + " = alloca " + valueType + ", align " + size + "\n";
                llirReg.insert_or_assign(valueName, registerAmount++);
                regType.insert_or_assign("%" + regNum, valueType);
                expect(";");
                tokNumCounter++;

                return ret += sent();
            }


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
                        ret = addIndent() + valueName + " = " + valueType + "()\n";
                    } else if (data != "") {
                        ret = addIndent() + valueName + " = " + data + "\n";
                    } else {
                    }
                }
                else if (langMode == CPP) {
                    if (valueType == "string") {
                       ret = addIndent() + "__Cru_string " + valueName;
                        if (data != "")
                            ret += " = " + data ;
                        else ;

                    } else if (valueType == "vec") {
                        ret += "__Cru_Vec_" + vecType + " " + valueName;
                        if (vecType == "string") {
                            if (data != "")
                                ret += " = {"+ data +"}";
                            else ;
                        }
                    } else {
                        ret = addIndent() + valueType + " " + valueName;
                        if (data != "")
                            ret += " = " + data;
                        else
                            classAndInstance[valueName] = nowClassName;
                    }

                    ret += ";\n";
                }
                else if (langMode == LLIR) {
                    string regNum = std::to_string(registerAmount);
                    string size = typeSize[valueType];
                    if (LLIRnowVar == "")
                        LLIRnowVar = data;
                    else 
                        ret = addIndent() + data + "\n"; 

                    reg r1 = {"%"+regNum, valueType, size};
                    string value = LLIRnowVar;

                    ret += move(addIndent(), r1, value);

                    llirReg.insert_or_assign(valueName, registerAmount++);
                    regType.insert_or_assign(r1.regName, r1.type);
                    LLIRnowVar = "";
                }
                expect(";");
                tokNumCounter++;

                ret += sent();
            }

            return ret;

        } break;
        case IF: {
            string ret;
            string evalS;
            string sentS;
            string elseS;
            string r1;
            string r2;
            string r3;

            expect("if");
            tokNumCounter++;
            evalS = eval();
            if (langMode == PYTHON)
                ret = addIndent() + "if " + evalS + ":\n";
            else if (langMode == CPP)
                ret = addIndent() + "if (" + evalS + ") {\n";
            else if (langMode == LLIR)
                ret = evalS;
            expect("{");
            tokNumCounter++;
            if (langMode == LLIR) {
                registerAmount--;

                r1 = "%" + std::to_string(registerAmount++);
                r2 = "%" + std::to_string(registerAmount++);

                sentS = sent();

                r3 = "%" + std::to_string(registerAmount++);
            }
            else {
                indent++;

                sentS = sent();

                indent--;
            }
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            if (langMode == LLIR) {
                /*
                 * if -> if data -> else -> else data
                 */
                string tmpR2 = {r2[1]};
                string tmpR3 = {r3[1]};

                ret += addIndent() + "br i1 " + r1 + ", label " + r2 + ", label " + r3 + "\n\n";

                if (token[tokNumCounter].tokChar == "else") {
                    tokNumCounter++;
                    expect("{");
                    tokNumCounter++;
                    elseS =  sent();
                    expect("}");
                    tokNumCounter++;
                    expect(";");
                    tokNumCounter++;

                    r3 = std::to_string(registerAmount++);

                    ret += tmpR2 + ":    ; preds = %" + tmpR2 + "\n" + sentS +addIndent() + "br label %" + r3 + "\n";
                    ret += tmpR3 + ":    ; preds = %" + tmpR3 + "\n" ;

                    ret += elseS + "\n" + addIndent() + "br label %" + r3 + "\n";
                    ret += r3 + ":    ; preds = %" + r3 + "\n";
                } else {
                    ret += tmpR2 + ":    ; preds = %" + tmpR2 + "\n" + sentS +addIndent() + "br label %" + tmpR3 + "\n";
                    ret += tmpR3 + ":    ; preds = %" + tmpR3 + "\n" ;
                }
            } else {
                ret += sentS;
            }
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
            string data = funCall("");
            tokNumCounter++;

            if (langMode == PYTHON) {
                ret = addIndent() + "print(" + data + ")\n";
            }
            else if (langMode == CPP) {
                ret = addIndent() + "__CRU_Strput(" + data + ", sizeof(" + data + "));\n";
            }
            else if (langMode == LLIR) {
                string loadR1 = "%" + std::to_string(registerAmount++);
                string loadR2 = "%" + std::to_string(registerAmount++);
                string loadR3 = "%" + std::to_string(registerAmount++);
                string printfR;
                string size = typeSize[regType[data]];
                
                strDefine += "@.str." + std::to_string(strAmount) + " = private unnamed_addr constant [3 x i8] c\"\%d\\00\", align ";
                if (size != "") 
                    strDefine += size + "\n";
                else 
                    strDefine += "4\n";

                if (regType[data] != "") {
                    ret =  addIndent() + loadR1 + " = load " + regType[data] + ", " + regType[data] + "* " + data + ", align " + typeSize[regType[data]] + "\n";
                }
                else {
                    ret += addIndent() + loadR1 + " = alloca i32, align 4\n" ; 
                    ret += addIndent() + "store i32 " + data + ", i32* " + loadR1 + ", align 4\n";
                    ret += addIndent() + loadR2 + " = load i32, i32* " + loadR1 + ", align 4\n" ; 
                }
                ret += addIndent() + loadR3 + " = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str." + std::to_string(strAmount++) +", i64 0, i64 0), i32 noundef " + loadR2+ ")\n"; 
                if (putDefExists == False) {
                    functionDefine += "declare i32 @printf(i8* noundef, ...) #" + std::to_string(funcDefQuantity++);
                }
                else ;
                putDefExists = True;
            }

            if (token[tokNumCounter].tokNum == CANMA) {
                tokNumCounter++;
                if (token[tokNumCounter].tokChar == "int") {
                    ret = addIndent() + "printf(\"%d\"," + data + ");\n";
                }
                else if (token[tokNumCounter].tokChar == "vec") {
                    ret = addIndent() + "__Cru_Vec_string_put(&" + data +");\n";
                }
                tokNumCounter++;
            }
            expect(";");
            tokNumCounter++;

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
            else if (langMode == CPP) {
                ret = addIndent() + "return " + addSub() + ";";
            }
            else if (langMode == LLIR) {
                string data = addSub();
                string type = regType[data];
                if (type == "") {
                    ret += addIndent() + "ret " + nowType + " " + data;
                } else {
                    ret = addIndent() + "%" + std::to_string(registerAmount) + " = load " + type +
                          ", " + type + "* " + data + ", align " + typeSize[regType[data]] + "\n";
                    ret += addIndent() + "ret " + nowType + " %" + std::to_string(registerAmount);
                    registerAmount++;
                }
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
            else if (token[tokNumCounter+1].tokNum == PLUS) { 
                string var = word(); 
                string loadR = "%" + std::to_string(registerAmount++);
                string storeR = "%" + std::to_string(registerAmount++);

                tokNumCounter++;
                expect("+");
                tokNumCounter++;
                expect("+");
                tokNumCounter++;
                expect(";");
                tokNumCounter++;

                if (langMode == LLIR) {
                    ret = addIndent() + loadR + " = load i32, i32* " + var + ", align 4\n";
                    ret += addIndent() + storeR + " = add nsw i32 " + loadR + ", 1\n";
                    ret += addIndent() + "store i32 " + storeR + ", i32* " + var + ", align 4\n";
                }
                return ret + sent();
            } else if (token[tokNumCounter+1].tokNum == MIN) { 
                string var = word(); 
                string loadR = "%" + std::to_string(registerAmount++);
                string storeR = "%" + std::to_string(registerAmount++);

                tokNumCounter++;
                expect("-");
                tokNumCounter++;
                expect("-");
                tokNumCounter++;
                expect(";");
                tokNumCounter++;

                if (langMode == LLIR) {
                    ret = addIndent() + loadR + " = load i32, i32* " + var + ", align 4\n";
                    ret += addIndent() + storeR + " = sub nsw i32 " + loadR + ", 1\n";
                    ret += addIndent() + "store i32 " + storeR + ", i32* " + var + ", align 4\n";
                }
                return ret + sent();
            } else if (token[tokNumCounter+1].tokChar == "<-") {
                tokNumCounter++;
                tokNumCounter++;
                if (token[tokNumCounter].tokChar == "{") {
                    tokNumCounter++;
                    ret = "char *__tmp[] = {" +  funcCallArtgment() + "};\n" + ret + " = _Vec(__tmp)";
                    expect("}");
                }
                else {
                    if (langMode == LLIR) {
                        string data = funCall("");
                        string size = typeSize[regType[ret]];
                        if (regType.find(data) != regType.end()) {
                            string type = regType[data];
                            string size = typeSize[regType[data]];
                            string data = addSub();
                            string tmpReg = "%" + std::to_string(registerAmount);
                            string mainReg = ret;

//                            registerAmount++;

                            string tmp = "%" + std::to_string(registerAmount);

                            ret = data;
//                            ret += addIndent() + tmp + " = load " + type + ", " + type + "* " + tmpReg + ", align " + size + "\n";
                            ret += addIndent() + "store " + type + " " + tmp + ", " + type + "* " + mainReg + ", align " + size + "\n";

                            llirReg.insert_or_assign(tmp, registerAmount);
                            regType.insert_or_assign("%" + std::to_string(registerAmount++), "i32");

                        } else {
                            ret = addIndent() + "store " + regType[ret] + " " + data + ", " + regType[ret] + "* " + ret + ", align " + size + "\n";
                        }
                        tokNumCounter++;
                        expect(";");
                        tokNumCounter++;
                        ret += sent();
                        return ret;
                    } else {
                        ret += " = " + addSub();
                    }
                }

                tokNumCounter++;
                expect(";");
                tokNumCounter++;
            }
            else {
                tokNumCounter++;
                tokNumCounter++;
            }
            ret = addIndent() + ret;
            if (enumEnabled == True)
                ret += ",\n";
            else {
                if (langMode == LLIR) {
                    ret += "\n";
                } else {
                    ret += ";\n";
                }
            }

            ret += sent();
            return ret;
        } break;
    }
    return ret;
}
