#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <string.h>

#define BALL_SPACING 36.0f
#define MAX_BALLS 50
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define MAX_PARTICLES 200
#define PATH_POINTS 120

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_LEVEL_SELECT,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_WON,
    GAME_STATE_LOST
} GameState;

GameState currentState = GAME_STATE_MENU;

typedef struct {
    char name[50];
    int initialBalls;
    float chainSpeed;
    int colorCount;
    char description[100];
} LevelConfig;

LevelConfig levels[3] = {
    {"Easy", 12, 35.0f, 3, "Slow speed, 3 colors"},
    {"Medium", 18, 55.0f, 4, "Medium speed, 4 colors"},
    {"Hard", 25, 75.0f, 5, "Fast speed, 5 colors"}
};

int currentLevel = 0;
float CHAIN_SPEED = 35.0f;
int GAME_COLOR_COUNT = 3;

typedef struct {
    char text[200];
    float displayTime;
    float maxTime;
    int active;
} Hint;

Hint currentHint = { "", 0.0f, 3.0f, 0 };

void showHint(const char* text, float duration) {
    if (!text) return;
    strncpy(currentHint.text, text, 199);
    currentHint.text[199] = '\0';
    currentHint.displayTime = 0.0f;
    currentHint.maxTime = duration > 0.0f ? duration : 3.0f;
    currentHint.active = 1;
}

void updateHint(float dt) {
    if (currentHint.active) {
        currentHint.displayTime += dt;
        if (currentHint.displayTime >= currentHint.maxTime) {
            currentHint.active = 0;
        }
    }
}

int score = 0;
int combo = 0;
int maxCombo = 0;
int totalShots = 0;
int successfulShots = 0;
float comboTimer = 0.0f;
const float COMBO_TIMEOUT = 3.0f;

void addScore(int ballsRemoved) {
    if (ballsRemoved >= 3) {
        combo++;
        if (combo > maxCombo) maxCombo = combo;
        comboTimer = 0.0f;
        int points = ballsRemoved * 10 * combo;
        score += points;
        char hintText[100];
        if (combo > 1) {
            snprintf(hintText, sizeof(hintText), "COMBO x%d! +%d points", combo, points);
        }
        else {
            snprintf(hintText, sizeof(hintText), "+%d points", points);
        }
        showHint(hintText, 2.0f);
    }
}

void updateCombo(float dt) {
    if (combo > 0) {
        comboTimer += dt;
        if (comboTimer >= COMBO_TIMEOUT) {
            combo = 0;
            comboTimer = 0.0f;
        }
    }
}

