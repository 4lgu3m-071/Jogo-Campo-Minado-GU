#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <raylib.h>

typedef struct{
    bool bomba;
    bool estaAberta;
    bool bandeira;
    int vizinhos;
}Celula;

typedef enum {
    MENU,
    JOGANDO,
    FIM
} EstadoJogo;

/*
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
*/

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

//-------------- tirar essa bomba dps que usar o raylib --------------------
/*

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
*/
//---------------------------------------------------------------------------------------------------------------------------

Celula **expandirTabuleiro(Celula **tabuleiroAntigo, int *linhas, int *colunas, int addLinhas, int addColunas) {
    int novaL = *linhas + addLinhas;
    int novaC = *colunas + addColunas;

    Celula **novo = criarTabuleiro(novaL, novaC);

    // Copiar valores antigos do tabuleiro para o novo tabuleiro
    for (int i = 0; i < *linhas; i++)
        for (int j = 0; j < *colunas; j++)
            novo[i][j] = tabuleiroAntigo[i][j];

    destruirTabuleiro(tabuleiroAntigo, *linhas);

    *linhas = novaL;
    *colunas = novaC;
    return novo;
}

void calcularVizinhosTabuleiro(Celula **t, int linhas, int colunas) {

    for (int linha = 0; linha < linhas; linha++) {
        for (int coluna = 0; coluna < colunas; coluna++) {

            if (t[linha][coluna].bomba) {
                t[linha][coluna].vizinhos = -1; // o menos um é para dizer que é uma bomba tlgd
                continue;
            }

            int NBombas = 0;

            for (int i = linha - 1; i <= linha + 1; i++) {
                for (int j = coluna - 1; j <= coluna + 1; j++) {

                    if (i < 0 || i >= linhas || j < 0 || j >= colunas)
                        continue;

                    if (t[i][j].bomba)
                        NBombas++;
                }
            }

            t[linha][coluna].vizinhos = NBombas;
        }
    }
}

void abrirCelulaRecursiva(int linha, int coluna, Celula **t, int linhas, int colunas, int *celulasAbertas) {

    if (linha < 0 || linha >= linhas || coluna < 0 || coluna >= colunas)
        return;

    if (t[linha][coluna].estaAberta)
        return;

    if (t[linha][coluna].bomba)
        return;

    t[linha][coluna].estaAberta = true;
    (*celulasAbertas)++;

    // EU DO FUTURO VIM AQUI ME LEMBRAR PRA NUNCA ESQUECER DE INCREMENTAR UM PONTEIRO USANDO *VARIAVEL E SIM (*VARIAVEL)!!!!!!!!!!!1

    // IMPORTANTE VER SE CELULAS ABERTAS TA SENDO CONTADO CERTO (PROVAVELEMNTE ESTA)
    
    if (t[linha][coluna].vizinhos > 0)
        return;

    // Se vizinhos == 0, abre ao redor
    for (int i = linha - 1; i <= linha + 1; i++) {
        for (int j = coluna - 1; j <= coluna + 1; j++) {

            if (i == linha && j == coluna)
                continue;

            abrirCelulaRecursiva(i, j, t, linhas, colunas, celulasAbertas);
        }
    }
}

int contarBombas(Celula **t, int linhas, int colunas) {
    int total = 0;
    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            if (t[i][j].bomba) total++;
    return total;
}


