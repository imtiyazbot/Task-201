#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

//  SETTINGS

const int CELL_SIZE = 20;
const int COLUMNS = 30;
const int ROWS = 30;

const int WINDOW_WIDTH = CELL_SIZE * COLUMNS;
const int WINDOW_HEIGHT = CELL_SIZE * ROWS;

const float MOVE_DELAY = 0.005f;

// Directions
const int DIR_UP = 0;
const int DIR_DOWN = 1;
const int DIR_LEFT = 2;
const int DIR_RIGHT = 3;

// GAME STATE

std::vector<sf::Vector2i> snake;
sf::Vector2i food;

int currentDirection = DIR_RIGHT;
int requestedDirection = DIR_RIGHT;

int score = 0;
bool gameOver = false;
bool gameStarted = false;

// Picks a random empty cell for the food to appear in

void spawnFood()
{
    bool cellIsBlocked = true;
    while (cellIsBlocked)
    {
        food.x = rand() % COLUMNS;
        food.y = rand() % ROWS;

        // Assume the position is empty
        cellIsBlocked = false;

        // Check if food is on the snake
        for (int i = 0; i < (int)snake.size(); i++)
        {
            if (snake[i] == food)
            {
                cellIsBlocked = true;
                break;
            }
        }
    }
}

// Reset everything for a new game

void resetGame()
{
    // Remove old snake
    snake.clear();
    int centerX = COLUMNS / 2;
    int centerY = ROWS / 2;

    // Create a snake with 3 blocks
    snake.push_back(sf::Vector2i(centerX, centerY));      // Head
    snake.push_back(sf::Vector2i(centerX - 1, centerY));  // Body
    snake.push_back(sf::Vector2i(centerX - 2, centerY));  // Tail

    currentDirection = DIR_RIGHT;
    requestedDirection = DIR_RIGHT;
    score = 0;
    gameOver = false;
    gameStarted = true;

    srand((unsigned)time(0));
    spawnFood();
}
// can't reverse straight into snake's own body

void updateDirection()
{
    if (requestedDirection == DIR_UP && currentDirection != DIR_DOWN)
        currentDirection = DIR_UP;

    if (requestedDirection == DIR_DOWN && currentDirection != DIR_UP)
        currentDirection = DIR_DOWN;

    if (requestedDirection == DIR_LEFT && currentDirection != DIR_RIGHT)
        currentDirection = DIR_LEFT;

    if (requestedDirection == DIR_RIGHT && currentDirection != DIR_LEFT)
        currentDirection = DIR_RIGHT;
}

// Calculates the next position of the snake's head

sf::Vector2i getNextHeadPosition()
{
    sf::Vector2i nextHead = snake[0];

    if (currentDirection == DIR_UP)
        nextHead.y--;

    if (currentDirection == DIR_DOWN)
        nextHead.y++;

    if (currentDirection == DIR_LEFT)
        nextHead.x--;

    if (currentDirection == DIR_RIGHT)
        nextHead.x++;

    return nextHead;
}

// if snake hits wall

bool hitWall(sf::Vector2i position)
{
    if (position.x < 0 || position.x >= COLUMNS)
        return true;

    if (position.y < 0 || position.y >= ROWS)
        return true;

    return false;
}

// if snake hits itself

bool hitSelf(sf::Vector2i position)
{
    for (int i = 0; i < (int)snake.size(); i++)
    {
        if (snake[i] == position)
            return true;
    }
    return false;
}

// Move the snake

void moveSnake()
{
    updateDirection();
    sf::Vector2i nextHead = getNextHeadPosition();

    if (hitWall(nextHead) || hitSelf(nextHead))
    {
        gameOver = true;
        return;
    }
    // Add new head
    snake.insert(snake.begin(), nextHead);

    if (nextHead == food)
    {
        score += 10;
        spawnFood();
    }
    else
    {
        snake.pop_back();
    }
}
// INPUT

void handleInput(sf::RenderWindow& window)
{
    sf::Event event;

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            // Movement
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                requestedDirection = DIR_UP;

            if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                requestedDirection = DIR_DOWN;

            if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
                requestedDirection = DIR_LEFT;

            if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
                requestedDirection = DIR_RIGHT;

            // Start or Restart
            if (event.key.code == sf::Keyboard::Return)
            {
                if (!gameStarted || gameOver)
                    resetGame();
            }

            // Quit
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
        }
    }
}

// Draw a single square on the grid