// ИСПРАВЛЕННЫЙbitmap шрифт без зеркальности
void drawBitmapChar(float x, float y, char c, float scale) {
    if (scale <= 0.0f) return;
    glPointSize(3.0f * scale);
    glBegin(GL_POINTS);

    int pattern[7] = { 0, 0, 0, 0, 0, 0, 0 };

    if (c == 'A') {
        int p[7] = { 0b00100, 0b01010, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'B') {
        int p[7] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'C') {
        int p[7] = { 0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'D') {
        int p[7] = { 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'E') {
        int p[7] = { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'F') {
        int p[7] = { 0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'G') {
        int p[7] = { 0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'H') {
        int p[7] = { 0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'I') {
        int p[7] = { 0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'K') {
        int p[7] = { 0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'L') {
        int p[7] = { 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'M') {
        int p[7] = { 0b10001, 0b11011, 0b10101, 0b10001, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'N') {
        int p[7] = { 0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'O') {
        int p[7] = { 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'P') {
        int p[7] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'R') {
        int p[7] = { 0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'S') {
        int p[7] = { 0b01110, 0b10001, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'T') {
        int p[7] = { 0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'U') {
        int p[7] = { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'V') {
        int p[7] = { 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b01010, 0b00100 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'W') {
        int p[7] = { 0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'X') {
        int p[7] = { 0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'Y') {
        int p[7] = { 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'Z') {
        int p[7] = { 0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c >= '0' && c <= '9') {
        int digits[10][7] = {
            {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110},
            {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},
            {0b01110, 0b10001, 0b00001, 0b01110, 0b10000, 0b10000, 0b11111},
            {0b11111, 0b00001, 0b00001, 0b01111, 0b00001, 0b00001, 0b11111},
            {0b10001, 0b10001, 0b10001, 0b11111, 0b00001, 0b00001, 0b00001},
            {0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b10001, 0b01110},
            {0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},
            {0b11111, 0b00001, 0b00010, 0b00100, 0b00100, 0b00100, 0b00100},
            {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},
            {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110}
        };
        memcpy(pattern, digits[c - '0'], sizeof(digits[0]));
    }
    else if (c == ':') {
        int p[7] = { 0b00000, 0b00100, 0b00000, 0b00000, 0b00000, 0b00100, 0b00000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '+') {
        int p[7] = { 0b00000, 0b00100, 0b00100, 0b11111, 0b00100, 0b00100, 0b00000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '!') {
        int p[7] = { 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000, 0b00100 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '-') {
        int p[7] = { 0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '%') {
        int p[7] = { 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'x') {
        int p[7] = { 0b00000, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b00000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '/') {
        int p[7] = { 0b00001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'a') {
        int p[7] = { 0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b10001, 0b01111 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'e') {
        int p[7] = { 0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 's') {
        int p[7] = { 0b00000, 0b01111, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'u') {
        int p[7] = { 0b00000, 0b10001, 0b10001, 0b10001, 0b10001, 0b10011, 0b01101 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'm') {
        int p[7] = { 0b00000, 0b11010, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'o') {
        int p[7] = { 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'r') {
        int p[7] = { 0b00000, 0b10110, 0b11001, 0b10000, 0b10000, 0b10000, 0b10000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'i') {
        int p[7] = { 0b00100, 0b00000, 0b01100, 0b00100, 0b00100, 0b00100, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 't') {
        int p[7] = { 0b00100, 0b00100, 0b11110, 0b00100, 0b00100, 0b00100, 0b00011 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'n') {
        int p[7] = { 0b00000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'p') {
        int p[7] = { 0b00000, 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'l') {
        int p[7] = { 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'w') {
        int p[7] = { 0b00000, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'd') {
        int p[7] = { 0b00001, 0b00001, 0b01111, 0b10001, 0b10001, 0b10011, 0b01101 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'c') {
        int p[7] = { 0b00000, 0b01110, 0b10001, 0b10000, 0b10000, 0b10001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'h') {
        int p[7] = { 0b10000, 0b10000, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'b') {
        int p[7] = { 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b10001, 0b11110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'y') {
        int p[7] = { 0b00000, 0b10001, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == 'g') {
        int p[7] = { 0b00000, 0b01111, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == ',') {
        int p[7] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01000 };
        memcpy(pattern, p, sizeof(p));
    }
    else if (c == '.') {
        int p[7] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b00100 };
        memcpy(pattern, p, sizeof(p));
    }

    // ИСПРАВЛЕНО: правильный порядок отрисовки (сверху вниз)
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (pattern[row] & (1 << (4 - col))) {  // Исправлено: читаем справа налево
                glVertex2f(x + col * 2 * scale, y + (6 - row) * 2 * scale);
            }
        }
    }

    glEnd();
    glPointSize(1.0f);
}

void drawText(float x, float y, const char* text, float scale) {
    if (!text) return;
    float offset = 0;
    for (int i = 0; text[i] && i < 200; i++) {
        drawBitmapChar(x + offset, y, text[i], scale);
        offset += 12 * scale;
    }
}

void drawNumber(float x, float y, int number, float scale) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", number);
    drawText(x, y, buffer, scale);
}

void drawCircle(float x, float y, float radius, int segments) {
    if (radius <= 0.0f || segments < 3) return;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * i / segments;
        glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
    }
    glEnd();
}

void drawGradientCircle(float x, float y, float radius, float r, float g, float b, int segments) {
    if (radius <= 0.0f || segments < 3) return;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glVertex2f(x, y);

    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * i / segments;
        glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
    }
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
    glVertex2f(x - radius * 0.3f, y + radius * 0.3f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * i / segments;
        glVertex2f(
            x - radius * 0.3f + cosf(angle) * radius * 0.4f,
            y + radius * 0.3f + sinf(angle) * radius * 0.4f
        );
    }
    glEnd();
    glDisable(GL_BLEND);
}

void drawShadow(float x, float y, float radius, int segments) {
    if (radius <= 0.0f || segments < 3) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    glVertex2f(x + 3, y - 3);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * i / segments;
        glVertex2f(x + 3 + cosf(angle) * (radius + 2), y - 3 + sinf(angle) * (radius + 2));
    }
    glEnd();
    glDisable(GL_BLEND);
}

void drawRoundedRect(float x, float y, float width, float height, float radius) {
    if (width <= 0.0f || height <= 0.0f || radius < 0.0f) return;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + width / 2, y + height / 2);

    for (int i = 0; i <= 10; i++) {
        float angle = -1.5708f + (1.5708f * i / 10.0f);
        glVertex2f(x + width - radius + cosf(angle) * radius,
            y + height - radius + sinf(angle) * radius);
    }
    for (int i = 0; i <= 10; i++) {
        float angle = 0.0f + (1.5708f * i / 10.0f);
        glVertex2f(x + width - radius + cosf(angle) * radius,
            y + radius + sinf(angle) * radius);
    }
    for (int i = 0; i <= 10; i++) {
        float angle = 1.5708f + (1.5708f * i / 10.0f);
        glVertex2f(x + radius + cosf(angle) * radius,
            y + radius + sinf(angle) * radius);
    }
    for (int i = 0; i <= 10; i++) {
        float angle = 3.1416f + (1.5708f * i / 10.0f);
        glVertex2f(x + radius + cosf(angle) * radius,
            y + height - radius + sinf(angle) * radius);
    }
    glEnd();
}

// ПОРТАЛ в конце пути
void drawPortal(float x, float y, float time) {
    // Внешнее свечение
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int ring = 0; ring < 4; ring++) {
        float r = 40.0f + ring * 15.0f;
        float alpha = 0.3f - ring * 0.06f;

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.3f, 0.1f, 0.8f, alpha);
        glVertex2f(x, y);
        glColor4f(0.5f, 0.2f, 1.0f, 0.0f);
        for (int i = 0; i <= 32; i++) {
            float angle = 2.0f * 3.1415926f * i / 32.0f + time + ring * 0.5f;
            glVertex2f(x + cosf(angle) * r, y + sinf(angle) * r);
        }
        glEnd();
    }

    // Вращающееся кольцо
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * 3.1415926f * i / 32.0f + time * 2.0f;
        float pulse = 0.5f + 0.5f * sinf(time * 3.0f + i * 0.2f);
        glColor4f(0.6f, 0.3f, 1.0f, pulse);
        glVertex2f(x + cosf(angle) * 35.0f, y + sinf(angle) * 35.0f);
    }
    glEnd();
    glLineWidth(1.0f);

    // Центральная воронка
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.1f, 0.0f, 0.3f, 0.9f);
    glVertex2f(x, y);
    glColor4f(0.4f, 0.2f, 0.9f, 0.6f);
    for (int i = 0; i <= 32; i++) {
        float angle = 2.0f * 3.1415926f * i / 32.0f - time * 3.0f;
        glVertex2f(x + cosf(angle) * 25.0f, y + sinf(angle) * 25.0f);
    }
    glEnd();

    // Частицы вокруг портала
    for (int i = 0; i < 8; i++) {
        float angle = time * 2.0f + i * 0.785f;
        float radius = 30.0f + sinf(time * 3.0f + i) * 5.0f;
        float px = x + cosf(angle) * radius;
        float py = y + sinf(angle) * radius;

        glColor4f(0.8f, 0.5f, 1.0f, 0.8f);
        drawCircle(px, py, 3.0f, 8);
    }

    glDisable(GL_BLEND);
}

typedef struct {
    float x, y, width, height;
    char text[50];
    int hovered;
    float pulseTime;
} Button;

Button menuButtons[3];
Button levelButtons[3];
int buttonCount = 0;

void initMenuButtons() {
    buttonCount = 2;
    menuButtons[0].x = -150; menuButtons[0].y = 50;
    menuButtons[0].width = 300; menuButtons[0].height = 60;
    strncpy(menuButtons[0].text, "PLAY", sizeof(menuButtons[0].text) - 1);

    menuButtons[1].x = -150; menuButtons[1].y = -40;
    menuButtons[1].width = 300; menuButtons[1].height = 60;
    strncpy(menuButtons[1].text, "EXIT", sizeof(menuButtons[1].text) - 1);

    for (int i = 0; i < 2; i++) {
        menuButtons[i].hovered = 0;
        menuButtons[i].pulseTime = 0;
    }
}

void initLevelButtons() {
    for (int i = 0; i < 3; i++) {
        levelButtons[i].x = -350 + i * 250;
        levelButtons[i].y = 0;
        levelButtons[i].width = 220;
        levelButtons[i].height = 180;
        strncpy(levelButtons[i].text, levels[i].name, sizeof(levelButtons[i].text) - 1);
        levelButtons[i].hovered = 0;
        levelButtons[i].pulseTime = 0;
    }
}

void drawButton(Button* btn, float dt) {
    if (!btn) return;
    btn->pulseTime += dt * 3.0f;
    float pulse = btn->hovered ? 1.0f + sinf(btn->pulseTime) * 0.1f : 1.0f;
    float x = btn->x;
    float y = btn->y;
    float w = btn->width * pulse;
    float h = btn->height * pulse;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    drawRoundedRect(x + 5, y - 5, w, h, 15);
    glDisable(GL_BLEND);

    if (btn->hovered) {
        glColor3f(0.3f, 0.6f, 0.9f);
    }
    else {
        glColor3f(0.2f, 0.4f, 0.7f);
    }
    drawRoundedRect(x, y, w, h, 15);

    glColor3f(0.5f, 0.7f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 40; i++) {
        float angle = 6.2832f * i / 40.0f;
        float px = x + w / 2 + cosf(angle) * (w / 2);
        float py = y + h / 2 + sinf(angle) * (h / 2);
        glVertex2f(px, py);
    }
    glEnd();
    glLineWidth(1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    float textWidth = strlen(btn->text) * 12 * 1.0f;
    float textX = x + (w - textWidth) / 2;
    drawText(textX, y + h / 2 - 7, btn->text, 1.0f);
}

int checkButtonHover(Button* btn, double mx, double my) {
    if (!btn) return 0;
    return (mx >= btn->x && mx <= btn->x + btn->width &&
        my >= btn->y && my <= btn->y + btn->height);
}

typedef struct {
    float x, y, vx, vy;
    float r, g, b;
    float life, size;
    int active;
} Particle;

Particle particles[MAX_PARTICLES];

void initParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++)
        particles[i].active = 0;
}

void spawnParticles(float x, float y, float r, float g, float b, int count) {
    if (count <= 0) return;
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (!particles[i].active) {
            particles[i].x = x;
            particles[i].y = y;
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 50.0f + (rand() % 100);
            particles[i].vx = cosf(angle) * speed;
            particles[i].vy = sinf(angle) * speed;
            particles[i].r = r;
            particles[i].g = g;
            particles[i].b = b;
            particles[i].life = 1.0f;
            particles[i].size = 3.0f + (rand() % 4);
            particles[i].active = 1;
            count--;
        }
    }
}

void updateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].vy -= 200.0f * dt;
            particles[i].life -= dt * 2.0f;
            if (particles[i].life <= 0.0f)
                particles[i].active = 0;
        }
    }
}

void drawParticles() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].life);
            drawCircle(particles[i].x, particles[i].y, particles[i].size, 8);
        }
    }
    glDisable(GL_BLEND);
}

typedef struct {
    float x, y, radius;
    float r, g, b;
    float distance;
} Ball;

typedef struct {
    float x, y, angle;
} Frog;

typedef struct {
    float x, y;
} PathPoint;

typedef struct {
    float x, y, vx, vy;
    int color;
    int active;
} ShotBall;

Ball gBalls[MAX_BALLS];
int gBallCount = 0;
int gameWon = 0, gameLost = 0;
float gameOverTimer = 0.0f;

void getColorByIndex(int colorIndex, float* r, float* g, float* b) {
    if (!r || !g || !b) return;
    switch (colorIndex % 5) {
    case 0: *r = 1.0f; *g = 0.0f; *b = 0.0f; break;
    case 1: *r = 0.0f; *g = 1.0f; *b = 0.0f; break;
    case 2: *r = 0.0f; *g = 0.0f; *b = 1.0f; break;
    case 3: *r = 1.0f; *g = 1.0f; *b = 0.0f; break;
    case 4: *r = 1.0f; *g = 0.0f; *b = 1.0f; break;
    }
}

int getColorIndex(float r, float g, float b) {
    if (r > 0.5f && g < 0.5f && b < 0.5f) return 0;
    if (r < 0.5f && g > 0.5f && b < 0.5f) return 1;
    if (r < 0.5f && g < 0.5f && b > 0.5f) return 2;
    if (r > 0.5f && g > 0.5f && b < 0.5f) return 3;
    if (r > 0.5f && g < 0.5f && b > 0.5f) return 4;
    return 0;
}

int isColorInChain(Ball* balls, int count, int color) {
    if (!balls || count <= 0) return 0;
    for (int i = 0; i < count; i++) {
        if (getColorIndex(balls[i].r, balls[i].g, balls[i].b) == color)
            return 1;
    }
    return 0;
}

int getValidRandomColor(Ball* balls, int count) {
    if (GAME_COLOR_COUNT <= 0) return 0;
    if (count == 0) return rand() % GAME_COLOR_COUNT;
    int color, attempts = 0;
    do {
        color = rand() % GAME_COLOR_COUNT;
        attempts++;
        if (attempts > 10) break;
    } while (!isColorInChain(balls, count, color));
    return color;
}

// УЛУЧШЕННЫЕ ПУТИ - теперь вокруг лягушки в центре
PathPoint path[PATH_POINTS];
float totalPathLength = 0.0f;

void buildSpiralPath() {
    // Спираль снаружи к центру (к лягушке)
    float centerX = 0.0f;
    float centerY = 0.0f;
    float startRadius = 350.0f;
    float endRadius = 60.0f;

    for (int i = 0; i < PATH_POINTS; i++) {
        float t = (float)i / (PATH_POINTS - 1);
        float angle = t * 3.14159f * 5.0f;  // 5 оборотов
        float radius = startRadius - t * (startRadius - endRadius);

        path[i].x = centerX + cosf(angle) * radius;
        path[i].y = centerY + sinf(angle) * radius;
    }

    totalPathLength = 0.0f;
    for (int i = 0; i < PATH_POINTS - 1; i++) {
        float dx = path[i + 1].x - path[i].x;
        float dy = path[i + 1].y - path[i].y;
        totalPathLength += sqrtf(dx * dx + dy * dy);
    }
}

void buildWavePath() {
    // S-образный извилистый путь вокруг центра
    for (int i = 0; i < PATH_POINTS; i++) {
        float t = (float)i / (PATH_POINTS - 1);

        // Создаём плавную S-образную кривую, которая огибает центр
        float angle = t * 3.14159f * 4.0f;  // 4 волны
        float radius = 320.0f - t * 260.0f;  // От 320 до 60

        // Добавляем волнистость
        float wave = sinf(t * 3.14159f * 8.0f) * 30.0f;
        float effectiveRadius = radius + wave;

        // Не подходим слишком близко к центру
        if (effectiveRadius < 80.0f) effectiveRadius = 80.0f;

        path[i].x = cosf(angle) * effectiveRadius;
        path[i].y = sinf(angle) * effectiveRadius * 0.9f;  // Слегка сплющиваем
    }

    totalPathLength = 0.0f;
    for (int i = 0; i < PATH_POINTS - 1; i++) {
        float dx = path[i + 1].x - path[i].x;
        float dy = path[i + 1].y - path[i].y;
        totalPathLength += sqrtf(dx * dx + dy * dy);
    }
}

void buildFigureEightPath() {
    // Красивая двойная спираль вокруг центра (как в оригинальной Zuma)
    for (int i = 0; i < PATH_POINTS; i++) {
        float t = (float)i / (PATH_POINTS - 1);

        // Создаём двойную спираль с постепенным сужением
        float angle = t * 3.14159f * 7.0f;  // Много витков
        float radius = 340.0f - t * 280.0f;  // От 340 до 60

        // Добавляем вторую спираль (создаёт эффект восьмёрки)
        float secondSpiral = sinf(t * 3.14159f * 12.0f) * 25.0f;
        float effectiveRadius = radius + secondSpiral;

        // Не подходим слишком близко
        if (effectiveRadius < 70.0f) effectiveRadius = 70.0f;

        path[i].x = cosf(angle) * effectiveRadius;
        path[i].y = sinf(angle) * effectiveRadius;
    }

    totalPathLength = 0.0f;
    for (int i = 0; i < PATH_POINTS - 1; i++) {
        float dx = path[i + 1].x - path[i].x;
        float dy = path[i + 1].y - path[i].y;
        totalPathLength += sqrtf(dx * dx + dy * dy);
    }
}

void buildPathForLevel(int level) {
    switch (level) {
    case 0: buildSpiralPath(); break;
    case 1: buildWavePath(); break;
    case 2: buildFigureEightPath(); break;
    default: buildSpiralPath(); break;
    }
}

float segmentLength(PathPoint a, PathPoint b) {
    float dx = b.x - a.x, dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

void getPathPointByDistance(float dist, float* x, float* y) {
    if (!x || !y) return;
    float traveled = 0.0f;
    for (int i = 0; i < PATH_POINTS - 1; i++) {
        float segLen = segmentLength(path[i], path[i + 1]);
        if (traveled + segLen >= dist) {
            float t = (segLen > 0.0f) ? (dist - traveled) / segLen : 0.0f;
            *x = path[i].x + t * (path[i + 1].x - path[i].x);
            *y = path[i].y + t * (path[i + 1].y - path[i].y);
            return;
        }
        traveled += segLen;
    }
    *x = path[PATH_POINTS - 1].x;
    *y = path[PATH_POINTS - 1].y;
}

void getPathDirection(float dist, float* dx, float* dy) {
    if (!dx || !dy) return;
    float x1, y1, x2, y2;
    getPathPointByDistance(dist, &x1, &y1);
    getPathPointByDistance(dist + 1.0f, &x2, &y2);
    *dx = x2 - x1;
    *dy = y2 - y1;
    float len = sqrtf((*dx) * (*dx) + (*dy) * (*dy));
    if (len > 0.0f) {
        *dx /= len;
        *dy /= len;
    }
}

// УЛУЧШЕННАЯ логика коллизии
int checkShotCollision(Ball* balls, int count, ShotBall* shot, int* insertBefore) {
    if (!balls || !shot || count <= 0) return -1;

    float minDist = 999999.0f;
    int closestIndex = -1;

    for (int i = 0; i < count; i++) {
        float dx = balls[i].x - shot->x;
        float dy = balls[i].y - shot->y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < balls[i].radius + 18.0f && dist < minDist) {
            minDist = dist;
            closestIndex = i;
        }
    }

    if (closestIndex >= 0 && insertBefore) {
        // Определяем, вставить до или после шара
        float dirX, dirY;
        getPathDirection(balls[closestIndex].distance, &dirX, &dirY);

        float toShotX = shot->x - balls[closestIndex].x;
        float toShotY = shot->y - balls[closestIndex].y;

        // Проекция на направление пути
        float projection = toShotX * dirX + toShotY * dirY;

        *insertBefore = (projection > 0) ? 0 : 1;  // 0 = после, 1 = до
    }

    return closestIndex;
}

int insertBall(Ball* balls, int* ballCount, int hitIndex, ShotBall* shot, int insertBefore) {
    if (!balls || !ballCount || !shot) return hitIndex;
    if (*ballCount >= MAX_BALLS) return hitIndex;
    if (hitIndex < 0 || hitIndex >= *ballCount) return hitIndex;

    int insertPos = insertBefore ? hitIndex : hitIndex + 1;

    // Сдвигаем шары
    for (int i = *ballCount; i > insertPos; i--)
        balls[i] = balls[i - 1];

    // Вставляем новый шар
    balls[insertPos].radius = 18.0f;
    getColorByIndex(shot->color, &balls[insertPos].r, &balls[insertPos].g, &balls[insertPos].b);

    // Вычисляем позицию на пути
    if (insertPos == 0) {
        if (*ballCount > 0)
            balls[0].distance = balls[1].distance + BALL_SPACING;
        else
            balls[0].distance = 0.0f;
    }
    else if (insertPos == *ballCount) {
        balls[insertPos].distance = balls[insertPos - 1].distance - BALL_SPACING;
    }
    else {
        balls[insertPos].distance = (balls[insertPos - 1].distance + balls[insertPos + 1].distance) / 2.0f;
    }

    (*ballCount)++;

    // Корректируем расстояния
    for (int i = insertPos + 1; i < *ballCount; i++) {
        float target = balls[i - 1].distance - BALL_SPACING;
        if (balls[i].distance > target)
            balls[i].distance = target;
    }

    return insertPos;
}

int removeMatches(Ball* balls, int* ballCount, int index) {
    if (!balls || !ballCount) return 0;
    if (index < 0 || index >= *ballCount) return 0;

    int left = index, right = index;
    int centerColor = getColorIndex(balls[index].r, balls[index].g, balls[index].b);

    while (left - 1 >= 0 && getColorIndex(balls[left - 1].r, balls[left - 1].g, balls[left - 1].b) == centerColor)
        left--;
    while (right + 1 < *ballCount && getColorIndex(balls[right + 1].r, balls[right + 1].g, balls[right + 1].b) == centerColor)
        right++;

    int removeCount = right - left + 1;
    if (removeCount < 3) return 0;

    for (int i = left; i <= right; i++)
        spawnParticles(balls[i].x, balls[i].y, balls[i].r, balls[i].g, balls[i].b, 8);

    int newCount = *ballCount - removeCount;
    for (int i = left; i < newCount; i++) {
        balls[i] = balls[i + removeCount];
    }
    *ballCount = newCount;

    return removeCount;
}

int currentBallColor = 0, nextBallColor = 1;
int chainCollapsing = 0;
const float COLLAPSE_SPEED = 220.0f;
Frog frog = { 0.0f, 0.0f, 0.0f };
ShotBall shot = { 0 };
float shotCooldown = 0.0f;
const float SHOT_DELAY = 0.35f, SHOT_SPEED = 280.0f;

// УЛУЧШЕННЫЙ фон
void drawBackground() {
    // Градиентный фон
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.15f, 0.1f, 0.25f);
    glVertex2f(0, 0);
    glColor3f(0.05f, 0.05f, 0.15f);
    for (int i = 0; i <= 64; i++) {
        float angle = 2.0f * 3.1415926f * i / 64;
        glVertex2f(cosf(angle) * 600, sinf(angle) * 600);
    }
    glEnd();

    // Звёзды на фоне
    srand(12345);  // Фиксированный seed для постоянных звёзд
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100; i++) {
        float x = (rand() % 1000) - 500;
        float y = (rand() % 800) - 400;
        float brightness = 0.3f + (rand() % 100) / 200.0f;
        glColor3f(brightness, brightness, brightness);
        glVertex2f(x, y);
    }
    glEnd();
    glPointSize(1.0f);
    srand((unsigned int)time(NULL));  // Восстанавливаем случайность

    if (currentState == GAME_STATE_PLAYING || currentState == GAME_STATE_PAUSED) {
        // Подсветка пути
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.2f, 0.2f, 0.4f, 0.3f);
        glLineWidth(12.0f);
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < PATH_POINTS; j++)
            glVertex2f(path[j].x, path[j].y);
        glEnd();
        glDisable(GL_BLEND);

        // Основная линия пути
        glColor3f(0.25f, 0.25f, 0.35f);
        glLineWidth(6.0f);
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < PATH_POINTS; j++)
            glVertex2f(path[j].x, path[j].y);
        glEnd();
        glLineWidth(1.0f);
    }
}

void drawFrog(float x, float y, float angle) {
    glLoadIdentity();
    glTranslatef(x, y, 0);
    glRotatef(angle * 57.2958f, 0, 0, 1);

    // Тень
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(2, -4);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= 32; i++) {
        float a = 2.0f * 3.1415926f * i / 32;
        glVertex2f(2 + cosf(a) * 35, -4 + sinf(a) * 20);
    }
    glEnd();
    glDisable(GL_BLEND);

    // Основное тело (овальное)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.25f, 0.75f, 0.3f);
    glVertex2f(0, 0);
    glColor3f(0.2f, 0.65f, 0.25f);
    for (int i = 0; i <= 32; i++) {
        float a = 2.0f * 3.1415926f * i / 32;
        glVertex2f(cosf(a) * 28, sinf(a) * 22);
    }
    glEnd();

    // Пятна на спине
    glColor3f(0.15f, 0.55f, 0.2f);
    drawCircle(-10, -3, 5, 16);
    drawCircle(8, -5, 4, 16);
    drawCircle(-3, -8, 4, 16);
    drawCircle(12, 2, 3, 16);

    // Светлое брюшко
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.7f, 0.9f, 0.7f, 0.8f);
    glVertex2f(0, -8);
    glColor4f(0.6f, 0.85f, 0.65f, 0.6f);
    for (int i = 0; i <= 16; i++) {
        float a = 3.1415926f + (3.1415926f * i / 16.0f);
        glVertex2f(cosf(a) * 18, -8 + sinf(a) * 12);
    }
    glEnd();
    glDisable(GL_BLEND);

    // Передние лапы
    glColor3f(0.22f, 0.7f, 0.27f);
    // Левая лапа
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-18, -5);
    for (int i = 0; i <= 12; i++) {
        float a = -2.0f + (4.0f * i / 12.0f);
        glVertex2f(-18 + cosf(a) * 8, -5 + sinf(a) * 8);
    }
    glEnd();

    // Пальцы левой лапы
    glColor3f(0.2f, 0.65f, 0.25f);
    drawCircle(-23, -8, 3, 12);
    drawCircle(-20, -10, 3, 12);
    drawCircle(-17, -9, 3, 12);

    // Правая лапа
    glColor3f(0.22f, 0.7f, 0.27f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(18, -5);
    for (int i = 0; i <= 12; i++) {
        float a = -2.0f + (4.0f * i / 12.0f);
        glVertex2f(18 + cosf(a) * 8, -5 + sinf(a) * 8);
    }
    glEnd();

    // Пальцы правой лапы
    glColor3f(0.2f, 0.65f, 0.25f);
    drawCircle(23, -8, 3, 12);
    drawCircle(20, -10, 3, 12);
    drawCircle(17, -9, 3, 12);

    // Задние лапы (сложенные)
    glColor3f(0.22f, 0.7f, 0.27f);
    // Левая задняя
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-22, 8);
    for (int i = 0; i <= 12; i++) {
        float a = 0.5f + (2.5f * i / 12.0f);
        glVertex2f(-22 + cosf(a) * 10, 8 + sinf(a) * 10);
    }
    glEnd();

    // Правая задняя
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(22, 8);
    for (int i = 0; i <= 12; i++) {
        float a = 0.5f + (2.5f * i / 12.0f);
        glVertex2f(22 + cosf(a) * 10, 8 + sinf(a) * 10);
    }
    glEnd();

    // Голова (овальная)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.28f, 0.8f, 0.32f);
    glVertex2f(0, 10);
    glColor3f(0.25f, 0.75f, 0.3f);
    for (int i = 0; i <= 32; i++) {
        float a = 2.0f * 3.1415926f * i / 32;
        glVertex2f(cosf(a) * 24, 10 + sinf(a) * 18);
    }
    glEnd();

    // Большие глаза
    // Левый глаз (выпуклый)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.3f, 0.85f, 0.35f);
    glVertex2f(-12, 20);
    glColor3f(0.25f, 0.75f, 0.3f);
    for (int i = 0; i <= 16; i++) {
        float a = 2.0f * 3.1415926f * i / 16;
        glVertex2f(-12 + cosf(a) * 10, 20 + sinf(a) * 10);
    }
    glEnd();

    // Белок левого глаза
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-12, 20, 8, 20);

    // Зрачок левого глаза (следит за курсором)
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-12 + cosf(angle) * 3, 20 + sinf(angle) * 3, 5, 16);

    // Блик в левом глазу
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(-13 + cosf(angle) * 3, 22 + sinf(angle) * 3, 2, 8);

    // Правый глаз (выпуклый)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.3f, 0.85f, 0.35f);
    glVertex2f(12, 20);
    glColor3f(0.25f, 0.75f, 0.3f);
    for (int i = 0; i <= 16; i++) {
        float a = 2.0f * 3.1415926f * i / 16;
        glVertex2f(12 + cosf(a) * 10, 20 + sinf(a) * 10);
    }
    glEnd();

    // Белок правого глаза
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(12, 20, 8, 20);

    // Зрачок правого глаза
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(12 + cosf(angle) * 3, 20 + sinf(angle) * 3, 5, 16);

    // Блик в правом глазу
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(11 + cosf(angle) * 3, 22 + sinf(angle) * 3, 2, 8);

    // Ноздри
    glColor3f(0.15f, 0.45f, 0.2f);
    drawCircle(-6, 14, 2, 12);
    drawCircle(6, 14, 2, 12);

    // Рот (улыбка)
    glColor3f(0.15f, 0.45f, 0.2f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; i++) {
        float t = i / 12.0f;
        float xx = -10 + t * 20;
        float yy = 8 - sinf(t * 3.14159f) * 4;
        glVertex2f(xx, yy);
    }
    glEnd();
    glLineWidth(1.0f);

    // Язык (труба для стрельбы)
    glColor3f(0.25f, 0.75f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-3, 5);
    glVertex2f(3, 5);
    glVertex2f(4, -20);
    glVertex2f(-4, -20);
    glEnd();

    // Кончик языка/трубы
    glColor3f(0.2f, 0.65f, 0.25f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, -20);
    for (int i = 0; i <= 16; i++) {
        float a = 2.0f * 3.1415926f * i / 16;
        glVertex2f(cosf(a) * 5, -20 + sinf(a) * 5);
    }
    glEnd();

    // Контур для объёма
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.15f, 0.55f, 0.2f, 0.4f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 32; i++) {
        float a = 2.0f * 3.1415926f * i / 32;
        glVertex2f(cosf(a) * 28, sinf(a) * 22);
    }
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);

    glLoadIdentity();
}

void drawGameUI() {
    glColor3f(1.0f, 1.0f, 0.3f);
    drawText(-480, 370, "SCORE", 1.5f);
    glColor3f(0.3f, 1.0f, 0.3f);
    drawNumber(-380, 370, score, 1.8f);

    if (combo > 1) {
        float comboAlpha = 1.0f - (comboTimer / COMBO_TIMEOUT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.0f, 0.0f, 0.0f, comboAlpha * 0.7f);
        drawRoundedRect(-100, 340, 200, 50, 10);

        glColor4f(1.0f, 0.8f, 0.0f, comboAlpha);
        drawText(-70, 365, "COMBO", 1.5f);
        glColor4f(1.0f, 0.5f, 0.0f, comboAlpha);
        char comboStr[20];
        snprintf(comboStr, sizeof(comboStr), "x%d", combo);
        drawText(60, 365, comboStr, 2.0f);
        glDisable(GL_BLEND);
    }

    if (gBallCount > 0) {
        float progress = (totalPathLength > 0.0f) ? (gBalls[gBallCount - 1].distance / totalPathLength) : 0.0f;
        float barWidth = 400, barHeight = 30;
        float barX = -barWidth / 2, barY = 320;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        drawRoundedRect(barX + 3, barY - 3, barWidth, barHeight, 12);
        glDisable(GL_BLEND);

        glColor3f(0.15f, 0.15f, 0.2f);
        drawRoundedRect(barX, barY, barWidth, barHeight, 12);

        if (progress > 0.7f)
            glColor3f(1.0f, 0.2f, 0.2f);
        else if (progress > 0.4f)
            glColor3f(1.0f, 0.8f, 0.2f);
        else
            glColor3f(0.2f, 1.0f, 0.4f);

        if (progress > 0.01f)
            drawRoundedRect(barX, barY, barWidth * progress, barHeight, 12);

        glColor3f(0.5f, 0.5f, 0.6f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 40; i++) {
            float angle = 6.2832f * i / 40.0f;
            glVertex2f(barX + barWidth / 2 + cosf(angle) * (barWidth / 2),
                barY + barHeight / 2 + sinf(angle) * (barHeight / 2));
        }
        glEnd();
        glLineWidth(1.0f);

        glColor3f(1.0f, 1.0f, 1.0f);
        drawNumber(-15, 328, (int)(progress * 100), 1.2f);
        drawText(15, 328, "%", 1.2f);
    }

    if (currentState == GAME_STATE_PAUSED) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        drawRoundedRect(-150, 200, 300, 80, 15);

        glColor4f(1.0f, 1.0f, 0.3f, 1.0f);
        float pausedWidth = 6 * 12 * 2.0f;
        drawText(-pausedWidth / 2, 240, "PAUSED", 2.0f);

        glColor4f(0.8f, 0.8f, 1.0f, 1.0f);
        float resumeWidth = 17 * 12 * 0.8f;
        drawText(-resumeWidth / 2, 210, "Press P to resume", 0.8f);

        glDisable(GL_BLEND);
    }
}

void drawHintBar() {
    if (currentHint.active) {
        float alpha = 1.0f;
        if (currentHint.displayTime > currentHint.maxTime - 0.5f)
            alpha = (currentHint.maxTime - currentHint.displayTime) / 0.5f;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float boxWidth = 600, boxHeight = 60;
        float boxX = -boxWidth / 2, boxY = -360;

        glColor4f(0.0f, 0.0f, 0.0f, alpha * 0.6f);
        drawRoundedRect(boxX + 4, boxY - 4, boxWidth, boxHeight, 12);

        glColor4f(0.05f, 0.05f, 0.15f, alpha * 0.95f);
        drawRoundedRect(boxX, boxY, boxWidth, boxHeight, 12);

        glColor4f(0.4f, 0.7f, 1.0f, alpha);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 40; i++) {
            float angle = 6.2832f * i / 40.0f;
            glVertex2f(boxX + boxWidth / 2 + cosf(angle) * (boxWidth / 2),
                boxY + boxHeight / 2 + sinf(angle) * (boxHeight / 2));
        }
        glEnd();
        glLineWidth(1.0f);

        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        float textWidth = strlen(currentHint.text) * 12 * 1.3f;
        drawText(boxX + (boxWidth - textWidth) / 2, boxY + 25, currentHint.text, 1.3f);

        glDisable(GL_BLEND);
    }
}

void initGame() {
    if (currentLevel < 0 || currentLevel >= 3) currentLevel = 0;

    gBallCount = levels[currentLevel].initialBalls;
    if (gBallCount > MAX_BALLS) gBallCount = MAX_BALLS;

    CHAIN_SPEED = levels[currentLevel].chainSpeed;
    GAME_COLOR_COUNT = levels[currentLevel].colorCount;

    buildPathForLevel(currentLevel);

    for (int i = 0; i < gBallCount; i++) {
        gBalls[i].radius = 18.0f;
        int color = rand() % GAME_COLOR_COUNT;
        getColorByIndex(color, &gBalls[i].r, &gBalls[i].g, &gBalls[i].b);
        gBalls[i].distance = -i * BALL_SPACING;
    }

    currentBallColor = getValidRandomColor(gBalls, gBallCount);
    nextBallColor = getValidRandomColor(gBalls, gBallCount);

    score = 0;
    combo = 0;
    maxCombo = 0;
    totalShots = 0;
    successfulShots = 0;
    comboTimer = 0.0f;
    gameWon = 0;
    gameLost = 0;
    gameOverTimer = 0.0f;
    chainCollapsing = 0;
    shot.active = 0;

    char welcomeText[150];
    snprintf(welcomeText, sizeof(welcomeText), "%s level - Press H for hints! Press P to pause!", levels[currentLevel].name);
    showHint(welcomeText, 5.0f);
}

// УЛУЧШЕННОЕ МЕНЮ с анимацией
void renderMenu(float dt) {
    drawBackground();

    float time = (float)glfwGetTime();
    float titlePulse = 1.0f + sinf(time * 3.0f) * 0.15f;

    // Тень заголовка
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    float titleWidth = 4 * 12 * 3.0f * titlePulse;  // "ZUMA" = 4 буквы
    drawText(-titleWidth / 2 + 5, 275, "ZUMA", 3.0f * titlePulse);
    glDisable(GL_BLEND);

    // Заголовок с градиентом
    glColor3f(0.2f + sinf(time) * 0.2f, 0.8f, 1.0f);
    drawText(-titleWidth / 2, 280, "ZUMA", 3.0f * titlePulse);

    glColor3f(0.7f, 0.7f, 0.9f);
    float subtitleWidth = 16 * 12 * 1.3f;  // "ENHANCED EDITION"
    drawText(-subtitleWidth / 2, 220, "ENHANCED EDITION", 1.3f);

    // Вращающиеся шары вокруг
    for (int i = 0; i < 12; i++) {
        float angle = time * 0.5f + i * (6.2832f / 12.0f);
        float radius = 280.0f + sinf(time * 2.0f + i) * 20.0f;
        float x = cosf(angle) * radius;
        float y = sinf(angle * 0.8f) * 180 + 50;
        float r, g, b;
        getColorByIndex(i % 5, &r, &g, &b);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawShadow(x, y, 18, 24);
        glDisable(GL_BLEND);

        drawGradientCircle(x, y, 18, r, g, b, 24);
    }

    // Кнопки
    for (int i = 0; i < buttonCount; i++)
        drawButton(&menuButtons[i], dt);

    // Инструкции
    glColor3f(0.5f, 0.5f, 0.6f);
    drawText(-180, -340, "CLICK - SELECT", 1.0f);
    drawText(-180, -365, "H - HINT IN GAME", 1.0f);
    drawText(-180, -390, "P - PAUSE", 1.0f);
    drawText(-180, -415, "ESC - BACK", 1.0f);
}

void renderLevelSelect(float dt) {
    drawBackground();

    glColor3f(0.3f, 0.9f, 1.0f);
    float levelTitleWidth = 12 * 12 * 2.2f;  // "SELECT LEVEL"
    drawText(-levelTitleWidth / 2, 300, "SELECT LEVEL", 2.2f);

    for (int i = 0; i < 3; i++) {
        Button* btn = &levelButtons[i];
        drawButton(btn, dt);

        // Звёзды сложности
        for (int s = 0; s <= i; s++) {
            float sx = btn->x + btn->width / 2 - (i + 1) * 15 + s * 30;
            float sy = btn->y + btn->height - 50;

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
            drawCircle(sx + 2, sy - 2, 10, 5);
            glDisable(GL_BLEND);

            glColor3f(1.0f, 0.9f, 0.2f);
            drawCircle(sx, sy, 10, 5);
        }

        glColor3f(0.8f, 0.8f, 0.9f);
        char desc[50];
        snprintf(desc, sizeof(desc), "%d balls", levels[i].initialBalls);
        float textWidth1 = strlen(desc) * 12 * 0.9f;
        drawText(btn->x + (btn->width - textWidth1) / 2, btn->y + 30, desc, 0.9f);

        snprintf(desc, sizeof(desc), "Speed x%.1f", levels[i].chainSpeed / 35.0f);
        float textWidth2 = strlen(desc) * 12 * 0.9f;
        drawText(btn->x + (btn->width - textWidth2) / 2, btn->y + 10, desc, 0.9f);
    }

    // Кнопка назад
    glColor3f(0.3f, 0.3f, 0.4f);
    drawRoundedRect(-120, -300, 240, 60, 12);

    glColor3f(0.5f, 0.7f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 40; i++) {
        float angle = 6.2832f * i / 40.0f;
        glVertex2f(-120 + 120 + cosf(angle) * 120, -270 + sinf(angle) * 30);
    }
    glEnd();
    glLineWidth(1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    float backWidth = 4 * 12 * 1.8f;  // "BACK"
    drawText(-backWidth / 2, -280, "BACK", 1.8f);
}

void renderGame(float dt) {
    drawBackground();

    // Рисуем портал в конце пути
    drawPortal(path[PATH_POINTS - 1].x, path[PATH_POINTS - 1].y, (float)glfwGetTime());

    // Тени шаров
    for (int i = 0; i < gBallCount; i++)
        drawShadow(gBalls[i].x, gBalls[i].y, gBalls[i].radius, 32);

    // Шары
    for (int i = 0; i < gBallCount; i++)
        drawGradientCircle(gBalls[i].x, gBalls[i].y, gBalls[i].radius,
            gBalls[i].r, gBalls[i].g, gBalls[i].b, 32);

    // Лягушка в центре
    drawFrog(frog.x, frog.y, frog.angle);

    // Текущий шар у лягушки
    if (!shot.active && !gameWon && !gameLost) {
        float ballX = frog.x + cosf(frog.angle) * 25;  // Ближе к лягушке
        float ballY = frog.y + sinf(frog.angle) * 25;
        float r, g, b;
        getColorByIndex(currentBallColor, &r, &g, &b);
        drawShadow(ballX, ballY, 10, 24);
        drawGradientCircle(ballX, ballY, 10, r, g, b, 24);
    }

    // Следующий шар
    if (!gameWon && !gameLost) {
        float r, g, b;
        getColorByIndex(nextBallColor, &r, &g, &b);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.1f, 0.1f, 0.2f, 0.7f);
        drawCircle(frog.x - 50, frog.y - 50, 15, 24);
        glDisable(GL_BLEND);

        drawShadow(frog.x - 50, frog.y - 50, 10, 24);
        drawGradientCircle(frog.x - 50, frog.y - 50, 10, r, g, b, 24);

        glColor3f(0.7f, 0.7f, 0.8f);
        drawText(frog.x - 70, frog.y - 70, "NEXT", 0.6f);
    }

    // Выстрел
    if (shot.active) {
        float r, g, b;
        getColorByIndex(shot.color, &r, &g, &b);
        drawShadow(shot.x, shot.y, 18, 32);
        drawGradientCircle(shot.x, shot.y, 18, r, g, b, 32);
    }

    drawParticles();
    drawGameUI();
    drawHintBar();

    // Экран победы/поражения
    if (gameWon) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        drawRoundedRect(-250, -100, 500, 200, 20);

        glColor4f(0.2f, 1.0f, 0.3f, 0.9f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 40; i++) {
            float angle = 6.2832f * i / 40.0f;
            glVertex2f(cosf(angle) * 250, sinf(angle) * 100);
        }
        glEnd();
        glLineWidth(1.0f);
        glDisable(GL_BLEND);

        glColor3f(0.3f, 1.0f, 0.4f);
        float victoryWidth = 8 * 12 * 2.5f;
        drawText(-victoryWidth / 2, 50, "VICTORY!", 2.5f);

        glColor3f(0.8f, 0.8f, 1.0f);
        float finalWidth = 11 * 12 * 1.3f;
        drawText(-finalWidth / 2, 10, "FINAL SCORE", 1.3f);

        glColor3f(1.0f, 1.0f, 0.3f);
        char scoreStr[20];
        snprintf(scoreStr, sizeof(scoreStr), "%d", score);
        float scoreWidth = strlen(scoreStr) * 12 * 2.0f;
        drawNumber(-scoreWidth / 2, -25, score, 2.0f);

        glColor3f(0.7f, 0.9f, 1.0f);
        char stats[50];
        snprintf(stats, sizeof(stats), "Max Combo: x%d", maxCombo);
        float statsWidth1 = strlen(stats) * 12 * 1.0f;
        drawText(-statsWidth1 / 2, -60, stats, 1.0f);

        int accuracy = totalShots > 0 ? (successfulShots * 100 / totalShots) : 0;
        snprintf(stats, sizeof(stats), "Accuracy: %d%%", accuracy);
        float statsWidth2 = strlen(stats) * 12 * 1.0f;
        drawText(-statsWidth2 / 2, -80, stats, 1.0f);
    }
    else if (gameLost) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        drawRoundedRect(-250, -80, 500, 160, 20);

        glColor4f(1.0f, 0.2f, 0.2f, 0.9f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 40; i++) {
            float angle = 6.2832f * i / 40.0f;
            glVertex2f(cosf(angle) * 250, sinf(angle) * 80);
        }
        glEnd();
        glLineWidth(1.0f);
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.3f, 0.3f);
        float gameOverWidth = 9 * 12 * 2.5f;
        drawText(-gameOverWidth / 2, 30, "GAME OVER", 2.5f);

        glColor3f(0.8f, 0.8f, 1.0f);
        float yourScoreWidth = 10 * 12 * 1.3f;
        drawText(-yourScoreWidth / 2, -10, "YOUR SCORE", 1.3f);

        glColor3f(1.0f, 0.8f, 0.3f);
        char scoreStr[20];
        snprintf(scoreStr, sizeof(scoreStr), "%d", score);
        float scoreWidth = strlen(scoreStr) * 12 * 2.0f;
        drawNumber(-scoreWidth / 2, -50, score, 2.0f);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        mx -= WINDOW_WIDTH / 2;
        my = WINDOW_HEIGHT / 2 - my;

        if (currentState == GAME_STATE_MENU) {
            for (int i = 0; i < buttonCount; i++) {
                if (checkButtonHover(&menuButtons[i], mx, my)) {
                    if (i == 0) {
                        currentState = GAME_STATE_LEVEL_SELECT;
                    }
                    else if (i == 1) {
                        glfwSetWindowShouldClose(window, 1);
                    }
                }
            }
        }
        else if (currentState == GAME_STATE_LEVEL_SELECT) {
            for (int i = 0; i < 3; i++) {
                if (checkButtonHover(&levelButtons[i], mx, my)) {
                    currentLevel = i;
                    initGame();
                    currentState = GAME_STATE_PLAYING;
                }
            }
            if (mx >= -120 && mx <= 120 && my >= -300 && my <= -240)
                currentState = GAME_STATE_MENU;
        }
        else if (currentState == GAME_STATE_PLAYING) {
            if (gameWon || gameLost) return;
            if (!shot.active && shotCooldown <= 0.0f) {
                totalShots++;
                float dx = (float)mx - frog.x;
                float dy = (float)my - frog.y;
                float len = sqrtf(dx * dx + dy * dy);
                if (len < 0.001f) return;
                dx /= len;
                dy /= len;
                shot.x = frog.x;
                shot.y = frog.y;
                shot.vx = dx * SHOT_SPEED;
                shot.vy = dy * SHOT_SPEED;
                shot.color = currentBallColor;
                currentBallColor = nextBallColor;
                nextBallColor = getValidRandomColor(gBalls, gBallCount);
                shot.active = 1;
                shotCooldown = SHOT_DELAY;
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            if (currentState == GAME_STATE_PLAYING || currentState == GAME_STATE_PAUSED)
                currentState = GAME_STATE_MENU;
            else if (currentState == GAME_STATE_LEVEL_SELECT)
                currentState = GAME_STATE_MENU;
        }
        else if (key == GLFW_KEY_P) {
            if (currentState == GAME_STATE_PLAYING) {
                currentState = GAME_STATE_PAUSED;
                showHint("Game paused. Press P to resume", 2.0f);
            }
            else if (currentState == GAME_STATE_PAUSED) {
                currentState = GAME_STATE_PLAYING;
                showHint("Game resumed!", 2.0f);
            }
        }
        else if (key == GLFW_KEY_H) {
            if (currentState == GAME_STATE_PLAYING && gBallCount > 0) {
                int lastBallColor = getColorIndex(gBalls[gBallCount - 1].r,
                    gBalls[gBallCount - 1].g,
                    gBalls[gBallCount - 1].b);

                if (lastBallColor == currentBallColor) {
                    showHint("HINT: Shoot now! Current ball matches!", 4.0f);
                }
                else if (lastBallColor == nextBallColor) {
                    showHint("HINT: Wait! Next ball will match!", 4.0f);
                }
                else {
                    showHint("HINT: Create combos by matching 3+ colors!", 4.0f);
                }
            }
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    double mx = xpos - WINDOW_WIDTH / 2;
    double my = WINDOW_HEIGHT / 2 - ypos;

    if (currentState == GAME_STATE_MENU) {
        for (int i = 0; i < buttonCount; i++)
            menuButtons[i].hovered = checkButtonHover(&menuButtons[i], mx, my);
    }
    else if (currentState == GAME_STATE_LEVEL_SELECT) {
        for (int i = 0; i < 3; i++)
            levelButtons[i].hovered = checkButtonHover(&levelButtons[i], mx, my);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        printf("GLFW init failed\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
        "Zuma - Enhanced Edition", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    initParticles();
    initMenuButtons();
    initLevelButtons();

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2,
        -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    float lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        if (dt > 0.1f) dt = 0.1f;

        glClearColor(0.05f, 0.05f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        updateHint(dt);

        if (currentState == GAME_STATE_PLAYING) {
            updateCombo(dt);
        }

        switch (currentState) {
        case GAME_STATE_MENU:
            renderMenu(dt);
            break;
        case GAME_STATE_LEVEL_SELECT:
            renderLevelSelect(dt);
            break;
        case GAME_STATE_PAUSED:
            renderGame(dt);
            break;
        case GAME_STATE_PLAYING:
            if (gBallCount == 0 && !gameWon) {
                gameWon = 1;
                char winText[150];
                snprintf(winText, sizeof(winText), "VICTORY! Score: %d Max Combo: x%d", score, maxCombo);
                showHint(winText, 3.0f);
            }
            if (gBallCount > 0 && gBalls[gBallCount - 1].distance >= totalPathLength && !gameLost) {
                gameLost = 1;
                showHint("DEFEAT! Try again!", 3.0f);
            }
            if (gameWon || gameLost) {
                gameOverTimer += dt;
                if (gameOverTimer > 3.0f)
                    currentState = GAME_STATE_MENU;
            }
            if (shotCooldown > 0.0f)
                shotCooldown -= dt;

            updateParticles(dt);

            if (!gameWon && !gameLost && !chainCollapsing) {
                if (gBallCount > 0) {
                    gBalls[0].distance += CHAIN_SPEED * dt;
                    for (int i = 1; i < gBallCount; i++) {
                        float target = gBalls[i - 1].distance - BALL_SPACING;
                        if (gBalls[i].distance < target)
                            gBalls[i].distance = target;
                    }
                }
            }

            if (chainCollapsing) {
                int done = 1;
                for (int i = 1; i < gBallCount; i++) {
                    float target = gBalls[i - 1].distance - BALL_SPACING;
                    if (gBalls[i].distance < target) {
                        gBalls[i].distance += COLLAPSE_SPEED * dt;
                        if (gBalls[i].distance > target)
                            gBalls[i].distance = target;
                        done = 0;
                    }
                }
                if (done) {
                    chainCollapsing = 0;
                    for (int i = 0; i < gBallCount; i++) {
                        if (removeMatches(gBalls, &gBallCount, i)) {
                            chainCollapsing = 1;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < gBallCount; i++)
                getPathPointByDistance(gBalls[i].distance, &gBalls[i].x, &gBalls[i].y);

            if (!gameWon && !gameLost) {
                double mx, my;
                glfwGetCursorPos(window, &mx, &my);
                mx -= WINDOW_WIDTH / 2;
                my = WINDOW_HEIGHT / 2 - my;
                frog.angle = atan2f((float)my - frog.y, (float)mx - frog.x);
            }

            if (shot.active) {
                shot.x += shot.vx * dt;
                shot.y += shot.vy * dt;

                int insertBefore = 0;
                int hitIndex = checkShotCollision(gBalls, gBallCount, &shot, &insertBefore);

                if (hitIndex != -1) {
                    successfulShots++;
                    int insertPos = insertBall(gBalls, &gBallCount, hitIndex, &shot, insertBefore);
                    int removed = removeMatches(gBalls, &gBallCount, insertPos);
                    if (removed > 0) {
                        chainCollapsing = 1;
                        addScore(removed);
                    }
                    shot.active = 0;
                }
                if (shot.x < -WINDOW_WIDTH / 2 || shot.x > WINDOW_WIDTH / 2 ||
                    shot.y < -WINDOW_HEIGHT / 2 || shot.y > WINDOW_HEIGHT / 2)
                    shot.active = 0;
            }
            renderGame(dt);
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}