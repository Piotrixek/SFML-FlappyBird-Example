#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <limits>
#include <cmath>
enum class GameState
{
    MainMenu,
    Playing,
    Paused,
    GameOver
};
enum class Difficulty
{
    Easy,
    Medium,
    Hard
};
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const std::string HIGHSCORE_FILE = "highscore.txt";
struct GameParameters
{
    float pipeSpawnInterval = 2.0f;
    float gapSize = 150.0f;
    float gravity = 0.5f;
    float pipeSpeed = 3.0f;
    float backgroundSpeed = 1.0f;
};
struct PlayerState
{
    float velocity = 0.f;
    int score = 0;
    int lives = 3;
    int highScore = 0;
};
void displayMainMenu(sf::RenderWindow &window, const sf::Font &font, int highScore);
void displayPauseMenu(sf::RenderWindow &window, const sf::Font &font);
void displayGameOver(sf::RenderWindow &window, const sf::Font &font, int score, int highScore);
Difficulty chooseDifficulty(sf::RenderWindow &window, const sf::Font &font);
int chooseLevel(sf::RenderWindow &window, const sf::Font &font);
sf::RenderWindow *createWindow(int width, int height, const std::string &title);
sf::CircleShape createBird(float radius, sf::Color color, float posX, float posY);
std::vector<sf::CircleShape> createHearts(int lives, int windowWidth);
sf::Text createText(const sf::Font &font, const std::string &str, int size, sf::Color color, float posX = 0, float posY = 0);
void resetGame(sf::CircleShape &bird, PlayerState &playerState, std::vector<sf::RectangleShape> &pipes, std::vector<sf::CircleShape> &hearts, GameParameters &params);
void updateGameParameters(Difficulty diff, int level, GameParameters &params);
int loadHighScore();
void saveHighScore(int newHighScore);
void handleInput(sf::RenderWindow &window, sf::CircleShape &bird, PlayerState &playerState, GameState &gameState, GameParameters &params, std::vector<sf::RectangleShape> &pipes, std::vector<sf::CircleShape> &hearts, const sf::Font &font);
void updateGame(sf::CircleShape &bird, PlayerState &playerState, GameParameters &params, std::vector<sf::RectangleShape> &pipes, sf::Clock &pipeClock, std::vector<sf::CircleShape> &hearts, GameState &gameState);
void handleBirdMovement(sf::CircleShape &bird, PlayerState &playerState, const GameParameters &params);
void spawnPipes(std::vector<sf::RectangleShape> &pipes, sf::Clock &pipeClock, const GameParameters &params, int score, int windowHeight, int windowWidth);
void updateAndScorePipes(std::vector<sf::RectangleShape> &pipes, const sf::CircleShape &bird, PlayerState &playerState, const GameParameters &params);
bool checkCollisions(const std::vector<sf::RectangleShape> &pipes, const sf::CircleShape &bird, int windowHeight);
void updateScoreText(sf::Text &scoreText, int score);
void updateHearts(std::vector<sf::CircleShape> &hearts, int lives, int windowWidth);
std::vector<sf::RectangleShape> createBackgroundElements(int count, int windowWidth, int windowHeight);
void updateBackground(std::vector<sf::RectangleShape> &backgroundElements, float speed, int windowWidth);
void drawBackground(sf::RenderWindow &window, const std::vector<sf::RectangleShape> &backgroundElements);
void drawGame(sf::RenderWindow &window, const sf::CircleShape &bird, const std::vector<sf::RectangleShape> &pipes, const std::vector<sf::CircleShape> &hearts, const sf::Text &scoreText, const std::vector<sf::RectangleShape> &backgroundElements);
int loadHighScore()
{
    std::ifstream file(HIGHSCORE_FILE);
    int score = 0;
    if (file.is_open())
    {
        file >> score;
        file.close();
    }
    return score;
}
void saveHighScore(int newHighScore)
{
    int currentHighScore = loadHighScore();
    if (newHighScore > currentHighScore)
    {
        std::ofstream file(HIGHSCORE_FILE);
        if (file.is_open())
        {
            file << newHighScore;
            file.close();
        }
    }
}
Difficulty chooseDifficulty(sf::RenderWindow &window, const sf::Font &font)
{
    sf::Text text = createText(font, "Choose Difficulty:\n1. Easy\n2. Medium\n3. Hard", 30, sf::Color::White);
    text.setPosition(WINDOW_WIDTH / 2.f - text.getLocalBounds().width / 2.f, WINDOW_HEIGHT / 2.f - text.getLocalBounds().height / 1.5f);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)
                    return Difficulty::Easy;
                if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)
                    return Difficulty::Medium;
                if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)
                    return Difficulty::Hard;
            }
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color(70, 70, 70));
        window.draw(text);
        window.display();
    }
    return Difficulty::Easy;
}
int chooseLevel(sf::RenderWindow &window, const sf::Font &font)
{
    sf::Text text = createText(font, "Choose Level (1-5):\n1\n2\n3\n4\n5", 30, sf::Color::White);
    text.setPosition(WINDOW_WIDTH / 2.f - text.getLocalBounds().width / 2.f, WINDOW_HEIGHT / 2.f - text.getLocalBounds().height / 1.5f);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)
                    return 1;
                if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)
                    return 2;
                if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)
                    return 3;
                if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4)
                    return 4;
                if (event.key.code == sf::Keyboard::Num5 || event.key.code == sf::Keyboard::Numpad5)
                    return 5;
            }
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color(70, 70, 70));
        window.draw(text);
        window.display();
    }
    return 1;
}
void updateGameParameters(Difficulty diff, int level, GameParameters &params)
{
    if (diff == Difficulty::Easy)
    {
        params.pipeSpawnInterval = 2.8f;
        params.gapSize = 220.f;
        params.gravity = 0.4f;
        params.pipeSpeed = 2.5f;
    }
    else if (diff == Difficulty::Medium)
    {
        params.pipeSpawnInterval = 2.2f;
        params.gapSize = 180.f;
        params.gravity = 0.5f;
        params.pipeSpeed = 3.0f;
    }
    else
    {
        params.pipeSpawnInterval = 1.7f;
        params.gapSize = 140.f;
        params.gravity = 0.6f;
        params.pipeSpeed = 3.5f;
    }
    int clamped_level = std::max(1, std::min(level, 5));
    params.pipeSpawnInterval = std::max(0.8f, params.pipeSpawnInterval - (clamped_level - 1) * 0.15f);
    params.gapSize = std::max(80.f, params.gapSize - (clamped_level - 1) * 10.f);
    params.gravity += (clamped_level - 1) * 0.03f;
    params.pipeSpeed += (clamped_level - 1) * 0.2f;
    params.backgroundSpeed = params.pipeSpeed * 0.3f;
}
void displayMainMenu(sf::RenderWindow &window, const sf::Font &font, int highScore)
{
    sf::Text title = createText(font, "Enhanced Flappy Bird", 50, sf::Color::Yellow, 0, 100);
    title.setPosition(WINDOW_WIDTH / 2.f - title.getLocalBounds().width / 2.f, 100);
    sf::Text prompt = createText(font, "Press Enter to Start", 30, sf::Color::White, 0, 250);
    prompt.setPosition(WINDOW_WIDTH / 2.f - prompt.getLocalBounds().width / 2.f, 250);
    sf::Text hsText = createText(font, "High Score: " + std::to_string(highScore), 25, sf::Color(200, 200, 200), 0, 350);
    hsText.setPosition(WINDOW_WIDTH / 2.f - hsText.getLocalBounds().width / 2.f, 350);
    window.clear(sf::Color(40, 40, 80));
    window.draw(title);
    window.draw(prompt);
    window.draw(hsText);
    window.display();
}
void displayPauseMenu(sf::RenderWindow &window, const sf::Font &font)
{
    sf::Text pauseText = createText(font, "Paused\nPress P to Resume", 50, sf::Color(255, 255, 255, 180));
    pauseText.setPosition(WINDOW_WIDTH / 2.f - pauseText.getLocalBounds().width / 2.f, WINDOW_HEIGHT / 2.f - pauseText.getLocalBounds().height / 1.5f);
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);
    window.draw(pauseText);
    window.display();
}
void displayGameOver(sf::RenderWindow &window, const sf::Font &font, int score, int highScore)
{
    sf::Text gameOverText = createText(font, "Game Over", 60, sf::Color::Red);
    gameOverText.setPosition(WINDOW_WIDTH / 2.f - gameOverText.getLocalBounds().width / 2.f, 150);
    std::string scoreStr = "Score: " + std::to_string(score);
    if (score > highScore)
    {
        scoreStr += " (New High Score!)";
    }
    sf::Text scoreText = createText(font, scoreStr, 35, sf::Color::White);
    scoreText.setPosition(WINDOW_WIDTH / 2.f - scoreText.getLocalBounds().width / 2.f, 250);
    sf::Text hsText = createText(font, "High Score: " + std::to_string(std::max(score, highScore)), 25, sf::Color(200, 200, 200));
    hsText.setPosition(WINDOW_WIDTH / 2.f - hsText.getLocalBounds().width / 2.f, 310);
    sf::Text prompt = createText(font, "Press Enter to Restart", 30, sf::Color::White);
    prompt.setPosition(WINDOW_WIDTH / 2.f - prompt.getLocalBounds().width / 2.f, 400);
    window.clear(sf::Color(80, 40, 40));
    window.draw(gameOverText);
    window.draw(scoreText);
    window.draw(hsText);
    window.draw(prompt);
    window.display();
}
sf::RenderWindow *createWindow(int width, int height, const std::string &title)
{
    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(width, height), title);
    window->setFramerateLimit(60);
    return window;
}
sf::CircleShape createBird(float radius, sf::Color color, float posX, float posY)
{
    sf::CircleShape bird(radius);
    bird.setFillColor(color);
    bird.setPosition(posX, posY);
    bird.setOrigin(radius, radius);
    return bird;
}
std::vector<sf::CircleShape> createHearts(int lives, int windowWidth)
{
    std::vector<sf::CircleShape> hearts;
    for (int i = 0; i < lives; ++i)
    {
        sf::CircleShape heart(10.f);
        heart.setFillColor(sf::Color::Red);
        heart.setPosition(windowWidth - 40.f - i * 30.f, 15.f);
        hearts.push_back(heart);
    }
    return hearts;
}
void updateHearts(std::vector<sf::CircleShape> &hearts, int lives, int windowWidth)
{
    hearts = createHearts(lives, windowWidth);
}
sf::Text createText(const sf::Font &font, const std::string &str, int size, sf::Color color, float posX, float posY)
{
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(posX, posY);
    return text;
}
void resetGame(sf::CircleShape &bird, PlayerState &playerState, std::vector<sf::RectangleShape> &pipes, std::vector<sf::CircleShape> &hearts, GameParameters &params)
{
    bird.setPosition(200, WINDOW_HEIGHT / 2.f);
    bird.setRotation(0);
    playerState.velocity = 0;
    playerState.score = 0;
    playerState.lives = 3;
    pipes.clear();
    updateHearts(hearts, playerState.lives, WINDOW_WIDTH);
}
std::vector<sf::RectangleShape> createBackgroundElements(int count, int windowWidth, int windowHeight)
{
    std::vector<sf::RectangleShape> elements;
    for (int i = 0; i < count; ++i)
    {
        float width = (float)(std::rand() % 30 + 20);
        float height = (float)(std::rand() % (windowHeight / 2) + windowHeight / 4);
        float xPos = (float)(windowWidth + i * (windowWidth / count + 50));
        float yPos = (float)(windowHeight - height);
        sf::RectangleShape element(sf::Vector2f(width, height));
        sf::Uint8 shade = (sf::Uint8)(std::rand() % 50 + 50);
        element.setFillColor(sf::Color(shade, shade, shade + 30));
        element.setPosition(xPos, yPos);
        elements.push_back(element);
    }
    return elements;
}
void updateBackground(std::vector<sf::RectangleShape> &backgroundElements, float speed, int windowWidth)
{
    for (auto &element : backgroundElements)
    {
        element.move(-speed, 0);
        if (element.getPosition().x + element.getSize().x < 0)
        {
            float newX = (float)(windowWidth + std::rand() % (windowWidth / 2));
            element.setPosition(newX, element.getPosition().y);
            float height = (float)(std::rand() % (WINDOW_HEIGHT / 2) + WINDOW_HEIGHT / 4);
            element.setSize(sf::Vector2f(element.getSize().x, height));
            element.setPosition(newX, WINDOW_HEIGHT - height);
        }
    }
}
void drawBackground(sf::RenderWindow &window, const std::vector<sf::RectangleShape> &backgroundElements)
{
    for (const auto &element : backgroundElements)
    {
        window.draw(element);
    }
}
void handleBirdMovement(sf::CircleShape &bird, PlayerState &playerState, const GameParameters &params)
{
    playerState.velocity += params.gravity;
    bird.move(0, playerState.velocity);
    if (bird.getPosition().y - bird.getRadius() < 0)
    {
        bird.setPosition(bird.getPosition().x, bird.getRadius());
        playerState.velocity = 0;
    }
    float targetRotation = std::min(90.f, std::max(-30.f, playerState.velocity * 4.f));
    bird.setRotation(targetRotation);
}
void spawnPipes(std::vector<sf::RectangleShape> &pipes, sf::Clock &pipeClock, const GameParameters &params, int score, int windowHeight, int windowWidth)
{
    if (pipeClock.getElapsedTime().asSeconds() > params.pipeSpawnInterval)
    {
        pipeClock.restart();
        float pipeWidth = 80.f;
        float minPipeHeight = 50.f;
        float availableHeight = windowHeight - params.gapSize - 2 * minPipeHeight;
        float gapYOffset = static_cast<float>(std::rand() % (int)std::max(1.f, availableHeight));
        float topPipeHeight = minPipeHeight + gapYOffset;
        float bottomPipeY = topPipeHeight + params.gapSize;
        float bottomPipeHeight = windowHeight - bottomPipeY;
        int colorPhase = (score / 10) % 3;
        sf::Color pipeColor;
        if (colorPhase == 0)
            pipeColor = sf::Color(0, 180, 0);
        else if (colorPhase == 1)
            pipeColor = sf::Color(0, 150, 150);
        else
            pipeColor = sf::Color(0, 120, 200);
        sf::RectangleShape topPipe(sf::Vector2f(pipeWidth, topPipeHeight));
        topPipe.setFillColor(pipeColor);
        topPipe.setPosition((float)windowWidth, 0);
        sf::RectangleShape bottomPipe(sf::Vector2f(pipeWidth, bottomPipeHeight));
        bottomPipe.setFillColor(pipeColor);
        bottomPipe.setPosition((float)windowWidth, bottomPipeY);
        pipes.push_back(topPipe);
        pipes.push_back(bottomPipe);
    }
}
bool checkCollisions(const std::vector<sf::RectangleShape> &pipes, const sf::CircleShape &bird, int windowHeight)
{
    for (const auto &pipe : pipes)
    {
        if (bird.getGlobalBounds().intersects(pipe.getGlobalBounds()))
        {
            return true;
        }
    }
    if (bird.getPosition().y + bird.getRadius() > windowHeight)
    {
        return true;
    }
    return false;
}
void updateAndScorePipes(std::vector<sf::RectangleShape> &pipes, const sf::CircleShape &bird, PlayerState &playerState, const GameParameters &params)
{
    bool passedPipe = false;
    for (size_t i = 0; i < pipes.size(); ++i)
    {
        pipes[i].move(-params.pipeSpeed, 0);
        if (i % 2 == 0)
        {
            if (pipes[i].getPosition().x + pipes[i].getSize().x < bird.getPosition().x && !passedPipe)
            {
                float birdX = bird.getPosition().x;
                float closestPipeX = std::numeric_limits<float>::max();
                for (size_t j = 0; j < pipes.size(); j += 2)
                {
                    if (pipes[j].getPosition().x + pipes[j].getSize().x > birdX)
                    {
                        closestPipeX = std::min(closestPipeX, pipes[j].getPosition().x);
                    }
                }
                if (pipes[i].getPosition().x < birdX && pipes[i].getPosition().x > closestPipeX - params.pipeSpeed * 5)
                {
                    passedPipe = true;
                }
            }
        }
    }
    if (!pipes.empty() && pipes[0].getPosition().x + pipes[0].getSize().x < 0)
    {
        pipes.erase(pipes.begin());
        pipes.erase(pipes.begin());
        playerState.score++;
        if (playerState.score > playerState.highScore)
        {
            playerState.highScore = playerState.score;
        }
    }
}
void updateScoreText(sf::Text &scoreText, int score)
{
    scoreText.setString("Score: " + std::to_string(score));
}
void updateGame(sf::CircleShape &bird, PlayerState &playerState, GameParameters &params, std::vector<sf::RectangleShape> &pipes, sf::Clock &pipeClock, std::vector<sf::CircleShape> &hearts, GameState &gameState)
{
    handleBirdMovement(bird, playerState, params);
    spawnPipes(pipes, pipeClock, params, playerState.score, WINDOW_HEIGHT, WINDOW_WIDTH);
    updateAndScorePipes(pipes, bird, playerState, params);
    if (checkCollisions(pipes, bird, WINDOW_HEIGHT))
    {
        playerState.lives--;
        updateHearts(hearts, playerState.lives, WINDOW_WIDTH);
        if (playerState.lives <= 0)
        {
            gameState = GameState::GameOver;
            saveHighScore(playerState.score);
        }
        else
        {
            bird.setPosition(200, WINDOW_HEIGHT / 2.f);
            playerState.velocity = 0;
            bird.setRotation(0);
            pipes.clear();
            pipeClock.restart();
        }
    }
}
void drawGame(sf::RenderWindow &window, const sf::CircleShape &bird, const std::vector<sf::RectangleShape> &pipes, const std::vector<sf::CircleShape> &hearts, const sf::Text &scoreText, const std::vector<sf::RectangleShape> &backgroundElements)
{
    window.clear(sf::Color(135, 206, 235));
    drawBackground(window, backgroundElements);
    for (const auto &pipe : pipes)
    {
        window.draw(pipe);
    }
    window.draw(bird);
    for (const auto &heart : hearts)
    {
        window.draw(heart);
    }
    window.draw(scoreText);
    window.display();
}
void handleInput(sf::RenderWindow &window, sf::CircleShape &bird, PlayerState &playerState, GameState &gameState, GameParameters &params, std::vector<sf::RectangleShape> &pipes, std::vector<sf::CircleShape> &hearts, const sf::Font &font)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            switch (gameState)
            {
            case GameState::MainMenu:
                if (event.key.code == sf::Keyboard::Enter)
                {
                    Difficulty diff = chooseDifficulty(window, font);
                    int level = chooseLevel(window, font);
                    updateGameParameters(diff, level, params);
                    resetGame(bird, playerState, pipes, hearts, params);
                    playerState.highScore = loadHighScore();
                    gameState = GameState::Playing;
                }
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            case GameState::Playing:
                if (event.key.code == sf::Keyboard::Space)
                {
                    playerState.velocity = -8.0f;
                }
                if (event.key.code == sf::Keyboard::P)
                {
                    gameState = GameState::Paused;
                }
                if (event.key.code == sf::Keyboard::Escape)
                    gameState = GameState::Paused;
                break;
            case GameState::Paused:
                if (event.key.code == sf::Keyboard::P)
                {
                    gameState = GameState::Playing;
                }
                if (event.key.code == sf::Keyboard::Enter)
                {
                    gameState = GameState::MainMenu;
                }
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            case GameState::GameOver:
                if (event.key.code == sf::Keyboard::Enter)
                {
                    gameState = GameState::MainMenu;
                }
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            }
        }
    }
}
int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    sf::RenderWindow *windowPtr = createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, " Flappy Bird - SFML");
    if (!windowPtr)
        return -1;
    sf::RenderWindow &window = *windowPtr;
    sf::Font font;
    if (!font.loadFromFile("fonts/arial.ttf"))
    {
#ifdef _WIN32
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
            return -1;
#else
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
            if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf"))
                return -1;
#endif
    }
    GameState gameState = GameState::MainMenu;
    GameParameters gameParams;
    PlayerState playerState;
    playerState.highScore = loadHighScore();
    sf::CircleShape bird = createBird(20.f, sf::Color::Yellow, 200, WINDOW_HEIGHT / 2.f);
    std::vector<sf::RectangleShape> pipes;
    std::vector<sf::CircleShape> hearts = createHearts(playerState.lives, WINDOW_WIDTH);
    std::vector<sf::RectangleShape> backgroundElements = createBackgroundElements(15, WINDOW_WIDTH, WINDOW_HEIGHT);
    sf::Clock pipeClock;
    sf::Clock deltaClock;
    sf::Text scoreText = createText(font, "Score: 0", 24, sf::Color::White, 10, 10);
    while (window.isOpen())
    {
        handleInput(window, bird, playerState, gameState, gameParams, pipes, hearts, font);
        if (gameState == GameState::Playing)
        {
            updateGame(bird, playerState, gameParams, pipes, pipeClock, hearts, gameState);
            updateScoreText(scoreText, playerState.score);
            updateBackground(backgroundElements, gameParams.backgroundSpeed, WINDOW_WIDTH);
        }
        window.clear();
        switch (gameState)
        {
        case GameState::MainMenu:
            displayMainMenu(window, font, playerState.highScore);
            break;
        case GameState::Playing:
            drawGame(window, bird, pipes, hearts, scoreText, backgroundElements);
            break;
        case GameState::Paused:
            drawGame(window, bird, pipes, hearts, scoreText, backgroundElements);
            displayPauseMenu(window, font);
            break;
        case GameState::GameOver:
            displayGameOver(window, font, playerState.score, playerState.highScore);
            break;
        }
    }
    delete windowPtr;
    return 0;
}