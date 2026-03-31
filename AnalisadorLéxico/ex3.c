#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    SOMA,
    ATRIBUICAO,
    IGUALDADE,
    WHILE,
    EOS
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    float atributo_numero;
    char atributo_ID[16];
} TInfoAtomo;

FILE *fonte;
int linha_atual = 1;

TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    int c;

    while ((c = fgetc(fonte)) == ' ' || c == '\t' || c == '\n' || c == '\r') {
        if (c == '\n') {
            linha_atual++;
        }
    }

    info.linha = linha_atual;

    if (c == EOF) {
        info.atomo = EOS;
        return info;
    }

    if (c == '+') {
        info.atomo = SOMA;
        return info;
    }

    if (c == '=') {
        int proximo = fgetc(fonte);
        if (proximo == '=') {
            info.atomo = IGUALDADE;
        } else {
            ungetc(proximo, fonte);
            info.atomo = ATRIBUICAO;
        }
        return info;
    }

    if (c >= 'a' && c <= 'z') {
        int i = 0;
        info.atributo_ID[i++] = c;
        int estado_id = 1; 
        
        while (1) {
            c = fgetc(fonte);
            
            if (estado_id == 1) {
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
                    if (i < 15) {
                        info.atributo_ID[i++] = c;
                    } else {
                        info.atomo = ERRO;
                        return info;
                    }
                } else if (c == '_') {
                    if (i < 15) {
                        info.atributo_ID[i++] = c;
                    } else {
                        info.atomo = ERRO;
                        return info;
                    }
                    estado_id = 2;
                } else {
                    ungetc(c, fonte);
                    info.atributo_ID[i] = '\0';
                    
                    if (strcmp(info.atributo_ID, "while") == 0) {
                        info.atomo = WHILE;
                        return info;
                    }
                    
                    info.atomo = ERRO;
                    return info;
                }
            } else if (estado_id == 2 || estado_id == 3) {
                if (c >= '0' && c <= '9') {
                    if (i < 15) {
                        info.atributo_ID[i++] = c;
                    } else {
                        info.atomo = ERRO;
                        return info;
                    }
                    estado_id = 3;
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                    info.atomo = ERRO;
                    return info;
                } else {
                    ungetc(c, fonte);
                    info.atributo_ID[i] = '\0';
                    info.atomo = IDENTIFICADOR;
                    return info;
                }
            }
        }
    } else if (c >= '0' && c <= '9') {
        char buf[64];
        int i = 0;
        buf[i++] = c;
        int estado = 1;

        while (1) {
            c = fgetc(fonte);
            if (estado == 1) {
                if (c >= '0' && c <= '9') {
                    buf[i++] = c;
                } else if (c == '.') {
                    buf[i++] = c;
                    estado = 2;
                } else {
                    info.atomo = ERRO;
                    return info;
                }
            } else if (estado == 2) {
                if (c >= '0' && c <= '9') {
                    buf[i++] = c;
                    estado = 3;
                } else {
                    info.atomo = ERRO;
                    return info;
                }
            } else if (estado == 3) {
                if (c >= '0' && c <= '9') {
                    buf[i++] = c;
                } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    info.atomo = ERRO;
                    return info;
                } else {
                    ungetc(c, fonte);
                    buf[i] = '\0';
                    info.atributo_numero = atof(buf);
                    info.atomo = NUMERO;
                    return info;
                }
            }
        }
    }

    info.atomo = ERRO;
    return info;
}

int main() {
    fonte = fopen("teste.txt", "r");
    
    if (fonte == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    while (1) {
        TInfoAtomo token = obter_atomo();
        
        if (token.atomo == EOS) {
            break;
        }
        
        if (token.atomo == ERRO) {
            printf("Erro lexico na linha %d\n", token.linha);
            break;
        }
        
        if (token.atomo == IDENTIFICADOR) {
            printf("%d# IDENTIFICADOR | %s\n", token.linha, token.atributo_ID);
        } else if (token.atomo == NUMERO) {
            printf("%d# NUMERO | %f\n", token.linha, token.atributo_numero);
        } else if (token.atomo == SOMA) {
            printf("%d# SOMA\n", token.linha);
        } else if (token.atomo == ATRIBUICAO) {
            printf("%d# ATRIBUICAO\n", token.linha);
        } else if (token.atomo == IGUALDADE) {
            printf("%d# IGUALDADE\n", token.linha);
        } else if (token.atomo == WHILE) {
            printf("%d# WHILE\n", token.linha);
        }
    }

    fclose(fonte);
    return 0;
}