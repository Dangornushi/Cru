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

        tokNumCounter++;

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
        cout << "Err: " << token[tokNumCounter].tokChar << "But: " << str << endl;
        exit(1);
    }
}

string getVarName(Register Regs, string var) {
    if (var[0] == '%')
        var = Regs.llirReg[var];
    if (var[0] == '@')
        var = Regs.Reg[Regs.llirReg[var]].name;
    return var;
}

// 所有権があるか判定
bool Node::determinationOfOwnership(string *var) {
    *var = getVarName(Regs, *var);

    if (!Regs.Reg[*var].ownerShip && isDigit(*var) && (*var)[0] != '"'){
        cout << "Err: moved var > " << var << endl;
        exit(1);
    }
    return Regs.Reg[*var].ownerShip;
}

void Node::drop(string var) {
    string varName = getVarName(Regs, var);
    if (varName[0] != '&')
        Regs.Reg[varName].ownerShip = false;
    else 
        Regs.Reg[varName].ownerShip = true;
}

void Node::give(string var) {
    Regs.Reg[getVarName(Regs, var)].ownerShip = true;
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

            ret += addIndent() + loadPointerReg + " = " + load(r1, Regs.Reg[Regs.llirReg[r1]].type, Regs.Reg[Regs.llirReg[r1]].len);
            ret += addIndent() + loadVariableReg + " = " + load(loadPointerReg, "i32", "4");
            Regs.nowVar = loadVariableReg;
            Regs.Reg["$__tmp_r"].type = "i32*";
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

            ansReg               = "%" + std::to_string(registerAmount);
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN;
            oneBeforeInstruction = opToIR[nextOP];
            ret += addIndent() + ansReg + " = " + oneBeforeInstruction + " nsw i32 " + newNowVar + ", " + newS2 +"\n\n";

            Regs.nowVar = ansReg;
            Regs.Reg[ansReg].type = "i32";
            llirType[Regs.nowVar] = "i32";

        } else {
            s2 = mulDiv();
            ret += op + s2;
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

    if (langMode == LLIR && nextOPisTrue) {
        r1  = ret;
        ret = "";

        if (r1[0] == ' ') {
            ret += r1 + "\n";
        } else {
            Regs.nowVar = "%" + std::to_string(registerAmount++);
            ret += addIndent() + Regs.nowVar + " = " + load(r1, "i32", "4");
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

            if (ret[0] != ' ')
                Regs.nowVar = r1;

            newNowVar = Regs.nowVar;
            s2        = funCall("");

            if (s2[0] == '%') {
                newS2 = "%" + std::to_string(registerAmount++);
                ret += addIndent() + newS2 + " = " + load(s2, "i32", "4");
            } else if (!isDigit(s2)) {
                newS2 = s2;
            } else {
                newS2 = "%" + std::to_string(registerAmount - 1);
                ret += s2 + "\n";
            }

            ansReg               = "%" + std::to_string(registerAmount++);
            nextOPisTrue         = token[tokNumCounter + 1].tokNum == MUL || token[tokNumCounter + 1].tokNum == DIV;
            oneBeforeInstruction = opToIR[nextOP];
            ret += addIndent() + ansReg + " = " + oneBeforeInstruction + " nsw i32 " + newNowVar + ", " + newS2 +"\n";

            Regs.nowVar = ansReg;
            llirType[Regs.nowVar] = "i32";

        } else {
            s2 = mulDiv();
            ret += op + s2;
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
            ret += "%" + r1 + " = " + oneBeforeInstruction + " i32 @" + funcName + "(" + argment + ")\n";

            Regs.nowVar = "%" + r1;

        }else {
            ret = funcName + "(" + argment + ")";
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
    if (langMode == LLIR && Regs.Reg[token[tokNumCounter].tokChar].regName != "") {
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

        if (regL[0] == '%') {
            // regL is register.

            string loadedRegL = "%" + std::to_string(registerAmount);
            type              = Regs.Reg[Regs.llirReg[regL]].type;
            len               = Regs.Reg[Regs.llirReg[regL]].len;
            ret += addIndent() + loadedRegL + " = " + load(regL, type, len);
            Regs.Reg[regL].name = loadedRegL;
            regL                = "%" + std::to_string(registerAmount++);

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
    }

    return ret;

}

string Node::comparison(int i, string ret) {
    if (token[i + 1].tokNum == LRIPPLE) {
        tokNumCounter = i + 1;
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

