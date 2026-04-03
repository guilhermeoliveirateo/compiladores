// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef SYMTAB_H
#define SYMTAB_H

// Categoria do símbolo na tabela
typedef enum {
    catVariavel,      // variável simples ou vetor
    catParametro,     // parâmetro formal de sub-rotina
    catProcedimento,  // procedimento (proc)
    catFuncao         // função (fn)
} CatSimbolo;

// Tipo de dado do símbolo
typedef enum {
    tipoInt,   // int
    tipoBool,  // bool
    tipoChar,  // char
    tipoVoid   // retorno de procedimento ou tipo desconhecido
} TipoSimbolo;

// Inicializa a tabela e abre o escopo global (nível 0)
void tsInit(void);

// Libera toda a memória da tabela
void tsFim(void);

// Empilha novo nível, descricao aparece no log
void tsAbrirEscopo(const char *descricao);

// Desempilha escopo ativo (símbolos são mantidos para o log)
void tsFecharEscopo(void);

// Insere símbolo no escopo corrente; retorna 0 em redeclaração
int ts_insert(const char *lexema, CatSimbolo cat, TipoSimbolo tipo, int extra);

// Percorre da pilha corrente até o global, retorna 1 se encontrado
int ts_lookup(const char *lexema, CatSimbolo *catOut, TipoSimbolo *tipoOut, int *extraOut);

// Grava o log da tabela no arquivo .ts
void tsGravarLog(void);

const char *tsCatStr(CatSimbolo cat);
const char *tsTipoStr(TipoSimbolo tipo);

#endif
