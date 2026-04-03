// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

// Analisador léxico da linguagem SAL — implementado como AFD.
// A variável `estado` representa o estado corrente; o switch(estado)
// dentro do loop principal realiza todas as transições, um caractere por iteração.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"
#include "diag.h"

struct LexEstado {
    FILE *arquivo;   // arquivo fonte aberto pelo chamador
    int proxChar;  // lookahead de 1 caractere (ou EOF)
    int linha;     // linha atual (base 1)
};

typedef struct {
    const char *lexema;
    TokenCat cat;
} PalavraReservada;

static const PalavraReservada palavrasReservadas[] = {
    { "bool",      sBOOL      },
    { "char",      sCHAR      },
    { "do",        sDO        },
    { "else",      sELSE      },
    { "end",       sEND       },
    { "false",     sFALSE     },
    { "fn",        sFN        },
    { "for",       sFOR       },
    { "globals",   sGLOBALS   },
    { "if",        sIF        },
    { "int",       sINT       },
    { "locals",    sLOCALS    },
    { "loop",      sLOOP      },
    { "main",      sMAIN      }, // reservado implícito
    { "match",     sMATCH     },
    { "module",    sMODULE    },
    { "otherwise", sOTHERWISE },
    { "print",     sPRINT     },
    { "proc",      sPROC      },
    { "ret",       sRETURN    },
    { "scan",      sSCAN      },
    { "start",     sSTART     },
    { "step",      sSTEP      },
    { "to",        sTO        },
    { "true",      sTRUE      },
    { "until",     sUNTIL     },
    { "v",         sOR        }, // operador de disjunção
    { "when",      sWHEN      },
    { "while",     sWHILE     },
    { NULL,        sEOF       }  // sentinela
};

typedef enum {
    qINICIAL = 0,    // entre tokens; ignora espaços e comentários
    qIDENT,          // lendo identificador ou palavra reservada
    qINTEIRO,        // lendo constante inteira
    qSTRING,         // dentro de literal de cadeia "..."
    qCHAR_CORPO,     // leu '\'', aguardando conteúdo do char
    qCHAR_FIM,       // leu conteúdo do char, aguardando '\''
    qMENOR,          // leu '<'  → <  <=  <>
    qMAIOR,          // leu '>'  → >  >=
    qIGUAL,          // leu '='  → =  =>
    qDOISPONTOS,     // leu ':'  → :  :=
    qPONTO,          // leu '.'  → aguarda segundo '.'
    qARROBA,         // leu '@'  → comentário de linha ou de bloco
    qCOMENT_LINHA,   // dentro de comentário de linha @ ...
    qCOMENT_BLOCO,   // dentro de comentário de bloco @{ ... }@
    qCOMENT_BLOCO_F  // leu '}' dentro do bloco; aguarda '@'
} Estado;

// Consome proxChar e faz a leitura do seguinte do arquivo e atualiza o contador de linhas
static int proximoChar(LexEstado *ls) {
    int c = ls->proxChar;
    ls->proxChar = fgetc(ls->arquivo);

    if (c == '\n') ls->linha++;
    return c;
}

// Pesquisa na tabela de palavras reservadas e retorna sIDENTIF se não encontrado
static TokenCat buscaReservada(const char *lex) {
    int i;

    for (i = 0; palavrasReservadas[i].lexema != NULL; i++) {
        if (strcmp(palavrasReservadas[i].lexema, lex) == 0)
            return palavrasReservadas[i].cat;
    }

    return sIDENTIF;
}

// Aloca e inicializa o estado do léxico e carrega o primeiro caractere de lookahead
LexEstado *lexInit(FILE *arquivo) {
    LexEstado *ls = (LexEstado *)malloc(sizeof(LexEstado));

    if (!ls) {
        fprintf(stderr, "Erro fatal: falha ao alocar estado do lexico.\n");
        exit(1);
    }

    ls->arquivo = arquivo;
    ls->linha = 1;
    ls->proxChar = fgetc(arquivo); // carrega o primeiro caractere

    return ls;
}

// Libera a memória alocada por lexInit
void lexFim(LexEstado *ls) {
    free(ls);
}

