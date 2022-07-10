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

        (langMode == PYTHON) ? ret = "def " + Name + " (" + argment + ") :\n" +
                                     Data + functionDefinition() + "\n"
                             : ret = Type + " " + Name + " (" + argment + ") {\n" +
                                     Data + "\n}" + functionDefinition();

        return ret;
    }
    else if (token[tokNumCounter].tokNum == CLASS) {
        expect("class");
        tokNumCounter++;

        Name = token[tokNumCounter++].tokChar;

        expect("{");

        tokNumCounter++;

        indent++;

        Data = functionDefinition();

        indent--;

        expect("}");

        tokNumCounter++;

        valMemory.push_back({0, False, Type, Name});

        (langMode == PYTHON) ? ret = "class " + Name + argment + ":\n\n" + Data + functionDefinition()
                             : ret = Type + " " + Name + " (" + argment + ") {\n" + Data + "\n}" + functionDefinition();

        return ret;
    }
    else if (token[tokNumCounter].tokNum == PUB) {

        expect("pub");
        tokNumCounter++; // fn

        expect("fn");
        tokNumCounter++; // fn

        Name = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET)
                argment = ", " + funcDefArtgment();
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

        (langMode == PYTHON) ? ret = addIndent() + "def " + Name + " (self" + argment + "):\n" +
                                     Data+ "\n" + functionDefinition() + "\n"
                             : ret = Type + " " + Name + " (" + argment + ") {\n" +
                                     Data + "\n}" + functionDefinition();
        return ret;

    }
    return "";
}

