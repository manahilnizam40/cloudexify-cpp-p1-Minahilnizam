#include "raylib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Load best score from file
int loadBestScore() {
    ifstream file("bestscore.txt");
    int score = 0;
    if (file.is_open()) {
        file >> score;
        file.close();
    }
    return score;
}

// Save best score to file
void saveBestScore(int score) {
    int current = loadBestScore();
    if (score < current || current == 0) {
        ofstream file("bestscore.txt");
        if (file.is_open()) {
            file << score;
            file.close();
        }
    }
}

// Helper Function to draw interactive buttons
bool DrawButton(Rectangle rect, const char* text, Color btnColor) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);

    Color drawColor = hovered ? Color{ (unsigned char)(btnColor.r * 0.8), (unsigned char)(btnColor.g * 0.8), (unsigned char)(btnColor.b * 0.8), 255 } : btnColor;
    DrawRectangleRec(rect, drawColor);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);

    int fontSize = 16;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, WHITE);

    return (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "CloudExify - GUI Number Guessing Game");
    SetTargetFPS(60);

    srand(time(0));

    int maxRange = 100;
    int secret = (rand() % maxRange) + 1;
    int attempts = 0;
    int maxAttempts = 10;
    int previousDiff = -1;
    int bestScore = loadBestScore();

    char guessInput[10] = "\0";
    int letterCount = 0;

    string feedback = "Select difficulty and type a number to start!";
    bool gameOver = false;
    bool gameWon = false;

    while (!WindowShouldClose()) {
        // --- INPUT HANDLING ---
        if (!gameOver) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= '0' && key <= '9') && (letterCount < 5)) {
                    guessInput[letterCount] = (char)key;
                    guessInput[letterCount + 1] = '\0';
                    letterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                guessInput[letterCount] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && letterCount > 0) {
                int userGuess = atoi(guessInput);
                attempts++;

                if (userGuess < 1 || userGuess > maxRange) {
                    feedback = "Out of range! Guess between 1 and " + to_string(maxRange);
                    attempts--;
                } else if (userGuess == secret) {
                    feedback = "CORRECT! You won in " + to_string(attempts) + " attempts!";
                    gameOver = true;
                    gameWon = true;
                    saveBestScore(attempts);
                    bestScore = loadBestScore();
                } else {
                    int diff = abs(userGuess - secret);
                    string hint = "";
                    if (previousDiff != -1) {
                        if (diff < previousDiff) hint = " (Getting Warmer!)";
                        else hint = " (Getting Colder!)";
                    }
                    previousDiff = diff;

                    if (userGuess < secret) feedback = "Too LOW! Try higher." + hint;
                    else feedback = "Too HIGH! Try lower." + hint;

                    if (attempts >= maxAttempts) {
                        feedback = "GAME OVER! The secret number was " + to_string(secret);
                        gameOver = true;
                    }
                }
                guessInput[0] = '\0';
                letterCount = 0;
            }
        }

        // --- DRAWING GRAPHICS ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Header Title Bar
        DrawRectangle(0, 0, screenWidth, 80, DARKBLUE);
        DrawText("CLOUDEXIFY NUMBER GUESSING GAME", 160, 25, 26, WHITE);

        // Best Score
        DrawText(TextFormat("Best Score: %d attempts", bestScore), 310, 95, 20, DARKGRAY);

        // Difficulty Buttons
        DrawText("Difficulty:", 120, 140, 20, BLACK);
        if (DrawButton(Rectangle{ 240, 130, 130, 40 }, "Easy (1-50)", GREEN)) {
            maxRange = 50; maxAttempts = 7; secret = (rand() % 50) + 1;
            attempts = 0; gameOver = false; gameWon = false; previousDiff = -1;
            feedback = "Difficulty set to Easy (1-50)";
        }
        if (DrawButton(Rectangle{ 390, 130, 130, 40 }, "Medium (1-100)", BLUE)) {
            maxRange = 100; maxAttempts = 10; secret = (rand() % 100) + 1;
            attempts = 0; gameOver = false; gameWon = false; previousDiff = -1;
            feedback = "Difficulty set to Medium (1-100)";
        }
        if (DrawButton(Rectangle{ 540, 130, 130, 40 }, "Hard (1-200)", RED)) {
            maxRange = 200; maxAttempts = 12; secret = (rand() % 200) + 1;
            attempts = 0; gameOver = false; gameWon = false; previousDiff = -1;
            feedback = "Difficulty set to Hard (1-200)";
        }

        // Guess Input Box
        DrawText("Enter Guess & Press ENTER:", 260, 210, 20, BLACK);
        DrawRectangle(300, 250, 200, 50, LIGHTGRAY);
        DrawRectangleLines(300, 250, 200, 50, DARKBLUE);
        DrawText(guessInput, 320, 262, 28, MAROON);

        // Feedback Text
        DrawText(feedback.c_str(), 160, 340, 20, gameWon ? GREEN : (gameOver ? RED : DARKBLUE));
        DrawText(TextFormat("Attempts Used: %d / %d", attempts, maxAttempts), 300, 400, 20, DARKGRAY);

        // Restart Button
        if (DrawButton(Rectangle{ 325, 460, 150, 45 }, "Restart Game", PURPLE)) {
            secret = (rand() % maxRange) + 1;
            attempts = 0;
            previousDiff = -1;
            gameOver = false;
            gameWon = false;
            feedback = "New game started! Type your guess.";
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
