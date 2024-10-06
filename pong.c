#include <stdio.h>

#define POINTS_TO_WIN 2
#define WIDTH 80
#define HEIGHT 24
#define PADDLE_HEIGHT 3

void setCursor(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }

void cleanScreen() {
    printf("\033[2J\033[H");
    setCursor(1, 0);
}

void cleanHistory(int ballX, int ballY) {
    for (int y = 1; y < HEIGHT; y++) {
        setCursor(2, y);
        for (int x = 0; x < WIDTH - 4; x++) {
            if (x + 2 == ballX && y == ballY) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void drawVictoryScreen(int winner) {
    cleanScreen();
    setCursor(WIDTH / 2 - 9, HEIGHT / 2 - 1);
    printf("Игрок %d победил!", winner);
    setCursor(WIDTH / 2 - 10, HEIGHT / 2 + 1);
    printf("Нажми q для выхода");
    setCursor(0, HEIGHT + 6);
}

void drawHotkeys() {
    setCursor(0, HEIGHT + 1);
    printf("║            P1            ║        Системные        ║            P2           ║\n");
    printf("║                          ║       d - траектория    ║                         ║\n");
    printf("║        a - Вверх         ║       r - рестарт       ║        k - Вверх        ║\n");
    printf("║        z - Вниз          ║       q - выход         ║        m - Вниз         ║\n");
    printf("╚══════════════════════════╩═════════════════════════╩═════════════════════════╝\n");
}

void initDisplay() {
    cleanScreen();
    setCursor(0, 0);
    printf("╔");
    for (int i = 0; i < WIDTH - 2; i++) {
        printf("═");
    }
    printf("╗");
    setCursor(0, HEIGHT);
    printf("╠");
    for (int i = 0; i < WIDTH - 2; i++) printf("═");
    printf("╣");
    for (int y = 0; y < HEIGHT; y++) {
        setCursor(0, y + 1);
        printf("║");
        setCursor(WIDTH - 1, y + 1);
        printf("║");
    }
    drawHotkeys();
}

int updateScreen(int fPScore, int sPScore, int ballX, int ballY, int ballX_prev, int ballY_prev,
                 int paddleLeft, int paddleRight, int historyMode) {
    int exitStatus;
    if (fPScore >= POINTS_TO_WIN || sPScore >= POINTS_TO_WIN) {
        drawVictoryScreen(fPScore >= POINTS_TO_WIN ? 1 : 2);
        exitStatus = 0;
    } else {
        exitStatus = 1;
        setCursor(WIDTH / 2 - 6, 0);
        printf("  %2d | %2d ", fPScore, sPScore);
        setCursor(0, HEIGHT + 1);

        for (int y = 1; y < HEIGHT; y++) {
            setCursor(1, y);
            if (y >= paddleLeft && y < paddleLeft + PADDLE_HEIGHT) {
                printf("█");
            } else {
                printf(" ");
            }
            setCursor(WIDTH - 2, y);
            if (y >= paddleRight && y < paddleRight + PADDLE_HEIGHT) {
                printf("█");
            } else {
                printf(" ");
            }
        }

        if (ballX_prev != -1) {
            setCursor(ballX_prev, ballY_prev);
            if (historyMode) {
                printf("*");
            } else
                printf(" ");
        }
        setCursor(ballX, ballY);
        printf("●");

        setCursor(0, HEIGHT + 6);
        for (int x = 0; x < WIDTH; x++) {
            printf(" ");
        }
        setCursor(0, HEIGHT + 6);
    }

    return exitStatus;
}

char getInput() {
    // считываем только первый символ из ввода и ждём Enter
    char c1 = getchar();
    char c2 = getchar();
    while (c2 != '\n') {
        c2 = getchar();
    }
    return c1;
}

int main() {
    int fPScore = 0, sPScore = 0;
    int ballX = WIDTH / 2, ballY = HEIGHT / 2;
    int ballX_prev = -1, ballY_prev = -1;
    int paddleLeft = HEIGHT / 2 - PADDLE_HEIGHT / 2, paddleRight = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    int ballDirX = 1, ballDirY = 1;
    int historyMode = 0;
    int gameRunning = 1;
    int justRestarted = 0;

    initDisplay();
    updateScreen(fPScore, sPScore, ballX, ballY, ballX_prev, ballY_prev, paddleLeft, paddleRight,
                 historyMode);

    while (gameRunning) {
        char key = getInput();

        switch (key) {
            case 'a':
                if (paddleLeft > 1) paddleLeft--;
                break;
            case 'z':
                if (paddleLeft < HEIGHT - PADDLE_HEIGHT) paddleLeft++;
                break;
            case 'k':
                if (paddleRight > 1) paddleRight--;
                break;
            case 'm':
                if (paddleRight < HEIGHT - PADDLE_HEIGHT) paddleRight++;
                break;
            case 'd':
                historyMode = !historyMode;
                if (historyMode == 0) cleanHistory(ballX, ballY);
                break;
            case 'r':
                fPScore = sPScore = 0;
                ballX_prev = ballX;
                ballY_prev = ballY;
                justRestarted = 1;
                ballX = WIDTH / 2;
                ballY = HEIGHT / 2;
                paddleLeft = paddleRight = HEIGHT / 2 - PADDLE_HEIGHT / 2;
                ballDirX = ballDirY = 1;
                break;
            case 'q':
                gameRunning = 0;
                continue;
        }

        if (ballY <= 1 || ballY >= HEIGHT - 1) {
            ballDirY = -ballDirY;
        }

        if (ballX == 2 && ballY + ballDirY >= paddleLeft && ballY + ballDirY < paddleLeft + PADDLE_HEIGHT) {
            ballDirX = 1;
            if (ballY + ballDirY == paddleLeft)
                ballDirY = -1;
            else if (ballY + ballDirY == paddleLeft + PADDLE_HEIGHT - 1)
                ballDirY = 1;
        }

        if (ballX == WIDTH - 3 && ballY + ballDirY >= paddleRight &&
            ballY + ballDirY < paddleRight + PADDLE_HEIGHT) {
            ballDirX = -1;
            if (ballY + ballDirY == paddleRight)
                ballDirY = -1;
            else if (ballY + ballDirY == paddleRight + PADDLE_HEIGHT - 1)
                ballDirY = 1;
        }

        if (!justRestarted) {
            ballX_prev = ballX;
            ballY_prev = ballY;
            ballX += ballDirX;
            ballY += ballDirY;
        } else {
            justRestarted = 0;
        }

        if (ballX <= 0) {
            sPScore++;
            ballX = WIDTH / 2;
            ballY = HEIGHT / 2;
        }
        if (ballX >= WIDTH - 1) {
            fPScore++;
            ballDirX = -ballDirX;
            ballX = WIDTH / 2;
            ballY = HEIGHT / 2;
        }

        if (!updateScreen(fPScore, sPScore, ballX, ballY, ballX_prev, ballY_prev, paddleLeft, paddleRight,
                          historyMode)) {
            continue;
        }
    }
    return 0;
}