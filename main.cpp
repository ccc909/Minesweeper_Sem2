#include <SFML/Graphics.hpp>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <vector>
#include <cstring>
#include <time.h>
#include <math.h>
#include <iostream>
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ROW_HEIGHT = 30;
const int ROWS_PER_PAGE = 10;
int unrevealed;
using namespace sf;

class cell {
  public: bool mine = 0;
  bool revealed = 0;
  bool value = 0;
  bool flag = 0; // could have assigned a mine a value of 9 instead
}; //board cell declaration


class    Entry {
    public:
    std::string date;
    std::string difficulty;
    int time;
};

Entry create_entry(const std::string& date, const std::string& difficulty, int time) {
    Entry entry;
    entry.date = date;
    entry.difficulty = difficulty;
    entry.time = time;

    return entry;
}

void save_entry(const Entry& entry) {
    std::ofstream file("data.txt", std::ios::app);
    if (file.is_open()) {
        file << entry.date << " " << entry.time << " " << entry.difficulty << "\n";
        file.close();
    }
}


std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm* date = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(date, "%Y-%m-%d");

    return oss.str();
}



std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::string substring;
    std::istringstream stream(input);

    while (std::getline(stream, substring, delimiter)) {
        result.push_back(substring);
    }

    return result;
}




int timesMenu(){

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Records");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {

        return EXIT_FAILURE;
    }

    sf::Text rowText("", font, 16);
    rowText.setFillColor(sf::Color::Black);

    sf::RectangleShape nextButton(sf::Vector2f(80, 30));
    nextButton.setFillColor(sf::Color::Green);
    sf::Text nextButtonText("Next", font, 16);
    nextButtonText.setFillColor(sf::Color::Black);
    sf::RectangleShape prevButton(sf::Vector2f(80, 30));
    prevButton.setFillColor(sf::Color::Green);
    sf::Text prevButtonText("Previous", font, 16);
    prevButtonText.setFillColor(sf::Color::Black);

    std::ifstream file("data.txt");
    std::string line;
    std::vector<std::vector<std::string>> data;

    while (std::getline(file, line)) {
        std::vector<std::string> row = splitString(line, ',');
        data.push_back(row);
    }

    int totalPages = (data.size() - 1) / ROWS_PER_PAGE + 1;
    int currentPage = 1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    if (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPage < totalPages) {
                            currentPage++;
                        }
                    }

                    if (prevButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPage > 1) {
                            currentPage--;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        int startIdx = (currentPage - 1) * ROWS_PER_PAGE;
        int endIdx = startIdx + ROWS_PER_PAGE;
        if (endIdx > data.size()) {
            endIdx = data.size();
        }

        for (int i = startIdx; i < endIdx; i++) {
            std::string rowString;
            for (const auto& cell : data[i]) {
                rowString += cell + "    ";
            }
            rowText.setString(rowString);
            rowText.setPosition(10, (i - startIdx) * ROW_HEIGHT);
            window.draw(rowText);
        }

        nextButton.setPosition(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 40);
        nextButtonText.setPosition(WINDOW_WIDTH - 80, WINDOW_HEIGHT - 35);
        window.draw(nextButton);
        window.draw(nextButtonText);

        prevButton.setPosition(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 40);
        prevButtonText.setPosition(WINDOW_WIDTH - 180, WINDOW_HEIGHT - 35);
        window.draw(prevButton);
        window.draw(prevButtonText);

        window.display();
    }
    return 0;
}


int random(int min, int max) { //random num in range
  return rand() % (min + 1 - max) + min;
}