void drawSquare(sf::RenderWindow& window, int gridX, int gridY, sf::Color color)
{
    sf::RectangleShape square;

    square.setSize(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    square.setPosition(gridX * CELL_SIZE + 1, gridY * CELL_SIZE + 1);
    square.setFillColor(color);

    window.draw(square);
}

// Center text

void centerText(sf::Text& text, float yPos)
{
    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin(
        bounds.left + bounds.width / 2,
        bounds.top + bounds.height / 2
    );

    text.setPosition(WINDOW_WIDTH / 2, yPos);
}
// MAIN

int main()
{
    srand((unsigned)time(0));

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Snake Game"
    );

    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    bool fontLoaded = font.loadFromFile("arial.ttf");

    // Score text
    sf::Text scoreText;
    if (fontLoaded)
    {
        scoreText.setFont(font);
        scoreText.setCharacterSize(18);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(8.f, 4.f);
    }

    // Menu texts
    sf::Text bigText,smallText;

    if (fontLoaded)
    {
        bigText.setFont(font);
        bigText.setCharacterSize(40);

        smallText.setFont(font);
        smallText.setCharacterSize(20);
    }

    // Dark overlay for menus
    sf::RectangleShape darkOverlay(
        sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)
    );

    darkOverlay.setFillColor(sf::Color(0, 0, 0, 170));

    // Timer
    sf::Clock clock;
    float timePassed = 0.f;

    // Game Loop

    while (window.isOpen())
    {
        // Handle input
        handleInput(window);

        // Update game
        if (gameStarted && !gameOver)
        {
            timePassed += clock.restart().asSeconds();

            if (timePassed >= MOVE_DELAY)
            {
                timePassed = 0.f;
                moveSnake();
            }
        }
        else
        {
            clock.restart();
        }

        // Begin drawing
        window.clear(sf::Color(18, 18, 18));

        // Draw grid
        for (int x = 0; x <= COLUMNS; x++)
        {
            sf::RectangleShape line;

            line.setSize(sf::Vector2f(1.f, WINDOW_HEIGHT));
            line.setPosition(x * CELL_SIZE, 0.f);
            line.setFillColor(sf::Color(38, 38, 38));

            window.draw(line);
        }

        for (int y = 0; y <= ROWS; y++)
        {
            sf::RectangleShape line;

            line.setSize(sf::Vector2f(WINDOW_WIDTH, 1.f));
            line.setPosition(0.f, y * CELL_SIZE);
            line.setFillColor(sf::Color(38, 38, 38));

            window.draw(line);
        }

        // Draw game objects
        if (gameStarted)
        {
            drawSquare(window, food.x, food.y, sf::Color(210, 45, 45));

            for (int i = 0; i < (int)snake.size(); i++)
            {
                sf::Color color;

                if (i == 0)
                    color = sf::Color(0, 220, 70);
                else
                    color = sf::Color(0, 150, 55);

                drawSquare(window, snake[i].x, snake[i].y, color);
            }

            // Draw score
            if (fontLoaded)
            {
                std::ostringstream ss;
                ss << "Score: " << score;

                scoreText.setString(ss.str());

                window.draw(scoreText);
            }
        }

        // START SCREEN

        if (!gameStarted)
        {
            window.draw(darkOverlay);

            if (fontLoaded)
            {
                bigText.setString("SNAKE");
                bigText.setFillColor(sf::Color(0, 220, 70));
                centerText(bigText, WINDOW_HEIGHT / 2.f - 55.f);
                window.draw(bigText);

                smallText.setString(
                    "Press ENTER to Start\n\n"
                    "Arrow Keys or WASD to move\n\n"
                    "ESC to Quit"
                );

                smallText.setFillColor(sf::Color(210, 210, 210));
                centerText(smallText, WINDOW_HEIGHT / 2.f + 35.f);
                window.draw(smallText);
            }
        }

        // GAME OVER SCREEN

        else if (gameOver)
        {
            window.draw(darkOverlay);

            if (fontLoaded)
            {
                bigText.setString("GAME OVER");
                bigText.setFillColor(sf::Color(210, 45, 45));
                centerText(bigText, WINDOW_HEIGHT / 2.f - 65.f);
                window.draw(bigText);

                std::ostringstream ss;

                ss << "Score: " << score
                   << "\n\nPress ENTER to Play Again"
                   << "\n\nESC to Quit";

                smallText.setString(ss.str());
                smallText.setFillColor(sf::Color(210, 210, 210));
                centerText(smallText, WINDOW_HEIGHT / 2.f + 25.f);
                window.draw(smallText);
            }
        }

        window.display();
    }

    return 0;
}
