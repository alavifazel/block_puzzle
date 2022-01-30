#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <termios.h>
#include <stdio.h>
#include <cstdlib>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::to_string;

size_t HEIGHT(10), WIDTH(50);
size_t GAME_WIDTH(11), GAME_HEIGHT(11);
size_t SIMPLE_SHAPE_PROB(10), INTERMEDIATE_SHAPE_PROB(10), COMPLEX_SHAPE_PROB(10);
size_t SCORE(0), HIGHEST_SCORE(0);

struct Block {
    vector<vector<char>> text;
    int x, y;
};

using v2d_char = vector<vector<char>>;

// Prototypes
void loop();
int selectDifficultyScreen();
void customDifficultyScreen(bool error);
int startingScreen();
void gameScreen();
void renderGameScreen(v2d_char board, Block block);
void moveBlockUp(v2d_char& board, Block& block);
void moveBlockDown(v2d_char& board, Block& block);
void moveBlockLeft(v2d_char& board, Block& block);
void moveBlockRight(v2d_char& board, Block& block);
void setBlockOnBoard(v2d_char& board, Block block);
void checkScoreCondition(v2d_char& board);
void applyOverlap(v2d_char& board, Block block);
void addBlock(v2d_char& board, Block block);
Block newBlock(v2d_char board);
bool blockFits(v2d_char board, Block block);
v2d_char pickBlock(int simpleBlockProb, int intermBlockProb, int complexBlockProb);
v2d_char initBoard(int width, int height);
void printMainFrame(v2d_char body, int highestScore, int score);
void printHeaderText(v2d_char nextBlockVector);
void addText(v2d_char& input, char c, int x, int y);
void addText(v2d_char& input, string text, int x, int y);
void print(v2d_char text);
void printShape(vector<v2d_char> shape);
void updateBoard(v2d_char& board, Block block);
void removeBlock(v2d_char& board, Block block);

const vector<v2d_char> simpleShapes{
  {{'#'}},
  {{'#'}, {'#'}},
  {{'#'}, {'#'}},
  {{'#', '#'}, {'#', '#'}}
};

const vector<v2d_char> intermediateShapes{
  {{' ', '#'}, {'#', '#'}},
  {{'#', ' '}, {'#', '#'}},
  {{'#', '#'}, {'#', ' '}},
  {{'#', '#'}, {' ','#'}},
  {{'#', '#', '#'}},
  {{'#', '#', '#'}},
  {{'#', '#', '#', '#'}},
  {{'#'}, {'#'}, {'#'}, {'#'}}
};

const vector<v2d_char> complexShapes{
  {{'#', '#', ' '}, {' ', '#', '#'}},
  {{' ', '#', '#'}, {'#', '#', ' '}},
  {{'#', ' '}, {'#', '#'}, {' ', '#'}},
  {{' ', '#'}, {'#', '#'}, {'#', ' '}},

  {{'#', '#', '#'}, {' ', '#', ' '}},
  {{' ', '#', ' '}, {'#', '#', '#'}},
  {{' ', '#'}, {'#', '#'}, {' ', '#'}},
  {{' ', '#'}, {'#', '#'}, {'#', ' '}},

  {{'#', '#', '#'}, {'#', ' ', ' '}},
  {{'#', ' ', ' '}, {'#', '#', '#'}},
  {{'#', '#', '#'}, {' ', ' ', '#'}},
  {{' ', ' ', '#'}, {'#', '#', '#'}},
};

struct termios oldTerm, currentTerm;
void initTermios(int echo) {
  tcgetattr(0, &oldTerm);
  currentTerm = oldTerm;
  currentTerm.c_lflag &= ~ICANON;
  if (echo) {
      currentTerm.c_lflag |= ECHO;
  } else {
      currentTerm.c_lflag &= ~ECHO;
  }
  tcsetattr(0, TCSANOW, &currentTerm);
}

void resetTermios(void) {
  tcsetattr(0, TCSANOW, &oldTerm);
}

char _getch(int echo = 0) {
  char c;
  initTermios(echo);
  c = getchar();
  resetTermios();
  return c;
}

