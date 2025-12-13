#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

#define CIMA 72
#define BAIXO 80
#define ENTER 13


void novo_jogo(){
    
	system("cls");
    printf("    INICIANDO NOVO JOGO    \n");
    printf("       \n");    
    printf("c�digo principal aqui  \n");
    system("pause");
    

}

void ultimo_jogo(){
	system("cls");
    printf("    CARREGANDO �LTIMO JOGO    \n");	
    printf("       \n");
    printf("    c�digo com base �ltimo save   \n");
    system ("pause");
   
}

void mostrar_highscore(){
	system("cls");
    printf("    HIGHSCORE ATUAL    \n");
    printf("       \n");
	printf("    c�digo da maior pontua��o    \n");
    system ("pause");

}

void fechar_jogo(){
	system("cls");
    printf("    ENCERRANDO JOGO    \n");	
    exit(0);
}


int main() {

    setlocale(LC_ALL, "portuguese_Brazil"); 
    int escolha = 0;
    bool confirma = false;
    char *opcoes[] = {
        "Iniciar novo jogo",
        "Carregar �ltimo jogo",
        "HIGHSCORE",
        "Sair"
    };
    
    int total = 4;
    int tecla;

    while (confirma == false) {

        system("cls");
        printf(" CAMPO MINADO ULTIMATE\n\n");
        printf(" ------MENU------- \n\n");

        for (int i = 0; i < total; i++) {
            if (i == escolha)
                printf("  -> %s\n", opcoes[i]);
            else
                printf("     %s\n", opcoes[i]);
        }

        tecla = getch();

        if (tecla == 224) {
            tecla = getch();

            if (tecla == CIMA) {
                escolha--;
                if (escolha < 0)
                    escolha = total - 1;
            }
            else if (tecla == BAIXO) {
                escolha++;
                if (escolha >= total)
                    escolha = 0;
            }
        }

        else if (tecla == ENTER) {
            
            switch(escolha) {
            	
            	case(0): 
            	novo_jogo();
            	break;
            	
            	case(1):
            	ultimo_jogo();
            	break;

            	case(2):
            	mostrar_highscore();
                break;
    	
            	case(3):
            	fechar_jogo();
                break;
        	
			}
        }
    }

    return 0;
}