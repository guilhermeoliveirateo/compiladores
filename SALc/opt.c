// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opt.h"

static Opcoes opcoesGlobais = {
    NULL, 0, 0, 0
};

// Imprime a mensagem de uso e encerra com código de erro
static void usageERR(void) {
    fprintf(
        stderr,
            "Uso: salc <arquivo.sal> [--tokens] [--symtab] [--trace]\n"
            "\n"
            "  <arquivo.sal>  Arquivo fonte SAL a ser compilado\n"
            "  --tokens       Gera <arquivo>.tk com a lista de tokens\n"
            "  --symtab       Gera <arquivo>.ts com a tabela de simbolos\n"
            "  --trace        Gera <arquivo>.trc com o rastro da analise\n"
    );
    exit(1);
}

// Percorre argv reconhecendo flags e o arquivo fonte e encerra em caso de uso inválido
void opts_parse(int argc, char *argv[], Opcoes *opts) {
    int i;

    opts->arquivoFonte = NULL;
    opts->gerarTokens = 0;
    opts->gerarSymtab = 0;
    opts->gerarTrace = 0;

    if (argc < 2)
        usageERR();

    for (i = 1; i < argc; i++) {
        
        if (strcmp(argv[i], "--tokens") == 0) {
            opts->gerarTokens = 1;
        } else if (strcmp(argv[i], "--symtab") == 0) {
            opts->gerarSymtab = 1;
        } else if (strcmp(argv[i], "--trace") == 0) {
            opts->gerarTrace = 1;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Erro: opcao desconhecida '%s'\n", argv[i]);
            usageERR();
        } else {
            if (opts->arquivoFonte != NULL) {
                fprintf(stderr, "Erro: mais de um arquivo fonte informado.\n");
                usageERR();
            }

            opts->arquivoFonte = argv[i];
        }
    }

    if (opts->arquivoFonte == NULL) {
        fprintf(stderr, "Erro: arquivo fonte nao informado.\n");
        usageERR();
    }

    opcoesGlobais = *opts;
}

// Retorna ponteiro para a variável global preenchida por opts_parse
Opcoes *opts_get(void) {
    return &opcoesGlobais;
}
