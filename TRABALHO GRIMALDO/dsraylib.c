#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Estrutura do seu código
typedef struct {
    bool bomba;
    bool estaAberta;
    int vizinhos;
} Celula;

// Variáveis globais do jogo (baseadas no seu código)
int difficulty;
int linhas, colunas;
Celula **tabuleiro;
int bombasRestantes;
bool jogoAtivo = true;
bool vitoria = false;
bool primeiraJogada = true;

// Configurações visuais
#define LARGURA_CELULA 40
#define ALTURA_CELULA 40
#define MARGEM 2

// SUAS FUNÇÕES IMPLEMENTADAS -------------------------------------------------

int GerarDificuldade() {
    int dificuldade;
    printf("Escolha a dificuldade:\n");
    printf("1 - Facil (10x10)\n");
    printf("2 - Medio (15x15)\n");
    printf("3 - Dificil (20x20)\n");
    scanf("%d", &dificuldade);
    return dificuldade;
}

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
    switch(difficulty) {
        case 1: bombas = (linhas * colunas) / 6; break;
        case 2: bombas = (linhas * colunas) / 5; break;
        case 3: bombas = (linhas * colunas) / 4; break;
        default: bombas = (linhas * colunas) / 6; break;
    }
    
    bombasRestantes = bombas;
    
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

// NOVAS FUNÇÕES NECESSÁRIAS PARA O JOGO --------------------------------------


void CalcularVizinhos(Celula **t, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (!t[i][j].bomba) {
                int cont = 0;
                // Verificar todas as 8 direções
                
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (di == 0 && dj == 0) continue;
                        
                        int ni = i + di;
                        int nj = j + dj;
                        
                        if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas) {
                            if (t[ni][nj].bomba) {
                                cont++;
                            }
                        }
                    }
                }
                t[i][j].vizinhos = cont;
            }
        }
    }
}

// Função para abrir células recursivamente (expansão de zeros)
void AbrirCelulaRecursiva(int x, int y) {
    if (x < 0 || x >= linhas || y < 0 || y >= colunas) return;
    if (tabuleiro[x][y].estaAberta) return;
    
    tabuleiro[x][y].estaAberta = true;
    
    // Se for zero, abrir vizinhos
    if (tabuleiro[x][y].vizinhos == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) continue;
                AbrirCelulaRecursiva(x + di, y + dj);
            }
        }
    }
}

// Função para verificar vitória
bool VerificarVitoria() {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (!tabuleiro[i][j].bomba && !tabuleiro[i][j].estaAberta) {
                return false;
            }
        }
    }
    return true;
}

// Função para processar clique do jogador
void ProcessarClique(int x, int y) {
    if (!jogoAtivo || x < 0 || x >= linhas || y < 0 || y >= colunas) return;
    
    // Primeira jogada: garantir que não clique em bomba
    if (primeiraJogada) {
        primeiraJogada = false;
        // Se clicou em bomba, reposicionar todas as bombas
        if (tabuleiro[x][y].bomba) {
            // Remover todas as bombas
            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {
                    tabuleiro[i][j].bomba = false;
                }
            }
            // Sortear novamente, garantindo que esta posição não tenha bomba
            int bombas;
            switch(difficulty) {
                case 1: bombas = (linhas * colunas) / 6; break;
                case 2: bombas = (linhas * colunas) / 5; break;
                case 3: bombas = (linhas * colunas) / 4; break;
            }
            
            int bx, by;
            for (int i = 0; i < bombas; i++) {
                bx = rand() % linhas;
                by = rand() % colunas;
                
                // Garantir que não seja a posição clicada
                if ((bx == x && by == y) || tabuleiro[bx][by].bomba) {
                    i--;
                } else {
                    tabuleiro[bx][by].bomba = true;
                }
            }
            // Recalcular vizinhos
            CalcularVizinhos(tabuleiro, linhas, colunas);
        }
    }
    
    // Se já está aberta, não faz nada
    if (tabuleiro[x][y].estaAberta) return;
    
    // Abrir a célula
    tabuleiro[x][y].estaAberta = true;
    
    // Se era bomba, fim de jogo
    if (tabuleiro[x][y].bomba) {
        jogoAtivo = false;
        vitoria = false;
        // Abrir todas as bombas
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (tabuleiro[i][j].bomba) {
                    tabuleiro[i][j].estaAberta = true;
                }
            }
        }
        return;
    }
    
    // Se for zero, abrir área recursivamente
    if (tabuleiro[x][y].vizinhos == 0) {
        AbrirCelulaRecursiva(x, y);
    }
    
    // Verificar vitória
    if (VerificarVitoria()) {
        jogoAtivo = false;
        vitoria = true;
    }
}

// FUNÇÕES DE DESENHO DO RAYLIB ------------------------------------------------

// Função para converter coordenadas de tela para índices da matriz
Vector2 TelaParaMatriz(int mouseX, int mouseY) {
    Vector2 posMatriz;
    posMatriz.x = mouseX / (LARGURA_CELULA + MARGEM);
    posMatriz.y = mouseY / (ALTURA_CELULA + MARGEM);
    return posMatriz;
}