// lex_next — loop principal do automato.
// Peek em ls->proxChar sem consumir e switch(estado) decide a transição.
// Caracteres são consumidos via proximoChar() quando a transição avança.
Token lex_next(LexEstado *ls) {

    Estado estado = qINICIAL;
    Token  tok;

    int pos = 0;
    int c;

    tok.linha = ls->linha; // linha inicial que será atualizada ao encontrar cada token
    tok.lexema[0] = '\0';

    while (1) {
        c = ls->proxChar; // peek — lê sem consumir e só proximoChar() avança o cursor

        switch (estado) {

            case qINICIAL:

                if (c == EOF) {
                    proximoChar(ls);
                    tok.cat = sEOF;

                    strncpy(tok.lexema, "EOF", MAX_LEXEMA - 1);
                    tok.lexema[MAX_LEXEMA - 1] = '\0';

                    return tok;
                }

                if (isspace(c)) {
                    proximoChar(ls);
                    break;
                }

                if (isalpha(c) || c == '_') {
                    tok.linha = ls->linha; // registra a linha de início do token
                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = (char)proximoChar(ls);

                    else proximoChar(ls); // descarta se lexema estourou o buffer
                    estado = qIDENT;

                    break;
                }

                if (isdigit(c)) {
                    tok.linha = ls->linha;

                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = (char)proximoChar(ls);
                    else 
                        proximoChar(ls);

                    estado = qINTEIRO;
                    break;
                }

                if (c == '"') {

                    tok.linha = ls->linha;
                    proximoChar(ls); // consome '"' de abertura sem acumular no lexema

                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = '"';

                    estado = qSTRING;
                    break;
                }

                if (c == '\'') {

                    tok.linha = ls->linha;
                    proximoChar(ls); // consome '\'' de abertura sem acumular no lexema

                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = '\'';

                    estado = qCHAR_CORPO;
                    break;
                }

                if (c == '@') {
                    proximoChar(ls);
                    estado = qARROBA;
                    
                    break;
                }

                // Operadores e delimitadores: consome o caractere
                tok.linha = ls->linha;
                proximoChar(ls);

                switch (c) {
                    // Operadores que precisam de lookahead
                    case '<': 
                        estado = qMENOR;      
                        break;
                    case '>': 
                        estado = qMAIOR;      
                        break;
                    case '=': 
                        estado = qIGUAL;      
                        break;
                    case ':': 
                        estado = qDOISPONTOS; 
                        break;
                    case '.': 
                        estado = qPONTO;      
                        break;

                    // Tokens de um único caractere
                    case '+': 
                        tok.cat = sSOMA;      
                        tok.lexema[0] = '+'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '-': 
                        tok.cat = sSUBRAT;    
                        tok.lexema[0] = '-'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '*': 
                        tok.cat = sMULT;      
                        tok.lexema[0] = '*'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '/': 
                        tok.cat = sDIV;       
                        tok.lexema[0] = '/'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '^': 
                        tok.cat = sAND;       
                        tok.lexema[0] = '^'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '~': 
                        tok.cat = sNEG;       
                        tok.lexema[0] = '~'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '(': 
                        tok.cat = sABREPAR;   
                        tok.lexema[0] = '('; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case ')': 
                        tok.cat = sFECHAPAR;  
                        tok.lexema[0] = ')'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case '[': 
                        tok.cat = sABRECOL;   
                        tok.lexema[0] = '['; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case ']': 
                        tok.cat = sFECHACOL;  
                        tok.lexema[0] = ']'; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case ',': 
                        tok.cat = sVIRGULA;   
                        tok.lexema[0] = ','; 
                        tok.lexema[1] = '\0'; 
                        return tok;
                    case ';': 
                        tok.cat = sPONTEVIRG; 
                        tok.lexema[0] = ';'; 
                        tok.lexema[1] = '\0'; 
                        return tok;

                    default:
                        diag_error_lex(tok.linha, (char)c);
                        break;
                }
                break;

            case qIDENT:
                if (isalnum(c) || c == '_') {
                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = (char)proximoChar(ls);
                    else 
                        proximoChar(ls); // avança mesmo sem acumular para não travar o cursor
                } else {
                    // Próximo char não é parte do ident então finaliza sem consumir
                    tok.lexema[pos] = '\0';
                    tok.cat = buscaReservada(tok.lexema); // verifica se é palavra reservada
                    return tok;
                }
                break;

            case qINTEIRO:
                if (isdigit(c)) {
                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = (char)proximoChar(ls);
                    else 
                        proximoChar(ls);
                } else {
                    // Próximo char não é dígito então finaliza sem consumir
                    tok.lexema[pos] = '\0';
                    tok.cat = sCTEINT;
                    return tok;
                }
                break;

            case qSTRING:
                if (c == EOF || c == '\n') {
                    // EOF aqui é sempre erro, pois não permite multilinhas
                    fprintf(stderr, "[ERRO LEXICO] %d: literal de string nao terminado.\n", tok.linha);
                    exit(1);
                }
                if (c == '"') {
                    proximoChar(ls);

                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = '"';

                    tok.lexema[pos] = '\0';
                    tok.cat = sSTRING;
                    return tok;
                }
                if (pos < MAX_LEXEMA - 1) 
                    tok.lexema[pos++] = (char)proximoChar(ls);
                else 
                    proximoChar(ls);
                break;

            case qCHAR_CORPO:
                if (c == EOF) {
                    fprintf(stderr, "[ERRO LEXICO] %d: literal de caractere nao terminado.\n", tok.linha);
                    exit(1);
                }
                if (c != '\'') {
                    // Consome exatamente 1 caractere de conteúdo e o restante é tratado em qCHAR_FIM
                    if (pos < MAX_LEXEMA - 1) 
                        tok.lexema[pos++] = (char)proximoChar(ls);
                    else 
                        proximoChar(ls);
                }
                // qCHAR_FIM aguarda o '\'' de fechamento
                estado = qCHAR_FIM;
                break;

            case qCHAR_FIM:
                if (c != '\'') {
                    fprintf(stderr, "[ERRO LEXICO] %d: literal de caractere nao terminado.\n", tok.linha);
                    exit(1);
                }

                proximoChar(ls);
                if (pos < MAX_LEXEMA - 1) 
                    tok.lexema[pos++] = '\'';

                tok.lexema[pos] = '\0';
                tok.cat = sCTECHAR;

                return tok;

            case qMENOR:
                if (c == '>') {
                    proximoChar(ls);

                    tok.cat = sDIFERENTE;
                    tok.lexema[0] = '<'; 

                    tok.lexema[1] = '>'; 
                    tok.lexema[2] = '\0';
                } else if (c == '=') {
                    proximoChar(ls);

                    tok.cat = sMENORIG;
                    tok.lexema[0] = '<'; 

                    tok.lexema[1] = '='; 
                    tok.lexema[2] = '\0';
                } else {
                    tok.cat = sMENOR;
                    tok.lexema[0] = '<'; 
                    tok.lexema[1] = '\0';
                }
                return tok;

            case qMAIOR:
                if (c == '=') {
                    proximoChar(ls);
                    tok.cat = sMAIORIG;
                    tok.lexema[0] = '>'; 
                    tok.lexema[1] = '='; 
                    tok.lexema[2] = '\0';
                } else {
                    tok.cat = sMAIOR;
                    tok.lexema[0] = '>'; 
                    tok.lexema[1] = '\0';
                }

                return tok;

            case qIGUAL:
                if (c == '>') {
                    proximoChar(ls);

                    tok.cat = sIMPLIC;
                    tok.lexema[0] = '='; 

                    tok.lexema[1] = '>'; 
                    tok.lexema[2] = '\0';
                } else {
                    tok.cat = sIGUAL;
                    tok.lexema[0] = '='; 
                    tok.lexema[1] = '\0';
                }
                return tok;

            case qDOISPONTOS:
                if (c == '=') {
                    proximoChar(ls);

                    tok.cat = sATRIB;
                    tok.lexema[0] = ':'; 

                    tok.lexema[1] = '='; 
                    tok.lexema[2] = '\0';
                } else {
                    tok.cat = sDOISPONTOS;
                    tok.lexema[0] = ':'; 
                    tok.lexema[1] = '\0';
                }
                return tok;

            case qPONTO:
                if (c == '.') {
                    proximoChar(ls);

                    tok.cat = sPTOPTO;
                    tok.lexema[0] = '.'; 

                    tok.lexema[1] = '.'; 
                    tok.lexema[2] = '\0';

                    return tok;
                }
                // Ponto isolado não existe
                diag_error_lex(tok.linha, '.');
                break;

            case qARROBA:
                if (c == '{') {
                    proximoChar(ls);
                    estado = qCOMENT_BLOCO;
                } else {
                    estado = qCOMENT_LINHA;
                }
                break;

            case qCOMENT_LINHA:
                if (c == EOF || c == '\n') {
                    // Não consome '\n', ele volta para qINICIAL que o trata como espaço e incrementa a linha
                    estado = qINICIAL;
                } else {
                    proximoChar(ls);
                }
                break;

            case qCOMENT_BLOCO:
                if (c == EOF) {
                    estado = qINICIAL; // bloco não fechado e EOF retornado na próxima iteração
                } else if (c == '}') {
                    proximoChar(ls);
                    estado = qCOMENT_BLOCO_F;
                } else {
                    proximoChar(ls);
                }
                break;

            case qCOMENT_BLOCO_F:
                if (c == '@') {
                    proximoChar(ls); // consome '@' sequência '}@' fecha o bloco
                    estado = qINICIAL;
                } else {
                    // '}' sem '@' seguinte falso alarme volta ao interior do bloco
                    estado = qCOMENT_BLOCO;
                }
                break;
        }
    }
}

