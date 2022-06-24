#include "Lexer.hpp"

/*
 * ===---Lexer::lex(Target file's all data)---===
 */

vector<tokens> Lexer::lex(string fileData) {
    int i = 0;
    // inpout tokens
    vector<tokens> token;

    for (int i = 0; i < fileData.size(); i++) {
        // Now lokking word
        char nowChar = fileData[i];
        string addStr;
        int str_c = 0;

        // RESERV | OP | word

        if (nowChar == '\n' || nowChar == ' ' || nowChar == '\t') {
            continue;
        }
        /*===--- fn ---=== */
        if (fileData.substr(i, LEN_FN) == "fn ") {
            token.push_back({FN, "fn"});
            if (fileData.substr(i + LEN_FN, LEN_ENTRY) == "main") {
                token.push_back({ENTRY, "main"});
                i += LEN_ENTRY;
            }
            i += LEN_FN - 1;
            continue;
        }
        /*===--- return ---=== */
        if (fileData.substr(i, LEN_RETURN) == "return ") {
            token.push_back({RETURN, "return"});
            i += LEN_RETURN - 1;
            continue;
        }
        /*===--- let ---=== */
        if (fileData.substr(i, LEN_LET) == "let ") {
            token.push_back({LET, "let"});
            i += LEN_LET - 1;
            continue;
        }
        /*===--- mut ---=== */
        if (fileData.substr(i, LEN_MUT) == "@mut ") {
            token.push_back({MUT, "@mut"});
            i += LEN_MUT - 1;
            continue;
        }
        /*===--- put ---=== */
        if (fileData.substr(i, LEN_PUT) == "put ") {
            token.push_back({PUT, "put"});
            i += LEN_PUT - 1;
            continue;
        }
        /*===--- append ---=== */
        if (fileData.substr(i, LEN_APPEND) == "append ") {
            token.push_back({APPEND, "append"});
            i += LEN_PUT - 1;
            continue;
        }
        /*===--- if ---=== */
        if (fileData.substr(i, LEN_IF) == "if ") {
            token.push_back({IF, "if"});
            i += LEN_IF - 1;
            continue;
        }
        /*===--- <- ---===*/
		if (fileData.substr(i, 2) == "<-") {
            token.push_back({EQ, "<-"});
            i += 2;
            continue;
		}
        /*===--- for ---===*/
		if (fileData.substr(i, LEN_FOR) == "for ") {
            token.push_back({FOR, "for"});
            i += LEN_FOR-1;
            continue;
		}
        /*===--- in ---===*/
		if (fileData.substr(i, LEN_IN) == "in ") {
            token.push_back({IN, "in"});
            i += LEN_IN - 1;
            continue;
		}
        /*===--- op(MAP : OP) ---=== */
        if (OP.find(nowChar) != OP.end()) {
            token.push_back({OP.find(nowChar)->second, addStr.append(1, nowChar)});
            continue;
        }
        /*===--- else(STR | NUMBER | WORD) ---=== */
        addStr = "";
        char wordChar;
        while (1) {
            wordChar = fileData[i++];
            if (wordChar != '\n' && wordChar != ' ' && OP.find(wordChar) == OP.end()) {
                addStr.append(1, wordChar);
                continue;
            }
            break;
        }
        token.push_back({WORD, addStr});
        addStr = "";
        if (OP.find(wordChar) != OP.end())
            token.push_back({OP.find(wordChar)->second, addStr.append(1, wordChar)});
        i--;
    }
    return token;
}
