#include "Node.hpp"

/* ===--- class, fn, pub fn ---===*/

string Node::functionDefinition() {
    string argment = "";
    string Name;
    string Type;
    string Data;
    string ret;

    if (token[tokNumCounter].tokNum == FN) {

        int nowRegNum = registerAmount;

        expect("fn");
        tokNumCounter++; // fn

        Name = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET)  {
                registerAmount = 0;
                argment = funcDefArtgment();
            }
            expect(")");
            tokNumCounter++; // )
        }

        expect(":");

        tokNumCounter++;

        Type = word();

        string Fsent;

        Fsent = "fn " + Name + "(" + argment + ") : ";

        if (Type == "auto") {
            for (int i = 0; i < Fsent.length(); i++)
                cout << " " << std::flush;
            cout << "^ Can't use 'auto' for function return value." << endl;
        }

        if (Type == "int" && langMode == RUST) {
            Type = "usize";
        }
        else if (Type == "int" && langMode == LLIR) {
            Type = "i32";
        }

        nowType = Type;

        tokNumCounter++;

        expect("{");

        tokNumCounter++;

        indent++;

        string FDQ = std::to_string(funcDefQuantity++);

        Data = sent();

        registerAmount = nowRegNum;

        indent--;

        expect("}");

        tokNumCounter++;

        valMemory.push_back({0, False, Type, Name});

        switch (langMode) {
            case PYTHON:
                ret += "def " + Name + " (" + argment + ") :\n" + Data + functionDefinition() + "\n";
                break;

            case CPP:
                ret += Type + " " + Name + " (" + argment + ") {\n" + Data + "\n}" +
                       functionDefinition();
                break;

            case RUST: {
                ret += ret += "fn " + Name;
                if (Name == "main")
                    ret += " (boot_info: &'static BootInfo) -> ! {" + Data + "\n\tloop{}\n}\n\n";
                else
                    ret += "(" + argment + ") ->" + Type + " {\n" + Data + "\n}\n\n";
                ret += functionDefinition();
            } break;

            case LLIR: {
                indent++;
                ret +=
                    "define " + Type + " @" + Name + "(" + argment + ") #" + FDQ +" {\nentry:\n" + Data + "}\n\n";
                ret += functionDefinition();
                indent--;
            } break;

            default:
                exit(1);
            break;
        }
        return ret;
    } else if (token[tokNumCounter].tokNum == CLASS) {
        string overRide;

        expect("class");
        tokNumCounter++;

        Name = token[tokNumCounter++].tokChar;

        if (token[tokNumCounter].tokChar == "(") {
            tokNumCounter++;
            overRide = " (" + token[tokNumCounter].tokChar + ")";
            tokNumCounter++;
            expect(")");
            tokNumCounter++;
        }
        else ;

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

        if (langMode == PYTHON) {
            ret = "class " + Name + argment + ":\n\n" + Data;
        } else if (langMode == CPP) {
            ret = "typedef struct {\n" + selfData + "\n} " + Name + ";\n" + Data + "\n\n";
        } else if (langMode == RUST) {
            ret = "struct " + Name + overRide;
        }

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
    } else if (token[tokNumCounter].tokNum == ENUM) {
        tokNumCounter++;
        ret = token[tokNumCounter].tokChar;
        tokNumCounter++;
        expect("{");
        tokNumCounter++;
        indent++;
        enumEnabled = True;
        ret = "pub enum " + ret + " {\n" + sent() + "}\n";
        enumEnabled = False;
        indent--;
        expect("}");
        tokNumCounter++;
        ret += functionDefinition();
        return ret;
    } else if (token[tokNumCounter].tokNum == ATSIGN) {
        expect("@");
        tokNumCounter++;
        ret = funCall(); 
        tokNumCounter++;
        expect(";");
        tokNumCounter++;
        ret = "#[" + ret + "]\n" + functionDefinition();
        return ret;
    }
    return "";
}