int show(int vsize, int hsize, struct cell ** board, int x, int y) {

  if (x < 1 || x > vsize || y < 1 || y > hsize) {
    return -1; // out of bounds check
  }

  //check if the clicked cell has already been revealed
  if (board[x][y].revealed || board[x][y].flag) {
    return 0; // Already revealed
  }

  //Reveal the clicked cell

  board[x][y].revealed = 1;
  unrevealed--;

  //decrement counter and check for function context

  //if the clicked cell is a mine, return -2 t    o indicate game over
  if (board[x][y].mine) {
    return -2; // Game over
  }

  //if the clicked cell has a value of 0, reveal its neighbors
  if (board[x][y].value == 0) {
    //reveal all of the 8 neighbors of the clicked cell
    show(vsize, hsize, board, x - 1, y - 1);
    show(vsize, hsize, board, x - 1, y);
    show(vsize, hsize, board, x - 1, y + 1);
    show(vsize, hsize, board, x, y + 1);
    show(vsize, hsize, board, x + 1, y + 1);
    show(vsize, hsize, board, x + 1, y);
    show(vsize, hsize, board, x + 1, y - 1);
    show(vsize, hsize, board, x, y - 1);
  }

  return 1;
}






bool isMouseInsideButton(const sf::Sprite & button,
  const sf::RenderWindow & window) {
  sf::FloatRect buttonBounds = button.getGlobalBounds();
  sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
  return buttonBounds.contains(static_cast < float > (mousePosition.x), static_cast < float > (mousePosition.y));
}
int start(int board_height, int board_width, int mines) { //everything is badly implemented, have to start whole game again because i cant resize window properly

  int spriteSize = 16;
  int numsize = 23;
  time_t t;
  int i, j;
  srand((unsigned) time( & t));
  std::vector < Sprite > numbers;
  std::vector < Sprite > timernumbers;
  std::vector < Sprite > faces;
  std::vector < Sprite > diffs;
  Sprite empty_cell;
  Sprite undiscovered_cell;
  Sprite mine;
  Sprite flag;
  Sprite diff;
  bool leftClicked = false;
  bool rightClicked = false;
  bool enabled=true;
  bool win=false;
  bool gameover=false;
  unrevealed = board_height * board_width;
  std::vector < std::vector < sf::Sprite >> spriteGrid(board_width + 2, std::vector < sf::Sprite > (board_height + 2));

  sf::RenderWindow window(sf::VideoMode(board_width * 32, board_height * 32 + 46), "Minesweeper");

  sf::Texture spriteSheetTexture, timerTexture, faceTextures, diffTexture;
  if (!spriteSheetTexture.loadFromFile("2000.png") || !timerTexture.loadFromFile("numbers.png") || !faceTextures.loadFromFile("faces.png") || !diffTexture.loadFromFile("diffs.png")) {
    // error handling for failed texture loading
    return -1;
  }
  sf::Sprite spriteSheet(spriteSheetTexture), numSheet(timerTexture), faceSheet(faceTextures), diffSheet(diffTexture);
  struct cell ** board = new cell * [board_width + 3];
  for (int i = 0; i < board_width + 3; i++) {
    board[i] = new cell[board_height + 3];
  }

  int mc = 0;
  while (mc < mines) //generating game board on first loop
  {
    int r, c;
    r = random(1, board_width + 2);
    c = random(1, board_height + 2);
    if (!board[r][c].mine) {
      board[r][c].mine = 1;
      mc++;
    }
  }

  for (i = 1; i <= board_width; i++) {
    for (j = 1; j <= board_height; j++) {
      if (!board[i][j].mine)
        board[i][j].value = board[i][j - 1].mine + board[i - 1][j - 1].mine + board[i - 1][j].mine + board[i - 1][j + 1].mine + board[i][j + 1].mine + board[i + 1][j + 1].mine + board[i + 1][j].mine + board[i + 1][j - 1].mine;
    }
  }

  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 8; ++x) {
      sf::IntRect spriteRect(x * spriteSize, y * spriteSize, spriteSize, spriteSize);
      spriteSheet.setTextureRect(spriteRect);

      sf::Sprite sprite(spriteSheet);
      window.display();
      if (y == 0 && x == 0)
        undiscovered_cell = sprite;
      else if (y == 0 && x == 1)
        empty_cell = sprite;
      else if (y == 0 && x == 5)
        mine = sprite;
      else if (y == 0 && x == 2)
        flag = sprite;
      else if (y == 1)
        numbers.push_back(sprite);

    }
  }

  for (int x = 0; x < 10; x++) {
    IntRect numrect(x * 13, 0, 13, 23);
    numSheet.setTextureRect(numrect);
    Sprite sprite(numSheet);
    timernumbers.push_back(sprite);
  }

  for (int x = 0; x < 5; x++) {
    IntRect faceRect(x * 24, 0, 24, 24);
    faceSheet.setTextureRect(faceRect);
    Sprite sprite(faceSheet);
    faces.push_back(sprite);

  }
  for (int x = 0; x < 5; x++) {
    if (x == 0 || x == 3 || x == 4) {
      IntRect diffRect(x * 24, 0, 24, 24);
      diffSheet.setTextureRect(diffRect);
      Sprite sprite(diffSheet);
      diffs.push_back(sprite);
    }
  }

  Sprite & beginner = diffs[0], & intermediate = diffs[1], & expert = diffs[2];

  Sprite & facesprite = faces[0];
  spriteSize = 32;
  std::vector < std::vector < sf::Sprite >> showGrid(board_width + 2, std::vector < sf::Sprite > (board_height + 2, undiscovered_cell));

  for (i = 1; i <= board_width; i++) {
    for (j = 1; j <= board_height; j++) {
      if (!board[i][j].mine && board[i][j].value)
        spriteGrid[i - 1][j - 1] = numbers[board[i][j].value - 1];
      else if (!board[i][j].mine && !board[i][j].value)
        spriteGrid[i - 1][j - 1] = empty_cell;
      else
        spriteGrid[i - 1][j - 1] = mine;
    }
  }

  sf::Clock clock;
