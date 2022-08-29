#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>
#include <string>

string Node::let() {
    string ret;
    int    isPointer;
    int    index;
    string valueName;
    string valueType;
    string data;
    string vecType;
    string regSName;
    string arraySsents;
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
        tokNumCounter++;
        valueType = word() + valueType;
        tokNumCounter++;
    }

    else if ((token[tokNumCounter].tokNum == DOUBLEQUOT || valueType == "string"))
        valueType = variableType(langMode, "string") + "*";

    else if (( valueType.back() == '*' || valueType == "int"))
        valueType = variableType(langMode, "int") + "*";

    if (token[tokNumCounter].tokChar == "<") {
        tokNumCounter++;
        vecType = token[tokNumCounter].tokChar;
        valueType = "vec";
        tokNumCounter++;
        expect(">");
        tokNumCounter++;
    }


    llirType[valueName] = valueType;

    // TODO : ewfactoring

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
    } else if (valueType == "vec") {
        int words = 1;
        expect("[");
        tokNumCounter++;

        int arrayIndex = 0;

            while (token[tokNumCounter].tokChar != "]") {
                if (token[tokNumCounter].tokChar == ",") {
                    words++;
                    tokNumCounter++;
                    continue;
                }
                data = addSub();

                string tmp;

                for (index = 1; index < data.size() - 1; index++)
                    tmp += data[index];

                data = tmp;
                if (langMode == LLIR) {
                    strDefine += "@str." + std::to_string(strAmount) + " = private unnamed_addr constant [ " + std::to_string(index) + "x i8 ] c\"" + data + "\\00\", align 1\n";
                    arraySsents += "i8* getelementptr inbounds ([" + std::to_string(index) + " x i8], [" + std::to_string(index) + " x i8]* @str." + std::to_string(strAmount++) + ", i32 0, i32 0),";
                }
                else if (langMode == CPP)
                   arraySsents += regSName + "[" + std::to_string(arrayIndex++) + "] = \"" + data + "\";\n";
                else if (langMode == PYTHON)
                    arraySsents += "\"" + data + "\", ";

                string tmpVarName = "str." + regSName;
                tokNumCounter++;
            }

            arraySsents.pop_back();

            if (langMode==LLIR)
                strDefine += "@__const." + nowFuncName + "." + regSName + " = private unnamed_addr constant [" + std::to_string(words) + " x i8*] [" + arraySsents + "], align 16\n";

            else if (langMode == PYTHON)
                arraySsents.substr(0, arraySsents.size()-1);

        index = words;
        expect("]");
    }
    else if (token[tokNumCounter].tokChar == "\"") {
        tokNumCounter++;
        data = "\"" + token[tokNumCounter].tokChar + "\"";
        tokNumCounter++;
        expect("\"");
        switch (langMode){
            case CPP:
                valueType = "__Cru_string";
                break;
            case LLIR:
                valueType = "i8*";
                break;
        }
    } else {
        data = addSub();
        if (Regs.Reg[Regs.llirReg[data]].type[Regs.Reg[Regs.llirReg[data]].type.size()-2] == '8')
            valueType = variableType(langMode, "string");
        if  (!isDigit(data))
            valueType = variableType(langMode, "int");
        else {
            valueType = Regs.Reg[Regs.llirReg[data]].type;
            valueType.pop_back();
        }
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

    // 確認
    determinationOfOwnership(&data);

    // 所有権の破棄
    drop(data);

    switch (langMode) {
        case PYTHON: {

            // 所有権を与える
            give(valueName);
            ret += addIndent() + valueName + " = ";
            if (data == "class")
                ret += valueType + "()\n";
            else if (data != "" && valueType == "vec")
                ret += "[" + arraySsents + "]\n";
            else if (data != "")
                ret += data + "\n";
            expect(";");
            tokNumCounter++;

            Regs.llirReg[valueName] = valueName;
            Regs.Reg[valueName].name = valueName;
            Regs.Reg[valueName].regName = valueName;
            Regs.Reg[valueName].type = valueType;
            break;
        }
        case CPP: {
            string ofs;

            // 所有権を与える
            give(valueName);

            if (valueType == "__Cru_string") {
                ret = addIndent() + "__Cru_string " + valueName;

                if (!data.empty()) {
                    ret += " = ";

                    // if variabel is exist?
                    if (varExist(data))
                        ret += data;

                    else if (data != "")
                        ret += "{" + data + "}";
                }
                ofs = "__CRU_Stringput(&";
            } else if (valueType == "vec") {
                if (vecType == "string" && data != "") {
                    ret += addIndent() + "char **" + valueName + ";\n";
                    ret += addIndent() + arraySsents;
                }
            } else {
                ret = addIndent() + valueType + " " + valueName;

                if (data != "")
                    ret += " = " + data;
                else
                    classAndInstance[valueName] = nowClassName;
                ofs = "printf(\"\%d\n\", ";
            }

            tokNumCounter++;

            ret += ";\n";
            Regs.llirReg[valueName] = valueName;
            Regs.Reg[valueName].name = valueName;
            Regs.Reg[valueName].regName = valueName;
            Regs.Reg[valueName].type = valueType;
            Regs.Reg[valueName].outputFormatSpecifier = ofs;
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

            if (data[0] == ' ') {
                ret = data;
                LLIRnowVar = Regs.nowVar;
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
                    ret += addIndent() + loadReg + " = load " + valueType + ", " + valueType + "* " + Regs.llirReg[Regs.llirReg[value]]+ ", align " + typeSize[valueType] + "\n";
                    ret += addIndent() + "store " + valueType + "" + loadReg + ", " + valueType + "* " + r1  + ", align " + typeSize[valueType] + "\n";
                    type = valueType;
                    registerName = r1;
                }

                Regs.Reg[regSName].isMut   = isMut;
                Regs.Reg[registerName].ownerShip = true;

                Regs.llirReg[regSName] = r1;
                Regs.llirReg[r1] = registerName;
                Regs.llirReg[registerName] = regSName;
            } else if (valueType == "vec"){
                string allocaReg = "%" + std::to_string(registerAmount++);
                string bitcastReg = "%" + std::to_string(registerAmount);
                string dataLen = std::to_string(index);

                registerName = "@__const." + nowFuncName + "."+regSName;
                valueType = "[" + dataLen + "x i8*]";

                ret = addIndent() + allocaReg + " = alloca [ " + dataLen + " x i8* ], align 16\n";
                ret += addIndent() + bitcastReg + " = bitcast [" + dataLen + " x i8*]* " + allocaReg + " to i8*\n"; 
                ret += addIndent() + "call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 " + bitcastReg + ", i8* align 16 bitcast (" + valueType + "* " + registerName + " to i8*), i64 24, i1 false)\n";

                Regs.llirReg[regSName] = allocaReg;
                Regs.llirReg[allocaReg] = registerName;
                Regs.llirReg[registerName] = regSName;
                Regs.Reg[registerName].ownerShip = true;
                Regs.Reg[regSName].type = valueType;
            } else {
                // int
                registerName       = "%" + regNum;

                string pointerReg        = "%" + std::to_string(registerAmount++);
                string varReg            = "%" + std::to_string(registerAmount);

                valueType += "*";

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

                    
                    if (oneBeforeInstruction == "add" || oneBeforeInstruction == "sub" ||
                            oneBeforeInstruction == "mul" || oneBeforeInstruction == "div") {
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
                Regs.Reg[regSName].isMut = isMut;

                Regs.llirReg[regSName] = registerName;
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
            Regs.Reg[regSName].ownerShip = true;

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
            // {
            tokNumCounter++;
            expect("{");

            Register tmpRegs = Regs;

            // icmp
            if (langMode == LLIR)
                ifSentS += cmpL + ":\n";
            else
                indent++;

            tokNumCounter++;
            ifSentS += sent();

            (langMode==LLIR) ?
                0:
                indent--;

            Regs = tmpRegs;

            // }
            expect("}");
            tokNumCounter++;
            expect(";");
            tokNumCounter++;

            switch (langMode) {
                case CPP: {
                    ret = addIndent() + "if (" + evalSent + ") {\n";
                    ret += ifSentS;
                    ret += addIndent() + "}\n";
                    break;
                }

                case PYTHON: {
                    ret = addIndent() + "if " + evalSent + ":\n";
                    ret += ifSentS;
                    break;
                }

                case LLIR: {
                    cmpR = std::to_string(registerAmount++);

                    ret      = evalSent;
                    ret += addIndent() + "br i1 " + brAns + ", label %" + cmpL + ", label %" + cmpR + "\n";
                    ret += ifSentS;
                    ret += cmpR + ":\n";
                    break;
                }
            }

            ret += sent();
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
            string ofs;
            string loadR1;
            string loadR2;
            string putSent;

            expect("put");
            tokNumCounter++;
            string data = addSub();

            tokNumCounter++;

            loadR1 = "%" + std::to_string(registerAmount);

            ofs = Regs.Reg[data].outputFormatSpecifier;

            if (data[0] == '&') {
                // 借用
                data = addSub();

                string variable = Regs.llirReg[data];
                string borrow = "&" + Regs.llirReg[data]; 

                Regs.Reg[borrow] = Regs.Reg[variable];
                Regs.Reg[borrow].ownerShip = true;
                tokNumCounter++;
                data = Regs.Reg[borrow].regName;
                Regs.llirReg[borrow] = data;
                ofs = Regs.Reg[borrow].outputFormatSpecifier;
            }
            else; 

            if (data[0] == ' ') {

                ret += data;
                data = Regs.nowVar;
                Regs.nowVar = loadR1;
                loadR1 = data;
                if (langMode == LLIR)
                    ofs         = "\%d";
                else
                    putSent = "printf(\"\%d\", " + data + ");\n";

            } else if (data[0] == '\"') {
                data.erase(0, 1);
                data.pop_back();

                expect(";");
                tokNumCounter++;

                if (langMode == LLIR) {
                    // llivm ir
                    string typeArray  = "[ " + std::to_string(data.size() + 2) + " x i8 ]";
                    string stringWord = data;

                    data              = "@.str." + std::to_string(strAmount++);
                    strDefine += data + " = private unnamed_addr constant " + typeArray + " c\"" + stringWord + "\\0A\\00\", align 1\n";
                    ret += addIndent() + "%" + std::to_string(registerAmount++) + " = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds (" + typeArray + ", " + typeArray + "* " + data + ", i64 0, i64 0))\n";

                    if (!putDefExists)
                        functionDefine += "declare i32 @printf(i8* noundef, ...) #" + std::to_string(funcDefQuantity++) + "\n";
                    putDefExists = True;

                    return ret + sent();
                }
                ret += addIndent();
                if (langMode == CPP) {
                    ret += "printf(\""+ data + "\\n\");";     
                    return ret + sent();
                }
                ret += "print(\""+ data + "\");";     
                return ret + sent();

            } else if (data.find("(") != string::npos) {
                string noBracket;
                putSent = "printf(\"\%d\\n\", " + data + ");";
                for (auto i : data) {
                    if (i == '(') break;
                    noBracket += i;
                }
                ofs  = Regs.Reg[Regs.llirReg[noBracket]].outputFormatSpecifier;
            } else {
                if (langMode == LLIR) {
                    data = Regs.llirReg[data];
                    ofs  = Regs.Reg[data].outputFormatSpecifier;
                } else if (ofs.empty())
                    putSent += Regs.Reg["$__tmp_REG"].outputFormatSpecifier + data + ");\n";
                else
                    putSent = ofs + data + ");\n";
            }

            determinationOfOwnership(&data);

            drop(data);

            switch (langMode) {
                case PYTHON: {
                    ret = addIndent() + "print(" + data + ")\n";
                    break;
                }
                case CPP: {
                    ret = addIndent() + putSent;
                    break;
                }
                case LLIR: {
                    string printfR;
                    string size = typeSize[regType[data]];

                    string dataN = Regs.llirReg[data];
                    string type  = Regs.Reg[data].type;
                    string typeSpecifier = Regs.Reg[data].outputFormatSpecifier;
                    string iNum;

                    if (oneBeforeInstruction == "call" && data[0] == '%') {
                        iNum   = Regs.Reg[data].type;
                    }
                    else if (oneBeforeInstruction == "call" && data[0] == '@') {
                        ret += addIndent() + loadR1 + " = load i8*, i8** " + Regs.llirReg[data] + ", align " + typeSize["i8"] + "\n";
                        iNum   = "i8*";
                    }
                    else if (type == "vec") {
                        iNum = "i8*"; 
                        ofs = "\%s";
                    } 

                    else {
                        if (typeSpecifier == "\%s") {
                            loadR1 = "%" + std::to_string(registerAmount++);
                            ret += addIndent() + loadR1 + " = " + load(Regs.llirReg[data], type, typeSize[type]); 
                            iNum = type;
                        } else {
                            string loadR2;
                            string varType;

                            varType = Regs.Reg[data].type;

                            loadR1 = data;

                            if (varType.back() == '*') {
                                loadR1 = "%" + std::to_string(registerAmount++);
                                ret += addIndent() + loadR1 + " = " + load(data, varType, typeSize[varType]);
                                loadR2  = "%" + std::to_string(registerAmount++);

                                varType = Regs.Reg[data].type.substr(0, varType.size() - 1);

                                ret += addIndent() + loadR2 + " = " + load(loadR1, varType, typeSize[varType]);
                                loadR1 = loadR2;
                            }
                            iNum = varType;
                        }
                    }
                    strDefine += "@.str." + std::to_string(strAmount) +" = private unnamed_addr constant [4 x i8] c\"" + ofs + "\\0A\\00\", align ";

                    if (size != "")
                        strDefine += "1\n";
                    else
                        strDefine += "4\n";


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
        case RETURN: {
            string ret;
            
            expect("return");
            tokNumCounter++;

            switch (langMode) {
                case PYTHON:
                    ret = addIndent() + "return " + addSub();
                    break;
                case CPP: {
                    ret = addIndent() + "return " + addSub() + ";";
                    break;
                    }
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
