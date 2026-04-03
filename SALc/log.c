// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

static FILE *arqTokens = NULL;
static FILE *arqSymtab = NULL;
static FILE *arqTrace  = NULL;

static int habTokens = 0;
static int habSymtab = 0;
static int habTrace = 0;

// Substitui a extensão do arquivo fonte pela extensão fornecida
static void construirNomeSaida(const char *nomeBase, const char *ext, char *destino, int tamanho) {
    
    const char *ponto = strrchr(nomeBase, '.'); // busca a última ocorrência de '.' 
    int base = (ponto != NULL) ? (int)(ponto - nomeBase) : (int)strlen(nomeBase); // Se não houver extensão usa o nome inteiro como base

    // Garante que base + nova extensão + '\0' cabem no buffer destino
    if (base >= tamanho - (int)strlen(ext) - 1)
        base = tamanho - (int)strlen(ext) - 1;

    strncpy(destino, nomeBase, (size_t)base);
    destino[base] = '\0'; // termina na posição do ponto para poder concatenar a nova ext
    strncat(destino, ext, (size_t)(tamanho - base - 1));
}

// Abre os arquivos de log habilitados pelas flags e deriva os nomes a partir do fonte
void logInit(const char *nomeBase, int tokens, int symtab, int trace) {
    char caminho[512];

    habTokens = tokens;
    habSymtab = symtab;
    habTrace  = trace;

    if (tokens) {
        construirNomeSaida(nomeBase, ".tk", caminho, sizeof(caminho));
        
        arqTokens = fopen(caminho, "w");
        if (!arqTokens) {
            // Falha ao criar não é fatal apenas desabilita o log e continua
            fprintf(stderr, "Aviso: nao foi possivel criar '%s'\n", caminho);
            habTokens = 0;
        }
    }

    if (symtab) {
        construirNomeSaida(nomeBase, ".ts", caminho, sizeof(caminho));
        
        arqSymtab = fopen(caminho, "w");
        if (!arqSymtab) {
            fprintf(stderr, "Aviso: nao foi possivel criar '%s'\n", caminho);
            habSymtab = 0;
        }
    }

    if (trace) {
        construirNomeSaida(nomeBase, ".trc", caminho, sizeof(caminho));
        
        arqTrace = fopen(caminho, "w");
        if (!arqTrace) {
            fprintf(stderr, "Aviso: nao foi possivel criar '%s'\n", caminho);
            habTrace = 0;
        }
    }
}

// Fecha todos os arquivos de log que estiverem abertos
void logFim(void) {
    if (arqTokens) { 
        fclose(arqTokens); 
        arqTokens = NULL; 
    }

    if (arqSymtab) { 
        fclose(arqSymtab); 
        arqSymtab = NULL; 
    }

    if (arqTrace)  { 
        fclose(arqTrace);  
        arqTrace  = NULL; 
    }
}

// Grava um token no .tk no formato: <linha>  <CATEGORIA>  "<lexema>"
void logToken(int linha, const char *cat, const char *lexema) {
    if (arqTokens)
        fprintf(arqTokens, "%-4d  %-16s  \"%s\"\n", linha, cat, lexema);
}

// Grava uma entrada da TS no .ts no formato: SCOPE=...  id="..."  cat=...  tipo=...  extra=...
void logSimbolo(const char *escopo, const char *lexema, const char *cat, const char *tipo, int extra) {
    if (arqSymtab)
        fprintf(arqSymtab, "SCOPE=%-30s  id=\"%-20s\"  cat=%-14s  tipo=%-6s  extra=%d\n", escopo, lexema, cat, tipo, extra);
}

// Grava uma linha de rastreamento no .trc
void logTrace(const char *mensagem) {
    if (arqTrace)
        fprintf(arqTrace, "%s\n", mensagem);
}

// Funções de consulta que são usadas pelo parser para evitar chamadas desnecessárias
int logTokensHabilitado(void) { 
    return habTokens; 
}
int logSymtabHabilitado(void) { 
    return habSymtab; 
}
int logTraceHabilitado(void) { 
    return habTrace;  
}
