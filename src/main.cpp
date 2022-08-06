#include "Main.hpp"
#include "CRU.hpp"

#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <libproc.h>
#include <unistd.h>

/*
 * Copyright (c) 2022 Dangomsuhi
 * This software is released under the MIT License, see LICENSE.
 */

Main::Main(int n, char *arg[]) {
    langMode = LLIR;
    version  = "cru ver.0.0.1";
    cmdArg(1, arg, n);
    debugMode = 1;
}

string execDir() {
    vector<string> ret;
    string dir;
    int r;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    pid = getpid();
    r   = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
    if (r <= 0) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else;
    ret = pathSplit(string {pathbuf});
    for (auto tmp : ret)
        dir += tmp + "/";
    return dir;
}

vector<string> pathSplit(string p) {
    vector<string> ret;
    string data;

    for (auto tmp : p) {
        if (tmp == '/') {
            ret.insert(ret.end(), data);
            data = "";
            continue;
        }
        data += tmp;
    }
    return ret;
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
    if (langMode == PYTHON)
        lang = ".py";
    else if (langMode == CPP)
        lang = ".c";
    else if (langMode == RUST)
        lang = ".rs";
    else if (langMode == LLIR)
        lang = ".ll";

    writing_file.open(splitStr(fileName) + lang, std::ios::out);
    writing_file << runCode;
    writing_file.close();
}

void Main::run() {
    string compiler    = "";
    string option      = "";
    string compilefile = "";
    string outputfile  = "";
    string runCmd      = "";

    switch (langMode) {
        case CPP: {
            compiler    = "CC";
            option      = "-o";
            outputfile  = splitStr(fileName);
            compilefile = "crucache/" + splitStr(fileName) + ".c";

            runCmd      = compiler + " " + option + " " + outputfile + " " + compilefile;
            break;
        }

        case PYTHON: {
            compiler    = "python3";
            compilefile = splitStr(fileName) + ".py";

            runCmd      = compiler + " " + compilefile;
            break;
        }

        case RUST: {
            compilefile = splitStr(fileName) + ".rs";

            runCmd      = "cargo bootimage && qemu-system-x86_64 -drive format=raw,file=../target/x86_64-CRU_os/debug/bootimage-CRU_os.bin";
            break;
        }

        case LLIR: {
            outputfile  = splitStr(fileName);
            compilefile = splitStr(fileName) + ".ll";

            runCmd      = "llc " + compilefile + "; clang " + outputfile + ".s -o " + outputfile;
            break;
        }

        default:
            break;
    }

    char *cstr = new char[runCmd.size() + 1]; // メモリ確保

    std::char_traits<char>::copy(cstr, runCmd.c_str(), runCmd.size() + 1);
    mkdir("crucache", 0777);
    std::system(cstr);
    delete[] cstr;
}

void Main::cru() {

    open();

    Lexer lexer;
    Node node(langMode);

    token = lexer.lex(fileData);
    runCode += node.parse(token);

    runCode = node.strDefine + "\n" + runCode + "\n" + node.functionDefine;

    if (langMode == CPP) {
        runCode += "\nint start(void) {\n\treturn main();\n}";
    }
    else if (langMode == PYTHON) {
        runCode += "\nmain()";
    }

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
        } else if (arg[i][1] == 'P') {
            isEV3    = False;
            langMode = PYTHON;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'C') {
            langMode = CPP;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'R') {
            langMode = RUST;
            i++;
            return cmdArg(i, arg, Big);
        } else if (arg[i][1] == 'm') {
            langMode = PYTHON;
            isEV3    = True;
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
            } else if (!strcmp(arg[i], "--os")) {
                isOS = True;
                return cmdArg(i, arg, Big);
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
