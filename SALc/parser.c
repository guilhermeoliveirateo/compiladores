// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

// Analisador Sintático Descendente Recursivo (ASDR).
// Cada não-terminal possui uma função parse().
// A hierarquia de expressões segue a tabela de precedência do Manual SAL.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"
#include "symtab.h"
#include "diag.h"
#include "log.h"

#define MAX_PARAMS 64

static LexEstado *lexer = NULL;
static Token tokAtual;

// Contexto da sub-rotina corrente que é usado nas descrições de bloco
static char subNome[MAX_LEXEMA] = "global";
static char subTipo[8] = "proc";
static int blocoSeq = 0;

// Solicita o próximo token ao léxico e o registra no log se habilitado
static void avancar(void) {
    tokAtual = lex_next(lexer);

    if (logTokensHabilitado())
        logToken(tokAtual.linha, lexCatStr(tokAtual.cat), tokAtual.lexema);
}

// Verifica se tokAtual é o esperado e avança, reporta erro sintático se não for
static void esperar(TokenCat cat) {
    if (tokAtual.cat != cat)
        diag_error(tokAtual.linha, lexCatStr(cat), tokAtual.lexema);
    
    avancar();
}

// Avança e retorna 1 se tokAtual for o esperado, caso contrário retorna 0 sem avançar
static int aceitar(TokenCat cat) {
    if (tokAtual.cat == cat) {
        avancar();
        return 1;
    }

    return 0;
}

// Emite entrada de não-terminal no trace
static void traceEntra(const char *nome) {
    char buf[256];

    snprintf(buf, sizeof(buf), ">> %s (linha %d)", nome, tokAtual.linha);
    diag_info(buf);
}

// Emite saída de não-terminal no trace
static void traceSai(const char *nome) {
    char buf[256];

    snprintf(buf, sizeof(buf), "<< %s", nome);
    diag_info(buf);
}

static void parseBloco(void);
static void parseComando(void);
static void parseExpr(void);

// tpo ::= sINT | sBOOL | sCHAR
static TipoSimbolo parseTipo(void) {
    TipoSimbolo t;
    traceEntra("tipo");

    switch (tokAtual.cat) {
        case sINT:  
            t = tipoInt;  
            avancar();
        break;
        case sBOOL: 
            t = tipoBool; 
            avancar(); 
        break;
        case sCHAR: 
            t = tipoChar; 
            avancar(); 
        break;
        default:
            diag_error(tokAtual.linha, "int | bool | char", tokAtual.lexema);
            t = tipoInt; // nunca alcançado
    }

    traceSai("tipo");
    return t;
}

// decls+ ::= (id[,id]* : tpo ;)+  — variáveis simples ou vetores id[N]
static void parseDeclaracoes(CatSimbolo cat) {
    traceEntra("decls");

    while (tokAtual.cat == sIDENTIF) {
        char nomes[32][MAX_LEXEMA];
        
        int  tamanhos[32]; // 0 = escalar maior que isso vetor de tamanho declarado
        int  qtd = 0;
        
        TipoSimbolo tipo;
        int  i;

        strncpy(nomes[qtd], tokAtual.lexema, MAX_LEXEMA - 1);
        nomes[qtd][MAX_LEXEMA - 1] = '\0';

        tamanhos[qtd] = 0;
        avancar();

        if (aceitar(sABRECOL)) {
            if (tokAtual.cat != sCTEINT)
                diag_error(tokAtual.linha, "constante inteira", tokAtual.lexema);

            tamanhos[qtd] = atoi(tokAtual.lexema);
            avancar();
            esperar(sFECHACOL);
        }
        qtd++;

        while (aceitar(sVIRGULA)) {

            if (tokAtual.cat != sIDENTIF)
                diag_error(tokAtual.linha, "identificador", tokAtual.lexema);
            
            if (qtd < 32) { // descarta silenciosamente se ultrapassar o limite do buffer
                strncpy(nomes[qtd], tokAtual.lexema, MAX_LEXEMA - 1);
                nomes[qtd][MAX_LEXEMA - 1] = '\0';
                tamanhos[qtd] = 0;
            }
            
            avancar();

            if (aceitar(sABRECOL)) {
                
                if (tokAtual.cat != sCTEINT)
                    diag_error(tokAtual.linha, "constante inteira", tokAtual.lexema);
                
                if (qtd < 32) 
                    tamanhos[qtd] = atoi(tokAtual.lexema);

                avancar();
                esperar(sFECHACOL);
            }

            if (qtd < 32) 
                qtd++;
        }

        esperar(sDOISPONTOS);
        tipo = parseTipo();
        esperar(sPONTEVIRG);

        // Insere todos os ids da linha de declaração no escopo corrente
        for (i = 0; i < qtd; i++) {
            if (!ts_insert(nomes[i], cat, tipo, tamanhos[i]))
                fprintf(stderr, "[AVISO] linha %d: '%s' ja declarado neste escopo.\n", tokAtual.linha, nomes[i]);
        }
    }

    traceSai("decls");
}

