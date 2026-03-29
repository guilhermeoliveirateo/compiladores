/* Crie um programa na linguagem C capaz de ler um arquivo texto contendo código fonte também escrito em C, 
contando o quantidade de ocorrências das: letras maiúsculas (A...Z), letras minúsculas (a..z), dígitos (0...9) e espaços em branco.
Ao final, o programa deve apresentar as quantidades em cada categoria na tela. Obs: Não é necessário contar caracteres 
especiais ou acentuados. Caso não haja qualquer ocorrência de um determinado grupo, não é necessário imprimir a quantidade 0.*/

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