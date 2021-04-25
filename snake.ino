#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

const int PIN_RESET = 3; // LCD1 Reset
const int PIN_SCE = 4; // LCD2 Chip Select
const int PIN_DC = 5; // LCD3 Dat/Command
const int PIN_SDIN = 6; // LCD4 Data in
const int PIN_SCLK = 7; // LCD5 Clk
// LCD6 Vcc
// LCD7 Vled
// LCD8 Gnd

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_SCLK, PIN_SDIN, PIN_DC, PIN_SCE, PIN_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

bool gameover;

const int width = 42;
const int height = 24;

int x, y, fruitX, fruitY, score, countDown;

int tailX[100], tailY[100]; //snake coordinates

int nTail;

enum eDirecton {
  STOP = 0, LEFT, RIGHT, UP, DOWN
}; // Controls

eDirecton dir;

//JOYSTICK

#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

const byte rows[] = {
  ROW_1,
  ROW_2,
  ROW_3,
  ROW_4,
  ROW_5,
  ROW_6,
  ROW_7,
  ROW_8
};
const byte col[] = {
  COL_1,
  COL_2,
  COL_3,
  COL_4,
  COL_5,
  COL_6,
  COL_7,
  COL_8
};

int xJoystickPin = A1;
int yJoystickPin = A0;

int xJoystickPosition = 0;
int yJoystickPosition = 0;

void setup() {
  Serial.begin(9600);

  display.begin();

  display.setContrast(50);

  display.clearDisplay();

  resetAll();

  pinMode(xJoystickPin, INPUT);
  pinMode(yJoystickPin, INPUT);
}

void resetAll() {
  gameover = false;

  dir = STOP;

  x = width / 2;
  y = height / 2;

  fruitX = rand() % width;
  fruitY = rand() % height;

  nTail = 0;
  score = 0;
  countDown = 5;
}

void draw() {
  display.clearDisplay();

  display.drawLine(0, 0, 0, display.height() - 1, BLACK);
  display.drawLine(0, 0, display.width() - 1, 0, BLACK);
  display.drawLine(display.width() - 1, 0, display.width() - 1, display.height() - 1, BLACK);
  display.drawLine(0, display.height() - 1, display.width() - 1, display.height() - 1, BLACK);

  for (int i = 0; i < height; i++) {

    for (int j = 0; j < width; j++) {

      if (i == y && j == x) {
        // draw head
        drawSquare(j, i);
      } else if (i == fruitY && j == fruitX) {
        // draw fruit
        drawFruit(j, i);
      } else {
        // draw tail
        for (int k = 0; k < nTail; k++) {
          if (tailX[k] == j && tailY[k] == i) {
            drawSquare(j, i);
          }
        }
      }
    }
  }

  display.display();
}

void input() {
  xJoystickPosition = analogRead(xJoystickPin);
  yJoystickPosition = analogRead(yJoystickPin);

  if (800 < yJoystickPosition && 200 < xJoystickPosition && 800 > xJoystickPosition && dir != LEFT) {
    dir = RIGHT;
  } else if (800 < xJoystickPosition && 200 < yJoystickPosition && 800 > yJoystickPosition && dir != DOWN) {
    dir = UP;
  } else if (200 > yJoystickPosition && 200 < xJoystickPosition && 800 > xJoystickPosition && dir != RIGHT) {
    dir = LEFT;
  } else if (200 > xJoystickPosition && 200 < yJoystickPosition && 800 > yJoystickPosition && dir != UP) {
    dir = DOWN;
  }
}

void algorithm() {

  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;

  tailX[0] = x;
  tailY[0] = y;

  for (int i = 1; i < nTail; i++) {

    prev2X = tailX[i];
    prev2Y = tailY[i];

    tailX[i] = prevX;
    tailY[i] = prevY;

    prevX = prev2X;
    prevY = prev2Y;
  }

  switch (dir) {

  case LEFT:
    x--;
    break;

  case RIGHT:
    x++;
    break;

  case UP:
    y--;
    break;

  case DOWN:
    y++;
    break;

  default:
    break;
  }

  if (x >= width) {
    x = 0;
  } else if (x < 0) {
    x = width - 1;
  }

  if (y >= height) {
    y = 0;
  } else if (y < 0) {
    y = height - 1;
  }

  for (int i = 0; i < nTail; i++) {
    if (tailX[i] == x && tailY[i] == y) {
      gameover = true;
    }
  }

  if (x == fruitX && y == fruitY) {
    score += 10;

    fruitX = (rand() % (width - 1)) + 1;
    fruitY = (rand() % (height - 1)) + 1;

    nTail++;
  }
}

void drawSquare(int x, int y) {
  display.drawRect(x * 2, y * 2, 2, 2, BLACK);
}

void drawFruit(int x, int y) {
  display.fillCircle(x * 2, y * 2, 1, BLACK);
}

void showScore() {
  while (countDown != 0) {
    countDown--;
        
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, height / 2);
    display.print("Score : ");
    display.println(score);
    display.print("Start in... ");
    display.println(countDown);
    display.display();
    
    delay(1000);
  }
  resetAll();
}

void loop() {
  while (!gameover) {
    draw();
    input();
    algorithm();
    delay(100);
  }
  showScore();
}
