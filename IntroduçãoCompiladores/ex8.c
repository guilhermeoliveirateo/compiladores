#include <stdio.h>

int main(){

    FILE *arquivo = fopen("ex8.txt", "r");

    int maiusculas = 0;
    int minusculas = 0;
    int digitos = 0;
    int espacos = 0;
    int c;

    if (arquivo == NULL){
        printf("Erro na importação do arquivo");
        return 1;
    }

    while ((c = fgetc(arquivo)) != EOF){
        if (c == ' '){
            espacos++;
        }
        else if (c >= 'a' && c <= 'z'){
            minusculas++;
        }
        else if (c >= 'A' && c <= 'Z'){
            maiusculas++;
        }
        else if (c >= '0' && c <= '9'){
            digitos++;
        }
    }

    if (maiusculas > 0) printf("Maiúsculas: %d\n", maiusculas);
    if (minusculas > 0) printf("Minúsculas: %d\n", minusculas);
    if (digitos > 0) printf("Dígitos: %d\n", digitos);
    if (espacos > 0) printf("Espaços: %d\n", espacos);

    fclose(arquivo);

    return 0;

}