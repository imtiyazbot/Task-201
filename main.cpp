#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

const int CELL  = 20;
const int COLS  = 30;
const int ROWS  = 30;
const int W     = CELL * COLS;
const int H     = CELL * ROWS;

const float SPEED = 0.12f;

const int UP    = 0;
const int DOWN  = 1;
const int LEFT  = 2;
const int RIGHT = 3;

std::vector<sf::Vector2i> snake;

sf::Vector2i food;
int dir     = RIGHT;
int nextDir = RIGHT;
int score   = 0;        // score variable
bool started = false;

void spawnFood()
{
    bool blocked = true;
    while (blocked)
    {
        food.x = rand() % COLS;
        food.y = rand() % ROWS;

        blocked = false;
        for (int i = 0; i < (int)snake.size(); i++)
        {
            if (snake[i] == food)
            {
                blocked = true;
                break;
            }
        }
    }
}

void startGame()
{
    snake.clear();

    int cx = COLS / 2;
    int cy = ROWS / 2;
    snake.push_back({ cx,     cy });
    snake.push_back({ cx - 1, cy });
    snake.push_back({ cx - 2, cy });

    dir     = RIGHT;
    nextDir = RIGHT;
    score   = 0;         //reset score on new game
    started = true;

    srand((unsigned)time(0));
    spawnFood();
}

void moveSnake()
{
    if (nextDir == UP    && dir != DOWN)  dir = UP;
    if (nextDir == DOWN  && dir != UP)    dir = DOWN;
    if (nextDir == LEFT  && dir != RIGHT) dir = LEFT;
    if (nextDir == RIGHT && dir != LEFT)  dir = RIGHT;

    sf::Vector2i newHead = snake[0];
    if (dir == UP)    newHead.y -= 1;
    if (dir == DOWN)  newHead.y += 1;
    if (dir == LEFT)  newHead.x -= 1;
    if (dir == RIGHT) newHead.x += 1;

    snake.insert(snake.begin(), newHead);

    if (newHead == food)
    {
        score += 10;     // increase score by 10
        spawnFood();
    }
    else
    {
        snake.pop_back();
    }
}

void drawSquare(sf::RenderWindow& win, int gx, int gy, sf::Color color)
{
    sf::RectangleShape sq(sf::Vector2f(CELL - 2.f, CELL - 2.f));
    sq.setPosition(gx * CELL + 1.f, gy * CELL + 1.f);
    sq.setFillColor(color);
    win.draw(sq);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Snake Game");
    window.setFramerateLimit(60);

    // load font and score text
    sf::Font font;
    bool fontLoaded = font.loadFromFile("arial.ttf");

    sf::Text scoreTxt;
    if (fontLoaded)
    {
        scoreTxt.setFont(font);
        scoreTxt.setCharacterSize(18);
        scoreTxt.setFillColor(sf::Color::White);
        scoreTxt.setPosition(8.f, 4.f);
    }

    sf::Clock clock;
    float timePassed = 0.f;

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::KeyPressed)
            {
                if (e.key.code == sf::Keyboard::Up    || e.key.code == sf::Keyboard::W)
                    nextDir = UP;
                if (e.key.code == sf::Keyboard::Down  || e.key.code == sf::Keyboard::S)
                    nextDir = DOWN;
                if (e.key.code == sf::Keyboard::Left  || e.key.code == sf::Keyboard::A)
                    nextDir = LEFT;
                if (e.key.code == sf::Keyboard::Right || e.key.code == sf::Keyboard::D)
                    nextDir = RIGHT;

                if (e.key.code == sf::Keyboard::Return)
                    if (!started)
                        startGame();

                if (e.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        if (started)
        {
            timePassed += clock.restart().asSeconds();
            if (timePassed >= SPEED)
            {
                timePassed = 0.f;
                moveSnake();
            }
        }
        else
        {
            clock.restart();
        }

        window.clear(sf::Color(18, 18, 18));

        for (int x = 0; x <= COLS; x++)
        {
            sf::RectangleShape line(sf::Vector2f(1.f, H));
            line.setPosition(x * CELL, 0);
            line.setFillColor(sf::Color(38, 38, 38));
            window.draw(line);
        }
        for (int y = 0; y <= ROWS; y++)
        {
            sf::RectangleShape line(sf::Vector2f(W, 1.f));
            line.setPosition(0, y * CELL);
            line.setFillColor(sf::Color(38, 38, 38));
            window.draw(line);
        }

        if (started)
        {
            drawSquare(window, food.x, food.y, sf::Color(210, 45, 45));

            for (int i = 0; i < (int)snake.size(); i++)
            {
                sf::Color c = (i == 0) ? sf::Color(0, 220, 70) : sf::Color(0, 150, 55);
                drawSquare(window, snake[i].x, snake[i].y, c);
            }

            //draw score on screen
            if (fontLoaded)
            {
                std::ostringstream ss;
                ss << "Score: " << score;
                scoreTxt.setString(ss.str());
                window.draw(scoreTxt);
            }
        }

        window.display();
    }

    return 0;
}