// glob ::= sGLOBALS decls+
static void parseGlobais(void) {
    traceEntra("glob");
    esperar(sGLOBALS);

    parseDeclaracoes(catVariavel);
    traceSai("glob");
}

// Buffer temporário para adiar inserção de parâmetros até o escopo local ser aberto
typedef struct {
    char nome[MAX_LEXEMA];
    TipoSimbolo tipo;
} InfoParam;

// param ::= id ":" tpo ("," id ":" tpo)*, não insere na TS, retorna qtd
static int parseParametrosBuffer(InfoParam buf[], int maxBuf) {
    int qtd = 0;
    traceEntra("param");

    if (tokAtual.cat == sFECHAPAR) {
        traceSai("param");
        return 0; // lista vazia — ')' detectado via peek antes de consumir
    }

    // Coleta sem inserir na TS
    do {
        if (tokAtual.cat != sIDENTIF)
            diag_error(tokAtual.linha, "identificador", tokAtual.lexema);
        if (qtd < maxBuf) {
            strncpy(buf[qtd].nome, tokAtual.lexema, MAX_LEXEMA - 1);
            buf[qtd].nome[MAX_LEXEMA - 1] = '\0';
        }

        avancar();
        esperar(sDOISPONTOS);

        if (qtd < maxBuf) 
            buf[qtd].tipo = parseTipo();
        else              
            parseTipo(); // consome o tipo mesmo quando o buffer está cheio
        
        qtd++;
    } while (aceitar(sVIRGULA));

    traceSai("param");
    return qtd;
}

// bco ::= sSTART (cmd ";")* sEND
static void parseBloco(void) {

    char descrBloco[MAX_LEXEMA + 32];
    traceEntra("bco");

    blocoSeq++; // numeração sequencial dentro da sub-rotina para identificar blocos aninhados
    snprintf(descrBloco, sizeof(descrBloco), "%s:%s.block#%d", subTipo, subNome, blocoSeq);
    tsAbrirEscopo(descrBloco);

    esperar(sSTART);

    // sEOF como condição de parada evita loop infinito se 'end' estiver faltando
    while (tokAtual.cat != sEND && tokAtual.cat != sEOF) {
        parseComando();
        esperar(sPONTEVIRG);
    }

    esperar(sEND);

    tsFecharEscopo();
    traceSai("bco");
}

// fact ::= litl | id | vec | call | ~fact | -fact | (expr)
static void parseFator(void) {
    traceEntra("fact");

    switch (tokAtual.cat) {

        case sNEG:
            avancar();
            parseFator();
            break;

        case sSUBRAT:
            avancar();
            parseFator();
            break;

        case sABREPAR:
            avancar();
            parseExpr();
            esperar(sFECHAPAR);
            break;

        case sCTEINT:
        case sCTECHAR:
        case sSTRING:
        case sTRUE:
        case sFALSE:
            avancar();
            break;

        case sIDENTIF:
            avancar();
            if (aceitar(sABRECOL)) {
                // id[expr], acesso a vetor
                parseExpr();
                esperar(sFECHACOL);

            } else if (aceitar(sABREPAR)) {
                // id(args), chamada de função
                if (tokAtual.cat != sFECHAPAR) {
                    parseExpr();
                    while (aceitar(sVIRGULA)) 
                        parseExpr();
                }
                esperar(sFECHAPAR);
            }
            break;

        default:
            diag_error(tokAtual.linha, "expressao primaria", tokAtual.lexema);
    }

    traceSai("fact");
}

