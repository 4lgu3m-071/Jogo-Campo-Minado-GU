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
    bool vizinhoEscondido;
}Celula;

typedef enum {
    SELECAOJOGADOR,
    MENU,
    JOGANDO,
    FIM
} EstadoJogo;

typedef struct {
    int difficulty;
    int linhas;
    int colunas;
    double tempoAtual;
    int celulasAbertas;
    int metaVitoria;
    int hintLinha;
    int hintColuna;
} SaveHeader;

Celula **criarTabuleiro(int linhas, int colunas); //precisei por o >prototipo< (pra que senhor isso existe) aqui pq a funcao de salvar e carregar vem antes da funcao criarTabuleiro


//aqui comeca a parada
//------------------------ARQUIVO----------------------------------------
//parte chata ruim feia

void salvarJogo(Celula **tabuleiro, int linhas, int colunas, int difficulty, float tempoAtual, int celulasAbertas, int metaVitoria, int hintLinha, int hintColuna) {
    FILE *f = fopen("savegame.dat", "wb"); //aqui wb para escrever em binario
    if (!f) return; //caso de erro ao abrir o arquivo

    SaveHeader h = {
        difficulty,
        linhas,
        colunas,
        tempoAtual,
        celulasAbertas,
        metaVitoria,
        hintLinha,
        hintColuna
    };

    fwrite(&h, sizeof(SaveHeader), 1, f);

    for (int i = 0; i < linhas; i++) {
        fwrite(tabuleiro[i], sizeof(Celula), colunas, f);
    }

    fclose(f);
}

bool carregarJogo( //vai ser bool pq pode dar erro ou n ter arquivo etc
    Celula ***tabuleiro,
    int *linhas,
    int *colunas,
    int *difficulty,
    double *tempoAtual,
    int *celulasAbertas,
    int *metaVitoria,
    int *hintLinha,
    int *hintColuna
) {
    FILE *f = fopen("savegame.dat", "rb");
    if (!f) return false; //caso de erro

    SaveHeader h;
    fread(&h, sizeof(SaveHeader), 1, f);

    *difficulty = h.difficulty;
    *linhas = h.linhas;
    *colunas = h.colunas;
    *tempoAtual = h.tempoAtual;
    *celulasAbertas = h.celulasAbertas;
    *metaVitoria = h.metaVitoria;
    *hintLinha = h.hintLinha;
    *hintColuna = h.hintColuna;

    *tabuleiro = criarTabuleiro(*linhas, *colunas);

    for (int i = 0; i < *linhas; i++) {
        fread((*tabuleiro)[i], sizeof(Celula), *colunas, f);
    }

    fclose(f);
    return true;
}


//-----------------------------------------------------------------

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
        case 3: bombas = (linhas * colunas) / 4.7; break;
        case 4: bombas = (linhas * colunas) / 4.7; break;
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

void SortearBombasBoss(Celula **t, int linhas, int colunas, int difficulty) {

    int bombas;
    int tamanhoAntigoL = 15;
    int tamanhoAntigoC = 15;

    bombas = ((linhas - tamanhoAntigoL) * (colunas - tamanhoAntigoC)) / 1.3;
    

    int x, y;

    for (int i = 0; i < bombas; i++) {

        do {
            x = rand() % linhas;
            y = rand() % colunas;
        } while (x < tamanhoAntigoL && y < tamanhoAntigoC); //faz enquanto nao ficar fora do tabuleiro antigo

        if (t[x][y].bomba == false)
            t[x][y].bomba = true;
        else
            i--;
    }
    
}

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

