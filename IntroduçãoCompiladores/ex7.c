#include <stdio.h>

int main(){

    FILE *arquivo = fopen("ex7.txt", "r");

    int num_linhas = 1;
    int c = 0;

    if (arquivo == NULL){
        printf("Erro na importação do arquivo");
        return 1;
    }

    while ((c = fgetc(arquivo)) != EOF){
        if (c == '\n'){
            num_linhas++;
        }
    }

    printf("%d\n", num_linhas);

    fclose(arquivo);

    return 0;

}
