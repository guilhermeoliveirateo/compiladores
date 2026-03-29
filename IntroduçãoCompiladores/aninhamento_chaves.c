/* Usando linguagem C implemente um programa que, dado um arquivo contendo código fonte escrito na linguagem C, 
verifique se as chaves do programa fonte estão aninhadas de forma correta.*/

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