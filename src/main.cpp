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
    version = "cru ver.0.0.1";
	debugMode = 1;
    if (n > 1) {
        if (arg[1][0] != '-') {
            langMode = CPP;
            fileName = string(arg[1]);
        }
        else if (arg[1][1] == 'v') {
            cout << version << endl;
            exit(0);
        } else {
            cout << "Unknown option '" << arg[1] << "'." << endl;
            exit(0);
        }
    } else {
        cout << "Please add '-$command'." << endl;;
        exit(0);
    }
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

    runCode = "#include <stdio.h>\n\n";
    runCode += "int __CRU_Strput(char *__s1, int __size) {\n\tfor(size_t __i=0;__i<__size;__i++)\n\t\tprintf(\"%c\", __s1[__i]);\n\treturn 0;\n}\n";

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

int main(int argc, char *argcv[]) {
    Main start(argc, argcv);

	start.cru();

    return 0;
}
