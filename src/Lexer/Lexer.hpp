#pragma once

#include "../CRU.hpp"

#define LEN_FN 3
#define LEN_IF 3
#define LEN_IN 3
#define LEN_VEC 3
#define LEN_PUB 4
#define LEN_FOR 4
#define LEN_LET 4
#define LEN_PUT 4
#define LEN_SELF 4
#define LEN_MUT 5
#define LEN_ENUM 5
#define LEN_ELSE 5
#define LEN_ELIF 5
#define LEN_CALL 2
#define LEN_ENTRY 4
#define LEN_CLASS 6
#define LEN_WHILE 6
#define LEN_PRINT 6
#define LEN_CLASS 6
#define LEN_MAC_STRING 6 
#define LEN_RETURN 7
#define LEN_APPEND 7
#define LEN_IMPORT 7

/* Natural tokens */
enum RESERV {
    /* RESERVs */
    ENTRY,
    FN,
    LET,
    RETURN,
    PUT,
    PUB,
    PRINT,
    IF,
    ELSE,
    ELIF,
    SELF,
    WHILE,
    LOOP,
    CALL,
    MUT,
    FOR,
    IN,
    APPEND,
    VEC,
    IMPORT,
    MAC_STRING,
    CLASS,
    ENUM,
    EQEQ,

    /* TYPE AND VARs */
    WORD,
    NUMBER,
    STR,
    VAR,
    AUTO,

    /* OPs */
    LBRACKET,
    RBRACKET,
    LRIPPLE,
    RRIPPLE,
    CORON,
    SEMICORON,
    SQ,
    DQ,
    CANMA,
    PERIOD,
    BIG,
    MINI,
    DOUBLEQUOT,
    SINGLEQUOT,
    PLUS,
    MIN,
    MUL,
    DIV,
    EQ,
    TILDE,
    ADDRESS,
    LSQBRACKET,
    RSQBRACKET,
    ATSIGN,
 //   HASHTAG,
};

class Lexer {
    /* Mapping OPs */
    map<char, RESERV> OP = {
        {'(', LBRACKET},
        {')', RBRACKET},
        {'{', LRIPPLE},
        {'}', RRIPPLE},
        {'[', LSQBRACKET},
        {']', RSQBRACKET},
        {':', CORON},
        {';', SEMICORON},
        {'\'', SQ},
        {'"', DQ},
        {',', CANMA},
        {'.', PERIOD},
        {'<', BIG},
        {'>', MINI},
        {'+', PLUS},
        {'-', MIN},
        {'*', MUL},
        {'/', DIV},
        {'~', TILDE},
        {'&', ADDRESS},
        {'@', ATSIGN},
    };

  public:
    vector<tokens> lex(string fileData);
};