int main(void) {

    //config da parte visual
    int offsetX = 50;
    int offsetY = 80;
    #define TAM_CELULA 30

    InitWindow(900, 700, "Campo Minado - Raylib");
    SetTargetFPS(60);
    srand(time(NULL));

    EstadoJogo estado = MENU;

    int difficulty = 0;
    int linhas = 0, colunas = 0;
    Celula **tabuleiro = NULL;

    double tempoInicio = 0;
    double tempoAtual = 0;
    bool cronometroAtivo = false;

    bool venceu = false;
    int bombasTotais = 0;
    int celulasAbertas = 0;
    int metaVitoria = 0;

    while (!WindowShouldClose()) {

        // =====================
        // MENU
        // =====================
        if (estado == MENU) {

            if (IsKeyPressed(KEY_ONE)) difficulty = 1;
            if (IsKeyPressed(KEY_TWO)) difficulty = 2;
            if (IsKeyPressed(KEY_THREE)) difficulty = 3;
            if (IsKeyPressed(KEY_FOUR)) difficulty = 4;

            if (difficulty != 0) {
                
                //sim, pus linhas e colunas iguais duas vezes pois caso eu quisesse adaptar para retangular seria mais facil no futuro
                switch (difficulty){
                    case 1: linhas = colunas = 10;  break;
                    case 2: linhas = colunas = 15;  break;
                    case 3: linhas = colunas = 20;  break;
                    case 4: linhas = colunas = 15; break;
                }
                

                tabuleiro = criarTabuleiro(linhas, colunas);
                
                SortearBombas(tabuleiro, linhas, colunas, difficulty);
                bombasTotais = contarBombas(tabuleiro, linhas, colunas);
                metaVitoria = (linhas * colunas) - bombasTotais;
                calcularVizinhosTabuleiro(tabuleiro, linhas, colunas);


                tempoInicio = 0;
                tempoAtual = 0;
                cronometroAtivo = false;

                estado = JOGANDO;
            }
        }

        // =====================
        // JOGO
        // =====================
        else if (estado == JOGANDO) {

            //comeca o cronometro no primeiro clique
            if (cronometroAtivo == false && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                tempoInicio = GetTime();
                cronometroAtivo = true;
            }
            
            if (difficulty != 4) {
                if (cronometroAtivo == true)
                tempoAtual = GetTime() - tempoInicio;
            }
            else {
                //boss mode tem tempo maximo de 240 segundos e se passar de 200 segundos, o tabuleiro expande hahahah
                if (cronometroAtivo == true) {
                    tempoAtual = 240.0 - (GetTime() - tempoInicio);
                    if (tempoAtual <= 0) {
                        tempoAtual = 0;
                        venceu = false;
                        estado = FIM;
                    //tempoatual guarda o tempo restante do timer
                    }
                }
            }
            

            
            // FALTA!!!!!!!!!!!!
            // - expandir tabuleiro
            // - TESTAR checar vitória / derrota 

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                Vector2 mouse = GetMousePosition();

                int colunaClicada = (mouse.x - offsetX) / TAM_CELULA;
                int linhaClicada  = (mouse.y - offsetY) / TAM_CELULA;

                if (linhaClicada >= 0 && linhaClicada < linhas &&
                    colunaClicada >= 0 && colunaClicada < colunas) {

                    // ABRE A CÉLULA
                    //tabuleiro[linhaClicada][colunaClicada].estaAberta = true; // Acho que sera desnecessario? mentira vou usar para o fim do jogo
                    //nao precisa mais disso aqui pq ja ta na funcao abrirCelulaRecursiva

                    

                    
                    if (tabuleiro[linhaClicada][colunaClicada].estaAberta == true) {
                        //nada acontece se clicar numa celula ja aberta
                    }
                    else{
                        if (tabuleiro[linhaClicada][colunaClicada].bomba == true) {
                            // SE FOR BOMBA, FIM DE JOGO, GAME OVER, NAO SOBRARA NADA   
                            bool venceu = false;
                            estado = FIM;

                        }
                        else {
                            abrirCelulaRecursiva(linhaClicada, colunaClicada, tabuleiro, linhas, colunas, &celulasAbertas);

                            if (celulasAbertas == metaVitoria) {
                                venceu = true;
                                estado = FIM;
                            }

                        }
                    }
                    
                }
            }

        }

        // =====================
        // DESENHO
        // =====================

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (estado == MENU) {
            DrawText("ESCOLHA A DIFICULDADE", 250, 150, 30, BLACK);
            DrawText("1 - Facil", 350, 250, 20, BLACK);
            DrawText("2 - Medio", 350, 290, 20, BLACK);
            DrawText("3 - Dificil", 350, 330, 20, BLACK);
            DrawText("4 - Boss", 350, 370, 20, BLACK);
        }

        else if (estado == JOGANDO) {
            DrawText(TextFormat("Tempo: %.1f", tempoAtual), 20, 20, 20, BLACK);

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {

                    int x = offsetX + j * TAM_CELULA;
                    int y = offsetY + i * TAM_CELULA;

                    // CÉLULA FECHADA
                    if (tabuleiro[i][j].estaAberta == false) {
                        DrawRectangle(x, y, TAM_CELULA, TAM_CELULA, GRAY);
                        DrawRectangleLines(x, y, TAM_CELULA, TAM_CELULA, DARKGRAY);
                        // BOMBA (SÓ PARA TESTE, REMOVER DEPOIS)
                        if (tabuleiro[i][j].bomba) {
                            DrawCircle(x + TAM_CELULA/2, y + TAM_CELULA/2, 8, RED);
                        }
                    }
                    // CÉLULA ABERTA
                    else {
                        DrawRectangle(x, y, TAM_CELULA, TAM_CELULA, RAYWHITE);
                        DrawRectangleLines(x, y, TAM_CELULA, TAM_CELULA, LIGHTGRAY);

                        // BOMBA
                        if (tabuleiro[i][j].bomba) {
                            DrawCircle(x + TAM_CELULA/2, y + TAM_CELULA/2, 8, RED);
                        }
                        // NÚMERO
                        else if (tabuleiro[i][j].vizinhos > 0) {
                            DrawText(
                                TextFormat("%d", tabuleiro[i][j].vizinhos),
                                x + 10, y + 5, 20, BLUE
                            );
                        }
                    }
                }
            }

        }

        else if (estado == FIM) {
            if (venceu == true) {
                DrawText("PARABENS! VOCÊ VENCEU!", 100, 300, 30, GREEN);
                DrawText("Pressione R para reiniciar", 100, 400, 30, GREEN);
            }

            else {
            DrawText("FIM DE JOGO! VOCÊ PERDEU!", 100, 300, 30, RED);
            DrawText("Pressione R para reiniciar", 100, 400, 30, RED);

            }
                if (IsKeyPressed(KEY_R)) {
                    destruirTabuleiro(tabuleiro, linhas);
                    tabuleiro = NULL;
                    difficulty = 0;
                    estado = MENU;
                }
        }

        EndDrawing();
    }

    if (tabuleiro != NULL)
        destruirTabuleiro(tabuleiro, linhas);

    CloseWindow();
    return 0;
}








/*
int verificaCoordenadas(int linha, int coluna) {
    if (linha < 0 || linha >= tamCampo || coluna < 0 || coluna >= tamCampo) return 0; 
    return 1; 
}
*/