#include <iterator>
#include <list>
#include <ncurses.h>

#define HEIGHT 15
#define WIDTH 18

#define UP '^'
#define DOWN 'v'
#define LEFT '<'
#define RIGHT '>'

#define UP_KEY(c) (c == 'k' || c == 'w')
#define DOWN_KEY(c) (c == 'j' || c == 's')
#define LEFT_KEY(c) (c == 'h' || c == 'a')
#define RIGHT_KEY(c) (c == 'l' || c == 'd')

void gameOverHandler();

using namespace std;

class Coordinate {
public:
  int y, x;
};

Coordinate v0 = {5, 3}, v1 = {5, 4}, v2 = {5, 5};

list<Coordinate> snake;
Coordinate foodCoordinate;

char board[HEIGHT][WIDTH];
char defaultCharacter = ' ', snakeHead = RIGHT, snakeBody = 'o', food = '*';
bool stay = true, hasFood = false;
bool moveRight = false, moveLeft = false, moveDown = false, moveUp = false;
int score = 0, bestScore = 0;

/*
 * Utility Functions
 */

/**
 * set all board pieces to c
 */
void set(char c) {
  int y, x;
  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      board[y][x] = c;
    }
  }
}

void writeToBoard() {
  set(defaultCharacter);

  // write snake
  std::list<Coordinate>::iterator i;
  for (i = snake.begin(); i != snake.end(); i++) {
    board[i->y][i->x] = snakeBody;
    if (i->y == snake.front().y && i->x == snake.front().x) {
      board[i->y][i->x] = snakeHead;
    }
  }

  // write food
  int y = foodCoordinate.y, x = foodCoordinate.x;
  if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH) {
    board[y][x] = food;
  }
}

void newGame() {
  snake.clear();
  snake.push_front(v0);
  snake.push_front(v1);
  snake.push_front(v2);
  set(defaultCharacter);

  snakeHead = RIGHT;
  hasFood = false;
  foodCoordinate = {-1, -1};
  moveRight = false;
  moveLeft = false;
  moveDown = false;
  moveUp = false;
  score = 0;
}

/**
 * food
 */

void spawnFood() {
  int y = -1, x = -1;
  y = rand() % HEIGHT;
  x = rand() % WIDTH;

  if (x == 0) {
    x++;
  } else if (x == WIDTH - 1) {
    x--;
  }

  if (y == 0) {
    y++;
  } else if (y == HEIGHT - 1) {
    y--;
  }

  if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH) {
    std::list<Coordinate>::iterator i;
    for (i = snake.begin(); i != snake.end(); i++) {
      if (i->y == y && i->x == x) {
        return;
      }
    }

    foodCoordinate.y = y;
    foodCoordinate.x = x;
    hasFood = true;
  }
}

bool ateFood(Coordinate newHead) {
  return hasFood && newHead.y == foodCoordinate.y &&
         newHead.x == foodCoordinate.x;
}

void shrink(bool shouldShrink) {
  if (shouldShrink) {
    snake.pop_back();
  } else {
    hasFood = false;
    foodCoordinate.y = -1;
    foodCoordinate.x = -1;
    score++;
  }
}

bool valid(Coordinate newHead) {
  // check if snake hits the wall
  bool valid = (newHead.y >= 0 && newHead.y < HEIGHT && newHead.x >= 0 &&
                newHead.x < WIDTH);

  // check if snake hits itself
  std::list<Coordinate>::iterator i;
  for (i = snake.begin(); valid && i != snake.end(); i++) {
    if (newHead.y == i->y && newHead.x == i->x) {
      valid = false;
    }
  }

  return valid;
}

/**
 * movement
 */

void right() {
  Coordinate head = snake.front();
  Coordinate newHead = {head.y, head.x + 1};
  bool shouldShrink = !ateFood(newHead);

  if (valid(newHead)) {
    snakeHead = RIGHT;
    snake.push_front(newHead);
    shrink(shouldShrink);
  } else {
    gameOverHandler();
  }
}

void left() {
  Coordinate head = snake.front();
  Coordinate newHead = {head.y, head.x - 1};
  bool shouldShrink = !ateFood(newHead);

  if (valid(newHead)) {
    snakeHead = LEFT;
    snake.push_front(newHead);
    shrink(shouldShrink);
  } else {
    gameOverHandler();
  }
}

