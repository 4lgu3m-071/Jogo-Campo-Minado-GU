#include <raylib.h>

int main() {
    InitWindow(800, 400, "Teste Raylib");
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Funcionou!", 190, 200, 20, BLACK);
        EndDrawing();
    }
    
    CloseWindow();
}
