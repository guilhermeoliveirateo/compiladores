#include <stdio.h>
#include <stdbool.h>

bool verifica_par_de_as(const char *palavra) {
    int estado = 0;
    int i = 0;
    char caractere;

    while ((caractere = palavra[i]) != '\0') {
        
        switch (estado) {
            case 0:
                if (caractere == 'a')
                    estado = 1;
                else if (caractere == 'b')
                    estado = 0;
                else
                    return false;
                break;
                
            case 1:
                if (caractere == 'a')
                    estado = 0;
                else if (caractere == 'b')
                    estado = 1;
                else
                    return false;
                break;
        }
        i++;
    }

    if (estado == 0) return true;
    else return false;
}

int main() {
    const char *teste1 = "bbababa"; 
    const char *teste2 = "bbabaaba";
    const char *teste3 = "bbbb";

    printf("Teste 1 ('%s'): %s\n", teste1, verifica_par_de_as(teste1) ? "Aceito" : "Rejeitado");
    printf("Teste 2 ('%s'): %s\n", teste2, verifica_par_de_as(teste2) ? "Aceito" : "Rejeitado");
    printf("Teste 3 ('%s'): %s\n", teste3, verifica_par_de_as(teste3) ? "Aceito" : "Rejeitado");

    return 0;
}