int seconds;
  while (window.isOpen()) {
    sf::Time elapsed = clock.getElapsedTime();
    if(enabled)
    seconds = elapsed.asSeconds();
    Vector2i pos = Mouse::getPosition(window);

    int click_x = pos.x / (window.getSize().x / board_width);
    int click_y = (pos.y - 46) / ((window.getSize().y - 46) / board_height);
    int click_y_real = pos.y / (window.getSize().y / board_height);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

        if(event.type==Event::KeyPressed && event.key.code == Keyboard::T)
            timesMenu();
      if (event.type == Event::MouseButtonPressed) {

        if (pos.y < 46)

        {

          if (isMouseInsideButton(facesprite, window)) {
            window.close();
            start(board_height, board_width, mines);
          } else if (isMouseInsideButton(beginner, window)) {
            window.close();
            start(9, 9, 10);
          } else if (isMouseInsideButton(intermediate, window)) {
            window.close();
            start(16, 16, 40);
          } else if (isMouseInsideButton(expert, window)) {
            window.close();
            start(16, 30, 99);
          }
        } else {
          if (event.key.code == Mouse::Left)

          {
            int result = show(board_width, board_height, board, click_x + 1, click_y + 1);

            if (result == -2) {
                    enabled=false;
            gameover=true;
              for (int j = 0; j < board_height; j++)
                for (int i = 0; i < board_width; i++)
                  showGrid[i][j] = spriteGrid[i][j];

            }
            for (int j = 0; j < board_height; j++) {
              for (int i = 0; i < board_width; i++) {
                std::cout << board[i + 1][j + 1].value << " ";
                if (board[i + 1][j + 1].revealed && !board[i + 1][j + 1].mine)
                  showGrid[i][j] = spriteGrid[i][j];
              }
              std::cout << '\n';
            }
          }

          if (event.key.code == Mouse::Middle) {
            if (board[click_x + 1][click_y + 1].revealed) {
              click_x++;
              click_y++;
              if (show(board_width, board_height, board, click_x, click_y - 1) == -2 ||
                show(board_width, board_height, board, click_x + 1, click_y - 1) == -2 ||
                show(board_width, board_height, board, click_x + 1, click_y) == -2 ||
                show(board_width, board_height, board, click_x + 1, click_y + 1) == -2 ||
                show(board_width, board_height, board, click_x, click_y + 1) == -2 ||
                show(board_width, board_height, board, click_x - 1, click_y + 1) == -2 ||
                show(board_width, board_height, board, click_x - 1, click_y) == -2 ||
                show(board_width, board_height, board, click_x - 1, click_y - 1) == -2) {
                for (int j = 0; j < board_height; j++)

                  for (int i = 0; i < board_width; i++)

                    showGrid[i][j] = spriteGrid[i][j];
              }
              click_x--;
              click_y--;
              for (int j = 0; j < board_height; j++) {
                for (int i = 0; i < board_width; i++) {
                  std::cout << board[i + 1][j + 1].value << " ";
                  if (board[i + 1][j + 1].revealed && !board[i + 1][j + 1].mine)
                    showGrid[i][j] = spriteGrid[i][j];
                }
                std::cout << '\n';
              }
            }

          }
          if (event.key.code == Mouse::Right) {
            rightClicked = true;
            if (!board[click_x + 1][click_y + 1].revealed) {

              if (board[click_x + 1][click_y + 1].flag) {
                board[click_x + 1][click_y + 1].flag = 0;
                showGrid[click_x][click_y] = undiscovered_cell;
              } else {
                board[click_x + 1][click_y + 1].flag = 1;
                showGrid[click_x][click_y] = flag;
              }
            }
          }
        }

        if (unrevealed <= mines)
          {
              for (int j = 0; j < board_height; j++) {
                for (int i = 0; i < board_width; i++) {
                  if (!board[i + 1][j + 1].revealed && !board[i + 1][j + 1].flag)
                    showGrid[i][j] = flag;
                }
              }
            std::string diff1="Beginner";
              if(mines==10)
                diff1="Beginner";
              else if(mines==40)
                 diff1="Intermediate";
              else
                 diff1="Expert";
              save_entry(create_entry(get_current_date(),diff1,elapsed.asSeconds()));
              enabled=false;
              win=true;
          }
      }
    }
    window.clear();
