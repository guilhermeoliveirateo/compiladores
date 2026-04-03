// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag.h"
#include "log.h"

static char nomeArq[512] = "<desconhecido>";

// Registra o nome do arquivo fonte para composição das mensagens de erro
void diagInit(const char *nomeArquivo) {
    strncpy(nomeArq, nomeArquivo, sizeof(nomeArq) - 1);
    nomeArq[sizeof(nomeArq) - 1] = '\0';
}

// Reporta caractere inesperado no fonte e encerra o processo
void diag_error_lex(int linha, char c) {
    fprintf(stderr, "[ERRO LEXICO] %s:%d: caractere inesperado '%c' (0x%02X)\n", nomeArq, linha, c, (unsigned char)c);
    exit(1);
}

// Reporta erro sintático (token esperado x encontrado) e encerra o processo
void diag_error(int linha, const char *esperado, const char *encontrado) {
    fprintf(stderr, "[ERRO SINTATICO] %s:%d: esperado '%s', encontrado '%s'\n", nomeArq, linha, esperado, encontrado);
    exit(1);
}

// Encaminha mensagem informativa ao log de trace (entrada/saída de não-terminais)
void diag_info(const char *mensagem) {
    logTrace(mensagem);
}