void printShape(vector<v2d_char> shape) {
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape[i].size(); ++j) {
            for (int k = 0; k < shape[i][j].size(); ++k) {
                if (shape[i][j][k] != 0) {
                    cout << shape[i][j][k];
                }
                else {
                    cout << "-";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

}

void print(v2d_char text) {
    for (size_t i = 0; i < text.size(); ++i) {
        for (size_t j = 0; j < text[i].size(); ++j) {
            cout << text[i][j];
        }
        cout << endl;
    }
}

void addText(v2d_char& input, string text, int x, int y) {
    for (size_t i = 0; i < text.size(); ++i) {
        input[x][y + i] = text[i];
    }
}

void addText(v2d_char& input, char c, int x, int y) {
    input[x][y] = c;
}

void printHeaderText(v2d_char nextBlockVector) {
    size_t height(5), width(WIDTH);
    v2d_char output(height, vector<char>(width, ' '));
    string nextBlockText = "Next block: ";

    vector<string> nextBlock;
    for (int i = 0; i < nextBlockVector.size(); ++i) {
        nextBlock.push_back(string(nextBlockVector[i].begin(), nextBlockVector[i].end()));
    }

    addText(output, nextBlockText, 2, 5);
    for (int i = 0; i < nextBlock.size(); ++i) addText(output, nextBlock[i], 1 + i, 20);
    print(output);
}

void printMainFrame(v2d_char body, int highestScore, int score) {
    size_t height(GAME_HEIGHT), width(GAME_WIDTH * 2 + WIDTH);
    v2d_char output(height, vector<char>(width, ' '));

    for (int i = 0; i < body.size(); ++i) {
        for (int j = 0; j < body[i].size(); ++j) {
            addText(output, body[i][j], i, j * 2);
        }
    }

    vector<string> menuText = {
      "****************",
      "* Retry(1)     *",
      "* Hint (2)     *",
      "* Exit (3)     *",
      "****************"
    };

    string highScoreText = "HIGH SCORE: " + to_string(highestScore);
    string scoreText = "SCORE: " + to_string(score);

    addText(output, highScoreText, 1, GAME_WIDTH * 2 + 5);
    addText(output, scoreText, 2, GAME_WIDTH * 2 + 5);

    for (int i = 0; i < menuText.size(); ++i) addText(output, menuText[i], 5 + i, GAME_WIDTH * 2 + 5);

    print(output);
}


v2d_char initBoard(int width, int height) {
    v2d_char board;
    for (int i = 0; i < height; ++i) {
        vector<char> tmp;
        for (int j = 0; j < width; ++j) {
            tmp.push_back('-');
        }
        board.push_back(tmp);
    }
    return board;
}

bool isInRange(int number, int min, int max) { return (number >= min && number <= max) ? true : false; }
int randomInteger(int min, int max) {
    return min + std::rand() % (max - min);
}

v2d_char pickBlock(int simpleBlockProb, int intermBlockProb, int complexBlockProb) {
    int randomValue = randomInteger(0, simpleBlockProb + intermBlockProb + complexBlockProb);
    int current = 0;
    current += simpleBlockProb;
    if (randomValue < current) return simpleShapes[randomInteger(0, simpleShapes.size() - 1)];
    else current += intermBlockProb;
    if (randomValue < current) return intermediateShapes[randomInteger(0, intermediateShapes.size() - 1)];
    else return complexShapes[randomInteger(0, complexShapes.size() - 1)];
}

bool blockFits(v2d_char board, Block block) {
    bool fits = true;
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j) {
            if ((block.x + i > board[0].size() - 1 || block.y + j > board.size() - 1)) {
                fits = false;
            }
        }
    }
    return fits;
}

Block newBlock(v2d_char board) {
    Block block;
    block.text = pickBlock(SIMPLE_SHAPE_PROB, INTERMEDIATE_SHAPE_PROB, COMPLEX_SHAPE_PROB);
    block.x = 0;
    block.y = 0;
    return block;
}

vector<int> findFreeSpace(v2d_char board, Block block) {
    vector<int> res(2, -1);
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[0].size(); ++j) {
            bool blockFits = true;
            for (int k = 0; k < block.text.size(); ++k) {
                for (int l = 0; l < block.text[0].size(); ++l) {
                    if (i + block.text.size() >= board.size() ||
                        j + block.text[0].size() >= board[i].size() ) {
                        blockFits = false;
                        continue;
                    }
                    if (board[i + k][j + l] == '#') blockFits = false;
                }
            }
            if (blockFits) {
                res[0] = i;
                res[1] = j;
                return res;
            }
        }
    }
    return res;
}

void addBlock(v2d_char& board, Block block) {
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j) {
            if (block.text[i][j] == '#')
                board[block.x + i][block.y + j] = block.text[i][j];
        }
    }

}

void applyOverlap(v2d_char& board, Block block) {
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j) {
            char c = board[block.x + i][block.y + j];
            board[block.x + i][block.y + j] = '#';
            if (c != '#')
                board[block.x + i][block.y + j] = '#';
            else
                board[block.x + i][block.y + j] = 'O';
        }
    }
}

