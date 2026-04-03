// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef DIAG_H
#define DIAG_H

// Registra o nome do arquivo fonte para compor as mensagens de erro
void diagInit(const char *nomeArquivo);

// Reporta caractere inesperado e encerra o processo
void diag_error_lex(int linha, char c);

// Reporta erro sintático (esperado x encontrado) e encerra
void diag_error(int linha, const char *esperado, const char *encontrado);

// Reporta mensagem informativa ao log de trace, se habilitado
void diag_info(const char *mensagem);

#endif
