#include "raylib.h"
#include <stdio.h>

#define LARGURA_CELULA 40
#define ALTURA_CELULA 40
#define MARGEM 2
#define LINHAS 10
#define COLUNAS 10

typedef struct {
    bool bomba;
    bool estaAberta;
    bool bandeira;  // Adicione isso para marcar bandeiras
    int vizinhos;
} Celula;

Celula campo[LINHAS][COLUNAS];

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
    
    if (campo[linha][coluna].estaAberta) {
        // Célula aberta
        if (campo[linha][coluna].bomba) {
            DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, RED);
            DrawText("X", x + 15, y + 10, 20, WHITE);
        } else {
            DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, LIGHTGRAY);
            // Mostrar número de vizinhos se houver
            if (campo[linha][coluna].vizinhos > 0) {
                char texto[2];
                sprintf(texto, "%d", campo[linha][coluna].vizinhos);
                DrawText(texto, x + 15, y + 10, 20, BLUE);
            }
        }
    } else {
        // Célula fechada
        DrawRectangle(x, y, LARGURA_CELULA, ALTURA_CELULA, GRAY);
        
        // Mostrar bandeira se tiver
        if (campo[linha][coluna].bandeira) {
            DrawText("F", x + 15, y + 10, 20, RED);
        }
    }
    
    // Contorno da célula
    DrawRectangleLines(x, y, LARGURA_CELULA, ALTURA_CELULA, DARKGRAY);
}

// Função principal do jogo
int main(void) {
    const int screenWidth = COLUNAS * (LARGURA_CELULA + MARGEM) + MARGEM;
    const int screenHeight = LINHAS * (ALTURA_CELULA + MARGEM) + MARGEM;
    
    InitWindow(screenWidth, screenHeight, "Campo Minado");
    
    // Inicializar seu campo aqui (você já tem essa lógica)
    InicializarCampo(campo);  // Sua função existente
    
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Lógica de clique do mouse
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 matrizPos = TelaParaMatriz(mousePos.x, mousePos.y);
            
            int linha = (int)matrizPos.y;
            int coluna = (int)matrizPos.x;
            
            // Verificar se está dentro dos limites
            if (linha >= 0 && linha < LINHAS && coluna >= 0 && coluna < COLUNAS) {
                // Abrir célula (sua lógica existente)
                if (!campo[linha][coluna].bandeira) {
                    AbrirCelula(campo, linha, coluna);
                }
            }
        }
        
        // Clique direito para bandeiras
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePos = GetMousePosition();
            Vector2 matrizPos = TelaParaMatriz(mousePos.x, mousePos.y);
            
            int linha = (int)matrizPos.y;
            int coluna = (int)matrizPos.x;
            
            if (linha >= 0 && linha < LINHAS && coluna >= 0 && coluna < COLUNAS) {
                if (!campo[linha][coluna].estaAberta) {
                    campo[linha][coluna].bandeira = !campo[linha][coluna].bandeira;
                }
            }
        }
        
        // Desenho
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Desenhar todas as células
        for (int i = 0; i < LINHAS; i++) {
            for (int j = 0; j < COLUNAS; j++) {
                DesenharCelula(i, j);
            }
        }
        
        // Você pode adicionar texto de status aqui
        // DrawText("Campo Minado", 10, screenHeight - 30, 20, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}