#include "Node.hpp"

#include <sys/stat.h>
#include <fstream>

string parseQuort(const string s1) {
    string ret;

    for (int i=1; i< s1.size()-1;i++)
       ret += (s1[i]); 
    return ret;
}

Node::Node(int langMode) {
    this->langMode = langMode;
    this->indent = 0;
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
        ret += "\t";
    return ret;
}

string Node::addSub() {
    string ret = mulDiv();

    if (token[tokNumCounter + 1].tokNum == PLUS) {
        tokNumCounter += 2;
        string s2 = mulDiv();
        if (!isDigit(ret) && !isDigit(s2)) {
            return std::to_string(std::stoi(ret) + std::stoi(s2));
        }
        return ret + "+" + s2;
    }
    if (token[tokNumCounter + 1].tokNum == MIN) {
        string s1 = mulDiv();
        tokNumCounter += 2;
        string s2 = mulDiv();
        if (!isDigit(s1) && !isDigit(s2)) {
            return std::to_string(std::stoi(s1) - std::stoi(s2));
        }
        return s1 + "-" + s2;
    }

    return ret;
}

string Node::mulDiv() {
    if (token[tokNumCounter + 1].tokNum == MUL) {
        string s1 = funCall();
        tokNumCounter += 2;
        string s2 = funCall();
        return s1 + "*" + s2;
    }
    if (token[tokNumCounter + 1].tokNum == DIV) {
        string s1 = funCall();
        tokNumCounter += 2;
        string s2 = funCall();
        return s1 + "/" + s2;
    }
    return funCall();
}

string Node::funCall() {
    string ret;

    if (token[tokNumCounter + 1].tokNum == LBRACKET) {
        string funcName = token[tokNumCounter++].tokChar;
        if (funcName == "_add" && langMode == CPP)
            funcName = "__CRU_Add";

        tokNumCounter++;
        string argment = funcCallArtgment();
        expect(")");
        ret = funcName + "(" + argment + ")";
        return ret;
    }
    else if (token[tokNumCounter + 1].tokNum == PERIOD) {
        string ret = token[tokNumCounter++].tokChar;
        tokNumCounter++;
        ret += "." + funCall();
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
        ret = "self." + token[tokNumCounter].tokChar;

    } else {
        ret = token[tokNumCounter].tokChar;
    }

    return ret;
}

string Node::eval() {
    return comparison(tokNumCounter, token[tokNumCounter].tokChar);
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
        ret = "int " + doValue + " = " + iterate_1 + "; " + doValue + " < " + iterate_2 + "; " + doValue + "++";
    if (langMode == PYTHON)
        ret = doValue + " in range(" + iterate_2 + " - " + iterate_1 + ") ";
    return ret;
}

