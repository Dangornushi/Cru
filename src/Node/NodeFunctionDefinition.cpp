#include "Node.hpp"

/* ===--- class, fn, pub fn ---===*/

string Node::functionDefinition() {
    string argment = "";
    string Name;
    string Type;
    string Data;
    string ret;

    if (token[tokNumCounter].tokNum == FN) {

        expect("fn");
        tokNumCounter++; // fn

        Name = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET)
                argment = funcDefArtgment();
            tokNumCounter++; // )
        }

        expect(":");

        tokNumCounter++;

        Type = word();

        string Fsent;

        Fsent = "fn " + Name + "(" + argment + ") : ";

        if (Type == "auto") {
            cout << Fsent + Type << endl;
            ;
            for (int i = 0; i < Fsent.length(); i++)
                cout << " " << std::flush;
            cout << "^ Can't use 'auto' for function return value." << endl;
        }

        tokNumCounter++;

        expect("{");

        tokNumCounter++;

        indent++;

        Data = sent();

        indent--;

        expect("}");

        tokNumCounter++;

        valMemory.push_back({0, False, Type, Name});

        (langMode == PYTHON)
            ? ret = "def " + Name + " (" + argment + ") :\n" + Data + functionDefinition() + "\n"
            : ret = Type + " " + Name + " (" + argment + ") {\n" + Data + "\n}" +
                    functionDefinition();

        return ret;
    } else if (token[tokNumCounter].tokNum == CLASS) {

        expect("class");
        tokNumCounter++;

        Name = token[tokNumCounter++].tokChar;

        expect("{");

        string selfData = "";

        tokNumCounter++;

        classEnabled = True;
        nowClassName = Name;

        (langMode == PYTHON) ? indent++ : 1;

        if (token[tokNumCounter].tokNum == SELF) {

            expect("self");

            tokNumCounter++;
            expect("{");
            tokNumCounter++;

            isInit   = True;
            selfData = sent();

            if (langMode == PYTHON) {
                Data = addIndent() + "def __init__(self):\n\t" + selfData + "\n\n";
            } else if (langMode == CPP)
                ;

            expect("}");

            tokNumCounter++;
        }

        classEnabled = False;

        Data += functionDefinition();

        (langMode == PYTHON) ? indent-- : 1;

        indent--;
        expect("}");

        tokNumCounter++;

        valMemory.push_back({0, False, Type, Name});

        (langMode == PYTHON)
            ? ret = "class " + Name + argment + ":\n\n" + Data
            : ret = "typedef struct {\n" + selfData + "\n} " + Name + ";\n" + Data + "\n\n";

        return ret + functionDefinition();
    } else if (token[tokNumCounter].tokNum == PUB) {

        expect("pub");
        tokNumCounter++; // fn

        expect("fn");
        tokNumCounter++; // fn

        Name = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET) {
                argment = ", " + funcDefArtgment();
            }
            tokNumCounter++; // )
        }

        expect(":");

        tokNumCounter++;

        Type = word();

        string Fsent;

        Fsent = "pub fn " + Name + "(" + argment + ") : ";

        if (Type == "auto") {
            cout << Fsent + Type << endl;
            ;
            for (int i = 0; i < Fsent.length(); i++)
                cout << " " << std::flush;
            cout << "^ Can't use 'auto' for function return value." << endl;
        }

        tokNumCounter++;

        expect("{");

        tokNumCounter++;

        indent++;

        Data = sent();

        indent--;

        expect("}");

        tokNumCounter++;

        valMemory.push_back({0, False, Type, Name});

        (langMode == PYTHON)
            ? ret = addIndent() + "def " + Name + " (self" + argment + "):\n" + Data + "\n" +
                    functionDefinition() + "\n"
            : ret = Type + " " + nowClassName + Name + " (" + nowClassName + " *self" + argment +
                    ") {\n" + Data + "\n}" + functionDefinition();
        if (classEnabled == True) {
            argment += argment + "";
        }
        return ret;
    }
    return "";
}

