#include <stdio.h>
#include <string.h>

int main() {
    char *k[] = {
        "asm", "auto", "break", "case", "char", "const", "continue", 
        "default", "do", "double", "else", "enum", "extern", "float", 
        "for", "goto", "if", "int", "long", "register", "return", 
        "short", "signed", "sizeof", "static", "struct", "switch", 
        "typedef", "union", "unsigned", "void", "volatile", "while"
    };
    
    FILE *f = fopen("ex8.c", "r");
    char w[256];
    int c, i = 0, j, m;

    if (f == NULL) {
        return 1;
    }

    do {
        c = fgetc(f);
        
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            w[i++] = c;
        } else {
            w[i] = '\0';
            m = 0;
            
            for (j = 0; j < 33; j++) {
                if (strcmp(w, k[j]) == 0) {
                    m = 1;
                    break;
                }
            }
            
            for (j = 0; j < i; j++) {
                if (m == 1 && w[j] >= 'a' && w[j] <= 'z') {
                    putchar(w[j] - 32);
                } else {
                    putchar(w[j]);
                }
            }
            
            i = 0;
            
            if (c != EOF) {
                putchar(c);
            }
        }
    } while (c != EOF);

    fclose(f);
    
    return 0;
}