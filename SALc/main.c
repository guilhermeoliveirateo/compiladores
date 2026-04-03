// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#include <stdio.h>
#include <stdlib.h>
#include "opt.h"
#include "diag.h"
#include "log.h"
#include "lex.h"
#include "symtab.h"
#include "parser.h"

// Orquestra a execução, inicializa módulos, aciona a análise e encerra ordenadamente
int main(int argc, char *argv[]) {

    Opcoes opts;
    FILE *arqFonte = NULL;
    LexEstado *ls = NULL;

    opts_parse(argc, argv, &opts);
    arqFonte = fopen(opts.arquivoFonte, "r");

    if (!arqFonte) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", opts.arquivoFonte);
        return 1;
    }

    diagInit(opts.arquivoFonte);  // registra o nome do fonte para mensagens de erro
    logInit(opts.arquivoFonte, opts.gerarTokens, opts.gerarSymtab, opts.gerarTrace);
    ls = lexInit(arqFonte);       // léxico precisa do arquivo já aberto
    tsInit();                     // abre o escopo global antes do parser

    parserInit(ls);               // carrega o primeiro token ou lookahead para o parser
    parse_program();              // dispara a análise completa

    tsGravarLog();                // grava o .ts apenas após o parse terminar com sucesso

    // Encerra na ordem inversa da inicialização
    tsFim();
    lexFim(ls);
    logFim();
    fclose(arqFonte);

    printf("Compilacao concluida com sucesso!\n");
    return 0;
}