void checkScoreCondition(v2d_char& board) {
    for (int i = 0; i < board.size(); ++i) {
        bool rowIsComplete = true;
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == '-') rowIsComplete = false;
        }
        if (rowIsComplete) {
            for (int j = 0; j < board[i].size(); ++j) {
                board[i][j] = '-';
            }
            for (int k = i; k > 1; --k) {
                std::swap(board[k], board[k - 1]);
            }
            SCORE += 1;
            if (SCORE > HIGHEST_SCORE) {
                cout << "*New Record!";
            }
        }
    }
}

void setBlockOnBoard(v2d_char& board, Block block) {
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j)
        {
            if (block.text[i][j] == '#')
                board[block.x + i][block.y + j] = '#';
        }
    }
}

void updateBoard(v2d_char& board, Block block) {
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j) {
            if (block.text[i][j] == '#')
                board[block.x + i][block.y + j] = block.text[i][j];
            else if(board[block.x + i][block.y + j] != '#')
                board[block.x + i][block.y + j] = '-';
        }
    }
}

void removeBlock(v2d_char& board, Block block) {
    for (int i = 0; i < block.text.size(); ++i) {
        for (int j = 0; j < block.text[i].size(); ++j)
        {
            if (block.text[i][j] == '#')
                board[block.x + i][block.y + j] = '-';
        }
    }
}

bool blockMovableToRight(v2d_char board, Block block) {
    if (block.y + block.text[0].size() > board[0].size() - 1) return false;
    for (int i = 0; i < block.text.size(); ++i) {
        if (block.text[i][block.text[i].size() - 1] == '#' &&
            board[block.x + i][block.y + block.text[i].size()] == '#') return false;
    }
    for (int i = 0; i < block.text.size(); ++i) {
        int j = block.text[i].size() - 1;
        while (block.text[i][j] != '#' && j > 0) {
            if (board[block.x + i][block.y + j] == '#' && block.text[i][j] != '#') return false;
            j--;
        }
    }
    return true;
}

void moveBlockRight(v2d_char& board, Block& block) {
    if (!blockMovableToRight(board, block)) return;
    removeBlock(board, block);
    block.y++;
    updateBoard(board, block);
}

bool blockMovableToLeft(v2d_char board, Block block) {
    if (block.y <= 0) return false;
    for (int i = 0; i < block.text.size(); ++i) {
        if (block.text[i][0] == '#' &&
            board[block.x + i][block.y - 1] == '#') return false;
    }
    for (int i = 0; i < block.text.size(); ++i) {
        int j = 0;
        while (block.text[i][j] != '#' && j < block.text[i].size()) {
            if (board[block.x + i][block.y + j] == '#' && block.text[i][j] != '#') return false;
            ++j;
        }
    }
    return true;
}

void moveBlockLeft(v2d_char& board, Block& block) {
    if (!blockMovableToLeft(board, block)) return;
    removeBlock(board, block);
    block.y--;
    updateBoard(board, block);
}

bool blockMovableToDown(v2d_char board, Block block) {
    if (block.x + block.text.size() > board.size() - 1) return false;
    for (int i = 0; i < block.text[0].size(); ++i) {
        if (block.text[block.text.size() - 1][i] == '#' &&
            board[block.x + block.text.size()][block.y + i] == '#') return false;
    }
    for (int i = 0; i < block.text[0].size(); ++i) {
        int j = block.text.size() - 1;
        while (block.text[j][i] != '#' && j > 0) {
            if (board[block.x + j][block.y + i] == '#' && block.text[j][i] != '#') return false;
            j--;
        }
    }
    return true;
}

void moveBlockDown(v2d_char& board, Block& block) {
    if (!blockMovableToDown(board, block)) return;
    removeBlock(board, block);
    block.x++;
    updateBoard(board, block);
}

bool blockMovableUp(v2d_char& board, Block& block) {
    if (block.x <= 0) return false;
    for (int i = 0; i < block.text[block.text.size() - 1].size(); ++i) {
        if (block.text[0][i] == '#' &&
            board[block.x - 1][block.y + i] == '#') return false;
    }

    for (int i = 0; i < block.text[0].size(); ++i) {
        int j = 0;
        while (block.text[j][i] != '#' && j < block.text.size()) {
            if (board[block.x + j][block.y + i] == '#' && block.text[j][i] != '#') return false;
            ++j;
        }
    }
    return true;
}

