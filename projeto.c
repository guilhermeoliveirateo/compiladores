/*
 * Nome: Luis Henrique Ribeiro Fernandes - RA 10420079
 * Nome: Vinícius Brait Lorimier – RA 10420046
 * Nome: Guilherme Teodoro de Oliveira – RA 10425362
 * Nome: Raphael Grizante da Silva - RA 10416979
 * Nome: Vitor Tibães Santos - RA 10418976
 */

/* 
 * REPRESENTACAO DE GRAFOS - Versao 2026/1
 */

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<memory.h>

/* 
 * Estrutura de dados para representar grafos
 */
typedef struct a{ /* Celula de uma lista de arestas */
	int    vizinho;
	struct a *prox;
}Aresta;

typedef struct v{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
	int nome;
	Aresta *prim;
}Vertice;

/*
 * Estrutura auxiliar usada para representar uma aresta de corte (ponte)
 * atraves de seus dois extremos. Armazenar as pontes encontradas em um
 * vetor desta estrutura permite que a funcao identificaArestasCorte
 * retorne os resultados sem realizar nenhuma impressao internamente,
 * deixando a formatacao da saida a cargo da main.
 */
typedef struct p{
    int v1;
    int v2;
} Ponte;

/*
 * Declaracoes das funcoes para manipulacao de grafos 
 */
void criaGrafo(Vertice **G, int ordem);
void destroiGrafo(Vertice **G, int ordem);
int  acrescentaAresta(Vertice G[], int ordem, int v1, int v2);
void imprimeGrafo(Vertice G[], int ordem);
 

/*
 * Criacao de um grafo com ordem predefinida (passada como argumento),
 *   e, inicilamente, sem nenhuma aresta 
 */
void criaGrafo(Vertice **G, int ordem){
	int i;
	*G= (Vertice*) malloc(sizeof(Vertice)*ordem); /* Alcacao dinamica de um vetor de vertices */
	
	for(i=0; i<ordem; i++){
		(*G)[i].nome= i;
		(*G)[i].prim= NULL;    /* Cada vertice sem nenhuma aresta incidente */
	}
}

/*
 * Deasaloca a memoria dinamica usada para armazenar um grafo.
 */
void destroiGrafo(Vertice **G, int ordem){
	int i;
    Aresta *a, *n;
    
	for(i=0; i<ordem; i++){ /* Remove lista de adjacencia de cada vertice */
	    a= (*G)[i].prim;
        while (a!= NULL){
              n= a->prox;
              free(a);
              a= n;
        }
	}
    free(*G);  /* Remove o vetor de vertices */
}

/*  
 * Acrescenta uma nova aresta em um grafo previamente criado. 
 *   Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada 
 * Como o grafo nao e orientado, para uma aresta com extremos i e j, quando
 *   i != j, serao criadas, na estrutura de dados, arestas (i,j) e (j,i) .
 */
int acrescentaAresta(Vertice G[], int ordem, int v1, int v2){
    Aresta *A1, *A2;
    
	if (v1<0 || v1 >= ordem) /* Testo se vertices sao validos */
	   return 0;
	if (v2<0 || v2 >= ordem)
	   return 0;
	
	/* Acrescento aresta na lista do vertice v1 */
	A1= (Aresta *) malloc(sizeof(Aresta));
	A1->vizinho= v2;
	A1->prox= G[v1].prim;
	G[v1].prim= A1;

	if (v1 == v2) return 1; /* Aresta e um laco */

	/* Acrescento aresta na lista do vertice v2 se v2 != v1 */	
	A2= (Aresta *) malloc(sizeof(Aresta));
	A2->vizinho= v1;
	A2->prox= G[v2].prim;
	G[v2].prim= A2;
	
	return 1;
}


/*  
 * Imprime um grafo com uma notacao similar a uma lista de adjacencia.
 */
void imprimeGrafo(Vertice G[], int ordem){
	int i;
	Aresta *aux;

	printf("\nOrdem:   %d",ordem);
	printf("\nLista de Adjacencia:\n");

	for (i=0; i<ordem; i++){
		printf("\n    v%d: ", i);
		aux= G[i].prim;
		for( ; aux != NULL; aux= aux->prox)
			printf("  v%d", aux->vizinho);
	}
	printf("\n\n");

}

/* 
 * Retorna 1 se o grafo for conexo, ou 0 caso contrário.
 */