Sprite & sprite=faces[0];
    if(gameover)
         sprite = faces[4];
         else if(win)
            sprite=faces[3];
    sprite.setScale(2, 2);
    if(mines==10)
    sprite.setPosition(window.getSize().x / 2 - 48, 0);
        else
        sprite.setPosition(window.getSize().x / 2 - 24, 0);
    window.draw(sprite);

    Sprite & beginner = diffs[0], & intermediate = diffs[1], & expert = diffs[2];
    beginner.setScale(2, 2);
    intermediate.setScale(2, 2);
    expert.setScale(2, 2);
    beginner.setPosition(window.getSize().x - 48 * 3, 0);
    intermediate.setPosition(window.getSize().x - 48 * 2, 0);
    expert.setPosition(window.getSize().x - 48, 0);

    window.draw(beginner);
    window.draw(intermediate);
    window.draw(expert);

    for (int n = 0; n < 3; n++) {

      int digit = 0;
      if (n == 0) {
        digit = (seconds % 1000) / 100;
      } else if (n == 1) {
        digit = (seconds / 10) % 10;
      } else if (n == 2) {
        digit = seconds % 10;
      }
      if (seconds >= 1000)
        digit = 9; //swag fix

      Sprite & facesprite = timernumbers[digit];
      facesprite.setScale(2, 2);
      facesprite.setPosition(n * 26, 0);
      window.draw(facesprite);
    }

    for (int x = 0; x < board_width; x++) {
      for (int y = 0; y < board_height; y++) {

        sf::Sprite & sprite = showGrid[x][y];
        sprite.setScale(2, 2);
        sprite.setPosition(x * spriteSize, y * spriteSize + 46);

        window.draw(sprite);
      }
    }

    window.display();
  }
}

int main() {
  start(9, 9, 10);

}
