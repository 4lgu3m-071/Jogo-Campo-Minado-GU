#include <stdio.h>

int main() {
    // Tamanho do tabuleiro e do navio
    int tamanhoTabuleiro = 10;
    int tamanhoNavio = 3;
    char linha[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    // Declaração do tabuleiro
    int tabuleiro[10][10];  // Tamanho fixo 10x10

    // Inicializando o tabuleiro com 0 (água)
    for (int i = 0; i < tamanhoTabuleiro; i++) {
        for (int j = 0; j < tamanhoTabuleiro; j++) {
            tabuleiro[i][j] = 0;
        }
    }

    // Definindo os navios como vetores
    int navio_horizontal[3] = {3, 3, 3};
    int navio_vertical[3] = {3, 3, 3};

    // Coordenadas iniciais dos navios
    int linha_horizontal = 2;
    int coluna_horizontal = 4;

    int linha_vertical = 5;
    int coluna_vertical = 7;

    //  Posicionamento do navio horizontal
    if (coluna_horizontal + tamanhoNavio <= tamanhoTabuleiro) {
        int sobreposicao = 0;
        for (int i = 0; i < tamanhoNavio; i++) {
            if (tabuleiro[linha_horizontal][coluna_horizontal + i] == 3) 
            {
                sobreposicao = 1;
                break;
            }
        }

        if (!sobreposicao) {
            for (int i = 0; i < tamanhoNavio; i++) 
            {
                tabuleiro[linha_horizontal][coluna_horizontal + i] = navio_horizontal[i];
            }
        } else 
        {
            printf("Erro: Sobreposição detectada no navio horizontal!\n");
        }
    } else 
    {
        printf("Erro: Navio horizontal fora dos limites do tabuleiro!\n");
    }

    //  Posicionamento do navio vertical
    if (linha_vertical + tamanhoNavio <= tamanhoTabuleiro) {
        int sobreposicao = 0;
        for (int i = 0; i < tamanhoNavio; i++) {
            if (tabuleiro[linha_vertical + i][coluna_vertical] == 3) 
            {
                sobreposicao = 1;
                break;
            }
        }

        if (!sobreposicao) {
            for (int i = 0; i < tamanhoNavio; i++) 
            {
                tabuleiro[linha_vertical + i][coluna_vertical] = navio_vertical[i];
            }
        } else 
        {
            printf("Erro: Sobreposição detectada no navio vertical!\n");
        }
    } else 
    {
        printf("Erro: Navio vertical fora dos limites do tabuleiro!\n");
    }

    //  Exibindo o tabuleiro
    printf("\n===== TABULEIRO BATALHA NAVAL =====\n\n");

    // Cabeçalho das colunas
    printf("   ");
    for (int c = 0; c < 10; c++) 
    {
        printf("%c ", linha[c]);
    }
    printf("\n");

    // Linha do tabuleiro
    for (int i = 0; i < tamanhoTabuleiro; i++) 
    {
        printf("%d ", i);
        for (int j = 0; j < tamanhoTabuleiro; j++) 
        {
            printf(" %d", tabuleiro[i][j]);
        }
        printf("\n");
    }

    printf("\n 0 = Água, 3 = Navio\n");

    return 0;
}