void calcularVizinhosEscondidosBoss(Celula **t, int linhas, int colunas) {

    for (int linha = 0; linha < linhas; linha++) {
        for (int coluna = 0; coluna < colunas; coluna++) {

            if (t[linha][coluna].bomba) {
                t[linha][coluna].vizinhos = -1;
                t[linha][coluna].vizinhoEscondido = false;
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

            // ESCONDE ALGUNS VIZINHOS NO BOSS

            if (NBombas > 0) {
                
                t[linha][coluna].vizinhoEscondido = (rand() % 100 < 5);
            } else {
                t[linha][coluna].vizinhoEscondido = false;
            }
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

void DrawTextoComic(Font fonte, const char *texto, int x, int y, int tamanho, Color cor) {
    DrawTextEx(
        fonte,
        texto,
        (Vector2){ x, y },
        tamanho,
        1.0f,   // espaçamento entre letras
        cor
    );
}

void escolherCasaSegura(Celula **t, int linhas, int colunas, int *hl, int *hc) {
    int l, c;

    do {
        l = rand() % linhas;
        c = rand() % colunas;
    } while (t[l][c].vizinhos != 0 || t[l][c].bomba == true); // o .bomba nem e necessario mas so pra garantir

    *hl = l;
    *hc = c;
}

int main(void) {

    //config da parte visual
    int offsetX = 50;
    int offsetY = 120;
    #define TAM_CELULA 30

    InitWindow(1000, 900, "Campo Minado - Raylib");

    InitAudioDevice();
    Music stereoHearts = LoadMusicStream("Stereohearts.mp3");
    SetMusicVolume(stereoHearts, 0.8f); // volume (0.0 a 1.0)
    bool musicaTocando = false;
    PlayMusicStream(stereoHearts);
    musicaTocando = true;

    Font comicSans = LoadFontEx("comicbd.ttf", 64, NULL, 0);
    SetTextureFilter(comicSans.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(60);
    srand(time(NULL));

    char nomeJogador[21] = "";
    int nomeLen = 0;
    bool nomeConfirmado = false;

    EstadoJogo estado = SELECAOJOGADOR;

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

    int hintLinha = -1;
    int hintColuna = -1;

    bool carregouSave = false;

      

    while (!WindowShouldClose()) {

        if (musicaTocando) {
            UpdateMusicStream(stereoHearts);
        }

        // =====================
        // SELECAO DO NOME DE JOGADOR
        // =====================
        if (estado == SELECAOJOGADOR) {

            // Input do nome do jogador
            if (!nomeConfirmado) {

                int key = GetCharPressed();

                while (key > 0) {

                    // letras, números e espaço
                    if ((key >= 32) && (key <= 125) && nomeLen < 20) {
                        nomeJogador[nomeLen++] = (char)key;
                        nomeJogador[nomeLen] = '\0';
                    }

                    key = GetCharPressed();
                }

                
                if (IsKeyPressed(KEY_BACKSPACE) && nomeLen > 0) {
                    nomeJogador[--nomeLen] = '\0';
                }

                
                if (IsKeyPressed(KEY_ENTER) && nomeLen > 0) {
                    nomeConfirmado = true;
                }
            }
            if (nomeConfirmado) {
                estado = MENU;
            }

        }

        // =====================
        // MENU
        // =====================
        if (estado == MENU) {

            if (IsKeyPressed(KEY_ONE)) difficulty = 1;
            if (IsKeyPressed(KEY_TWO)) difficulty = 2;
            if (IsKeyPressed(KEY_THREE)) difficulty = 3;
            if (IsKeyPressed(KEY_FOUR)) difficulty = 4;
            

            //provavelmente nao e necessario resetar tudo aqui, mas deixa assim por garantia. Precisei resetar celulasAbertas e metavitoria aqui pra n dar ruim na hora de reiniciar o jogo
            if (difficulty != 0) {
            venceu = false;
            bombasTotais = 0;
            celulasAbertas = 0;
            metaVitoria = 0;
            hintColuna = -1;
            hintLinha = -1;
            }
            
            if (IsKeyPressed(KEY_FIVE)){
                difficulty = 0;
                if (carregarJogo(&tabuleiro, &linhas, &colunas, &difficulty, &tempoAtual, &celulasAbertas, &metaVitoria, &hintLinha, &hintColuna)) {
                    
                    cronometroAtivo = true; //precisa disso
                    tempoInicio = GetTime() - tempoAtual; //que parada foi essa aq

                    carregouSave = true;
                    estado = JOGANDO;
                }
            } 


            if (difficulty != 0 && carregouSave == false) {
                
                //sim, pus linhas e colunas iguais duas vezes pois caso eu quisesse adaptar para retangular seria mais facil no futuro
                switch (difficulty){
                    case 1: linhas = colunas = 10;  break;
                    case 2: linhas = colunas = 12;  break;
                    case 3: linhas = colunas = 15;  break;
                    case 4: linhas = colunas = 15; break;
                }
                

                tabuleiro = criarTabuleiro(linhas, colunas);
                SortearBombas(tabuleiro, linhas, colunas, difficulty);
                bombasTotais = contarBombas(tabuleiro, linhas, colunas);
                metaVitoria = (linhas * colunas) - bombasTotais;
                calcularVizinhosTabuleiro(tabuleiro, linhas, colunas);
                escolherCasaSegura(tabuleiro, linhas, colunas, &hintLinha, &hintColuna);

                celulasAbertas = 0;

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

            carregouSave = false;

            if (estado == JOGANDO && IsKeyPressed(KEY_S)) {
            salvarJogo(tabuleiro, linhas, colunas, difficulty, tempoAtual, celulasAbertas, metaVitoria, hintLinha, hintColuna);
            destruirTabuleiro(tabuleiro, linhas);
            tabuleiro = NULL;

            difficulty = 0; //SEM ISSO BUGA HORRIVEL NA HORA DE CARREGAR

            estado = MENU;
            }

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
                //boss mode tem tempo maximo de 300 segundos e se passar de 150 segundos, o tabuleiro expande e deixa alguns vizinhos escondidos hahahah
                if (cronometroAtivo == true) {
                    tempoAtual = 300.0 - (GetTime() - tempoInicio);

                    if (tempoAtual <= 150.0 && (linhas == 15 && colunas == 15)) {
                        tabuleiro = expandirTabuleiro(tabuleiro, &linhas, &colunas, 5, 5);
                        SortearBombasBoss(tabuleiro, linhas, colunas, difficulty);
                        calcularVizinhosEscondidosBoss(tabuleiro, linhas, colunas);

                        metaVitoria = (linhas * colunas) - contarBombas(tabuleiro, linhas, colunas);
                    }
                    if (tempoAtual <= 0) {
                        tempoAtual = 0;
                        venceu = false;
                        estado = FIM;
                    
                    }
                }
            }
            

            
            // FALTA!!!!!!!!!!!!
            // - colocar bandeiras
            
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {

                Vector2 mouse = GetMousePosition();

                int colunaClicada = (mouse.x - offsetX) / TAM_CELULA;
                int linhaClicada  = (mouse.y - offsetY) / TAM_CELULA;

                if (linhaClicada >= 0 && linhaClicada < linhas &&
                    colunaClicada >= 0 && colunaClicada < colunas) {

                    if (tabuleiro[linhaClicada][colunaClicada].estaAberta == false) {
                        tabuleiro[linhaClicada][colunaClicada].bandeira = !tabuleiro[linhaClicada][colunaClicada].bandeira; //alterna
                    }
                }
            }


            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                Vector2 mouse = GetMousePosition();

                int colunaClicada = (mouse.x - offsetX) / TAM_CELULA;
                int linhaClicada  = (mouse.y - offsetY) / TAM_CELULA;

                if (!tabuleiro[linhaClicada][colunaClicada].bandeira){

                    if (linhaClicada >= 0 && linhaClicada < linhas &&
                        colunaClicada >= 0 && colunaClicada < colunas) {

                        // ABRE A CÉLULA
                        //tabuleiro[linhaClicada][colunaClicada].estaAberta = true; // Acho que sera desnecessario? mentira vou usar para o fim do jogo
                        //nao precisa mais disso aqui pq ja ta na funcao abrirCelulaRecursiva

                        

                        
                        if (tabuleiro[linhaClicada][colunaClicada].estaAberta == false){
                            
                            if (tabuleiro[linhaClicada][colunaClicada].bomba == true) {
                                // SE FOR BOMBA, FIM DE JOGO, GAME OVER, NAO SOBRARA NADA   
                                venceu = false;
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

        }


        // =================================================================================================================
        // DESENHO
        // =====================

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (estado == SELECAOJOGADOR) {
            DrawTextoComic(comicSans, "Digite seu nome:", 300, 300, 40, BLACK);
            DrawTextoComic(comicSans, nomeJogador, 300, 360, 40, DARKGRAY);
            DrawTextoComic(comicSans, "Pressione ENTER para confirmar", 300, 420, 30, GRAY);
        }

        /*
        DrawTextoComic(comicSans, "Digite seu nome:", 200, 200, 40, BLACK);
        DrawRectangle(195, 255, 410, 45, LIGHTGRAY);
        DrawTextoComic(comicSans, nomeJogador, 200, 260, 32, BLACK);
        */

        else if (estado == MENU) {
            DrawTextoComic(comicSans, TextFormat("Ola %s!", nomeJogador), 20, 20, 50, DARKPURPLE);
            DrawTextoComic(comicSans,"Escolha a opcao :)", 150, 150, 60, BLACK);
            DrawTextoComic(comicSans,"DIFICULDADES", 350, 210, 40, DARKGRAY);
            DrawTextoComic(comicSans,"1 - Facil", 350, 250, 40, BLACK);
            DrawTextoComic(comicSans,"2 - Medio", 350, 290, 40, BLACK);
            DrawTextoComic(comicSans,"3 - Dificil", 350, 330, 40, BLACK);
            DrawTextoComic(comicSans,"4 - Boss (~muahahaha)", 350, 370, 40, BLACK);
            DrawTextoComic(comicSans,"5 - Carregar Ultimo Salvamento", 350, 480, 40, BLACK);
        }

        else if (estado == JOGANDO) {

            DrawTextoComic(comicSans, TextFormat("Tempo: %.1f", tempoAtual), 20, 20, 50, BLACK);
            //DEBUG CELULAS ABERTAS TALVEZ DEIXAR NO JOGO
            DrawTextoComic(comicSans, TextFormat("Abertas: %d/%d", celulasAbertas, metaVitoria), 20, 60, 32, BLACK);

            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {

                    int x = offsetX + j * TAM_CELULA;
                    int y = offsetY + i * TAM_CELULA;



                    // CÉLULA FECHADA
                    
                    if (tabuleiro[i][j].estaAberta == false) {
                        if (i == hintLinha && j == hintColuna) {
                        DrawRectangle(x, y, TAM_CELULA, TAM_CELULA, GREEN);
                        DrawRectangleLines(x, y, TAM_CELULA, TAM_CELULA, DARKGREEN);
                        }

                        else {
                        DrawRectangle(x, y, TAM_CELULA, TAM_CELULA, GRAY);
                        DrawRectangleLines(x, y, TAM_CELULA, TAM_CELULA, DARKGRAY);
                        }

                        if (tabuleiro[i][j].bandeira) {
                            DrawCircle(
                                x + TAM_CELULA/2,
                                y + TAM_CELULA/2,
                                TAM_CELULA/4,
                                BLUE
                            );
                        }


                        // DEBUG BOMBA (SÓ PARA TESTE, REMOVER DEPOIS)
                        /*if (tabuleiro[i][j].bomba) {
                            DrawCircle(x + TAM_CELULA/2, y + TAM_CELULA/2, 8, RED);
                        }*/
                        
                        
                        
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

                        if (difficulty == 4 && tabuleiro[i][j].vizinhoEscondido == true) {
                            DrawTextoComic(comicSans, "?", x + 8, y + 4, 20, DARKPURPLE);
                        } else if (tabuleiro[i][j].vizinhos > 0) {
                            DrawTextoComic(comicSans, TextFormat("%d", tabuleiro[i][j].vizinhos), x + 8, y + 4, 20, BLUE);
                        }
                    }


                }
            }

            

        }

        else if (estado == FIM) {
            if (venceu == true) {
                DrawTextoComic(comicSans, "PARABENS! VOCE VENCEU!", 100, 300, 50, GREEN);
                DrawTextoComic(comicSans, "Pressione R para reiniciar", 100, 400, 35, GREEN);
            }

            else {
            DrawTextoComic(comicSans, "FIM DE JOGO! VOCE PERDEU!", 100, 300, 50, RED);
            DrawTextoComic(comicSans, "Pressione R para reiniciar", 100, 400, 35, RED);

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

    if (tabuleiro != NULL) destruirTabuleiro(tabuleiro, linhas);

    UnloadMusicStream(stereoHearts);
    CloseAudioDevice();

    UnloadFont(comicSans);
    CloseWindow();
    return 0;
}