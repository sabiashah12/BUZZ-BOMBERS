#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Constants
const int resolutionX = 960;
const int resolutionY = 640;
const int boxPixelsX = 32;
const int boxPixelsY = 32;

// Game variables
int score = 0;
int currentLevel = 1;
bool honeycombCollected = false;

// Struct for bees
struct Bee {
    float x, y;
    Sprite sprite;
    bool isKillerBee;
    bool isDead;
    bool movingRight;
};

vector<Bee> bees;
bool bulletExists = false;

// Function prototypes
void spawnBees(vector<Bee>& bees, Texture& workerBeeTexture, Texture& killerBeeTexture, int level);
void moveBees(vector<Bee>& bees, float workerSpeed, float killerSpeed, Clock& beeMovementClock);
void checkCollision(vector<Bee>& bees, float bullet_x, float bullet_y, bool& bulletExists, int& score, bool& honeycombCollected);
void resetHoneycombPosition(Sprite& honeycombSprite);
void updateLevel(RenderWindow& window, vector<Bee>& bees, Texture& workerBeeTexture, Texture& killerBeeTexture, int& currentLevel, int& score);

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode(resolutionX, resolutionY), "Buzz Bombers", Style::Close | Style::Titlebar);
    window.setPosition(Vector2i(500, 200));
    
    // Initializing Background Music.
	Music bgMusic;
	if (!bgMusic.openFromFile("Music/Music3.ogg")) {
    	cout << "Error: Could not load music file!" << endl;
	}
	bgMusic.setVolume(50);
	bgMusic.setLoop(true);
	bgMusic.play();

    // Player setup
    float playerX = resolutionX / 2;
    float playerY = resolutionY - 64;
    Texture playerTexture;
    playerTexture.loadFromFile("Textures/spray.png");
    Sprite playerSprite(playerTexture);

    // Bullet setup
    Texture bulletTexture;
    bulletTexture.loadFromFile("Textures/bullet.png");
    Sprite bulletSprite(bulletTexture);
    float bulletX = playerX;
    float bulletY = playerY;

    // Ground setup
    RectangleShape ground(Vector2f(resolutionX, 64));
    ground.setPosition(0, resolutionY - 64);
    ground.setFillColor(Color::Green);

    // Bee textures
    Texture workerBeeTexture, killerBeeTexture;
    workerBeeTexture.loadFromFile("Textures/worker_bee.png");
    killerBeeTexture.loadFromFile("Textures/killer_bee.png");
    spawnBees(bees, workerBeeTexture, killerBeeTexture, currentLevel);

    // Honeycomb setup
    Texture honeycombTexture;
    honeycombTexture.loadFromFile("Textures/honeycomb.png");
    Sprite honeycombSprite(honeycombTexture);
    resetHoneycombPosition(honeycombSprite);

    // Font and text setup
    Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        cout << "Error: Could not load font!" << endl;
        return -1;
    }
    Text scoreText("Score: 0", font, 20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Text levelText("Level: 1", font, 20);
    levelText.setFillColor(Color::White);
    levelText.setPosition(resolutionX - 150, 10);

    Clock beeMovementClock;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }

        // Player movement
        if (Keyboard::isKeyPressed(Keyboard::Left) && playerX > 0) {
            playerX -= 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) && playerX < resolutionX - boxPixelsX) {
            playerX += 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::Space) && !bulletExists) {
            bulletX = playerX + 16;
            bulletY = playerY;
            bulletExists = true;
        }

        // Bullet movement
        if (bulletExists) {
            bulletY -= 10;
            if (bulletY < 0) {
                bulletExists = false;
            }
        }

        // Bee movement and collision handling
        float workerBeeSpeed = 0.5f + currentLevel * 0.2f;
        float killerBeeSpeed = 1.0f + currentLevel * 0.3f;
        moveBees(bees, workerBeeSpeed, killerBeeSpeed, beeMovementClock);
        checkCollision(bees, bulletX, bulletY, bulletExists, score, honeycombCollected);

        // Level progression
        updateLevel(window, bees, workerBeeTexture, killerBeeTexture, currentLevel, score);

        // Update texts
        scoreText.setString("Score: " + to_string(score));
        levelText.setString("Level: " + to_string(currentLevel));

        // Drawing everything
        window.clear();
        window.draw(ground);
        playerSprite.setPosition(playerX, playerY);
        window.draw(playerSprite);

        if (bulletExists) {
            bulletSprite.setPosition(bulletX, bulletY);
            window.draw(bulletSprite);
        }

        for (auto& bee : bees) {
            if (!bee.isDead) {
                window.draw(bee.sprite);
            }
        }

        window.draw(honeycombSprite);
        window.draw(scoreText);
        window.draw(levelText);
        window.display();
    }

    return 0;
}

void spawnBees(vector<Bee>& bees, Texture& workerBeeTexture, Texture& killerBeeTexture, int level) {
    bees.clear();
    int regularBeeCount = 20 + (level - 1) * 5;
    int killerBeeCount = 3 + (level - 1) * 5;

    for (int i = 0; i < regularBeeCount + killerBeeCount; i++) {
        Bee bee;
        bee.x = rand() % resolutionX;
        bee.y = rand() % (resolutionY / 3);
        bee.isKillerBee = i >= regularBeeCount; // First regularBeeCount are workers
        bee.isDead = false;
        bee.movingRight = true;
        bee.sprite.setTexture(bee.isKillerBee ? killerBeeTexture : workerBeeTexture);
        bee.sprite.setPosition(bee.x, bee.y);
        bees.push_back(bee);
    }
}

void moveBees(vector<Bee>& bees, float workerSpeed, float killerSpeed, Clock& beeMovementClock) {
    if (beeMovementClock.getElapsedTime().asMilliseconds() < 100) return;
    beeMovementClock.restart();

    for (auto& bee : bees) {
        if (bee.isDead) continue;
        float speed = bee.isKillerBee ? killerSpeed : workerSpeed;

        if (bee.movingRight) {
            bee.x += speed;
            if (bee.x >= resolutionX - boxPixelsX) {
                bee.movingRight = false;
                bee.sprite.setScale(-1, 1);
                bee.y += boxPixelsY;
            }
        } else {
            bee.x -= speed;
            if (bee.x <= 0) {
                bee.movingRight = true;
                bee.sprite.setScale(1, 1);
                bee.y += boxPixelsY;
            }
        }

        bee.sprite.setPosition(bee.x, bee.y);
    }
}

void checkCollision(vector<Bee>& bees, float bullet_x, float bullet_y, bool& bulletExists, int& score, bool& honeycombCollected) {
    for (auto& bee : bees) {
        if (!bee.isDead && bee.sprite.getGlobalBounds().contains(bullet_x, bullet_y)) {
            bee.isDead = true;
            score += bee.isKillerBee ? 1000 : 100;
            honeycombCollected = true;
            bulletExists = false;
            break;
        }
    }
}

void resetHoneycombPosition(Sprite& honeycombSprite) {
    honeycombSprite.setPosition(rand() % resolutionX, 50);
}

void updateLevel(RenderWindow& window, vector<Bee>& bees, Texture& workerBeeTexture, Texture& killerBeeTexture, int& currentLevel, int& score) {
    bool allBeesDead = true;
    for (const auto& bee : bees) {
        if (!bee.isDead) {
            allBeesDead = false;
            break;
        }
    }

    if (allBeesDead) {
        currentLevel++;
        score += 1000;
        spawnBees(bees, workerBeeTexture, killerBeeTexture, currentLevel);
    }
}
