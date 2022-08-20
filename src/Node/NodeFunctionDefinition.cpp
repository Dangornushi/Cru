#include "Node.hpp"
#include "../generator/Generate.hpp"

/* ===--- class, fn, pub fn ---===*/

string Node::functionDefinition() {
    ReturnArgumentAndMove argment;
    string Name;
    string Type;
    string Data;
    string ret;

    if (token[tokNumCounter].tokNum == FN) {

        int nowRegNum = registerAmount;

        expect("fn");
        tokNumCounter++; // fn

        Name = word();
        nowFuncName = Name;

        tokNumCounter++;

        registerAmount = 0;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET)  {
                argment = funcDefArgument();

                argment.argMove = argMove("    ", argment, &registerAmount, &Regs);
            }
            expect(")");
            tokNumCounter++; // )
        }

        expect(":");

        tokNumCounter++;

        Type = word();

        string Fsent;

        Fsent = "fn " + Name + "(" + argment.returnFunctionArgument + ") : ";

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

        oneBeforeInstruction = "store";

        Data = sent();

        registerAmount = nowRegNum;

        indent--;

        expect("}");

        tokNumCounter++;

        switch (langMode) {
            case PYTHON:
                ret += "def " + Name + " (" + argment.returnFunctionArgument + ") :\n" + Data + functionDefinition() + "\n";
                break;

            case CPP:
                ret += Type + " " + Name + " (" + argment.returnFunctionArgument + ") {\n" + Data + "\n}" + functionDefinition();
                break;

            case RUST: {
                ret += ret += "fn " + Name;
                if (Name == "main")
                    ret += " (boot_info: &'static BootInfo) -> ! {" + Data + "\n\tloop{}\n}\n\n";
                else
                    ret += "(" + argment.returnFunctionArgument + ") ->" + Type + " {\n" + Data + "\n}\n\n";
                ret += functionDefinition();
            } break;

            case LLIR: {
                string FDQ = std::to_string(funcDefQuantity++);

                indent++;
                ret += "define " + Type + " @" + Name + "(" + argment.returnFunctionArgument + ") #" + FDQ +" {\nentry:\n" + argment.argMove + Data;
                if (Type == "void") ret += "    ret void\n";
                ret += "}\n\n";
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

        nowClassName = Name;

        (langMode == PYTHON) ? indent++ : 1;

        if (token[tokNumCounter].tokNum == SELF) {

            expect("self");

            tokNumCounter++;
            expect("{");
            tokNumCounter++;

            selfData = sent();

            if (langMode == PYTHON) {
                Data = addIndent() + "def __init__(self):\n\t" + selfData + "\n\n";
            } else if (langMode == CPP)
                ;

            expect("}");

            tokNumCounter++;
        }

        Data += functionDefinition();

        (langMode == PYTHON) ? indent-- : 1;

        indent--;
        expect("}");

        tokNumCounter++;

        switch (langMode) {
            case PYTHON:
                ret = "class " + Name + argment.returnFunctionArgument + ":\n\n" + Data;
                break;

            case CPP:
                ret = "typedef struct {\n" + selfData + "\n} " + Name + ";\n" + Data + "\n\n";
                break;

            case RUST:
                ret = "struct " + Name + overRide;
                break;

            default:
                break;
        }

        return ret + functionDefinition();
    } else if (token[tokNumCounter].tokNum == PUB) {

        expect("pub");
        tokNumCounter++; // pub

        expect("fn");
        tokNumCounter++; // fn

        Name = word();

        tokNumCounter++;

        if (token[tokNumCounter].tokNum == LBRACKET) {
            tokNumCounter++; // (
            if (token[tokNumCounter].tokNum != RBRACKET) {
                argment.returnFunctionArgument = ", " + funcDefArgument().returnFunctionArgument;
            }
            tokNumCounter++; // )
        }

        expect(":");

        tokNumCounter++;

        Type = word();

        string Fsent;

        Fsent = "pub fn " + Name + "(" + argment.returnFunctionArgument + ") : ";

        if (Type == "auto") {
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

        (langMode == PYTHON)
            ? ret = addIndent() + "def " + Name + " (self" + argment.returnFunctionArgument + "):\n" + Data + "\n" +
                    functionDefinition() + "\n"
            : ret = Type + " " + nowClassName + Name + " (" + nowClassName + " *self" + argment.returnFunctionArgument +
                    ") {\n" + Data + "\n}" + functionDefinition();
        //argment.returnFunctionArgument += argment.returnFunctionArgument + "";
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
        ret = funCall(""); 
        tokNumCounter++;
        expect(";");
        tokNumCounter++;
        ret = "#[" + ret + "]\n" + functionDefinition();
        return ret;
    }
    else ;
    return "";
}