int eConexo(Vertice G[], int ordemG) {
    int i, mudou;
    Aresta *aux;
    
    /* aloca vetor de marcação e inicializa */
    int *marcado = (int *) malloc(sizeof(int) * ordemG);
    if (marcado == NULL) return 0; /* Falha na alocação */

    for (i = 0; i < ordemG; i++) {
        marcado[i] = 0;
    }

    /* escolha o vértice v0 e marque-o com 1  */
    if (ordemG > 0) {
        marcado[0] = 1;
    }

    /* loop até que nenhum novo vértice seja marcado */
    do {
        mudou = 0;
        for (i = 0; i < ordemG; i++) {
            /* se encontrar um vértice vi marcado com 1 */
            if (marcado[i] == 1) {
                aux = G[i].prim;
                while (aux != NULL) {
                    /* se houver uma aresta para um vj marcado com 0 */
                    if (marcado[aux->vizinho] == 0) {
                        marcado[aux->vizinho] = 1; /* marque vj com 1 */
                        mudou = 1; /* sinaliza que houve alteração */
                    }
                    aux = aux->prox;
                }
            }
        }
    } while (mudou);

    /* 4. Verifica se todos os vértices estão marcados com 1 */
    for (i = 0; i < ordemG; i++) {
        if (marcado[i] == 0) {
            free(marcado);
            return 0; /* Não é conexo */
        }
    }

    free(marcado);
    return 1; /* É conexo */
}

/*
 * Conta a quantidade de componentes conexas no grafo, usando o mesmo
 * algoritmo de marcacao do eConexo, mas reiniciando a marcacao a cada
 * vertice ainda nao alcancado (cada reinicio = uma nova componente).
 *
 * Parametro vIgnorado: indice de um vertice que deve ser tratado como
 * inexistente durante a contagem (util para detectar vertices de corte).
 * Para contar as componentes do grafo original basta passar -1 (ou
 * qualquer valor fora do intervalo [0, ordemG-1]).
 *
 * Retorna o numero de componentes conexas encontradas.
 */
int contaComponentesSemVertice(Vertice G[], int ordemG, int vIgnorado) {
    int i, j, mudou, numComp = 0;
    Aresta *aux;
    
    /* Vetor auxiliar para controlar quais vértices já foram visitados */
    int *marcado = (int *) malloc(sizeof(int) * ordemG);
    if (marcado == NULL) return 0;

    for (i = 0; i < ordemG; i++) marcado[i] = 0;

    for (i = 0; i < ordemG; i++) {
        /* Se o vértice não foi marcado e não é o que queremos ignorar,
           encontramos o início de um novo componente conexo */
        if (i != vIgnorado && marcado[i] == 0) {
            numComp++;
            marcado[i] = 1;

            /* Loop que marca todos os vizinhos alcançáveis a partir deste vértice inicial */
            do {
                mudou = 0;
                for (j = 0; j < ordemG; j++) {
                    /* Se o vértice j já foi alcançado, expandimos para os vizinhos dele */
                    if (marcado[j] == 1 && j != vIgnorado) {
                        aux = G[j].prim;
                        while (aux != NULL) {
                            /* Marca o vizinho se ele ainda estiver "limpo" e não for o ignorado */
                            if (aux->vizinho != vIgnorado && marcado[aux->vizinho] == 0) {
                                marcado[aux->vizinho] = 1;
                                mudou = 1; 
                            }
                            aux = aux->prox;
                        }
                    }
                }
            } while (mudou);
        }
    }

    free(marcado);
    return numComp;
}

/*
 * Identifica vertices de corte comparando o numero de componentes
 * antes e depois da "remocao" de cada vertice. Um vertice e de corte
 * se ao ser removido o numero de componentes do grafo aumenta.
 *
 * O vetor cortes[] deve ser previamente alocado pelo chamador com
 * tamanho ordemG e, ao final, conterá 1 nas posicoes correspondentes
 * a vertices de corte e 0 nas demais.
 */
void identificaCortes(Vertice G[], int ordemG, int cortes[]) {
    int i;
    int componentesOriginais;
    int componentesDepois;
    /* Calcula quantas partes o grafo tem originalmente */
    componentesOriginais = contaComponentesSemVertice(G, ordemG, -1);

    for (i = 0; i < ordemG; i++) {
        /* Calcula componentes simulando que o vértice i foi retirado */
        componentesDepois = contaComponentesSemVertice(G, ordemG, i);

        /* Se o número de componentes aumentou, i é um vertice de corte */
        if (componentesDepois > componentesOriginais) {
            cortes[i] = 1;
        } else {
            cortes[i] = 0;
        }
    }
}

/*
 * Conta as componentes do grafo ignorando a aresta entre v1Proibido e
 * v2Proibido (em ambos os sentidos, ja que o grafo nao e orientado).
 * Funciona como auxiliar de identificaArestasCorte: se remover uma
 * aresta aumenta o numero de componentes, ela e uma ponte.
 */
