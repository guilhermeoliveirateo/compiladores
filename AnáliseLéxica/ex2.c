#include <stdio.h>
#include <stdbool.h>

bool verifica_sem_aa(const char *palavra) {
    
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
                    estado = 2;
                else if (caractere == 'b')
                    estado = 0;
                else
                    return false;
                break;
            
            case 2:
                break;
        }
        i++;
    }

    if (estado == 0 || estado == 1) return true;
    else return false;
}

int main() {
    const char *teste1 = "ababaabbba"; 
    const char *teste2 = "ababababababa";
    const char *teste3 = "bbbb";

    printf("Teste 1 ('%s'): %s\n", teste1, verifica_sem_aa(teste1) ? "Aceito" : "Rejeitado");
    printf("Teste 2 ('%s'): %s\n", teste2, verifica_sem_aa(teste2) ? "Aceito" : "Rejeitado");
    printf("Teste 3 ('%s'): %s\n", teste3, verifica_sem_aa(teste3) ? "Aceito" : "Rejeitado");

    return 0;
}