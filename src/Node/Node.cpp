#include "Node.hpp"


// loop to the end of file.
string Node::parse(vector<tokens> geToken) {
    tokNumCounter = 0;
    string write;
    token = geToken;
	write = functionDefinition();

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
	for (int i=0;i<indent;i++)
		ret += "\t"; 
	return ret;
}


string Node::addSub() {
	string ret = mulDiv();

	if (token[tokNumCounter+1].tokNum == PLUS) {
		//string s1 = mulDiv();
		tokNumCounter += 2;
		string s2 = mulDiv();
		//tokNumCounter++;
        if (!isDigit(ret) && !isDigit(s2)) {
            return std::to_string(std::stoi(ret) + std::stoi(s2));
        }
		return ret + "+" + s2;
	}
	if (token[tokNumCounter+1].tokNum == MIN) {
		string s1 = mulDiv();
		tokNumCounter += 2;
		string s2 = mulDiv();
		//tokNumCounter++;
        if (!isDigit(s1) && !isDigit(s2)) {
            return std::to_string(std::stoi(s1) - std::stoi(s2));
        }
		return s1 + "-" + s2;
	}

	return ret;
}

string Node::mulDiv() {
	if (token[tokNumCounter+1].tokNum == MUL) {
		string s1 = funCall();
		tokNumCounter += 2;
		string s2 = funCall();
		//tokNumCounter++;
		return s1 + "*" + s2;
	}
	if (token[tokNumCounter+1].tokNum == DIV) {
		string s1 = funCall();
		tokNumCounter += 2;
		string s2 = funCall();
		//tokNumCounter++;
		return s1 + "/" + s2;
	}
	return funCall();
}

string Node::funCall() {
	if (token[tokNumCounter+1].tokNum == LBRACKET) {
		string funcName = token[tokNumCounter++].tokChar;
		tokNumCounter++;
		string argment = funcCallArtgment();
		expect(")");
        string ret = funcName + "(" + argment + ")";
		return ret;
	}
	return expr();
}

string Node::expr() {
	if (token[tokNumCounter].tokNum == LBRACKET) {
		tokNumCounter++;
		string ret = addSub();
		expect(")");
//		tokNumCounter++;
        return ret;
	}
	return word();
}


string Node::word() {
    return token[tokNumCounter].tokChar;
}

string Node::eval() {
    return comparison(tokNumCounter, token[tokNumCounter].tokChar);
}

string Node::comparison(int i, string ret) {
    if (token[i+1].tokNum == LRIPPLE) {
		tokNumCounter = i+1;
		return ret;
	}
	return ret+comparison(i+1, token[i+1].tokChar);
}


string Node::loop() {
	string doValue = "";
	string iterate_1;
	string iterate_2;

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

	return "int " + doValue + " = 0; " + doValue + " < " + iterate_2 + "; "+ doValue + "++";
}

