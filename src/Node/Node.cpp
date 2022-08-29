#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <map>

string parseQuort(const string s1) {
    string ret;

    for (int i = 1; i < s1.size() - 1; i++)
        ret += (s1[i]);
    return ret;
}

string variableType(int langMode, string typeName) {
    string type;
    map<string, string> varType = {
        {"int", "i32"},
        {"string", "i8"},
        {"vec&string", "i8*"},
    };

    switch(langMode) {
        case CPP: {
            (typeName == "string") ?
                type = "__Cru_string" :
                type = typeName;
            break;
        }
        case LLIR: {
            type = varType[typeName];
            break;
        }
    }
    return type;
}

Node::Node(int langMode) {
    this->indent          = 0;
    this->registerAmount  = 0;
    this->strAmount       = 0;
    this->funcDefQuantity = 0;
    this->usedReturn      = false;
    this->putDefExists    = false;
    this->enumEnabled     = false;
    this->classEnabled    = false;
    this->langMode        = langMode;

    this->typeSize = {
        {"i8", "1"},
        {"i32", "4"},
        {"i64", "8"},
        {"i8*", "8"},
        {"i32*", "8"},
        {"i64*", "8"},
        {"i8**", "8"},
    };
    this->opToIR = {
        {EQEQ, "eq"},
        {BIG, "slt"},
        {MINI, "sgt"},
        {PLUS, "add"},
        {MIN, "sub"},
        {DIV, "div"},
        {MUL, "mul"},
    };
    this->typeToOfs = {
        {"int", "\%d"},
        {"string", "\%s"},
    };
    this->typeToPrint = {
        {"int", "printf(\"\%d\", "},
        {"string", "__CRU_Stringput(&"},
    };
}

// loop to the end of file.
string Node::parse(vector<tokens> geToken) {
    tokNumCounter = 0;

    string ret;
    string write;
    string pathVec = execDir();
    string filedata;
    string filename;

    token = geToken;

    if (token[tokNumCounter].tokNum == IMPORT) {

        tokNumCounter++;
        filename = pathVec + parseQuort(word());

        std::ifstream reading_file;
        reading_file.open(filename, std::ios::in);

        while (std::getline(reading_file, filedata))
            write += filedata + "\n";

        expect(";");

        tokNumCounter++;
        write += parse(geToken);

    }

    if (langMode == CPP) {

        filename = pathVec + "std.h";

        std::ifstream reading_file;
        reading_file.open(filename, std::ios::in);

        while (std::getline(reading_file, filedata))
            write += filedata + "\n";
    }

    write += functionDefinition();

    return write;
}

void Node::expect(string str) {
    if (token[tokNumCounter].tokChar != str) {
        cout << "Err word: '" << token[tokNumCounter].tokChar << "', Expected : '" << str << "'" << endl;
        exit(1);
    }
}

bool Node::varExist(string variable) {
    if (Regs.Reg[Regs.llirReg[variable]].name.empty())
        return false;
    return true;
}

string getVarName(Register Regs, string *var) {
    string ret;
    if ((*var)[0] == '%')
        ret = Regs.llirReg[*var];
    if ((*var)[0] == '@')
        ret = Regs.Reg[*var].name;
    return ret;
}

// 所有権があるか判定
bool Node::determinationOfOwnership(string *var) {
    string tmp = getVarName(Regs, var);

    if (Regs.Reg[tmp].ownerShip == false && isDigit(tmp) && tmp[0] != '"' && tmp != "&"){
        cout << "Err: moved var > " << tmp << endl;
        exit(1);
    }
    return Regs.Reg[*var].ownerShip;
}

void Node::drop(string var) {
    string varName = getVarName(Regs, &var);

    if ((varName)[0] != '&')
        Regs.Reg[varName].ownerShip = false;
    else 
        Regs.Reg[varName].ownerShip = true;
}