void moveBlockUp(v2d_char& board, Block& block) {
    if (!blockMovableUp(board, block)) return;
    removeBlock(board, block);
    block.x--;
    updateBoard(board, block);
}

void hintScreen() {
    system("clear");
    cout << "\n\t";
    cout << " < Block Puzzle >\n";
    cout << "\tGame shortcuts:\n\n";
    cout << "d : Moves the block right\n";
    cout << "a : Moves the block left\n";
    cout << "w : Moves the block up\n";
    cout << "s : Moves the block down\n";
    cout << "\nPress any key to go back.";
    _getch();
}

void gameScreen() {
    v2d_char board = initBoard(GAME_WIDTH, GAME_HEIGHT);
    Block block = newBlock(board), nextBlock = newBlock(board);
    bool firstRun = true;
    renderGameScreen(board, nextBlock);
    for (;;) {
        renderGameScreen(board, nextBlock);

        char input = _getch();
        switch(input) {
          case '1':
            HIGHEST_SCORE = SCORE;
            SCORE = 0;
            gameScreen();
            break;
          case '2':
            hintScreen();
            break;
          case '3':
            SCORE = 0;
            return;
            break;
          case '\n':
            if (input == '\n') {
      	      if(firstRun) firstRun = false;
      	      else {
                  setBlockOnBoard(board, block);
                  checkScoreCondition(board);
                  block = nextBlock;
                  nextBlock = newBlock(board);
                  vector<int> freeSpace = findFreeSpace(board, block);
                  if (freeSpace[0] == -1 || freeSpace[1] == -1) {
                      cout << "\n\t\t*Game over!*\t\t\n";
                      HIGHEST_SCORE = SCORE;
                      SCORE = 0;
                      std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                      gameScreen();
                  }
                  else {
                      block.x = freeSpace[0];
                      block.y = freeSpace[1];
                  }
      	       }
            }
            break;
        }

        if (blockFits(board, block)) {
            if (input == 'd' || input == 'D') moveBlockRight(board, block);
            if (input == 'a' || input == 'A') moveBlockLeft(board, block);
            if (input == 's' || input == 'S') moveBlockDown(board, block);
            if (input == 'w' || input == 'W') moveBlockUp(board, block);
        }
        addBlock(board, block);
    }
}

void renderGameScreen(v2d_char board, Block nextBlock) {
  system("clear");
  printHeaderText(nextBlock.text);
  printMainFrame(board, HIGHEST_SCORE, SCORE);
}

int startingScreen() {
    system("clear");
    int choice;
    string padding = "\t\t";
    cout << "\n\n";
    cout << padding + "1) Start Game\n";
    cout << padding + "2) Select Difficulty\n";
    cout << padding + "3) Exit";
    cout << "\n\n";
    cout << "> Your Choice: ";
    cin >> choice;
    if (choice == 1 || choice == 2 || choice == 3) {
        return choice;
    }
    else loop();
}

void customDifficultyScreen(bool error = false) {
    system("clear");
    cout << "\n\n";
    cout << "\tEnter game Width and Height and the probability of simple, intermediate and complex shapes: \n";
    cout << "\tExample: 11 11 10 10 10\n\n";
    if (error) {
        cout << "Error: Invalid Width/Height (Game size should be bigger than 11x11)\n";
        cout << "> Your Input: ";
    }
    else {
        cout << "\n";
        cout << "> Your Input: ";
    }
    cin >> GAME_WIDTH >> GAME_HEIGHT >> SIMPLE_SHAPE_PROB >> INTERMEDIATE_SHAPE_PROB >> COMPLEX_SHAPE_PROB;
    if (GAME_WIDTH < 11 || GAME_HEIGHT < 11) {
        customDifficultyScreen(true);
    }
}

int selectDifficultyScreen() {
    system("clear");
    int choice;
    string padding = "\t\t";
    cout << "\n\n";
    cout << padding + "1) Default\n";
    cout << padding + "2) Your Choice\n";
    cout << padding + "3) Back\n";
    cout << "\n> Your Choice : ";
    cin >> choice;
    if (choice == 1 || choice == 3) {
        return choice;
    }
    else if (choice == 2) {
        customDifficultyScreen();
    }
    else selectDifficultyScreen();
}

void loop() {
    int choice = startingScreen();
    switch(choice) {
    case 1:
        gameScreen();
        break;
    case 2:
        selectDifficultyScreen();
        loop();
        break;
    case 3:
        exit(1);
    }
}

int main() {
    srand((unsigned int)std::time(nullptr));
    loop();
}