int contaComponentesSemAresta(Vertice G[], int ordemG, int v1Proibido, int v2Proibido) {
    int i, j, mudou, numComponentes = 0;
    int eArestaBloqueada;
    Aresta *aux;
    int *marcado = (int *) malloc(sizeof(int) * ordemG);

    for (i = 0; i < ordemG; i++) marcado[i] = 0;

    for (i = 0; i < ordemG; i++) {
        if (marcado[i] == 0) {
            numComponentes++;
            marcado[i] = 1;
            do {
                mudou = 0;
                for (j = 0; j < ordemG; j++) {
                    if (marcado[j] == 1) {
                        aux = G[j].prim;
                        while (aux != NULL) {
                            /* Verifica se a aresta atual e a que deve ser ignorada */
                            eArestaBloqueada = (j == v1Proibido && aux->vizinho == v2Proibido) ||
                                               (j == v2Proibido && aux->vizinho == v1Proibido);

                            if (!eArestaBloqueada && marcado[aux->vizinho] == 0) {
                                marcado[aux->vizinho] = 1;
                                mudou = 1;
                            }
                            aux = aux->prox;
                        }
                    }
                }
            } while (mudou);
        }
    }
    free(marcado);
    return numComponentes;
}

/*
 * Identifica as arestas de corte (pontes) do grafo e as armazena no
 * vetor listaPontes, que deve ser previamente alocado pelo chamador
 * com capacidade suficiente (no pior caso, igual ao numero de arestas
 * do grafo). Cada aresta nao-orientada e testada uma unica vez graças
 * a condicao i < aux->vizinho.
 *
 * Retorna a quantidade de pontes encontradas (e portanto o numero de
 * posicoes validas preenchidas em listaPontes).
 */
int identificaArestasCorte(Vertice G[], int ordemG, Ponte listaPontes[]) {
    int i, totalPontes = 0;
    int componentesOriginais;
    int componentesDepois;
    Aresta *aux;
    componentesOriginais = contaComponentesSemVertice(G, ordemG, -1);

    for (i = 0; i < ordemG; i++) {
        aux = G[i].prim;
        while (aux != NULL) {
            /* Testa cada aresta apenas uma vez. Lacos (i == aux->vizinho)
               sao ignorados aqui pois nunca podem ser arestas de corte. */
            if (i < aux->vizinho) {
                componentesDepois = contaComponentesSemAresta(G, ordemG, i, aux->vizinho);

                /* Se o numero de componentes aumentou, a aresta e uma ponte */
                if (componentesDepois > componentesOriginais) {
                    listaPontes[totalPontes].v1 = i;
                    listaPontes[totalPontes].v2 = aux->vizinho;
                    totalPontes++;
                }
            }
            aux = aux->prox;
        }
    }
    return totalPontes;
}

/*
 * Programa simples para testar a representacao de grafo
 */
int main(int argc, char *argv[]) {
    int i;
    int ordemG = 10; /* Vertices identificado de 0 ate 9 */
    int numComponentes;
    int qtePontes;
    int *resultados;
    Ponte *listaResultados;
    Vertice *G;

    criaGrafo(&G, ordemG);
    acrescentaAresta(G, ordemG, 0, 1);
    acrescentaAresta(G, ordemG, 0, 2);
    acrescentaAresta(G, ordemG, 0, 7);
    acrescentaAresta(G, ordemG, 2, 4);
    acrescentaAresta(G, ordemG, 2, 2);
    acrescentaAresta(G, ordemG, 2, 5);
    acrescentaAresta(G, ordemG, 3, 5);
    acrescentaAresta(G, ordemG, 4, 6);
    acrescentaAresta(G, ordemG, 3, 6);
    acrescentaAresta(G, ordemG, 7, 7);
    acrescentaAresta(G, ordemG, 8, 9);

    imprimeGrafo(G, ordemG);

    /* Quantidade de componentes conexas do grafo */
    numComponentes = contaComponentesSemVertice(G, ordemG, -1);
    printf("Componentes conexas: %d\n", numComponentes);

    /* Vertices de corte */
    resultados = (int *) malloc(sizeof(int) * ordemG);
    identificaCortes(G, ordemG, resultados);

    printf("Vertices de corte: ");
    for (i = 0; i < ordemG; i++) {
        if (resultados[i] == 1) {
            printf("v%d ", i);
        }
    }
    printf("\n");
    free(resultados);

    /* Arestas de corte. No pior caso, o numero de pontes e limitado
       pelo numero de arestas (ordemG * (ordemG - 1) / 2 e suficiente). */
    listaResultados = (Ponte *) malloc(sizeof(Ponte) * (ordemG * (ordemG - 1) / 2 + 1));
    qtePontes = identificaArestasCorte(G, ordemG, listaResultados);

    printf("Arestas de corte: ");
    for (i = 0; i < qtePontes; i++) {
        printf("(v%d, v%d) ", listaResultados[i].v1, listaResultados[i].v2);
    }
    printf("\n");
    free(listaResultados);

    destroiGrafo(&G, ordemG);

	printf("Pressione uma tecla para terminar\n");
    getchar();
	return(0);
}
