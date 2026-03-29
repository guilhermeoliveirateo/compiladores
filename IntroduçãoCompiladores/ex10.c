#include <stdio.h>

int main() {
    FILE *arquivo = fopen("ex8.c", "r");
    int c;
    int chaves = 0;

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    while ((c = fgetc(arquivo)) != EOF) {
        if (c == '{') {
            chaves++;
        } else if (c == '}') {
            chaves--;
            if (chaves < 0) {
                break;
            }
        }
    }

    if (chaves == 0) {
        printf("Chaves aninhadas corretamente.\n");
    } else {
        printf("Erro no aninhamento das chaves.\n");
    }

    fclose(arquivo);

    return 0;
}