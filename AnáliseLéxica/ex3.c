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
                    estado = 5;
                else if (caractere == 'c')
                    estado = 5;
                else
                    return false;
                break;
                
            case 1:
                if (caractere == 'a')
                    estado = 1;
                else if (caractere == 'b')
                    estado = 1;
                else if (caractere == 'c')
                    estado = 2;
                else
                    return false;
                break;
            
            case 2:
                if (caractere == 'a')
                    estado = 3;
                else if (caractere == 'b')
                    estado = 3;
                else if (caractere == 'c')
                    estado = 5;
                else
                    return false;
                break;

            case 3:
                if (caractere == 'a')
                    estado = 5;
                else if (caractere == 'b')
                    estado = 5;
                else if (caractere == 'c')
                    estado = 4;
                else
                    return false;
                break;
            
            case 4:
                if (caractere == 'a')
                    estado = 5;
                else if (caractere == 'b')
                    estado = 5;
                else if (caractere == 'c')
                    estado = 5;
                else
                    return false;
                break; 
            
            case 5:
                break;  
        }
        i++;
    }

    if (estado == 4) return true;
    else return false;
}

int main() {
    const char *teste1 = "abacc"; 
    const char *teste2 = "aabacac";
    const char *teste3 = "abacbc";

    printf("Teste 1 ('%s'): %s\n", teste1, verifica_sem_aa(teste1) ? "Aceito" : "Rejeitado");
    printf("Teste 2 ('%s'): %s\n", teste2, verifica_sem_aa(teste2) ? "Aceito" : "Rejeitado");
    printf("Teste 3 ('%s'): %s\n", teste3, verifica_sem_aa(teste3) ? "Aceito" : "Rejeitado");

    return 0;
}