// exarp ::= fact (* | / fact)*
static void parseExprMul(void) {
    traceEntra("exarp");
    parseFator();

    while (tokAtual.cat == sMULT || tokAtual.cat == sDIV) {
        avancar();
        parseFator();
    }

    traceSai("exarp");
}

// exari ::= exarp (+ | - exarp)*
static void parseExprAdi(void) {
    traceEntra("exari");
    parseExprMul();

    while (tokAtual.cat == sSOMA || tokAtual.cat == sSUBRAT) {
        avancar();
        parseExprMul();
    }

    traceSai("exari");
}

// exrel ::= exari (op_rel exari)*
static void parseExprRel(void) {
    traceEntra("exrel");
    parseExprAdi();

    while (tokAtual.cat == sMAIOR   || tokAtual.cat == sMAIORIG ||
           tokAtual.cat == sMENOR   || tokAtual.cat == sMENORIG ||
           tokAtual.cat == sIGUAL   || tokAtual.cat == sDIFERENTE) {
        avancar();
        parseExprAdi();
    }

    traceSai("exrel");
}

// exlog ::= exrel (^ exrel)*
static void parseExprLog(void) {
    traceEntra("exlog");
    parseExprRel();

    while (tokAtual.cat == sAND) {
        avancar();
        parseExprRel();
    }

    traceSai("exlog");
}

// expr ::= exlog (v exlog)*  — menor precedência entre binários
static void parseExpr(void) {
    traceEntra("expr");
    parseExprLog();

    while (tokAtual.cat == sOR) {
        avancar();
        parseExprLog();
    }

    traceSai("expr");
}

// out ::= sPRINT "(" expr ("," expr)* ")"
static void parsePrint(void) {
    traceEntra("out");

    esperar(sPRINT);
    esperar(sABREPAR);

    parseExpr();
    while (aceitar(sVIRGULA)) 
        parseExpr();

    esperar(sFECHAPAR);
    traceSai("out");
}

// inp ::= sSCAN "(" (id | vec) ")"
static void parseScan(void) {
    traceEntra("inp");

    esperar(sSCAN);
    esperar(sABREPAR);

    if (tokAtual.cat != sIDENTIF)
        diag_error(tokAtual.linha, "identificador", tokAtual.lexema);
    avancar();

    if (aceitar(sABRECOL)) {
        parseExpr();
        esperar(sFECHACOL);
    }

    esperar(sFECHAPAR);
    traceSai("inp");
}

// if ::= sIF "(" expr ")" cmd (sELSE cmd)?
static void parseIf(void) {
    traceEntra("if");

    esperar(sIF);
    esperar(sABREPAR);

    parseExpr();
    esperar(sFECHAPAR);

    parseComando();
    if (aceitar(sELSE)) 
        parseComando();

    traceSai("if");
}

// mat ::= sMATCH "(" expr ")" whn+ othr? sEND
// whn  ::= sWHEN wcnd sIMPLIC cmd ";"
// wcnd ::= witem ("," witem)*   witem ::= [-] sCTEINT [sPTOPTO [-] sCTEINT]
static void parseMatch(void) {
    traceEntra("mat");

    esperar(sMATCH);
    esperar(sABREPAR);

    parseExpr();
    esperar(sFECHAPAR);

    if (tokAtual.cat != sWHEN)
        diag_error(tokAtual.linha, "when", tokAtual.lexema);

    while (tokAtual.cat == sWHEN) {
        traceEntra("whn");
        avancar(); // consome 'when'

        do {
            aceitar(sSUBRAT); // sinal opcional
            if (tokAtual.cat != sCTEINT)
                diag_error(tokAtual.linha, "constante inteira", tokAtual.lexema);
            avancar();

            if (aceitar(sPTOPTO)) { // intervalo: val..val
                aceitar(sSUBRAT);

                if (tokAtual.cat != sCTEINT)
                    diag_error(tokAtual.linha, "constante inteira", tokAtual.lexema);
                avancar();
            }
        } while (aceitar(sVIRGULA));

        esperar(sIMPLIC);
        parseComando();
        esperar(sPONTEVIRG);
        traceSai("whn");
    }

    if (aceitar(sOTHERWISE)) {
        traceEntra("othr");

        esperar(sIMPLIC);
        parseComando();

        esperar(sPONTEVIRG);
        traceSai("othr");
    }

    esperar(sEND);
    traceSai("mat");
}

