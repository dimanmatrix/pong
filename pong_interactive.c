#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define POINTS_TO_WIN 3

#define WIDTH 80
#define HEIGHT 24
#define PADDLE_HEIGHT 10

int fPScore = 0;
int sPScore = 0;
int ballX, ballY;
int ballX_prev = -1, ballY_prev = -1;
int paddleLeft, paddleRight;
int ballDirX, ballDirY;
int gameRunning = 1;
int winningScreen = 0;
int historyMode = 1;

/*
    INTERACTIVE MODE
*/

int kbhit() {
  struct termios oldt, newt;
  int ch, oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }
  return 0;
}

/*
    /INTERACTIVE MODE
*/

void initDisplay();

void setCursor(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }

// Очистка экрана и перемещение курсора в начало
void cleanScreen() {
  printf("\033[2J\033[H");
  setCursor(1, 0);
};

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

// void setSymbol(int x, int y, char symbol) {
//     setCursor(x, y);
//     printf('%c', symbol);
// }

void setup() {
  ballX = WIDTH / 2;
  ballY = HEIGHT / 2;
  paddleLeft = paddleRight = HEIGHT / 2 - PADDLE_HEIGHT / 2;
  ballDirX = ballDirY = 1;
}

void drawVictoryScreen(int winner) {
  cleanScreen();
  initDisplay(1);

  setCursor(WIDTH / 2 - 9, HEIGHT / 2 - 1);
  printf("Игрок %d победил!", winner);

  setCursor(WIDTH / 2 - 10, HEIGHT / 2 + 1);
  printf("Нажми q для выхода"); // или r для перезапуска");

  setCursor(0, HEIGHT + 6); // (возвращаем курсор)
}

// обновлние экрана по принципу только необходимых пикселей
int updateScreen() {
  // Экран победы
  if (fPScore >= POINTS_TO_WIN || sPScore >= POINTS_TO_WIN) {
    if (!winningScreen) {
      winningScreen = 1;
      drawVictoryScreen(fPScore >= POINTS_TO_WIN ? 1 : 2);
    }
    return 0;
  }

  // обновляем счет
  setCursor(WIDTH / 2 - 6, 0);
  printf("  %2d | %2d ", fPScore, sPScore);
  setCursor(0, HEIGHT + 1);

  // обновляем ракетки
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

  // стираем старый мячик
  if (ballX_prev != -1) {
    setCursor(ballX_prev, ballY_prev);
    if (historyMode) {
      printf("*");
    } else
      printf(" ");
  }
  setCursor(ballX, ballY);
  printf("●");

  setCursor(0, HEIGHT + 6); // (пропускаем блок hotkeys и стираем строку ввода)
  for (int x = 0; x < WIDTH; x++) {
    printf(" ");
  }
  setCursor(0, HEIGHT + 6); // (возвращаем курсор)

  return 1;
}

void drawHotkeys() {
  setCursor(0, HEIGHT + 1);
  printf("║            P1            ║        Системные        ║            P2 "
         "          ║\n");
  printf("║                          ║       d - графики       ║               "
         "          ║\n");
  printf("║        a - Вверх         ║       r - рестарт       ║        k - "
         "Вверх        ║\n");
  printf("║        z - Вниз          ║       q - выход         ║        m - "
         "Вниз         ║\n");
  printf("╚══════════════════════════╩═════════════════════════╩═══════════════"
         "══════════╝\n");
}

void initDisplay(int stop_game) {
  // https://www.matematica.pt/en/useful/complete-ascii-table.php

  cleanScreen();
  setCursor(0, 0);

  // Отрисовка верхней границы со счетом
  printf("╔");
  for (int i = 0; i < WIDTH - 2; i++) {
    printf("═");
  }
  printf("╗");

  // Отрисовка нижней границы
  setCursor(0, HEIGHT);
  printf("╠"); // ╚
  for (int i = 0; i < WIDTH - 2; i++)
    printf("═");
  printf("╣"); // ╝

  for (int y = 0; y < HEIGHT; y++) {
    setCursor(0, y + 1);
    printf("║");
    setCursor(WIDTH - 1, y + 1);
    printf("║");
  }
  drawHotkeys();

  if (!stop_game) {
    updateScreen();
  }
}