void Node::give(string var) {
    string varName = getVarName(Regs, &var);
    Regs.Reg[varName].ownerShip = true;
}

string Node::addIndent() {
    string ret = "";
    for (int i = 0; i < indent; i++)
        ret += "    ";
    return ret;
}

string Node::addSub() {
    string ret = mulDiv();

    int nextWord;
    string op;
    string r1;
    string loadRet;
    int    nextOP       = token[tokNumCounter + 1].tokNum;
    bool   nextOPisTrue = nextOP == PLUS || nextOP == MIN;
    string loadPointerReg;

    if (langMode == LLIR && nextOPisTrue) {
        r1  = ret;
        ret = "";

        if (r1[0] == ' ') {
            ret += r1 + "\n";
        } else {
            //Regs.nowVar
            loadPointerReg = "%" + std::to_string(registerAmount++);
            string loadVariableReg = "%" + std::to_string(registerAmount++);
            string pointerType = Regs.Reg[Regs.llirReg[r1]].type;
            string variablesType = Regs.Reg[Regs.llirReg[r1]].type;

            variablesType.pop_back();

            ret += addIndent() + loadPointerReg + " = " + load(r1, pointerType, typeSize[pointerType]);
            ret += addIndent() + loadVariableReg + " = " + load(loadPointerReg, variablesType, "4");
            Regs.nowVar = loadVariableReg;
            Regs.Reg["$__tmp_r"].type = variableType(langMode, "int");//"i32";
        }
    }

    while (nextOPisTrue) {
        op = token[tokNumCounter + 1].tokChar;
        nextOP = token[tokNumCounter + 1].tokNum;
        tokNumCounter += 2;

        string s2;

        s2 = token[tokNumCounter].tokChar;

        if (!isDigit(ret) && !isDigit(s2)) {
            int Lnum = std::stoi(ret);
            int Rnum = std::stoi(s2);

            switch (nextWord){
                case PLUS:
                    return std::to_string(Lnum + Rnum);
                case MIN:
                    return std::to_string(Lnum - Rnum);
                default:
                    break;
            }
        }
        else if (langMode == LLIR) {

            string newS2;
            string newNowVar;
            string ansReg;
            string loadReg;

            if (ret[0] != ' ')
                Regs.nowVar = r1;

            newNowVar = Regs.nowVar;
            s2        = mulDiv();

            if (s2[0] == '%') {
                string pointerReg2  = "%" + std::to_string(registerAmount++);
                string variableReg2 = "%" + std::to_string(registerAmount++);

                ret += addIndent() + pointerReg2 + " = " + load(s2, "i32*", "8");
                ret += addIndent() + variableReg2 + " = " + load(pointerReg2, "i32", "4");
                newS2 = variableReg2;
            } else if (!isDigit(s2)) {
                newS2 = s2;
            } else {
                newS2 = "%" + std::to_string(registerAmount - 1);
                ret += s2 + "\n";
            }

            ansReg               = "%" + std::to_string(registerAmount++);
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN;
            oneBeforeInstruction = opToIR[nextOP];
            ret += addIndent() + ansReg + " = " + oneBeforeInstruction + " nsw i32 " + newNowVar + ", " + newS2 +"\n\n";

            Regs.nowVar = ansReg;
            Regs.Reg[ansReg].type = "i32";

        } else {
            ret += op + mulDiv();
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN;
            Regs.Reg["$__tmp_REG"].outputFormatSpecifier = "printf(\"\%d\\n\", ";
        }
    }

    return loadRet + ret;
}

