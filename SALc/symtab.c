// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "lex.h"
#include "log.h"

typedef struct Simbolo {
    char lexema[MAX_LEXEMA];
    CatSimbolo  cat;
    TipoSimbolo tipo;
    int extra;       // tamanho (vetor) ou número de params (sub-rotina)
    struct Simbolo *prox;
} Simbolo;

typedef struct Escopo {
    char descricao[MAX_LEXEMA + 32];
    int nivel;
    Simbolo *primSimbolo;
    Simbolo *ultSimbolo;
    struct Escopo *pai;      // ponteiro de pilha, escopo envolvente para lookup
    struct Escopo *proxLog;  // ponteiro de lista linear, ordem de criação para o log
} Escopo;

// Dois encadeamentos independentes, pai (pilha de lookup) e proxLog (lista para o log)
static Escopo *listaEscopos = NULL; // cabeça da lista de log (todos os escopos criados)
static Escopo *ultimoEscopo = NULL; // cauda da lista de log (para append O(1))
static Escopo *escopoAtual = NULL; // topo da pilha de escopos ativos

// Aloca um novo escopo, encadeia na lista de log e define escopoAtual como pai
static Escopo *criarEscopo(const char *descricao, int nivel) {

    Escopo *e = (Escopo *)malloc(sizeof(Escopo));
    if (!e) {
        fprintf(stderr, "Erro fatal: falha ao alocar escopo.\n");
        exit(1);
    }

    strncpy(e->descricao, descricao, (MAX_LEXEMA + 32) - 1);
    e->descricao[sizeof(e->descricao) - 1] = '\0';

    e->nivel = nivel;
    e->primSimbolo = NULL;

    e->ultSimbolo  = NULL;
    e->pai = escopoAtual; // preserva o escopo pai para lookup encadeado
    
    e->proxLog = NULL;

    // Encadeia ao final da lista de log para preservar a ordem de criação
    if (listaEscopos == NULL) {
        listaEscopos = e;
        ultimoEscopo = e;
    } else {
        ultimoEscopo->proxLog = e;
        ultimoEscopo = e;
    }

    return e;
}

// Inicializa a tabela e abre automaticamente o escopo global (nível 0)
void tsInit(void) {
    listaEscopos = NULL;
    ultimoEscopo = NULL;
    escopoAtual = NULL;
    tsAbrirEscopo("global");
}

// Libera toda a memória da tabela (todos os escopos e símbolos)
void tsFim(void) {
    Escopo *e = listaEscopos;

    while (e != NULL) {
        Escopo  *proxE = e->proxLog;
        Simbolo *s     = e->primSimbolo;

        while (s != NULL) {
            Simbolo *proxS = s->prox;
            free(s);
            s = proxS;
        }

        free(e);
        e = proxE;
    }

    listaEscopos = NULL;
    ultimoEscopo = NULL;
    escopoAtual  = NULL;
}

// Empilha um novo escopo, o nível é pai+1 para refletir o aninhamento de blocos
void tsAbrirEscopo(const char *descricao) {
    int nivel = (escopoAtual == NULL) ? 0 : escopoAtual->nivel + 1;
    escopoAtual = criarEscopo(descricao, nivel);
}

// Desempilha o escopo ativo, símbolos são mantidos em memória para o log final
void tsFecharEscopo(void) {
    if (escopoAtual != NULL)
        escopoAtual = escopoAtual->pai;
}

// Insere símbolo no escopo corrente, retorna 0 se já existir no mesmo escopo
int ts_insert(const char *lexema, CatSimbolo cat, TipoSimbolo tipo, int extra) {
    Simbolo *s;

    if (escopoAtual == NULL) 
        return 0;

    // Verifica redeclaração no mesmo escopo
    for (s = escopoAtual->primSimbolo; s != NULL; s = s->prox) {
        if (strcmp(s->lexema, lexema) == 0)
            return 0;
    }

    s = (Simbolo *)malloc(sizeof(Simbolo));
    if (!s) {
        fprintf(stderr, "Erro fatal: falha ao alocar simbolo.\n");
        exit(1);
    }

    strncpy(s->lexema, lexema, MAX_LEXEMA - 1);
    s->lexema[MAX_LEXEMA - 1] = '\0';

    s->cat   = cat;
    s->tipo  = tipo;

    s->extra = extra;
    s->prox  = NULL;

    // Insere no final para preservar ordem de declaração
    if (escopoAtual->primSimbolo == NULL) {
        escopoAtual->primSimbolo = s;
        escopoAtual->ultSimbolo  = s;
    } else {
        escopoAtual->ultSimbolo->prox = s;
        escopoAtual->ultSimbolo       = s;
    }

    return 1;
}

// Busca símbolo do escopo corrente até o global; retorna 1 se encontrado
int ts_lookup(const char *lexema, CatSimbolo *catOut, TipoSimbolo *tipoOut, int *extraOut) {
    
    Escopo  *e;
    Simbolo *s;

    // Percorre a cadeia de escopos (local → global) simulando regras de visibilidade
    for (e = escopoAtual; e != NULL; e = e->pai) {
        for (s = e->primSimbolo; s != NULL; s = s->prox) {
            if (strcmp(s->lexema, lexema) == 0) {
                // Preenche apenas os ponteiros não-nulos para uso flexível pelo chamador
                if (catOut)
                    *catOut = s->cat;

                if (tipoOut)  
                    *tipoOut = s->tipo;

                if (extraOut) 
                    *extraOut = s->extra;

                return 1;
            }
        }
    }

    return 0; // não encontrado em nenhum escopo visível
}

// Despeja toda a tabela no .ts em ordem de criação de escopos
void tsGravarLog(void) {
   
    Escopo  *e;
    Simbolo *s;

    if (!logSymtabHabilitado()) return;

    // Itera pela lista linear (proxLog)
    for (e = listaEscopos; e != NULL; e = e->proxLog) {
        for (s = e->primSimbolo; s != NULL; s = s->prox) {
            logSimbolo(e->descricao, s->lexema, tsCatStr(s->cat), tsTipoStr(s->tipo), s->extra);
        }
    }
}

// Converte categoria de símbolo para string legível (usado no log .ts)
const char *tsCatStr(CatSimbolo cat) {
    switch (cat) {
        case catVariavel:      
            return "variavel";
        case catParametro:     
            return "parametro";
        case catProcedimento:  
            return "proc";
        case catFuncao:        
            return "funcao";
        default:               
            return "desconhecido";
    }
}

// Converte tipo de dado para string legível (usado no log .ts)
const char *tsTipoStr(TipoSimbolo tipo) {
    switch (tipo) {
        case tipoInt:  
            return "int";
        case tipoBool: 
            return "bool";
        case tipoChar: 
            return "char";
        case tipoVoid: 
            return "void";
        default:       
            return "?";
    }
}
