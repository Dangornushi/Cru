#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>
#include <string>

string Node::sent() {
    string ret;
    ret = funCall("");

    switch (token[tokNumCounter].tokNum) {

        case LET: {

            int    isMut;
            int    isPointer;
            string valueName;
            string valueType;
            string data;
            string ret;
            string vecType;
            string regSName;

            expect("let");

            tokNumCounter++;

            isMut = False;

            if (token[tokNumCounter].tokNum == MUT) {
                isMut = True;
                tokNumCounter++;
            }

            regSName  = token[tokNumCounter].tokChar;
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

            if (valueType == "int" && langMode == LLIR)
                valueType = "i32";

            else if (valueType == "string" && langMode == LLIR)
                valueType = "i8";

            llirType[valueName] = valueType;

            // only define
            if (token[tokNumCounter].tokChar != "<-") {
                string regNum = std::to_string(registerAmount);
                string size   = typeSize[valueType];
                switch (langMode) {
                    case PYTHON: {
                        //
                        break;
                    }
                    case CPP: {
                        if (valueType == "vec") {
                            ret += "__Cru_Vec_" + vecType + " " + valueName + ";\n";
                        } else {
                            ret                         = addIndent() + valueType + " " + valueName + ";\n";
                            classAndInstance[valueName] = valueType;
                        }
                        break;
                    }
                    case LLIR: {
                        oneBeforeInstruction = "alloca";
                        ret = addIndent() + "%" + regNum + " = " + oneBeforeInstruction + valueType + ", align " + size + "\n";
                        Regs.llirReg["%" + regNum] = valueName;
                        break;
                    }
                    default:
                        break;
                }
                expect(";");
                tokNumCounter++;

                return ret += sent();
            }

            // define and move
            tokNumCounter++;

            if (token[tokNumCounter].tokChar == "{") {
                tokNumCounter++;

                expect("}");
            }

            else if (token[tokNumCounter].tokChar == "\"") {
                tokNumCounter++;
                data = "\"" + token[tokNumCounter].tokChar + "\"";
                tokNumCounter++;
                expect("\"");
            }
            else
                data = addSub();

            tokNumCounter++;
            int hasType;
            hasType = False;

            if (hasType == True && isMut == True) {
                cout << "Warning: value '" << valueName << "' has the @Mut option. " << endl;
                ret = addIndent() + valueName + " = " + data + ";\n" + sent();
                return ret;
            }
            if (hasType == True && isMut != True) {
                cout << "Error: value '" << valueName
                     << "' has already been defined. " << endl;
                exit(0);
            }
            else;
            switch (langMode) {
                case PYTHON: {
                    ret += addIndent() + valueName + " = ";
                    if (data == "class")
                        ret = valueType + "()\n";
                    else if (data != "")
                        ret = data + "\n";
                    break;
                }
                case CPP: {
                    if (valueType == "string") {
                        ret = addIndent() + "__Cru_string " + valueName;
                        if (data != "")
                            ret += " = {" + data + "}";
                        else
                            ;

                    } else if (valueType == "vec") {
                        ret += "__Cru_Vec_" + vecType + " " + valueName;
                        if (vecType == "string") {
                            if (data != "")
                                ret += " = {" + data + "}";
                            else
                                ;
                        }
                    } else {
                        ret = addIndent() + valueType + " " + valueName;

                        if (data != "")
                            ret += " = " + data;
                        else
                            classAndInstance[valueName] = nowClassName;
                    }

                    tokNumCounter++;

                    ret += ";\n" + sent();
                    break;
                }
                case LLIR: {
                    string regNum = std::to_string(registerAmount);
                    string size   = typeSize[valueType];
                    string value;

                    string tmp;
                    string strName;
                    string registerName;
                    string type;
                    int    index;

                    if (LLIRnowVar == "")
                        LLIRnowVar = data;
                    else
                        ret = addIndent() + data + "\n";

                    value = LLIRnowVar;

                    if (valueType == "i8") {
                        registerName =
                            "@.__const." + nowFuncName + "." + regSName;

                        for (index = 1; index < value.size() - 1; index++)
                            tmp += value[index];

                        value     = tmp;
                        type      = "[" + std::to_string(index - 1) + " x i8]";
                        strDefine = registerName +
                            " = private unnamed_addr constant " + type +
                            " c\"" + value + "\", align 1\n";

                        Regs.Reg[regSName]  = {regSName, registerName, type,
                                               std::to_string(index - 1), "\%s"};
                        Regs.registerAmount = registerAmount;

                        string r1           = "%" + std::to_string(registerAmount);

                        ret += strDef(addIndent(), &Regs, registerName, value);

                        oneBeforeInstruction = "call";

                        registerAmount = Regs.registerAmount;
                        regSName       = r1;

                        functionDefine += "declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1";
                    } else {
                        registerName       = "%" + regNum;
                        Regs.Reg[regSName] = {
                            regSName,
                            registerName,
                            valueType,
                            typeSize[valueType],
                            "\%d",
                        };

                        if (value[0] == '%') {
                            string tmp = value;
                            string tmp2;

                            tmp = "%" + std::to_string(registerAmount-1);
                            value = "%" + std::to_string(registerAmount);
                            registerAmount++;
                            tmp2 = "%" + std::to_string(registerAmount);

                            ret =  addIndent() + value + " = alloca " + valueType + ", align " + typeSize[valueType] + "\n" ; 
                            ret += addIndent() + tmp2 + " = " + load(tmp, valueType, typeSize[valueType]);
                            ret += addIndent() + "store " + valueType + " " + tmp2 + ", " + valueType + "* " + registerName  + ", align " + typeSize[valueType] + "\n";
                        }
                        else if (value[0] == ' ')  {
                            ret += value + move(addIndent(), Regs.Reg[regSName], Regs.nowVar);
                        }
                        else
                            ret += move(addIndent(), Regs.Reg[regSName], value );
                        oneBeforeInstruction = "store";
                    }
                    registerAmount++;


                    Regs.llirReg[registerName] = regSName;
                    LLIRnowVar                 = "";

                    expect(";");
                    tokNumCounter++;

                    Regs.nowVar = "";

                    ret += sent();
                    break;
                }
                default: break;
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
            Register tmpReg = Regs;

            if (langMode == LLIR) {
                registerAmount--;

                r1    = "%" + std::to_string(registerAmount++);
                r2    = "%" + std::to_string(registerAmount++);

                sentS = sent();

                r3    = "%" + std::to_string(registerAmount++);
            } else {
                indent++;

                sentS = sent();

                indent--;
            }
            Regs = tmpReg;

            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;
            if (langMode == LLIR) {
                r2.erase(0, 1);
                r3.erase(0, 1);
                /*
                 * if -> if data -> else -> else data
                 */
                string tmpR2 = {r2};
                string tmpR3 = {r3};

                ret += addIndent() + "br i1 " + r1 + ", label %" + r2 + ", label %" + r3 + "\n\n";

                if (token[tokNumCounter].tokChar == "else") {
                    tokNumCounter++;
                    expect("{");
                    tokNumCounter++;
                    elseS = sent();
                    expect("}");
                    tokNumCounter++;
                    expect(";");
                    tokNumCounter++;

                    r3 = std::to_string(registerAmount++);

                    ret += tmpR2 + ":\n" + sentS;
                    usedReturn == 0 ?  addIndent() + "br label %" + r3 + "\n": "";
                    ret += tmpR3 + ":\n";

                    ret += elseS + "\n";
                    usedReturn == 0 ? ret += addIndent() + "br label %" + r3 + "\n" + r3 + ":\n" :"";
                } else {
                    ret += tmpR2 + ":\n" + sentS;
                }
                usedReturn = false;
            } else {
                ret += sentS;
            }
            //ret += addIndent();
            if (langMode == CPP)
                ret += "}";

            return ret + sent();
            break;
        }
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

            switch (langMode) {
                case PYTHON: {
                    ret = addIndent() + "print(" + data + ")\n";
                    break;
                }
                case CPP:
                    ret = addIndent() + "__CRU_Stringput(&" + data + ");\n";
                    break;
                case LLIR: {
                    string loadR1;
                    string loadR2;
                    string printfR;
                    string size = typeSize[regType[data]];

                    if (data[0] == ' ') {
                        ret += data;
                        data = Regs.nowVar;
                        loadR1 = "%" + std::to_string(registerAmount++);
                        Regs.nowVar = loadR1;
                    }

                    strDefine += "@.str." + std::to_string(strAmount) +" = private unnamed_addr constant [3 x i8] c\"" + Regs.Reg[Regs.llirReg[data]].outputFormatSpecifier + "\\00\", align ";
                    if (size != "")
                        strDefine += "1\n";
                    else
                        strDefine += "4\n";

                    string dataN = Regs.llirReg[data];
                    string type  = Regs.Reg[Regs.llirReg[data]].type;
                    string typeSpecifier = Regs.Reg[Regs.llirReg[data]].outputFormatSpecifier;
                    string iNum;

                    if (oneBeforeInstruction == "call") {
                        loadR2 = loadR1;
                        loadR1 = "%" + std::to_string(registerAmount - 2);
                        iNum = "i32";

                    }

                    else if (loadR1.empty()) {
                        loadR1 = "%" + std::to_string(registerAmount++);
                        loadR2 = "%" + std::to_string(registerAmount++);
                        ret += addIndent() + loadR1 + " = " + load(data, Regs.Reg[dataN].type, typeSize[Regs.Reg[dataN].type]);
                        iNum = Regs.Reg[dataN].type;
                    } 

                    else {
                        loadR2 = "%" + std::to_string(registerAmount++);
                        if (typeSpecifier == "\%s") {
                            ret += addIndent() + loadR1;

                            ret += " = getelementptr inbounds " + type + ", " + type + "* " + Regs.llirReg[data] + ", i64 0, i64 0";
                            iNum = "i8*";
                            
                        } else {

                            ret += " = " + load(data, Regs.Reg[dataN].type, typeSize[Regs.Reg[dataN].type]);

                            ret += Regs.Reg[dataN].len;
                            iNum = "i32";
                        }
                        ret += "\n";
                    }

                    ret += addIndent() + loadR2 + " = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str." + std::to_string(strAmount++) + ", i64 0, i64 0), " + iNum + " noundef " + loadR1 + ")\n";

                    if (putDefExists == False)
                        functionDefine += "declare i32 @printf(i8* noundef, ...) #" + std::to_string(funcDefQuantity++);
                    else;

                    putDefExists = True;
                    break;
                }
                default:
                    break;
            }
            if (token[tokNumCounter].tokNum == CANMA) {
                tokNumCounter++;
                if (token[tokNumCounter].tokChar == "int")
                    ret = addIndent() + "printf(\"%d\"," + data + ");\n";
                else if (token[tokNumCounter].tokChar == "vec")
                    ret =
                        addIndent() + "__Cru_Vec_string_put(&" + data + ");\n";
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

            switch (langMode) {
                case PYTHON:
                    ret = addIndent() + "return " + addSub();
                    break;
                case CPP:
                    ret = addIndent() + "return " + addSub() + ";";
                    break;
                case LLIR: {
                    string data = addSub();
                    string type = Regs.Reg[Regs.llirReg[data]].type;

                    if (data[0] == ' ' && !Regs.nowVar.empty()) {
                        string tmp = "%" + std::to_string(registerAmount);

                        ret = data;


                        if (oneBeforeInstruction == "alloca") {
                            ret += load(addIndent(), {"", Regs.nowVar, nowType, llirType[nowType]}, tmp);
                        }
                        ret += addIndent() + "ret " + nowType + " " + Regs.nowVar + "\n";
                        Regs.nowVar.clear();
                    } else {

                        if (data[0] == ' ') {
                            ret = data + "\n";
                            ret += addIndent() + "ret " + nowType + " %" + std::to_string(registerAmount-1);
                            registerAmount++;
                            

                        } else if (!isDigit(data)){
                            ret += addIndent() + "ret " + nowType + " " + data;
                        } else {
                            string nextReg = "%" + std::to_string(registerAmount++);

                            ret += addIndent() + nextReg + " = " + load(data, nowType, typeSize[nowType]);
                            ret += addIndent() + "ret " + nowType + " " + nextReg;
                        }
                    }
                    break;
                }
            }

            // tokNumCounter++;
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            usedReturn = true;

            return ret + "\n" + sent();
        } break;

        case IMPORT: {

            tokNumCounter++;
            string        filedata;
            string        ret;

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

            if (token[tokNumCounter].tokChar == "}" ||
                token[tokNumCounter + 1].tokChar == "")
                return "";
            else if (token[tokNumCounter + 1].tokNum == PLUS) {
                string var    = word();
                string loadR  = "%" + std::to_string(registerAmount++);
                string storeR = "%" + std::to_string(registerAmount++);

                tokNumCounter++;
                expect("+");
                tokNumCounter++;
                expect("+");
                tokNumCounter++;
                expect(";");
                tokNumCounter++;

                if (langMode == LLIR) {
                    ret = addIndent() + loadR + " = " + load(var, "i32", "4");
                    ret += addIndent() + storeR + " = add nsw i32 " + loadR + ", 1\n";
                    ret += addIndent() + "store i32 " + storeR + ", i32* " + var + ", align 4\n";
                }
                return ret + sent();
            } else if (token[tokNumCounter + 1].tokNum == MIN) {
                string var    = word();
                string loadR  = "%" + std::to_string(registerAmount++);
                string storeR = "%" + std::to_string(registerAmount++);

                tokNumCounter++;
                expect("-");
                tokNumCounter++;
                expect("-");
                tokNumCounter++;
                expect(";");
                tokNumCounter++;

                if (langMode == LLIR) {
                    ret = addIndent() + loadR + " = " + load(var, "i32", "4");
                    ret += addIndent() + storeR + " = sub nsw i32 " + loadR + ", 1\n";
                    ret += addIndent() + "store i32 " + storeR + ", i32* " + var + ", align 4\n";
                }
                return ret + sent();
            } else if (token[tokNumCounter + 1].tokChar == "<-") {
                tokNumCounter++;
                tokNumCounter++;
                if (token[tokNumCounter].tokChar == "{") {
                    tokNumCounter++;
                    ret = "char *__tmp[] = {" + funcCallArtgment() + "};\n" + addIndent() + ret + " = _Vec(__tmp)";
                    tokNumCounter-=1;
                    expect("}");
                } else {
                    if (langMode == LLIR) {
                        string data = funCall("");
                        string size = typeSize[regType[ret]];
                        if (Regs.llirReg.find(data) != Regs.llirReg.end()) {

                            string type = Regs.Reg[Regs.llirReg[data]].type;
                            string size = Regs.Reg[Regs.llirReg[data]].len;
                            string data = addSub();
                            string tmpReg =
                                "%" + std::to_string(registerAmount);
                            string mainReg = ret;

                            string tmp     = "%" + std::to_string(registerAmount);

                            ret            = data;
                            ret += addIndent() + "store " + type + " " + tmp + ", " + type + "* " + mainReg + ", align " + size + "\n";

                            Regs.llirReg[tmp] = tmp;
                            regType[tmp] = "i32";
                            registerAmount++;

                        } else
                            ret = addIndent() + "store " + regType[ret] + " " +
                                data + ", " + regType[ret] + "* " + ret +
                                ", align " + size + "\n";
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
            } else {
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
