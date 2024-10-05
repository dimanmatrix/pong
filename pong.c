#include <stdio.h>

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_HEIGHT 3

int fPScore = 0;
int sPScore = 0;
int ballX, ballY;
int paddleLeft, paddleRight;
int ballDirX, ballDirY;
int gameRunning = 1;

void setup() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    paddleLeft = paddleRight = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ballDirX = ballDirY = 1;
}

void draw() {
    // Очистка экрана и перемещение курсора в начало
    printf("\033[2J\033[H");  
    
    // Отрисовка верхней границы со счетом
    printf("╔");
    for (int i = 0; i < WIDTH - 1; i++) {
        if (i == WIDTH / 2 - 5) {
            printf(" %2d | %2d ", fPScore, sPScore);
            i += 9;
        } else {
            printf("═");
        }
    }
    printf("╗\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < WIDTH - 2; x++) {
            if (x == ballX && y == ballY)
                printf("●");
            else if (x == 1 && y >= paddleLeft && y < paddleLeft + PADDLE_HEIGHT)
                printf("█");
            else if (x == WIDTH - 4 && y >= paddleRight && y < paddleRight + PADDLE_HEIGHT)
                printf("█");
            else
                printf(" ");
        }
        printf("║\n");
    }

    // Отрисовка нижней границы
    printf("╚");
    for (int i = 0; i < WIDTH - 2; i++)
        printf("═");
    printf("╝\n");
}

void update() {
    ballX += ballDirX;
    ballY += ballDirY;

    // Отскок мяча от верхней и нижней стенок
    if (ballY <= 0 || ballY >= HEIGHT - 1)
        ballDirY = -ballDirY;

    // Отскок мяча от ракеток
    if (ballX == 2 && ballY >= paddleLeft && ballY < paddleLeft + PADDLE_HEIGHT)
        ballDirX = -ballDirX;
    if (ballX == WIDTH - 5 && ballY >= paddleRight && ballY < paddleRight + PADDLE_HEIGHT)
        ballDirX = -ballDirX;

    // Подсчет очков
    if (ballX <= 0) {
        sPScore++;
        setup();
    }
    if (ballX >= WIDTH - 2) {
        fPScore++;
        setup();
    }
}

void handleInput() {
    char key = getchar();
    switch (key) {
        case 'a': if (paddleLeft > 0) paddleLeft--; break;
        case 'z': if (paddleLeft < HEIGHT - PADDLE_HEIGHT) paddleLeft++; break;
        case 'k': if (paddleRight > 0) paddleRight--; break;
        case 'm': if (paddleRight < HEIGHT - PADDLE_HEIGHT) paddleRight++; break;
        case 'q': gameRunning = 0; break;
        default: break;
    }
}

int main() {
    setup();
    while (gameRunning) {
        draw();
        update();
        handleInput();
    }
    return 0;
}

/*
    #include <stdlib.h> для  system("clear"); и exit(0);
*/