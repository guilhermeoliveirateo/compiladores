// Guilherme Teodoro de Oliveira - 10425362
// Vinícius Brait Lorimier - 10420046

#ifndef PARSER_H
#define PARSER_H

#include "lex.h"

// Armazena o estado do léxico e carrega o primeiro token
void parserInit(LexEstado *ls);

// Ponto de entrada da análise; implementa o não-terminal ini
void parse_program(void);

#endif
