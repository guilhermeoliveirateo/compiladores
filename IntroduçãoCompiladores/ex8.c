#include <stdio.h>

int main(){

    FILE *arquivo = fopen("teste.txt", "r");

    int num_maiusculas = 0;
    int num_minusculas = 0;
    int num_digitos = 0;
    int num_espacos = 0;

    int c = 0;

    if (arquivo == NULL){
        printf("Erro na importação do arquivo");
        return 1;
    }

    while ((c = fgetc(arquivo)) != EOF){
        if (c == " "){
            num_espacos += 1;
        }

        else if (c == "0")
    }

    printf("%d\n", num_linhas);

}