void updateState() {
  // if (fPScore >= POINTS_TO_WIN || sPScore >= POINTS_TO_WIN) {
  //     return 0;
  // }

  // Отскок мяча от верхней и нижней стенок
  if (ballY <= 1 || ballY >= HEIGHT - 1) {
    ballDirY = -ballDirY;
  }

  // Отскок мяча от левой ракетки
  if (ballX == 2 && ballY + ballDirY <= paddleLeft + PADDLE_HEIGHT &&
      ((ballDirY < 0 && ballY >= paddleLeft - 1) ||
       (ballDirY > 0 && ballY + ballDirY >= paddleLeft))) {
    ballDirX = 1;
    // Изменение направления по Y в зависимости от места удара
    if (ballY + ballDirY == paddleLeft)
      ballDirY = -1;
    else if (ballY + ballDirY == paddleLeft + PADDLE_HEIGHT - 1)
      ballDirY = 1;
  }

  // Отскок мяча от правой ракетки
  if (ballX == WIDTH - 3 && ballY + ballDirY <= paddleRight + PADDLE_HEIGHT &&
      ((ballDirY > 0 && ballY + ballDirY >= paddleRight) ||
       (ballDirY < 0 && ballY + ballDirY >= paddleRight - 1))) {
    ballDirX = -1;
    // Изменение направления по Y в зависимости от места удара
    if (ballY + ballDirY == paddleRight)
      ballDirY = -1;
    else if (ballY + ballDirY == paddleRight + PADDLE_HEIGHT - 1)
      ballDirY = 1;
  }

  ballX_prev = ballX;
  ballY_prev = ballY;
  ballX += ballDirX;
  ballY += ballDirY;

  // Подсчет очков
  if (ballX <= 0) {
    sPScore++;
    setup();
  }
  if (ballX >= WIDTH - 1) {
    fPScore++;
    setup();
  }
}

int handleInput() {
  if (kbhit()) {
    char key = getchar();
    switch (key) {
    case 'a':
      if (paddleLeft > 1)
        paddleLeft--;
      break;
    case 'z':
      if (paddleLeft < HEIGHT - PADDLE_HEIGHT)
        paddleLeft++;
      break;
    case 'k':
      if (paddleRight > 1)
        paddleRight--;
      break;
    case 'm':
      if (paddleRight < HEIGHT - PADDLE_HEIGHT)
        paddleRight++;
      break;
    case ' ':
      break;

    case 'd':
      historyMode = !historyMode;
      if (historyMode == 0)
        cleanHistory(ballX, ballY);
      break;

    case 'r':
      // возможность рестарта только после
      // if (fPScore >= POINTS_TO_WIN || sPScore >= POINTS_TO_WIN) {
      fPScore = sPScore = 0;
      ballX_prev = ballX;
      ballY_prev = ballY;
      winningScreen = 0;
      cleanHistory(ballX, ballY);
      setup();
      //}
      return 0;
    case 'q':
      gameRunning = 0;
      return 0;
    case '\n':
      return 0;
    default:
      return 0;
    }
  }
  return 1;
}

int main() {
  struct termios old_tio, new_tio;
  tcgetattr(STDIN_FILENO, &old_tio);
  new_tio = old_tio;
  new_tio.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

  srand(time(NULL));
  setup();
  initDisplay(0);

  while (gameRunning) {
    if (handleInput() && !winningScreen) {
      updateState();
    }
    if (!updateScreen()) {
      continue;
    };
    usleep(50000); // Задержка для контроля скорости игры
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
  return 0;
}