// TODO: 整理
string Node::mulDiv() {
    string ret = funCall("");

    int nextWord;
    string op;
    string r1;
    string loadRet;
    int    nextOP       = token[tokNumCounter + 1].tokNum;
    bool   nextOPisTrue = nextOP == MUL || nextOP == DIV;
    string loadPointerReg;

    if (langMode == LLIR && nextOPisTrue) {
        r1  = ret;
        ret = "";

        if (r1[0] == ' ') {
            ret += r1 + "\n";
        } else {
            //Regs.nowVar
            loadPointerReg = "%" + std::to_string(registerAmount++);
            string loadVariableReg = "%" + std::to_string(registerAmount++);
            string pointerType = Regs.Reg[Regs.llirReg[r1]].type;
            string variablesType = Regs.Reg[Regs.llirReg[r1]].type;

            variablesType.pop_back();

            ret += addIndent() + loadPointerReg + " = " + load(r1, pointerType, typeSize[pointerType]);
            ret += addIndent() + loadVariableReg + " = " + load(loadPointerReg, variablesType, "4");
            Regs.nowVar = loadVariableReg;
            Regs.Reg["$__tmp_r"].type = variableType(langMode, "int");//"i32";
        }
    }

    while (nextOPisTrue) {
        op = token[tokNumCounter + 1].tokChar;
        nextOP = token[tokNumCounter + 1].tokNum;
        tokNumCounter += 2;

        string s2;

        s2 = token[tokNumCounter].tokChar;

        if (!isDigit(ret) && !isDigit(s2)) {
            int Lnum = std::stoi(ret);
            int Rnum = std::stoi(s2);

            switch (nextWord){
                case MUL:
                    return std::to_string(Lnum * Rnum);
                case DIV:
                    return std::to_string(Lnum / Rnum);
                default:
                    break;
            }
        }
        else if (langMode == LLIR) {

            string newS2;
            string newNowVar;
            string ansReg;
            string loadReg;

            if (ret[0] != ' ')
                Regs.nowVar = r1;

            newNowVar = Regs.nowVar;
            s2        = funCall("");

            if (s2[0] == '%') {
                string pointerReg2  = "%" + std::to_string(registerAmount++);
                string variableReg2 = "%" + std::to_string(registerAmount++);

                ret += addIndent() + pointerReg2 + " = " + load(s2, "i32*", "8");
                ret += addIndent() + variableReg2 + " = " + load(pointerReg2, "i32", "4");
                newS2 = variableReg2;
            } else if (!isDigit(s2))
                newS2 = s2;
            else {
                newS2 = "%" + std::to_string(registerAmount - 1);
                ret += s2 + "\n";
            }

            ansReg               = "%" + std::to_string(registerAmount++);
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == MUL || token[tokNumCounter + 1].tokNum == DIV;
            oneBeforeInstruction = opToIR[nextOP];
            ret += addIndent() + ansReg + " = " + oneBeforeInstruction + " nsw i32 " + newNowVar + ", " + newS2 +"\n\n";

            Regs.nowVar = ansReg;
            Regs.Reg[ansReg].type = "i32";

        } else {
            ret += op + funCall("");
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN;
            Regs.Reg["$__tmp_REG"].outputFormatSpecifier = "printf(\"\%d\\n\", ";
        }
    }

    return loadRet + ret;
}

string Node::funCall(string instanceName) {
    string ret;
    
    if (token[tokNumCounter + 1].tokNum == LBRACKET) {
        string funcName = token[tokNumCounter++].tokChar;
        if (funcName == "_add" && langMode == CPP)
            funcName = "__CRU_Add";

        Regs.Reg[funcName] = { funcName, funcName,"i32", "4", "\%d" };
        Regs.llirReg[funcName] = funcName;

        tokNumCounter++;

        Regs.nowVar = funcName;

        string argment = funcCallArtgment();
        if (instanceName != "" && langMode == CPP) {
            (argment == "") ? argment = "&" + instanceName : argment = "&" + instanceName + ", " + argment;
        } else {}

        expect(")");

        if (langMode == LLIR) {
            string r1 = std::to_string(registerAmount++);

            oneBeforeInstruction = "call";

            ret = argmentLoadSentS;
            ret += addIndent() + "%" + r1 + " = " + oneBeforeInstruction + " i32 @" + funcName + "(" + argment + ")\n";

            r1 = "%" + r1;

            Regs.nowVar = r1;
            Regs.Reg[r1].type = "i32";

        }else {
            ret = funcName + "(" + argment + ")";
            Regs.nowVar = Regs.Reg[funcName].type;
        }
        return ret;
    } else if (token[tokNumCounter + 1].tokNum == PERIOD && token[tokNumCounter + 3].tokNum == LBRACKET) {
        classEnabled = True;
        string ret = token[tokNumCounter++].tokChar;
        expect(".");
        tokNumCounter++;
        if (langMode == CPP) {
            ret = classAndInstance[ret] + funCall(ret);
        }
        else if (langMode == PYTHON) {
            ret += "." + funCall("");
        }
        return ret;
    }
    return expr();
}

