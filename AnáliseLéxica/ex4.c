#include <stdio.h>

int main() {
    FILE *entrada = fopen("ex3.c", "r");
    FILE *saida = fopen("ex3_corrigido.c", "w");
    int c;
    int estado = 0;

    if (entrada == NULL || saida == NULL) {
        return 1;
    }

    while ((c = fgetc(entrada)) != EOF) {
        switch (estado) {
            case 0:
                if (c == '/') {
                    estado = 1;
                } else {
                    fputc(c, saida);
                }
                break;
            case 1:
                if (c == '*') {
                    estado = 2;
                } else if (c == '/') {
                    fputc('/', saida);
                    estado = 1;
                } else {
                    fputc('/', saida);
                    fputc(c, saida);
                    estado = 0;
                }
                break;
            case 2:
                if (c == '*') {
                    estado = 3;
                }
                break;
            case 3:
                if (c == '/') {
                    estado = 0;
                } else if (c == '*') {
                    estado = 3;
                } else {
                    estado = 2;
                }
                break;
        }
    }

    if (estado == 1) {
        fputc('/', saida);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}