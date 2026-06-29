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
int score   = 0;
bool over    = false;   // game over flag
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
    score   = 0;
    over    = false;    // reset game over flag
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

    // NEW: wall collision check
    if (newHead.x < 0 || newHead.x >= COLS ||
        newHead.y < 0 || newHead.y >= ROWS)
    {
        over = true;
        return;
    }

    //self collision check
    for (int i = 0; i < (int)snake.size(); i++)
    {
        if (snake[i] == newHead)
        {
            over = true;
            return;
        }
    }

    snake.insert(snake.begin(), newHead);

    if (newHead == food)
    {
        score += 10;
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

// helper to center text on screen
void centerOn(sf::Text& t, float yPos)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(W / 2.f, yPos);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(W, H), "Snake Game");
    window.setFramerateLimit(60);

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

    //big and small text for overlays
    sf::Text bigTxt, smallTxt;
    if (fontLoaded)
    {
        bigTxt.setFont(font);   bigTxt.setCharacterSize(40);
        smallTxt.setFont(font); smallTxt.setCharacterSize(20);
    }

    //dark overlay rectangle for screens
    sf::RectangleShape darkOverlay(sf::Vector2f(W, H));
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 170));

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

                // Enter now also restarts after game over
                if (e.key.code == sf::Keyboard::Return)
                    if (!started || over)
                        startGame();

                if (e.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        // stop updating when game is over
        if (started && !over)
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

            if (fontLoaded)
            {
                std::ostringstream ss;
                ss << "Score: " << score;
                scoreTxt.setString(ss.str());
                window.draw(scoreTxt);
            }
        }

        // start screen overlay
        if (!started)
        {
            window.draw(darkOverlay);
            if (fontLoaded)
            {
                bigTxt.setString("SNAKE");
                bigTxt.setFillColor(sf::Color(0, 220, 70));
                centerOn(bigTxt, H / 2.f - 55.f);
                window.draw(bigTxt);

                smallTxt.setString("Press ENTER to Start\n\nArrow Keys or WASD to move\n\nESC to Quit");
                smallTxt.setFillColor(sf::Color(210, 210, 210));
                centerOn(smallTxt, H / 2.f + 35.f);
                window.draw(smallTxt);
            }
        }

        //game over screen overlay
        else if (over)
        {
            window.draw(darkOverlay);
            if (fontLoaded)
            {
                bigTxt.setString("GAME OVER");
                bigTxt.setFillColor(sf::Color(210, 45, 45));
                centerOn(bigTxt, H / 2.f - 65.f);
                window.draw(bigTxt);

                std::ostringstream ss;
                ss << "Score: " << score << "\n\nPress ENTER to Play Again\n\nESC to Quit";
                smallTxt.setString(ss.str());
                smallTxt.setFillColor(sf::Color(210, 210, 210));
                centerOn(smallTxt, H / 2.f + 25.f);
                window.draw(smallTxt);
            }
        }

        window.display();
    }

    return 0;
}
