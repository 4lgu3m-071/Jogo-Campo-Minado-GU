#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef struct{
    bool bomba;
    bool estaAberta;
    bool bandeira;
    int vizinhos;
}Celula;

int GerarDificuldade(){
    int dificuldade;
    printf("Escolha a dificuldade:\n");
    printf("1 - Facil (10x10)\n");
    printf("2 - Medio (15x15)\n");
    printf("3 - Dificil (20x20)\n");
    printf("4 - Boss (15+x15+)\n");
    scanf("%d", &dificuldade);
    if (dificuldade < 1 || dificuldade > 4){
        printf("Dificuldade invalida. Escolha novamente.\n");
        return GerarDificuldade();
    }
    return dificuldade;
}

//aqui comeca a putaria
//----------------------------------------------------------------

Celula **criarTabuleiro(int linhas, int colunas) {
    Celula **t = malloc(linhas * sizeof(Celula*));

    for (int i = 0; i < linhas; i++) {
        t[i] = calloc(colunas, sizeof(Celula));
    }

    return t;
}

void destruirTabuleiro(Celula **t, int linhas) {
    for (int i = 0; i < linhas; i++)
        free(t[i]);

    free(t);
}

void SortearBombas(Celula **t, int linhas, int colunas, int difficulty) {

    int bombas;
    switch(difficulty){
        case 1: bombas = (linhas * colunas) / 6; break;
        case 2: bombas = (linhas * colunas) / 5; break;
        case 3: bombas = (linhas * colunas) / 4; break;
        case 4: bombas = (linhas * colunas) / 4; break;
    }

    int x, y;

    for (int i = 0; i < bombas; i++) {
        x = rand() % linhas;
        y = rand() % colunas;

        if (t[x][y].bomba == false)
            t[x][y].bomba = true;
        else
            i--;
    }
}

//-------------- tirar essa merda dps que usar o raylib --------------------


void imprimirTabuleiro(Celula **t, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {

        if(i==0){
            printf("   ");
                for(int k=0; k<colunas; k++){
                printf("X%d ", k);
                }
                printf("\n");
            }

        for (int j = 0; j < colunas; j++) {
            if (j == 0){
                printf("Y%d ", i);
            }

            printf("%d  ", t[i][j].bomba);
        }
        printf("\n");
    }
}

//---------------------------------------------------------------------------------------------------------------------------

Celula **expandirTabuleiro(Celula **tabuleiroAntigo, int *linhas, int *colunas, int addLinhas, int addColunas) {
    int novaL = *linhas + addLinhas;
    int novaC = *colunas + addColunas;

    Celula **novo = criarTabuleiro(novaL, novaC);

    // Copiar valores antigos
    for (int i = 0; i < *linhas; i++)
        for (int j = 0; j < *colunas; j++)
            novo[i][j] = tabuleiroAntigo[i][j];

    destruirTabuleiro(tabuleiroAntigo, *linhas);

    *linhas = novaL;
    *colunas = novaC;
    return novo;
}

int main(){

    system("cls");

    int difficulty;
    difficulty = GerarDificuldade();
    srand(time(NULL));

    int linhas, colunas;

    switch (difficulty){
        case 1: linhas = colunas = 10; break;
        case 2: linhas = colunas = 15; break;
        case 3: linhas = colunas = 20; break;
        case 4: linhas = colunas = 15; break;
    }

    Celula **tabuleiro = criarTabuleiro(linhas, colunas);

    SortearBombas(tabuleiro, linhas, colunas, difficulty);
    
    imprimirTabuleiro(tabuleiro, linhas, colunas);

    if (difficulty == 4){

        tabuleiro = expandirTabuleiro(tabuleiro, &linhas, &colunas, 2, 2);
    }
    

    printf("\nTabuleiro inicial:\n");
    imprimirTabuleiro(tabuleiro, linhas, colunas);

    return 0;
}
