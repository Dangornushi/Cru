#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>
#include <string>

string Node::let() {
    string ret;
    int    isPointer;
    string valueName;
    string valueType;
    string data;
    string vecType;
    string regSName;
    bool   isBorrow = false;
    bool   isMut    = false;

    expect("let");
tokNumCounter++;

    isMut = False;

    if (token[tokNumCounter].tokNum == MUT) {
        isMut = true;
        tokNumCounter++;
        valueType = "*";
    }

    regSName  = token[tokNumCounter].tokChar;
    valueName = word();

    tokNumCounter++;

    if (token[tokNumCounter].tokNum == CORON) {
        expect(":");

        tokNumCounter++;

        valueType = word() + valueType;

        tokNumCounter++;
    }

    if (token[tokNumCounter].tokChar == "<") {
        tokNumCounter++;
        vecType = token[tokNumCounter].tokChar;
        tokNumCounter++;
        expect(">");
        tokNumCounter++;
    }

    if ((token[tokNumCounter].tokNum == DOUBLEQUOT || valueType == "string") && langMode == LLIR) {
        valueType = "string";
        valueType = "i8"; 
    }

    else if ((valueType == "int" || valueType.back() == '*') && langMode == LLIR )
        valueType = "i32*";

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
                oneBeforeInstruction       = "alloca";
                ret                        = addIndent() + "%" + regNum + " = " + oneBeforeInstruction + valueType + ", align " + size + "\n";
                Regs.llirReg["%" + regNum] = valueName;
                break;
            }
            default:
                break;
        }
        expect(";");
        tokNumCounter++;

        return ret;
    }

    // define and move
    tokNumCounter++;

    if (token[tokNumCounter].tokChar == "{") {
        tokNumCounter++;

        expect("}");
    } else if (token[tokNumCounter].tokNum == ADDRESS) {
        isBorrow = true;
        tokNumCounter++;
    }
    else if (token[tokNumCounter].tokChar == "\"") {
        tokNumCounter++;
        data = "\"" + token[tokNumCounter].tokChar + "\"";
        tokNumCounter++;
        expect("\"");
        valueType = "i8*";
    } else {
        data = addSub();
        if (Regs.Reg[Regs.llirReg[data]].type[Regs.Reg[Regs.llirReg[data]].type.size()-2] == '8')
            valueType = "i8*";
        if  (!isDigit(data))
            valueType = "i32*";
        else
            valueType = Regs.Reg[Regs.llirReg[data]].type;
    }

    tokNumCounter++;
    int hasType;
    hasType = False;

    if (hasType == True && isMut == True) {
        cout << "Warning: value '" << valueName << "' has the @Mut option. " << endl;
        ret = addIndent() + valueName + " = " + data + ";\n";
        return ret;
    }
    if (hasType == True && isMut != True) {
        cout << "Error: value '" << valueName << "' has already been defined. " << endl;
        exit(0);
    } else;

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

            // 確認
            determinationOfOwnership(&data);

            // 所有権の破棄
            drop(data);

            // 所有権を与える
            give(valueName);

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
                    else;
                }
            } else {
                ret = addIndent() + valueType + " " + valueName;

                if (data != "")
                    ret += " = " + data;
                else
                    classAndInstance[valueName] = nowClassName;
            }

            tokNumCounter++;

            ret += ";\n";
            break;
        }
        case LLIR: {
            string regNum = std::to_string(registerAmount);
            string size;
            string value;

            string tmp;
            string strName;
            string registerName;
            string type;
            string outputFormatSpecifier;
            int    index;

            if (data[0] == ' ') {
                ret = data;
                LLIRnowVar = Regs.nowVar;
                registerAmount++;
                regNum = std::to_string(registerAmount);
                valueType = Regs.Reg["$__tmp_r"].type;
            }
            else if (LLIRnowVar == "")
                LLIRnowVar = data;
            else 
                ret = addIndent() + data + "\n";

            value = LLIRnowVar;

            if (value[0] == ' ') {
                ret += value;
                value = "%" + std::to_string(registerAmount++);
            }

            // 確認
            determinationOfOwnership(&value);

            // 所有権の破棄
            drop(value);

            if (valueType == "i8*") {
                // string

                string r1           = "%" + std::to_string(registerAmount);
                string loadReg      = "%" + std::to_string(registerAmount+1);

                size = typeSize[valueType];
                outputFormatSpecifier = "\%s";

                if (value[0] == '"') {

                    registerName = "@.str." + regSName;

                    for (index = 1; index < value.size() - 1; index++)
                        tmp += value[index];

                    value = tmp;

                    type  = "[" + std::to_string(index) + " x i8]";
                    strDefine += registerName + " = private unnamed_addr constant " + type + " c\"" + value + "\\00\", align 1\n";
                    Regs.Reg[regSName]  = {regSName, registerName, type, std::to_string(index - 1), "\%s"};
                    string len          = std::to_string(index - 1);

                    ret += addIndent() + r1 + " = alloca i8*, align 8\n";
                    ret += addIndent() + "store i8* getelementptr inbounds (" + type + ", " + type + "* " + registerName + ", i64 0, i64 0), i8** " + r1 + ", align 8\n";

                    oneBeforeInstruction = "call";
                }
                else {
                    registerAmount++;
                    ret += addIndent() + r1 + " = alloca i8*, align 8\n";
                    ret += addIndent() + loadReg + " = load " + valueType + ", " + valueType + "* " + Regs.llirReg[value] + ", align " + typeSize[valueType] + "\n";
                    ret += addIndent() + "store " + valueType + "" + loadReg + ", " + valueType + "* " + r1  + ", align " + typeSize[valueType] + "\n";
                    type = valueType;
                    registerName = r1;
                }

                Regs.Reg[regSName].isMut   = isMut;
                Regs.Reg[registerName].ownerShip = true;

                Regs.llirReg[regSName] = r1;
                Regs.llirReg[r1] = registerName;
                Regs.llirReg[registerName] = regSName;

            } else {
                // int
                registerName       = "%" + regNum;

                string pointerReg        = "%" + std::to_string(registerAmount++);
                string varReg            = "%" + std::to_string(registerAmount);

                string pointerType       = valueType;

                (valueType.size() == 4) ? valueType = valueType.substr(0, valueType.size() - 1) : "";

                ret += addIndent() + pointerReg + " = alloca " + pointerType + ", align " + typeSize[pointerType] + "\n";
                ret += addIndent() + varReg + " = alloca " + valueType + ", align " + typeSize[valueType] + "\n";
                ret += addIndent() + "store " + valueType + "* " + varReg + ", " + pointerType + "* " + pointerReg + ", align " + typeSize[pointerType] + "\n";

                if (value[0] == '%') {
                    registerAmount++;
                    string loadPointerReg1;
                    string loadVariableReg1;
                    string loadVariableReg2;

                    
                    if (oneBeforeInstruction == "add" || oneBeforeInstruction == "sub") {
                        loadPointerReg1 = value;
                    }
                    else {
                        loadPointerReg1  = "%" + std::to_string(registerAmount++);
                        loadVariableReg1 = "%" + std::to_string(registerAmount++);

                        ret += addIndent() + loadPointerReg1 + " = load " + valueType + "*, " + valueType + "** " + value + ", align " + typeSize[pointerType] + "\n";
                        ret += addIndent() + loadVariableReg1 + " = load " + valueType + ", " + valueType + "* " + loadPointerReg1 + ", align " + typeSize[valueType] + "\n";
                    }

                    loadVariableReg2 = "%" + std::to_string(registerAmount);
                    ret += addIndent() + loadVariableReg2 + " = load " + valueType + "*, " + valueType + "** " + pointerReg + ", align " + typeSize[pointerType] + "\n";

                    value  = loadVariableReg1;
                    varReg = loadVariableReg2;

                    ret += addIndent() + "store " + valueType + " " + loadPointerReg1 + ", " + pointerType + " " + varReg + ", align " + typeSize[valueType] + "\n\n";

                } else
                    ret += addIndent() + "store " + valueType + " " + value + ", " + valueType + "* " + varReg + ", align " + typeSize[valueType] + "\n\n";

                LLIRnowVar           = "";
                oneBeforeInstruction = "store";
                Regs.llirReg[registerName] = regSName;
                Regs.Reg[regSName].isMut = isMut;
                Regs.llirReg[regSName] = pointerReg;
                Regs.llirReg[pointerReg] = registerName;
                Regs.llirReg[registerName] = regSName;

                valueType = pointerType;
                size = std::to_string(index - 1);
                outputFormatSpecifier = "\%d";
            }

            Regs.Reg[regSName] = {
                    regSName,
                    registerName,
                    valueType,
                    size,
                    outputFormatSpecifier,
            };

            registerAmount++;

            LLIRnowVar                 = "";

            // 所有権を与える
            give(regSName);

            expect(";");
            tokNumCounter++;

            Regs.nowVar = "";
            break;
        }
        default:
            break;
    }

    return ret;
}