string Node::expr() {
    if (token[tokNumCounter].tokNum == LBRACKET) {
        tokNumCounter++;
        string ret = addSub();
        expect(")");
        return ret;
    }
    return word();
}

string Node::word() {
    string ret;
    if (token[tokNumCounter].tokChar == "\"") {
        ret = "\"" + token[tokNumCounter+1].tokChar + "\"";

        tokNumCounter +=2;
        return ret;
    } else if (token[tokNumCounter].tokNum == SELF) {
        expect("self");
        tokNumCounter++;
        expect(".");
        tokNumCounter++;
        ret = "self->" + token[tokNumCounter].tokChar;
        return ret;

    }

    if (token[tokNumCounter+1].tokNum == LSQBRACKET) {
        string index = token[tokNumCounter+2].tokChar; 

        if (isDigit(index)) {
        }
        if (langMode == LLIR) {
            string getelementptrReg = "%" + std::to_string(registerAmount++);
            string loadReg = "%" + std::to_string(registerAmount++);
            string type = Regs.Reg[token[tokNumCounter].tokChar].type;

            if (type[type.size()-1] == '*') {

                string loadReg2;// = "%" + std::to_string(registerAmount++);
                /*
  %3 = load i8**, i8*** %2, align 8
  %4 = getelementptr inbounds i8*, i8** %3, i64 0
                 */
                loadReg2 = loadReg;
                loadReg = getelementptrReg;
                getelementptrReg = loadReg2;

                ret = addIndent() + loadReg + " = " + load(Regs.llirReg[token[tokNumCounter].tokChar], type, "8");
                type = variableType(langMode, "vec&string");
                ret += addIndent() + getelementptrReg + " = getelementptr inbounds " + type + ", " + type + "* " + loadReg + ", i64 " + index + "\n";
                loadReg = "%" + std::to_string(registerAmount++);
            }
            else {
                ret = addIndent() + getelementptrReg + " = getelementptr inbounds [3 x i8*], [3 x i8*]*" + Regs.llirReg[token[tokNumCounter].tokChar] + ", i64 0, i64 " + index + "\n";
                type = "i8*";
            }
            ret += addIndent() + loadReg + " = " + load(getelementptrReg, type, "8");

            Regs.Reg[loadReg].type = "vec";
            Regs.nowVar = loadReg;
        }
        else if (langMode == CPP) {
            ret = token[tokNumCounter].tokChar + "[" + index + "]";
            Regs.Reg[ret].outputFormatSpecifier = "printf(\"\%s\\n\", ";
            Regs.nowVar = ret;
        }
        else {
            ret = token[tokNumCounter].tokChar + "[" + index + "]";
            Regs.nowVar = ret;
        }
        tokNumCounter+=3;

        expect("]");
    }

    else if (langMode == LLIR && Regs.Reg[token[tokNumCounter].tokChar].regName != "") {
        ret = Regs.Reg[token[tokNumCounter].tokChar].regName;
    }
    else {
        ret = token[tokNumCounter].tokChar;
    }

    return ret;
}

