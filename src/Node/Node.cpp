#include "Node.hpp"
#include "../generator/Generate.hpp"

#include <fstream>
#include <memory>
#include <sys/stat.h>
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
}

// loop to the end of file.
string Node::parse(vector<tokens> geToken) {
    tokNumCounter = 0;
    string write;
    token = geToken;
    if (token[tokNumCounter].tokNum == IMPORT) {

        tokNumCounter++;
        string filedata;
        string filename = parseQuort(word());
        string ret;

        std::ifstream reading_file;
        reading_file.open(filename, std::ios::in);

        while (std::getline(reading_file, filedata))
            write += filedata + "\n";

        tokNumCounter++;

        expect(";");

        tokNumCounter++;
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
    int nextWord = token[tokNumCounter + 1].tokNum;

    if (nextWord == PLUS || nextWord == MIN) {
        tokNumCounter += 2;
        string s2 = addSub();

        if (!isDigit(ret) && !isDigit(s2)) {
            return std::to_string(std::stoi(ret) + std::stoi(s2));
        }
        else;

        if (langMode == LLIR) {

            string tmp = ret;
            string type = regType[tmp];
            string r1 = "%" + std::to_string(registerAmount++);
            string r2 = "%" + std::to_string(registerAmount);
            string r3 = r2;

            if (ret[0] == '%') {
                ret = load(addIndent(), {"", ret, "i32", "4"}, r1);

            }
            if (s2[0] == '%') {
                ret += load(addIndent(), {"", s2, "i32", "4"}, r2);
                r2 = "%" + std::to_string(++registerAmount);
            }

            (nextWord == PLUS)
                ? ret += addIndent() + r2 +" = add nsw i32 " + r1 + ", " + r3 + "\n"
                : ret += addIndent() + r2 +" = sub nsw i32 " + r1 + ", " + r3 + "\n";
        } else
            ret += "+" + s2;
        return ret;
    }

    return ret;
}

string Node::mulDiv() {
    string ret = funCall("");

    if (token[tokNumCounter + 1].tokNum == MUL) {
        tokNumCounter += 2;
        string s2 = funCall("");

        if (langMode == LLIR) {
            string type = regType[ret];
            ret = addIndent() + "%" + std::to_string(registerAmount) + " = load " + type +", " + type + "* " + ret + ", align " + typeSize[type] + "\n";   
            registerAmount++;
            ret += addIndent() + "%" + std::to_string(registerAmount) +" = mul nsw i32 " + "%" + std::to_string(registerAmount-1) + ", " + s2 + "\n";
        }
        else {
            ret = ret + "*" + s2;
        }
        return ret;
    }
    if (token[tokNumCounter + 1].tokNum == DIV) {
        tokNumCounter += 2;
        string s2 = funCall("");
        if (langMode == LLIR) {
            string type = regType[ret];
            ret = addIndent() + "%" + std::to_string(registerAmount) + " = load " + type +", " + type + "* " + ret + ", align " + typeSize[type] + "\n";   
            registerAmount++;
            ret += addIndent() + "%" + std::to_string(registerAmount) +" = sdiv nsw i32 " + "%" + std::to_string(registerAmount-1) + ", " + s2 + "\n";
        }
        else {
            ret = ret + "/" + s2;
        }
        return ret;
    }
    return funCall("");
}

string Node::funCall(string instanceName) {
    string ret;

    if (token[tokNumCounter + 1].tokNum == LBRACKET) {
        string funcName = token[tokNumCounter++].tokChar;
        if (funcName == "_add" && langMode == CPP)
            funcName = "__CRU_Add";

        tokNumCounter++;
        string argment = funcCallArtgment();
        if (instanceName != "") {
            (argment == "") ? argment = "&" + instanceName : argment = "&" + instanceName + ", " + argment;
        } else {}

        expect(")");

        if (langMode == LLIR) {
            string r1 = std::to_string(registerAmount++);

            ret  = loads;

            ret += addIndent() + "%" + r1 + " = call i32 @" + funcName + "(" + argment + ")";

        }else {
            ret = funcName + "(" + argment + ")";
        }
        return ret;
    } else if (token[tokNumCounter + 1].tokNum == PERIOD &&
               token[tokNumCounter + 3].tokNum == LBRACKET) {
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
        int index = tokNumCounter;

        for (; token[index + 1].tokChar != "\""; index++)
            ret += token[index + 1].tokChar;
        ret = "\"" + ret + "\"";
        index++;

        tokNumCounter = index;
    } else if (token[tokNumCounter].tokNum == SELF) {
        expect("self");
        tokNumCounter++;
        expect(".");
        tokNumCounter++;
        ret = "self->" + token[tokNumCounter].tokChar;
        //tokNumCounter++;

    } else {
        if (langMode == LLIR && Regs.Reg.find(token[tokNumCounter].tokChar) != Regs.Reg.end()) {
            ret = Regs.Reg[token[tokNumCounter].tokChar].regName;
        }

        else
            ret = token[tokNumCounter].tokChar;
    }

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
        string op = token[tokNumCounter].tokChar;
        tokNumCounter++;
        string regR = funCall("");
        tokNumCounter++;

        if (regL[0] == '%') {
            // regL is register.
            ;
            type = Regs.Reg[regL].len;

            ret += addIndent() + "%" + std::to_string(registerAmount) + " = load " + Regs.Reg[regL].type + ", " +
                    Regs.Reg[regL].type + "* " + regL + ", align " + type + "\n";

            Regs.Reg[regL].name = "%" + std::to_string(registerAmount++);
            regL = "%" + std::to_string(registerAmount++);
        } else
            // regL is not register.
            ;
        if (regR[0] == '%') {
            // regR is register.
            type = typeSize[regType[regR]];

            ret += addIndent() + "%" + std::to_string(registerAmount) + " = load " + regType[regR] + ", " +
                   regType[regR] + "* " + regR + ", align " + type + "\n";

            regR = "%" + std::to_string(registerAmount++);
        } else
            // regR is not register.
            ;

        r1 = "%" + std::to_string(registerAmount++);

        (op == ">") ? opreg = "sgt" : opreg = "slt";

        ret += addIndent() + r1 + " = icmp " + opreg + " i32 " + regL + ", " + regR + "\n";
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

