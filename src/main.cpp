#include "Main.hpp"
#include "CRU.hpp"

#include <dirent.h>
#include <fstream>
#include <sys/stat.h>

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

    for (auto tmp : s1) {
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
    string lang;

    std::ofstream writing_file;
    (langMode == PYTHON) ? lang = ".py" : lang = ".c";
    writing_file.open("crucache/" + splitStr(fileName) + lang, std::ios::out);
    writing_file << runCode;
    writing_file.close();
}

void Main::run() {
    string compiler;
    string option;
    string compilefile;
    string outputfile;
    string runCmd;

    if (langMode == CPP) {
        compiler = "CC";
        option = "-o";
        outputfile = /*"crucache/" + */ splitStr(fileName);
        compilefile = "crucache/" + splitStr(fileName) + ".c";

        runCmd = compiler + " " + option + " " + outputfile + " " + compilefile;
    }
    if (langMode == PYTHON) {
        compiler = "python3";
        option = "";
        outputfile = "";
        compilefile = /*"crucache/" +*/ splitStr(fileName) + ".py";

        runCmd = compiler + " " + compilefile;
    }

    char *cstr = new char[runCmd.size() + 1]; // メモリ確保

    std::char_traits<char>::copy(cstr, runCmd.c_str(), runCmd.size() + 1);
    mkdir("crucache", 0777);
    std::system(cstr);
    delete[] cstr;
}

void Main::cru() {

    open();

    if (isEV3 == True) {
        runCode = "from pybricks import ev3brick as brick"
                  "\nfrom pybricks.ev3devices import (Motor, TouchSensor, ColorSensor, "
                  "InfraredSensor, UltrasonicSensor, GyroSensor)"
                  "\nfrom pybricks.parameters import (Port, Stop, Direction, Button, Color, "
                  "\n                                     SoundFile, ImageFile, Align)"
                  "\nfrom pybricks.tools import print, wait, StopWatch"
                  "\nfrom pybricks.robotics import DriveBase"
                  "\n\n";
    }

    if (langMode == CPP) {
        char tmp[1];
        runCode = "#include <stdio.h>\n";
        runCode += "#include <string.h>\n\n";
        runCode += "int __CRU_Charput(char __s1) {\n\t"
                   "printf(\"%c\", __s1);\n\t"
                   "return 0;\n"
                   "}\n"
                   "int __CRU_Strput(const char *__s1, int __size) {\n\t"
                   "for(size_t __i=0;__i<__size;__i++)\n\t\t"
                   "__CRU_Charput(__s1[__i]);\n\t"
                   "return 0;\n"
                   "}\n"
                   "int __CRU_Stringput(char *__s1) {\n\t"
                   "printf(\"%s\", __s1);\n\t"
                   "return 0;\n"
                   "}\n"
                   "char *__CRU_Add(char *s1, const char *s2) {\n\t"
                   "char buf[512];\n\t"
                   "sprintf(buf, \"%s%s\", s1, s2);\n\t"
                   "strcpy(s1, buf);\n\t"
                   "return s1;\n"
                   "}\n"
                   "\n";
    }
    if (langMode == PYTHON) {
        runCode += "def _add(s1, s2):\n\t"
                   "return s1 + s2\n\n";
    }

    Lexer lexer;
    Node node(langMode);

    token = lexer.lex(fileData);
    runCode += node.parse(token);

    (langMode == PYTHON) ? runCode += "\nmain()"
                         : runCode += "\nint start(void) {\n\treturn main();\n}";

    write();
    if (isRun == True)
        run();
}

int isDigit(string s1) {
    for (int i = 0; i < s1.length(); i++) {
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
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'p') {
            isEV3 = False;
            langMode = PYTHON;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'm') {
            langMode = PYTHON;
            isEV3 = True;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'r') {
            isRun = True;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == '-') {
            if (!strcmp(arg[i], "--logo")) {
                cout << " ▄▀▄▄▄▄   ▄▀▀▄▀▀▀▄  ▄▀▀▄ ▄▀▀▄\n"
                        "█ █    ▌ █   █   █ █   █    █\n"
                        "▐ █      ▐  █▀▀█▀  ▐  █    █ \n"
                        "  █       ▄▀    █    █    █  \n"
                        " ▄▀▄▄▄▄▀ █     █      ▀▄▄▄▄▀ \n"
                        "█     ▐  ▐     ▐             \n"
                        "▐                            \n"
                     << endl;
                exit(0);
            }
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