string Node::sent() {
    string ret;
    ret = funCall("");

    switch (token[tokNumCounter].tokNum) {

        case LET: {
            return let() + sent();
        } break;
        case IF: {
            string ifSentS;
            string evalSent;
            string cmpL;
            string cmpR;
            string brAns;
            string brSent;

            // if $1 == $2
            expect("if");
            tokNumCounter++;

            evalSent = eval();
            brAns    = "%" + std::to_string(registerAmount++);
            cmpL     = std::to_string(registerAmount++);
            // icmp
            ret      = evalSent;

            ifSentS += cmpL + ":\n";

            // {
            tokNumCounter++;
            expect("{");

            Register tmpRegs = Regs;

            tokNumCounter++;
            ifSentS += sent();

            Regs = tmpRegs;

            // }
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            cmpR     = std::to_string(registerAmount++);
             
            // br:
            ret += addIndent() + "br i1 " + brAns + ", label %" + cmpL + ", label %" + cmpR + "\n";
            ret += ifSentS;
            ret += cmpR + ":\n" + sent();
            break;
        }
        case FOR: {
            string ret;
            string loopS;
            string sentS;
            string brReg;

            string brCmp;
            string brRegL;
            string brRegR;
            string itrate;
            string nextWord;
 
            expect("for");
            tokNumCounter++;

            nextWord = token[tokNumCounter].tokChar;

            brReg    = std::to_string(registerAmount + 1);
            loopS    = loop();

            itrate   = Regs.Reg[nextWord].regName;

            brCmp    = "%" + std::to_string(registerAmount++);
            brRegL   = std::to_string(registerAmount++);

            expect("{");
            tokNumCounter++;

            indent++;
            sentS = sent();
            indent--;

            string a = std::to_string(registerAmount++);
            string b = std::to_string(registerAmount++);

            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            brRegR = std::to_string(registerAmount++);

            switch(langMode) {
                case PYTHON:
                    ret = addIndent() + "for " + loopS + ":\n" + sentS;
                    break;
                case CPP:
                    ret = addIndent() + "for (" + loopS + ") {\n" + sentS + "}\n";
                    break;
                case LLIR: {
                    // if
                    ret += loopS;
                    ret += addIndent() + "br i1 " + brCmp + ", label %" + brRegL + ", label %" +brRegR + "\n";

                    // if block
                    ret += brRegL + ":\n";
                    ret += sentS;

                    // increment
                    ret += addIndent() + "%" + a + " = load i32, i32*" + itrate + ", align 4\n";
                    ret += addIndent() + "%" + b + " = add nsw i32 %" + a + ", 1\n";
                    ret += addIndent() + "store i32 %" + b + ", i32* " + itrate + ", align 4\n";

                    ret += addIndent() + "br label %" + brReg + "\n";

                    // if end brock
                    ret += brRegR + ":\n";
                    break;
                }
            }

            return ret + sent();
            break;
        }
        case LOOP: {

            string loopSentS;
            string loopEntry;

            tokNumCounter++;
            expect("{");
            tokNumCounter++;

            loopEntry = std::to_string(registerAmount++);

            ret  = addIndent() + "br label %" + loopEntry + "\n";
            ret += loopEntry + ":\n";
            ret += sent();
            ret += addIndent() + "br label %" + loopEntry + "\n";

            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            return ret + sent();
            break;
        }
        case PUT: {
            string ret;

            expect("put");
            tokNumCounter++;
            string data = addSub();
            tokNumCounter++;

            if (data[0] == '&') {
                // 借用
                data = addSub();

                string variable = Regs.llirReg[data];
                string borrow = "&" + Regs.llirReg[data]; 

                Regs.Reg[borrow] = Regs.Reg[variable];
                Regs.Reg[borrow].ownerShip = true;
                tokNumCounter++;
                data = Regs.Reg[borrow].regName;
                Regs.llirReg[data] = borrow;
            }

            if (data[0] == ' ') {
                ret += data;
                data = Regs.nowVar;
                registerAmount++;
            }

            determinationOfOwnership(&data);

            drop(data);

            switch (langMode) {
                case PYTHON: {
                    ret = addIndent() + "print(" + data + ")\n";
                    break;
                }
                case CPP:
                    ret = addIndent() + "printf(\"\%d\", " + data + ");\n";
                    break;
                case LLIR: {
                    string loadR1;
                    string loadR2;
                    string printfR;
                    string size = typeSize[regType[data]];
                    string ofs;

                    loadR1 = "%" + std::to_string(registerAmount++);

                    if (data[0] == ' ') {
                        ret += data;
                        data        = Regs.nowVar;
                        Regs.nowVar = loadR1;
                        ofs         = "\%d";
                    } else
                        ofs = Regs.Reg[data].outputFormatSpecifier;

                    strDefine += "@.str." + std::to_string(strAmount) +" = private unnamed_addr constant [4 x i8] c\"" + ofs + "\\0A\\00\", align ";

                    if (size != "")
                        strDefine += "1\n";
                    else
                        strDefine += "4\n";

                    string dataN = Regs.llirReg[data];
                    string type  = Regs.Reg[data].type;
                    string typeSpecifier = Regs.Reg[data].outputFormatSpecifier;
                    string iNum;

                    if (oneBeforeInstruction == "call" && data[0] == '%') {
                        ret += addIndent() + loadR1 + " = load " + type + "*, " + type + "** " + data + ", align " + typeSize[type] + "\n";
                        iNum   = "i8*";
                    }
                    else if (oneBeforeInstruction == "call" && data[0] == '@') {
                        ret += addIndent() + loadR1 + " = load i8*, i8** " + Regs.llirReg[data] + ", align " + typeSize["i8"] + "\n";
                        iNum   = "i8*";
                    }

                    else if (loadR1.empty()) {
                        loadR1 = "%" + std::to_string(registerAmount++);
                        loadR2 = "%" + std::to_string(registerAmount++);
                        iNum   = Regs.Reg[dataN].type;
                        ret += addIndent() + loadR1 + " = " + load(data, iNum, typeSize[iNum]);
                    }

                    else {
                        ret += addIndent() + loadR1;

                        if (typeSpecifier == "\%s") {
                            ret += " = " + load(Regs.llirReg[data], type, typeSize[type]); 
                            iNum = type;
                        } else {
                            string loadR2;
                            string varType;

                            varType = Regs.Reg[data].type;

                            cout << data << endl;

                            ret += " = " + load(Regs.llirReg[data], varType, typeSize[varType]);
                            loadR2 = "%" + std::to_string(registerAmount++);

                            if (varType.back() == '*')
                                varType = Regs.Reg[data].type.substr(0, varType.size() - 1);

                            ret += addIndent() + loadR2 + " = " + load(loadR1, varType, typeSize[varType]);
                            loadR1 = loadR2;
                            iNum = varType;
                        }
                    }

                    if (Regs.Reg[Regs.llirReg[data]].isMut) { 
                        string tmp = load(loadR1, iNum, typeSize[iNum]);
                        loadR1 = "%" + std::to_string(registerAmount++);
                        ret += addIndent() + loadR1 + " = " + tmp;
                    }

                    loadR2 = "%" + std::to_string(registerAmount++);

                    ret += addIndent() + loadR2 + " = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str." + std::to_string(strAmount++) + ", i64 0, i64 0), " + iNum + " noundef " + loadR1 + ")\n";

                    if (putDefExists == False)
                        functionDefine += "declare i32 @printf(i8* noundef, ...) #" + std::to_string(funcDefQuantity++) + "\n";
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
                    ret = addIndent() + "__Cru_Vec_string_put(&" + data + ");\n";
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
                        ret        = data;

                        if (oneBeforeInstruction == "alloca")
                            ret += load(addIndent(), {"", Regs.nowVar, nowType, llirType[nowType]}, tmp);
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
                    ret =  addIndent() + loadR + " = " + load(var, "i32", "4");
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
                    ret =  addIndent() + loadR + " = " + load(var, "i32", "4");
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
                            string data    = addSub();
                            string type    = Regs.Reg[Regs.llirReg[data]].type;
                            string size    = Regs.Reg[Regs.llirReg[data]].len;
                            string tmp     = "%" + std::to_string(registerAmount);
                            string mainReg = ret;
                            ret            = data;

                            ret += addIndent() + "store " + type + " " + tmp + ", " + type + "* " + mainReg + ", align " + size + "\n";

                            Regs.llirReg[tmp] = tmp;
                            regType[tmp]      = "i32";
                            registerAmount++;

                        } else
                            ret = addIndent() + "store " + regType[ret] + " " + data + ", " + regType[ret] + "* " + ret + ", align " + size + "\n";
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
