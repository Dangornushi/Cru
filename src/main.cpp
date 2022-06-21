#include "Main.hpp"
#include "CRU.hpp"

#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

/* 
 * Copyright (c) 2022 Dangomsuhi
 * This software is released under the MIT License, see LICENSE.
*/

Main::Main(int n, char *arg[]) {
    langMode = CPP;
    version = "cru ver.0.0.1";
    cmdArg(1, arg, n);
	debugMode = 1;
}

string splitStr(string s1) {
    string ret;

    for (auto tmp:s1) {
        if (tmp == '.') 
            return ret;
        ret += tmp;
    }
    return "";
}

void Main::open() {
	string filedata;

	std::ifstream reading_file;
	reading_file.open(fileName, std::ios::in);

    while (std::getline(reading_file, filedata))
		fileData += filedata;
    return;
}

void Main::write() {    
    string fileBuf;

    std::ofstream writing_file;
    writing_file.open(splitStr(fileName)+".c", std::ios::out);
    writing_file << runCode;
    writing_file.close();
}

void Main::run() {
    string Compiler = "CC";
    string runCmd = Compiler + " -o crucache/" + splitStr(fileName) + " " + splitStr(fileName)+".c";
    char *cstr = new char[runCmd.size() + 1]; // メモリ確保

    std::char_traits<char>::copy(cstr, runCmd.c_str(), runCmd.size() + 1);

    if(mkdir("crucache", 0777)==0){
        std::system(cstr);
    }else
        ;
    delete[] cstr;
}

void Main::cru() {

    open();

    if (langMode == CPP) {
        runCode = "#include <stdio.h>\n\n";
        runCode +=
            "int __CRU_Strput(char *__s1, int __size) {\n\t"
                "for(size_t __i=0;__i<__size;__i++)\n\t\t"
                    "printf(\"%c\", __s1[__i]);\n\t"
                "return 0;\n"
            "}\n";
    }

    Lexer lexer;
    Node node(langMode);

    token = lexer.lex(fileData);
    runCode += node.parse(token);

    (langMode == PYTHON) ? runCode += "\nmain()"
                         : runCode += "\nint start(void) {\n\treturn main();\n}";

    write();
    run();
}

int isDigit(string s1) {
    for (int i=0;i<s1.length();i++) {
        if (!isdigit(s1[i]))
            return True;
    }
    return False;
}

void Main::cmdArg(int i, char *arg[], int Big) {
    if (Big > i) {
        if (arg[i][0] != '-') {
            fileName = string(arg[i]);
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'v') {
            cout << version << endl;
            exit(0);
        } else if (arg[i][1] == 'a') {
            cout << version << endl;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'p') {
            langMode = PYTHON;
            i++;
            return cmdArg(i, arg, Big);
        } else {
            cout << "Unknown option '" << arg[i] << "'." << endl;
            exit(0);
        }
    }
    return;
}

int main(int argc, char *argcv[]) {
    Main start(argc, argcv);

	start.cru();

    return 0;
}
