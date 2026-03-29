/* Implemente um programa em C que leia o conteúdo de um arquivo texto e contabilize a quantidade de linhas que há nele. 
Ao final, o programa deve imprimir na tela a quantidade total de linhas do arquivo.*/

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
