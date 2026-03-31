#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    ERRO,
    ID,
    NUMDEC,
    NUMHEXAD,
    NUMOCTAL,
    EOS
} TAtomo;

bool eh_id(const char *lexema) {
    int len = strlen(lexema);
    int idx_sublinhado = -1;
    
    for (int i = 0; i < len; i++) {
        if (lexema[i] == '_') {
            if (idx_sublinhado != -1) return false;
            idx_sublinhado = i;
        } else if (!isalnum(lexema[i])) {
            return false;
        }
    }
    return (idx_sublinhado > 0 && idx_sublinhado < len - 1);
}

bool eh_numdec(const char *lexema) {
    if (lexema[0] == '\0') return false;
    for (int i = 0; lexema[i] != '\0'; i++) {
        if (!isdigit(lexema[i])) return false;
    }
    return true;
}

bool eh_numhexad(const char *lexema) {
    if (lexema[0] != '0' || lexema[1] != 'x' || lexema[2] == '\0') return false;
    for (int i = 2; lexema[i] != '\0'; i++) {
        char c = lexema[i];
        if (!isdigit(c) && !(c >= 'A' && c <= 'E')) return false;
    }
    return true;
}

bool eh_numoctal(const char *lexema) {
    if (lexema[0] != '0' || lexema[1] != 'b' || lexema[2] == '\0') return false;
    for (int i = 2; lexema[i] != '\0'; i++) {
        if (lexema[i] != '0' && lexema[i] != '1') return false;
    }
    return true;
}

TAtomo analisar_proximo_atomo(const char **fonte, char *lexema_saida) {
    const char *s = *fonte;
    
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') {
        s++;
    }
    
    if (*s == '\0') {
        *fonte = s;
        lexema_saida[0] = '\0';
        return EOS;
    }
    
    int i = 0;
    while (*s != ' ' && *s != '\t' && *s != '\n' && *s != '\r' && *s != '\0') {
        lexema_saida[i++] = *s++;
    }
    lexema_saida[i] = '\0';
    
    *fonte = s;
    
    if (eh_numdec(lexema_saida)) return NUMDEC;
    if (eh_numhexad(lexema_saida)) return NUMHEXAD;
    if (eh_numoctal(lexema_saida)) return NUMOCTAL;
    if (eh_id(lexema_saida)) return ID;
    
    return ERRO;
}

int main() {
    const char *codigo = "var_1 123 0x1A \n \t 0b1010  0b_1  0x1F erro_ var__2";
    const char *ponteiro = codigo;
    char lexema[64];
    
    while (1) {
        TAtomo token = analisar_proximo_atomo(&ponteiro, lexema);
        
        if (token == EOS) break;
        
        switch (token) {
            case ID:       printf("ID       | %s\n", lexema); break;
            case NUMDEC:   printf("NUMDEC   | %s\n", lexema); break;
            case NUMHEXAD: printf("NUMHEXAD | %s\n", lexema); break;
            case NUMOCTAL: printf("NUMOCTAL | %s\n", lexema); break;
            case ERRO:     printf("ERRO     | %s\n", lexema); break;
            default: break;
        }
    }

    return 0;
}