// Converte uma categoria de token para sua representação textual
const char *lexCatStr(TokenCat cat) {
    switch (cat) {
        case sMODULE:      return "sMODULE";
        case sGLOBALS:     return "sGLOBALS";
        case sLOCALS:      return "sLOCALS";
        case sSTART:       return "sSTART";
        case sEND:         return "sEND";
        case sPROC:        return "sPROC";
        case sFN:          return "sFN";
        case sMAIN:        return "sMAIN";
        case sINT:         return "sINT";
        case sBOOL:        return "sBOOL";
        case sCHAR:        return "sCHAR";
        case sIF:          return "sIF";
        case sELSE:        return "sELSE";
        case sMATCH:       return "sMATCH";
        case sWHEN:        return "sWHEN";
        case sOTHERWISE:   return "sOTHERWISE";
        case sFOR:         return "sFOR";
        case sTO:          return "sTO";
        case sSTEP:        return "sSTEP";
        case sDO:          return "sDO";
        case sLOOP:        return "sLOOP";
        case sWHILE:       return "sWHILE";
        case sUNTIL:       return "sUNTIL";
        case sPRINT:       return "sPRINT";
        case sSCAN:        return "sSCAN";
        case sRETURN:      return "sRETURN";
        case sTRUE:        return "sTRUE";
        case sFALSE:       return "sFALSE";
        case sIDENTIF:     return "sIDENTIF";
        case sCTEINT:      return "sCTEINT";
        case sCTECHAR:     return "sCTECHAR";
        case sSTRING:      return "sSTRING";
        case sATRIB:       return "sATRIB";
        case sSOMA:        return "sSOMA";
        case sSUBRAT:      return "sSUBRAT";
        case sMULT:        return "sMULT";
        case sDIV:         return "sDIV";
        case sIGUAL:       return "sIGUAL";
        case sDIFERENTE:   return "sDIFERENTE";
        case sMAIOR:       return "sMAIOR";
        case sMENOR:       return "sMENOR";
        case sMAIORIG:     return "sMAIORIG";
        case sMENORIG:     return "sMENORIG";
        case sAND:         return "sAND";
        case sOR:          return "sOR";
        case sNEG:         return "sNEG";
        case sIMPLIC:      return "sIMPLIC";
        case sPTOPTO:      return "sPTOPTO";
        case sABREPAR:     return "sABREPAR";
        case sFECHAPAR:    return "sFECHAPAR";
        case sABRECOL:     return "sABRECOL";
        case sFECHACOL:    return "sFECHACOL";
        case sVIRGULA:     return "sVIRGULA";
        case sPONTEVIRG:   return "sPONTEVIRG";
        case sDOISPONTOS:  return "sDOISPONTOS";
        case sEOF:         return "sEOF";
        default:           return "DESCONHECIDO";
    }
}
