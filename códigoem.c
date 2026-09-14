#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Numeracao dos estados do AFD
// q0=0 (Inicio), q1=1 (Sinal), q2=2 (Int), q3=3 (Virgula),
// q4=4 (Float), q5=5 (Final Int), q6=6 (Final Float)
#define Q0 0
#define Q1 1
#define Q2 2
#define Q3 3
#define Q4 4
#define Q5 5
#define Q6 6
#define QERRO -1

typedef struct {
    int x;
    int y;
    const char* label;
} EstadoVisual;

int main(void) {
    const int screenWidth = 960;
    const int screenHeight = 620;
    InitWindow(screenWidth, screenHeight, "Simulador AFD - Analisador Lexico TIPO 2");
    SetTargetFPS(60);

    char entrada[100] = "";
    int len = 0;
    bool editando = true;

    int i = 0;
    int estado = Q0;
    int inicio_token = -1;
    int fim_token = -1;
    bool token_encontrado = false;
    bool processamento_concluido = false;
    char mensagem_final[100] = "";
    int caractere_anterior = -1; // indice do caractere mostrado na animacao

    float timer = 0.0f;
    float tempo_entre_passos = 1.2f;

    // Array indexado pelo numero do estado (0 a 6)
    EstadoVisual posicoes[7] = {
        {80,  300, "q0"},  // 0 - Inicio
        {230, 300, "q1"},  // 1 - Sinal (+/-)
        {400, 300, "q2"},  // 2 - Int (digito)
        {570, 300, "q3"},  // 3 - Virgula
        {740, 300, "q4"},  // 4 - Float (digito apos virgula)
        {400, 470, "q5"},  // 5 - Final Int
        {740, 470, "q6"}   // 6 - Final Float
    };

    const char* estadoLabels[7] = {
        "Inicio", "Sinal", "Int", "Virgula", "Float", "Final Int", "Final Float"
    };

    Rectangle inputBox = { 20, 50, 500, 35 };
    Rectangle btnIniciar = { 540, 50, 150, 35 };
    Rectangle btnLimpar = { 710, 50, 100, 35 };

    while (!WindowShouldClose()) {
        // --- INPUT ---
        if (editando) {
            int key = GetCharPressed();
            while (key > 0) {
                if (len < 99 && key >= 32 && key <= 126) {
                    entrada[len] = (char)key;
                    len++;
                    entrada[len] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
                len--;
                entrada[len] = '\0';
            }
            if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && len > 0) {
                editando = false;
                i = 0; estado = Q0; inicio_token = -1; fim_token = -1;
                token_encontrado = false; processamento_concluido = false;
                mensagem_final[0] = '\0'; timer = 0.0f;
            }
            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btnIniciar) && len > 0) {
                    editando = false;
                    i = 0; estado = Q0; inicio_token = -1; fim_token = -1;
                    token_encontrado = false; processamento_concluido = false;
                    mensagem_final[0] = '\0'; timer = 0.0f;
                }
                if (CheckCollisionPointRec(mouse, btnLimpar)) {
                    entrada[0] = '\0'; len = 0;
                }
            }
        }

        // --- LOGICA DO AFD ---
        if (!editando && !processamento_concluido) {
            // Pular caracteres irrelevantes no q0 sem gastar tempo
            if (estado == Q0) {
                while (entrada[i] != '\0' && entrada[i] != '+' && entrada[i] != '-' && !isdigit((unsigned char)entrada[i])) {
                    i++;
                }
                // Buffer: se encontrar sinal no q0, consume imediatamente e armazena
                // mas NAO incrementa i (o digito sera lido no proximo tick)
                if (entrada[i] == '+' || entrada[i] == '-') {
                    inicio_token = i;
                    caractere_anterior = i; // mostra o sinal na animacao
                    i++; // avanca pra posicao do digito (sera lido no proximo tick)
                    estado = Q1; // vai pra q1 (Sinal) mas o tick ja foi consumido aqui
                    timer = 0; // reseta pra dar um passo visual imediato
                }
            }

            timer += GetFrameTime();

            if (timer >= tempo_entre_passos) {
                timer = 0.0f;

                if (entrada[i] != '\0' && !token_encontrado && estado != QERRO) {
                    char c = entrada[i];

                    switch (estado) {
                        case Q0: // Inicio
                            if (c == '+' || c == '-') {
                                inicio_token = i;
                                caractere_anterior = i;
                                estado = Q1; // -> Sinal
                                i++;
                            } else if (isdigit((unsigned char)c)) {
                                inicio_token = i;
                                caractere_anterior = i;
                                estado = Q2; // -> Int
                                i++;
                            }
                            break;

                        case Q1: // Sinal
                            caractere_anterior = i; // mostra o digito apos o sinal
                            if (isdigit((unsigned char)c)) {
                                estado = Q2; // -> Int
                                i++;
                            } else {
                                estado = QERRO;
                            }
                            break;

                        case Q2: // Int
                            caractere_anterior = i;
                            if (isdigit((unsigned char)c)) {
                                estado = Q2; // loop Int
                                i++;
                            } else if (c == ',') {
                                estado = Q3; // -> Virgula
                                i++;
                            } else {
                                estado = Q5; // lookahead -> Final Int
                            }
                            break;

                        case Q3: // Virgula
                            caractere_anterior = i;
                            if (isdigit((unsigned char)c)) {
                                estado = Q4; // -> Float
                                i++;
                            } else {
                                estado = QERRO;
                            }
                            break;

                        case Q4: // Float
                            caractere_anterior = i;
                            if (isdigit((unsigned char)c)) {
                                estado = Q4; // loop Float
                                i++;
                            } else {
                                estado = Q6; // lookahead -> Final Float
                            }
                            break;
                    }

                    if (estado == Q5 || estado == Q6) {
                        fim_token = i;
                        token_encontrado = true;
                        processamento_concluido = true;
                    }
                } else {
                    // Fim da string ou erro
                    if (!token_encontrado && (estado == Q2 || estado == Q4)) {
                        fim_token = i;
                        token_encontrado = true;
                        estado = (estado == Q2) ? Q5 : Q6;
                    }
                    processamento_concluido = true;
                }

                if (processamento_concluido) {
                    if (token_encontrado) {
                        char lexema[50] = {0};
                        strncpy(lexema, entrada + inicio_token, fim_token - inicio_token);
                        sprintf(mensagem_final, "Token aceito! Lexema: %s", lexema);
                    } else {
                        sprintf(mensagem_final, "Nenhum token foi reconhecido.");
                    }
                }
            }
        }

        // Botao limpar apos conclusao
        if (!editando && processamento_concluido) {
            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, btnLimpar)) {
                entrada[0] = '\0'; len = 0;
                editando = true; processamento_concluido = false;
                mensagem_final[0] = '\0';
            }
        }

        // --- DESENHO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Atividade de Implementacao - AFD TIPO 2", 20, 15, 20, DARKGRAY);

        // Input
        DrawRectangleRec(inputBox, WHITE);
        DrawRectangleLinesEx(inputBox, 2, editando ? BLUE : GRAY);
        if (len == 0 && editando) {
            DrawText("Digite a entrada (ex: -3,14 ou +42)", inputBox.x + 10, inputBox.y + 8, 15, LIGHTGRAY);
        }
        DrawText(entrada, inputBox.x + 10, inputBox.y + 8, 20, BLACK);
        if (editando) {
            int textW = MeasureText(entrada, 20);
            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawRectangle(inputBox.x + 10 + textW + 1, inputBox.y + 6, 2, 23, BLACK);
            }
        }

        // Botoes
        DrawRectangleRec(btnIniciar, (len > 0 && editando) ? DARKBLUE : LIGHTGRAY);
        DrawText("Iniciar (Enter)", btnIniciar.x + 15, btnIniciar.y + 8, 15, WHITE);
        DrawRectangleRec(btnLimpar, DARKGRAY);
        DrawText("Limpar", btnLimpar.x + 20, btnLimpar.y + 8, 15, WHITE);

        // Status
        if (!editando) {
            DrawText(TextFormat("Entrada: %s", entrada), 20, 100, 20, BLACK);
            if (!processamento_concluido && caractere_anterior >= 0 && caractere_anterior < (int)strlen(entrada)) {
                DrawText(TextFormat("Pos: %d  Lendo: '%c'", caractere_anterior, entrada[caractere_anterior]), 20, 130, 20, BLUE);
            } else if (!processamento_concluido) {
                DrawText("Iniciando...", 20, 130, 20, BLUE);
            }
        }

        // === ARESTAS (linhas) ===
        int lw = 2; // espessura
        Color cAresta = LIGHTGRAY;

        // Linha principal: q0 -> q1 -> q2 -> q3 -> q4
        DrawLine(posicoes[Q0].x + 28, posicoes[Q0].y, posicoes[Q1].x - 28, posicoes[Q1].y, cAresta);
        DrawLine(posicoes[Q1].x + 28, posicoes[Q1].y, posicoes[Q2].x - 28, posicoes[Q2].y, cAresta);
        DrawLine(posicoes[Q2].x + 28, posicoes[Q2].y, posicoes[Q3].x - 28, posicoes[Q3].y, cAresta);
        DrawLine(posicoes[Q3].x + 28, posicoes[Q3].y, posicoes[Q4].x - 28, posicoes[Q4].y, cAresta);

        // Descida: q2 -> q5 (Final Int)
        DrawLine(posicoes[Q2].x, posicoes[Q2].y + 28, posicoes[Q5].x, posicoes[Q5].y - 28, cAresta);
        // Descida: q4 -> q6 (Final Float)
        DrawLine(posicoes[Q4].x, posicoes[Q4].y + 28, posicoes[Q6].x, posicoes[Q6].y - 28, cAresta);

        // === ROTULOS DAS ARESTAS ===
        DrawText("[+/-]", (posicoes[Q0].x + posicoes[Q1].x) / 2 - 15, posicoes[Q0].y - 25, 14, DARKBLUE);
        DrawText("[D]", (posicoes[Q1].x + posicoes[Q2].x) / 2 - 8, posicoes[Q1].y - 25, 14, BLACK);
        DrawText("[,]", (posicoes[Q2].x + posicoes[Q3].x) / 2 - 8, posicoes[Q2].y - 25, 14, BLACK);
        DrawText("[D]", (posicoes[Q3].x + posicoes[Q4].x) / 2 - 8, posicoes[Q3].y - 25, 14, BLACK);

        // Loop em cima de q2 (Int)
        DrawText("[D]", posicoes[Q2].x - 8, posicoes[Q2].y - 55, 14, BLACK);
        // Loop em cima de q4 (Float)
        DrawText("[D]", posicoes[Q4].x - 8, posicoes[Q4].y - 55, 14, BLACK);

        // q2 -> q5
        DrawText("[outro]", posicoes[Q2].x + 35, (posicoes[Q2].y + posicoes[Q5].y) / 2, 14, BLACK);
        // q4 -> q6
        DrawText("[outro]", posicoes[Q4].x + 35, (posicoes[Q4].y + posicoes[Q6].y) / 2, 14, BLACK);

        // === ESTADOS (bolinhas) ===
        for (int j = 0; j < 7; j++) {
            Color corEstado = DARKGRAY;

            if (!editando) {
                if (estado == j) corEstado = GREEN;
                if (estado == QERRO && j == Q0) corEstado = RED;
                if (processamento_concluido && token_encontrado && (j == Q5 || j == Q6) && estado == j) {
                    corEstado = GOLD;
                }
            }

            DrawCircle(posicoes[j].x, posicoes[j].y, 28, corEstado);
            DrawText(posicoes[j].label, posicoes[j].x - 10, posicoes[j].y - 7, 18, (corEstado.r > 100 && corEstado.g > 100) ? BLACK : WHITE);
            DrawText(estadoLabels[j], posicoes[j].x - 25, posicoes[j].y + 38, 13, BLACK);
        }

        // Seta indicando inicio (q0)
        DrawTriangle(
            (Vector2){posicoes[Q0].x - 45, posicoes[Q0].y},
            (Vector2){posicoes[Q0].x - 35, posicoes[Q0].y - 8},
            (Vector2){posicoes[Q0].x - 35, posicoes[Q0].y + 8},
            DARKGRAY
        );

        // Mensagem final
        if (processamento_concluido) {
            Color cMsg = token_encontrado ? DARKGREEN : RED;
            DrawRectangle(15, 555, 930, 40, Fade(cMsg, 0.15f));
            DrawRectangleLinesEx((Rectangle){15, 555, 930, 40}, 2, Fade(cMsg, 0.5f));
            DrawText(mensagem_final, 25, 563, 20, cMsg);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
