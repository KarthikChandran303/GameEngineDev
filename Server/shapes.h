#ifndef H_SHAPES
#define H_SHAPES
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <SFML/Graphics.hpp>

void setOrginCenter(sf::Shape &shape);

class Platform : public sf::RectangleShape
{
public:
    Platform(float width, float height);
};

class MovingPlatform : public Platform
{
public:
    MovingPlatform(float width, float height);
    sf::Vector2f velocity;
    void move(float deltaTime, std::mutex *_mutex);
    void addPathPoint(sf::Vector2f point);
    void setNextTarget();
    sf::Vector2f getTarget();
    void moveToTarget(float deltaTime, std::mutex *_mutex);

private:
    sf::Vector2f currentTarget;
    int currentIndex;
    std::vector<sf::Vector2f> targetPositions;
};

class StationaryPlatform : public Platform
{
public:
    StationaryPlatform(float width, float height);
};

class Player : public sf::CircleShape
{
public:
    Player(float radius);
    bool jumping = false;
    void updatePos(float deltaTime);
    void setVelocity(sf::Vector2f newVelocity);
    void setMaxSpeed(float newMaxSpeed);
    sf::Vector2f getVelocity();
    sf::Vector2f acceleration;

private:
    sf::Vector2f velocity;
    float maxSpeed;
};

#endif