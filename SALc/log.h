// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef LOG_H
#define LOG_H

// Abre os arquivos de log conforme as flags
void logInit(const char *nomeBase, int tokens, int symtab, int trace);

// Fecha todos os arquivos de log abertos
void logFim(void);

// Grava uma linha no arquivo .tk
void logToken(int linha, const char *cat, const char *lexema);

// Grava uma entrada no arquivo .ts
void logSimbolo(const char *escopo, const char *lexema, const char *cat, const char *tipo, int extra);

// Grava uma mensagem no arquivo .trc
void logTrace(const char *mensagem);

int logTokensHabilitado(void);
int logSymtabHabilitado(void);
int logTraceHabilitado(void);

#endif