string Node::eval() {
    string ret;
    string r1;
    string len;
    string type;
    /*
     * regL
     * regR
     * regN = if regL, regR
     */

    if (langMode == LLIR) {
        string opreg;
        string regL  = funCall("");
        tokNumCounter++;
        int    op    = token[tokNumCounter].tokNum;
        string opStr = token[tokNumCounter++].tokChar;
        string regR  = funCall("");

        //TODO

        if (regL[0] == '%') {
            // regL is register.

            string loadedRegL = "%" + std::to_string(registerAmount++);
            string loadedRegL2 = "%" + std::to_string(registerAmount++);
            type              = Regs.Reg[Regs.llirReg[regL]].type;
            len               = Regs.Reg[Regs.llirReg[regL]].len;
            ret += addIndent() + loadedRegL + " = " + load(regL, type, len);
            ret += addIndent() + loadedRegL2 + " = " + load(loadedRegL, "i32", "4");
            Regs.Reg[regL].name = loadedRegL2;
            regL                = loadedRegL2;// "%" + std::to_string(registerAmount++);

        } else;
            // regL is not register.

        if (regR[0] == '%') {
            // regR is register.

            string loadedRegR = "%" + std::to_string(registerAmount);
            type              = Regs.Reg[Regs.llirReg[regR]].type;
            len               = Regs.Reg[Regs.llirReg[regR]].len;
            ret += addIndent() + loadedRegR + " = " + load(regL, type, len);
            Regs.Reg[regR].name = loadedRegR;
            regR                = "%" + std::to_string(registerAmount++);

        } else;
            // regR is not register.

        r1                   = "%" + std::to_string(registerAmount);
        opreg                = opToIR[op];
        oneBeforeInstruction = "icmp";

        ret += addIndent() + r1 + " = " + oneBeforeInstruction + " " + opreg + " i32 " + regL + ", " + regR + "\n";
    }
    else {
        ret = comparison(tokNumCounter, token[tokNumCounter].tokChar);
        tokNumCounter--;

    }

    return ret;

}

string Node::comparison(int i, string ret) {
    if (token[i + 1].tokNum == LRIPPLE) {
        tokNumCounter = i+1;

        return ret;
    }
    return ret + comparison(i + 1, token[i + 1].tokChar);
}

string Node::loop() {
    string doValue = "";
    string iterate_1;
    string iterate_2;
    string ret;

    doValue = word();
    tokNumCounter++;
    expect("in");
    tokNumCounter++;
    iterate_1 = addSub();
    tokNumCounter++;
    expect("~");
    tokNumCounter++;
    iterate_2 = addSub();
    tokNumCounter++;

    switch (langMode) {
        case PYTHON:
            ret = doValue + " in range(" + iterate_2 + " - " + iterate_1 + ") ";
            break;
        case CPP:
            ret = "int " + doValue + " = " + iterate_1 + "; " + doValue + " < " + iterate_2 + "; " + doValue + "++";
            break;
        case LLIR: {
            vector<tokens> tmpToken = token;
            int tmpCounter = tokNumCounter;

            tokNumCounter = 0;
            token = {{LET, "let"}, {WORD, doValue}, {CORON, ":"}, {WORD, "int"}, {EQ, "<-"}, {WORD, iterate_1}, {SEMICORON, ";"}};

            ret += let();
            ret += addIndent() + "br label %" + std::to_string(registerAmount) + "\n\n";
            ret += std::to_string(registerAmount++) + ":\n";
            ret += addIndent() + "%" + std::to_string(registerAmount) + " = load i32, i32*%" + std::to_string(registerAmount-2) + ", align 4\n";
            registerAmount++;
            ret += addIndent() + "%" + std::to_string(registerAmount) + " = icmp sle i32 %" + std::to_string(registerAmount-1) + ", " + iterate_2 + "\n";

            tokNumCounter = tmpCounter; 
            token = tmpToken;
            break;
        }
    }

    return ret;
}

