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
    this->langMode = langMode;
    this->indent   = 0;
    this->enumEnabled = False;
    this->classEnabled = False;
    this->registerAmount = 0;
    this->typeSize = {
        {"i8", "1"},
        {"i32", "4"},
        {"i64", "8"},
    };
    this->strAmount = 0;
    this->funcDefQuantity = 0;
    this->putDefExists = False;
    this->opToIR = {
        {EQEQ, "eq"},
        {BIG, "slt"},
        {MINI, "sgt"},
    };
    this->usedReturn = false;
}

// loop to the end of file.
string Node::parse(vector<tokens> geToken) {
    tokNumCounter = 0;
    string write;
    string pathVec = execDir();
    string filedata;
    string filename;
    string ret;

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
    bool nextOP = token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN;

    if (langMode == LLIR && nextOP) {
        r1        = ret;
        ret = "";
        if (r1[0] == ' ') {
            ret += r1 + "\n";
            Regs.nowVar = "%" + std::to_string(registerAmount-1);
        } else {
            Regs.nowVar = "%" + std::to_string(registerAmount++);
            ret += addIndent() + Regs.nowVar + " = " + load(r1, "i32", "4");
        }
    }

    while (token[tokNumCounter + 1].tokNum == PLUS || token[tokNumCounter + 1].tokNum == MIN) {
        op = token[tokNumCounter + 1].tokChar;
        tokNumCounter += 2;
        string s2;
        if (!isDigit(ret) && !isDigit(token[tokNumCounter].tokChar)) {
            switch (nextWord){
                case PLUS:
                    return std::to_string(std::stoi(ret) + std::stoi(s2));
                case MIN:
                    return std::to_string(std::stoi(ret) - std::stoi(s2));
                default:
                    break;
            }
        }
        else;

        if (langMode == LLIR) {

            string tmp;
            string s2;
            string newS2;
            string newNowVar;
            string ansReg;

            if (ret[0] != ' ') {
                Regs.nowVar = r1;
            }

            s2 = mulDiv();

            if (s2[0] == '%') {
                newS2 = "%" + std::to_string(registerAmount++);
                ret += addIndent() + newS2 + " = " + load(s2, "i32", "4");

            }  else if (!isDigit(s2)) {
                newS2 = s2;

            } else {
                newS2 = "%" + std::to_string(registerAmount-1);
                ret += s2 + "\n";

            }

            newNowVar = Regs.nowVar;

            ansReg = "%" + std::to_string(registerAmount++);

            ret += addIndent();
            ret += ansReg + " = ";

            if (op == "+")
                oneBeforeInstruction = "add";
            if (op == "-")
                oneBeforeInstruction = "sub";

            ret += oneBeforeInstruction + " nsw i32 " + newNowVar + ", " + newS2 +"\n";

            Regs.nowVar = ansReg;
            llirType[Regs.nowVar] = "i32";

        } else {

            s2 = addSub();

            ret += op + s2;
        }
    }

    return loadRet + ret;
}

// TODO: 整理
string Node::mulDiv() {
    string ret = funCall("");
    string type;

    type = regType[ret];

    if (token[tokNumCounter + 1].tokNum == MUL) {
        tokNumCounter += 2;
        string s2 = funCall("");

        if (langMode == LLIR) {
            // load(string ret, stirng type, string typeSize);
            ret = addIndent() + "%" + std::to_string(registerAmount) + " = " + load(ret, type, typeSize[type]);   
            registerAmount++;
            ret += addIndent() + "%" + std::to_string(registerAmount) +" = mul nsw i32 " + "%" + std::to_string(registerAmount-1) + ", " + s2 + "\n";
        }
        else {
            ret = ret + "*" + s2;
        }
        return ret;
    }

    type = regType[ret];

    if (token[tokNumCounter + 1].tokNum == DIV) {
        tokNumCounter += 2;
        string s2 = funCall("");
        if (langMode == LLIR) {
            ret = addIndent() + "%" + std::to_string(registerAmount) + " = " + load(ret, type, typeSize[type]);   
            registerAmount+=2;
            ret += addIndent() + "%" + std::to_string(registerAmount) +" = sdiv nsw i32 " + "%" + std::to_string(registerAmount-1) + ", " + s2 + "\n";
        }
        else {
            ret = ret + "/" + s2;
        }
        return ret;
    }
    return ret;
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

        if (instanceName != "") {
            (argment == "") ? argment = "&" + instanceName : argment = "&" + instanceName + ", " + argment;
        } else {}

        expect(")");

        if (langMode == LLIR) {
            string r1 = std::to_string(registerAmount++);

            ret  = loads;

            loads = "";

            oneBeforeInstruction = "call";

            ret += addIndent() + "%" + r1 + " = " + oneBeforeInstruction + " i32 @" + funcName + "(" + argment + ")\n";

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
        //tokNumCounter++;
        return ret;

    }
    if (langMode == LLIR && Regs.Reg.find(token[tokNumCounter].tokChar) != Regs.Reg.end()) {
            ret = Regs.Reg[token[tokNumCounter].tokChar].regName;
    }

    else
        ret = token[tokNumCounter].tokChar;

    return ret;
}

string Node::eval() {
    string ret;
    string r1;
    string type;
    /*
     * regL
     * regR
     * regN = if regL, regR
     */

    if (langMode == LLIR) {
        string opreg;
        string regL = funCall("");
        tokNumCounter++;
        int op = token[tokNumCounter].tokNum;
        tokNumCounter++;
        string regR = funCall("");
        tokNumCounter++;

        if (regL[0] == '%') {
            // regL is register.
            type = Regs.Reg[Regs.llirReg[regL]].len;

            ret += addIndent() + "%" + std::to_string(registerAmount) + " = load " + Regs.Reg[Regs.llirReg[regL]].type + ", " +
                    Regs.Reg[Regs.llirReg[regL]].type + "* " + regL + ", align " + type + "\n";

            Regs.Reg[regL].name = "%" + std::to_string(registerAmount);
            regL = "%" + std::to_string(registerAmount++);
        } else
            // regL is not register.
            ;

        if (regR[0] == '%') {
            // regR is register.
            type = typeSize[regType[regR]];

            ret += addIndent() + "%" + std::to_string(registerAmount) + " = load " + regType[regR] + ", " + regType[regR] + "* " + regR + ", align " + type + "\n";

            regR = "%" + std::to_string(registerAmount++);
        } else
            // regR is not register.
            ;

        r1 = "%" + std::to_string(registerAmount++);

        opreg = opToIR[op];

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

    if (langMode == CPP)
        ret = "int " + doValue + " = " + iterate_1 + "; " + doValue + " < " + iterate_2 + "; " +
              doValue + "++";
    if (langMode == PYTHON)
        ret = doValue + " in range(" + iterate_2 + " - " + iterate_1 + ") ";
    return ret;
}