// fr ::= sFOR id ":=" expr sTO expr (sSTEP expr)? sDO cmd
static void parseFor(void) {
    traceEntra("fr");
    esperar(sFOR);

    if (tokAtual.cat != sIDENTIF)
        diag_error(tokAtual.linha, "identificador", tokAtual.lexema);

    avancar();
    esperar(sATRIB);
    parseExpr(); // valor inicial

    esperar(sTO);
    parseExpr(); // valor final

    if (aceitar(sSTEP))
        parseExpr(); // passo

    esperar(sDO);
    parseComando();
    traceSai("fr");
}

// wh ::= sLOOP sWHILE "(" expr ")" cmd  —  'loop' já consumido
static void parseLoopWhile(void) {
    traceEntra("wh");

    esperar(sWHILE);
    esperar(sABREPAR);

    parseExpr();
    esperar(sFECHAPAR);

    parseComando();
    traceSai("wh");
}

// rpt ::= sLOOP (cmd ";")* sUNTIL "(" expr ")"  —  'loop' já consumido
static void parseLoopUntil(void) {
    traceEntra("rpt");

    while (tokAtual.cat != sUNTIL && tokAtual.cat != sEOF) {
        parseComando();
        esperar(sPONTEVIRG);
    }

    esperar(sUNTIL);
    esperar(sABREPAR);

    parseExpr();
    esperar(sFECHAPAR);

    traceSai("rpt");
}

// ret ::= sRETURN expr
static void parseRet(void) {
    traceEntra("ret");
    esperar(sRETURN);

    parseExpr();
    traceSai("ret");
}

// cmd ::= out | inp | if | mat | fr | wh | rpt | atr | call | ret | bco
static void parseComando(void) {
    traceEntra("cmd");

    switch (tokAtual.cat) {

        case sPRINT:  
            parsePrint();  
        break;
        case sSCAN:   
            parseScan();   
        break;
        case sIF:     
            parseIf();     
        break;
        case sMATCH:  
            parseMatch();  
        break;
        case sFOR:    
            parseFor();    
        break;
        case sRETURN: 
            parseRet();    
        break;
        case sSTART:  
            parseBloco();  
        break;

        case sLOOP:
            avancar(); // consome 'loop'
            if (tokAtual.cat == sWHILE)
                parseLoopWhile();
            else
                parseLoopUntil();
            break;

        case sIDENTIF: {
            // Próximo token distingue: sATRIB → atribuição, sABRECOL → vetor, sABREPAR → chamada
            avancar();

            if (tokAtual.cat == sATRIB) {
                traceEntra("atr");
                avancar();

                parseExpr();
                traceSai("atr");
            } else if (tokAtual.cat == sABRECOL) {
                traceEntra("atr");
                avancar();

                parseExpr();       // índice
                esperar(sFECHACOL);

                esperar(sATRIB);
                parseExpr();       // valor

                traceSai("atr");
            } else if (tokAtual.cat == sABREPAR) {
                traceEntra("call");
                avancar();

                if (tokAtual.cat != sFECHAPAR) {
                    parseExpr();
                    while (aceitar(sVIRGULA)) 
                        parseExpr();
                }

                esperar(sFECHAPAR);
                traceSai("call");
            } else {
                diag_error(tokAtual.linha, ":= ou [ ou (", tokAtual.lexema);
            }
            break;
        }

        default:
            diag_error(tokAtual.linha, "comando", tokAtual.lexema);
    }

    traceSai("cmd");
}