void down() {
  Coordinate head = snake.front();
  Coordinate newHead = {head.y + 1, head.x};
  bool shouldShrink = !ateFood(newHead);

  if (valid(newHead)) {
    snakeHead = DOWN;
    snake.push_front(newHead);
    shrink(shouldShrink);
  } else {
    gameOverHandler();
  }
}

void up() {
  Coordinate head = snake.front();
  Coordinate newHead = {head.y - 1, head.x};
  bool shouldShrink = !ateFood(newHead);

  if (valid(newHead)) {
    snakeHead = UP;
    snake.push_front(newHead);
    shrink(shouldShrink);
  } else {
    gameOverHandler();
  }
}

/*
 * IO Functions
 */

void print(const char *temp, int y, int x) {
  int i = 0;
  while (temp[i] != '\0') {
    mvaddch(y, x++, temp[i++]);
  }
}

void printBindings() {
  const char *bindings[6] = {"Commands:",
                             "q - Quit\n",
                             "h (or a) - Move left\n",
                             "l (or d) - Move right\n",
                             "j (or s) - Move down\n",
                             "k (or w) - Move up\n"};

  int i, y = 2, x = 83;
  for (i = 0; i < 6; i++) {
    if (i == 1) {
      x++;
    }
    print(bindings[i], y++, x);
  }
}

void printBoard() {
  int y, x, temp;
  temp = 2;
  for (y = 0; y < HEIGHT; y++) {
    move(temp, 5);
    printw("+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--"
           "-+---+\n");
    move(temp + 1, 5);
    for (x = 0; x < WIDTH; x++) {
      printw("| %c ", board[y][x]);
    }
    printw("|\n");
    temp += 2;
  }
  move(temp, 5);
  printw("+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--"
         "-+---+\n");
}

void printScore() {
  move(1, 29);
  printw("BEST SCORE (%d) - SCORE (%d)\n", bestScore, score);
}

void gameOverHandler() {
  char c;

  if (score > bestScore) {
    bestScore = score;
  }

  while (true) {
    clear();
    printBoard();
    printScore();
    print("Commands:\n", 2, 83);
    print("q - Quit\n", 3, 84);
    print("n - New game\n", 4, 84);
    print("Game Over!\n", 33, 39);
    move(2, 92);
    refresh();

    c = getch();
    if (c == 'q') {
      stay = false;
      break;
    } else if (c == 'n') {
      newGame();
      break;
    }
  }
}

void writeOutBestScore() {
  FILE *out = fopen("best_score.txt", "w");
  fprintf(out, "%d", bestScore);
  fclose(out);
}

void readInBestScore() {
  FILE *in = fopen("best_score.txt", "r");
  if (in == NULL) {
    writeOutBestScore();
  } else {
    fscanf(in, "%d", &bestScore);
    fclose(in);
  }
}

int main() {
  readInBestScore();

  newGame();
  initscr();

  while (stay) {
    timeout(115);
    clear();
    writeToBoard();
    if (hasFood == false) {
      spawnFood();
    }
    printBoard();
    printScore();
    printBindings();
    move(2, 92);
    refresh();

    if (stay) {
      char c = getch();
      if (c == 'q') {
        stay = false;
      } else if (RIGHT_KEY(c) && snakeHead != LEFT) {
        // move right
        moveRight = true, moveLeft = false, moveDown = false, moveUp = false;
      } else if (LEFT_KEY(c) && snakeHead != RIGHT) {
        // move left
        moveRight = false, moveLeft = true, moveDown = false, moveUp = false;
      } else if (DOWN_KEY(c) && snakeHead != UP) {
        // move down
        moveRight = false, moveLeft = false, moveDown = true, moveUp = false;
      } else if (UP_KEY(c) && snakeHead != DOWN) {
        // move up
        moveRight = false, moveLeft = false, moveDown = false, moveUp = true;
      } else {
        if (moveRight) {
          right();
        } else if (moveLeft) {
          left();
        } else if (moveDown) {
          down();
        } else if (moveUp) {
          up();
        }
      }
    }
  }

  endwin();
  writeOutBestScore();
  return 0;
}