// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef LEX_H
#define LEX_H

#include <stdio.h>

// Categorias de tokens
typedef enum {
    // Palavras reservadas
    sMODULE,      // module
    sGLOBALS,     // globals
    sLOCALS,      // locals
    sSTART,       // start
    sEND,         // end
    sPROC,        // proc
    sFN,          // fn
    sMAIN,        // main (reservado implícito)
    sINT,         // int
    sBOOL,        // bool
    sCHAR,        // char
    sIF,          // if
    sELSE,        // else
    sMATCH,       // match
    sWHEN,        // when
    sOTHERWISE,   // otherwise
    sFOR,         // for
    sTO,          // to
    sSTEP,        // step
    sDO,          // do
    sLOOP,        // loop
    sWHILE,       // while
    sUNTIL,       // until
    sPRINT,       // print
    sSCAN,        // scan
    sRETURN,      // ret
    sTRUE,        // true
    sFALSE,       // false

    // Identificadores e literais
    sIDENTIF,     // identificador definido
    sCTEINT,      // constante inteira
    sCTECHAR,     // constante de caractere
    sSTRING,      // literal de cadeia

    // Operadores
    sATRIB,       // :=
    sSOMA,        // +
    sSUBRAT,      // -  
    sMULT,        // *
    sDIV,         // /
    sIGUAL,       // =
    sDIFERENTE,   // <>
    sMAIOR,       // >
    sMENOR,       // <
    sMAIORIG,     // >=
    sMENORIG,     // <=
    sAND,         // ^  
    sOR,          // v  
    sNEG,         // ~  
    sIMPLIC,      // => implicação (cláusula when)
    sPTOPTO,      // .. intervalo (cláusula when)

    // Delimitadores
    sABREPAR,     // (
    sFECHAPAR,    // )
    sABRECOL,     // [
    sFECHACOL,    // ]
    sVIRGULA,     // ,
    sPONTEVIRG,   // ;
    sDOISPONTOS,  // :

    sEOF
} TokenCat;

#define MAX_LEXEMA 256

typedef struct {
    TokenCat cat;
    char lexema[MAX_LEXEMA];
    int linha;
} Token;

// Handle opaco do estado interno do léxico
typedef struct LexEstado LexEstado;

// lexInit, cria o estado do léxico e o arquivo já deve estar aberto
LexEstado  *lexInit(FILE *arquivo);

// lexFim, libera recursos alocados por lexInit
void lexFim(LexEstado *ls);

// lex_next, retorna o próximo token; descarta espaços e comentários
Token lex_next(LexEstado *ls);

// lexCatStr, converte categoria para string
const char *lexCatStr(TokenCat cat);

#endif
