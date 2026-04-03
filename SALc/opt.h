// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef OPT_H
#define OPT_H

// Opções de execução
typedef struct {
    char *arquivoFonte; // caminho para o arquivo .sal
    int gerarTokens;  // --tokens : gera arquivo .tk
    int gerarSymtab;  // --symtab : gera arquivo .ts
    int gerarTrace;   // --trace  : gera arquivo .trc
} Opcoes;

// Interpreta argv e preenche opts; encerra em caso de erro
void opts_parse(int argc, char *argv[], Opcoes *opts);

// retorna ponteiro para a variável global inicializada por opts_parse
Opcoes *opts_get(void);

#endif