// Função para desenhar uma célula
void DesenharCelula(int linha, int coluna) {
    int x = coluna * (LARGURA_CELULA + MARGEM);
    int y = linha * (ALTURA_CELULA + MARGEM);
    
    Celula *cel = &tabuleiro[linha][coluna];
    
    if (cel->estaAberta) {
        // Célula aberta
        if (cel->bomba) {
            DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, RED);
            DrawText("💣", x + 10, y + 5, 20, WHITE);
        } else {
            DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, LIGHTGRAY);
            // Mostrar número de vizinhos se houver
            if (cel->vizinhos > 0) {
                Color cores[] = {BLUE, GREEN, RED, DARKBLUE, MAROON, SKYBLUE, BLACK, GRAY};
                Color corNumero = cores[cel->vizinhos - 1];
                
                char texto[2];
                sprintf(texto, "%d", cel->vizinhos);
                DrawText(texto, x + 15, y + 10, 20, corNumero);
            }
        }
    } else {
        // Célula fechada
        DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, GRAY);
        DrawRectangle(x + 2, y + 2, LARGURA_CELULA - 4, ALTURA_CELULA - 4, DARKGRAY);
    }
    
    // Contorno da célula
    DrawRectangleLines(x, y, LARGURA_CELULA, ALTURA_CELULA, BLACK);
}

// Função principal
int main(void) {
    // USANDO SUAS FUNÇÕES ORIGINAIS ------------------------------------------
    system("cls");  // Mantive do seu código
    
    printf("Escolha a dificuldade no terminal:\n");
    printf("1 - Facil (10x10)\n");
    printf("2 - Medio (15x15)\n");
    printf("3 - Dificil (20x20)\n");
    printf("Digite a dificuldade (1-3): ");
    scanf("%d", &difficulty);
    
    srand(time(NULL));
    
    // Configurar tamanho do tabuleiro (do seu código)
    switch (difficulty) {
        case 1: linhas = colunas = 10; break;
        case 2: linhas = colunas = 15; break;
        case 3: linhas = colunas = 20; break;
        default: linhas = colunas = 10; break;
    }
    
    // Criar tabuleiro (sua função)
    tabuleiro = criarTabuleiro(linhas, colunas);
    
    // Sortear bombas (sua função)
    SortearBombas(tabuleiro, linhas, colunas, difficulty);
    
    // Calcular vizinhos (nova função necessária)
    CalcularVizinhos(tabuleiro, linhas, colunas);
    
    printf("\nJogo inicializado! Feche o terminal para ver a janela do jogo.\n");
    
    // CONFIGURAÇÃO DO RAYLIB -------------------------------------------------
    const int screenWidth = colunas * (LARGURA_CELULA + MARGEM) + MARGEM;
    const int screenHeight = linhas * (ALTURA_CELULA + MARGEM) + MARGEM + 50; // +50 para painel de status
    
    InitWindow(screenWidth, screenHeight, "Campo Minado - Raylib");
    SetTargetFPS(60);
    
    // Loop principal do jogo
    while (!WindowShouldClose()) {
        // Lógica de clique do mouse
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && jogoAtivo) {
            Vector2 mousePos = GetMousePosition();
            Vector2 matrizPos = TelaParaMatriz(mousePos.x, mousePos.y);
            
            int linha = (int)matrizPos.y;
            int coluna = (int)matrizPos.x;
            
            ProcessarClique(linha, coluna);
        }
        
        // Tecla R para reiniciar
        if (IsKeyPressed(KEY_R)) {
            // Destruir tabuleiro antigo (sua função)
            destruirTabuleiro(tabuleiro, linhas);
            
            // Criar novo tabuleiro (sua função)
            tabuleiro = criarTabuleiro(linhas, colunas);
            
            // Sortear bombas (sua função)
            SortearBombas(tabuleiro, linhas, colunas, difficulty);
            
            // Calcular vizinhos
            CalcularVizinhos(tabuleiro, linhas, colunas);
            
            // Resetar estado do jogo
            jogoAtivo = true;
            vitoria = false;
            primeiraJogada = true;
        }
        
        // DESENHO ------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Desenhar todas as células
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                DesenharCelula(i, j);
            }
        }
        
        // Painel de status
        DrawRectangle(0, screenHeight - 50, screenWidth, 50, LIGHTGRAY);
        
        if (!jogoAtivo) {
            if (vitoria) {
                DrawText("VITORIA! Pressione R para reiniciar", 
                         screenWidth/2 - MeasureText("VITORIA! Pressione R para reiniciar", 20)/2, 
                         screenHeight - 35, 20, GREEN);
            } else {
                DrawText("GAME OVER! Pressione R para reiniciar", 
                         screenWidth/2 - MeasureText("GAME OVER! Pressione R para reiniciar", 20)/2, 
                         screenHeight - 35, 20, RED);
            }
        } else {
            char status[50];
            sprintf(status, "Bombas: %d | Clique para jogar | R = Reiniciar", bombasRestantes);
            DrawText(status, 10, screenHeight - 35, 20, DARKGRAY);
        }
        
        // Instruções
        DrawText("Campo Minado com Raylib", 10, 10, 20, DARKBLUE);
        
        EndDrawing();
    }
    
    // Limpeza (sua função)
    destruirTabuleiro(tabuleiro, linhas);
    
    CloseWindow();
    return 0;
}