// func ::= sFN id "(" param? ")" ":" tpo [locals decls] bco
// Coleta params em buffer e insere função no global depois abre escopo local e por fim insere params
static void parseFuncao(void) {
    char      nome[MAX_LEXEMA];
    char      descrEscopo[MAX_LEXEMA + 16];

    TipoSimbolo tipoRet;
    InfoParam params[MAX_PARAMS];

    int       nParams;
    int       i;

    traceEntra("func");
    esperar(sFN);

    if (tokAtual.cat != sIDENTIF)
        diag_error(tokAtual.linha, "identificador", tokAtual.lexema);

    strncpy(nome, tokAtual.lexema, MAX_LEXEMA - 1);
    nome[MAX_LEXEMA - 1] = '\0';
    avancar();

    esperar(sABREPAR);
    nParams = parseParametrosBuffer(params, MAX_PARAMS);

    esperar(sFECHAPAR);
    esperar(sDOISPONTOS);
    tipoRet = parseTipo();

    // Insere no escopo global ANTES de abrir o local — a função pertence ao módulo, não a si mesma
    if (!ts_insert(nome, catFuncao, tipoRet, nParams))
        fprintf(stderr, "[AVISO] linha %d: funcao '%s' ja declarada.\n", tokAtual.linha, nome);

    snprintf(descrEscopo, sizeof(descrEscopo), "fn:%s.locals", nome);
    tsAbrirEscopo(descrEscopo); // abre escopo local agora que a assinatura já foi registrada

    // Insere os parâmetros do buffer no escopo local recém-aberto
    for (i = 0; i < nParams; i++)
        ts_insert(params[i].nome, catParametro, params[i].tipo, 0);

    strncpy(subNome, nome, MAX_LEXEMA - 1);
    subNome[MAX_LEXEMA - 1] = '\0';

    strncpy(subTipo, "fn", sizeof(subTipo) - 1);
    blocoSeq = 0; // reinicia contador de blocos para esta sub-rotina

    if (aceitar(sLOCALS))
        parseDeclaracoes(catVariavel);

    parseBloco();

    tsFecharEscopo();
    traceSai("func");
}

// proc ::= sPROC id "(" param? ")" [locals decls] bco e cobre também princ (proc main)
static void parseProcedimento(void) {
    char nome[MAX_LEXEMA];
    char descrEscopo[MAX_LEXEMA + 16];

    InfoParam params[MAX_PARAMS];
    int nParams;
    int i;

    traceEntra("proc");
    esperar(sPROC);

    if (tokAtual.cat != sIDENTIF && tokAtual.cat != sMAIN)
        diag_error(tokAtual.linha, "identificador ou main", tokAtual.lexema);

    strncpy(nome, tokAtual.lexema, MAX_LEXEMA - 1);
    nome[MAX_LEXEMA - 1] = '\0';
    avancar();

    esperar(sABREPAR);
    nParams = parseParametrosBuffer(params, MAX_PARAMS);
    esperar(sFECHAPAR);

    // Insere no escopo global ANTES de abrir o loca
    if (!ts_insert(nome, catProcedimento, tipoVoid, nParams))
        fprintf(stderr, "[AVISO] linha %d: procedimento '%s' ja declarado.\n", tokAtual.linha, nome);

    snprintf(descrEscopo, sizeof(descrEscopo), "proc:%s.locals", nome);
    tsAbrirEscopo(descrEscopo); // abre escopo local após registrar a assinatura no global

    // Insere os parâmetros do buffer no escopo local recém-aberto
    for (i = 0; i < nParams; i++)
        ts_insert(params[i].nome, catParametro, params[i].tipo, 0);

    strncpy(subNome, nome, MAX_LEXEMA - 1);
    subNome[MAX_LEXEMA - 1] = '\0';

    strncpy(subTipo, "proc", sizeof(subTipo) - 1);
    blocoSeq = 0; // reinicia contador de blocos para esta sub-rotina

    if (aceitar(sLOCALS))
        parseDeclaracoes(catVariavel);

    parseBloco();

    tsFecharEscopo();
    traceSai("proc");
}

// ini ::= sMODULE id ";" glob? (func | proc)* princ
void parse_program(void) {
    traceEntra("ini");

    esperar(sMODULE);

    if (tokAtual.cat != sIDENTIF)
        diag_error(tokAtual.linha, "identificador", tokAtual.lexema);
    avancar(); // consome nome do módulo

    esperar(sPONTEVIRG);

    if (tokAtual.cat == sGLOBALS)
        parseGlobais();

    while (tokAtual.cat == sFN || tokAtual.cat == sPROC) {
        if (tokAtual.cat == sFN)
            parseFuncao();
        else
            parseProcedimento();
    }

    if (tokAtual.cat != sEOF)
        diag_error(tokAtual.linha, "fim de arquivo", tokAtual.lexema);

    traceSai("ini");
}

// Armazena o estado do léxico e carrega o primeiro token para iniciar a análise
void parserInit(LexEstado *ls) {
    lexer = ls;
    tokAtual = lex_next(lexer); // carrega o primeiro token

    if (logTokensHabilitado())
        logToken(tokAtual.linha, lexCatStr(tokAtual.cat), tokAtual.lexema);
}
