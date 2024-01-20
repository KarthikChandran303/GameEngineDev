#ifndef H_COMPONENT
#define H_COMPONENT
#include <SFML/Graphics.hpp>
#include "vector.h"
#include <zmq.hpp>

class GameObject;

class Component;

class ComponentManager{
    public:
    enum ComponentType {Transform, Collision, Sprite, Render, pathFollow, playerController, Spawn, Event};
    enum Shapes {Rectangle, Circle};
    //static Component * createComponent(ComponentType type);
};

class Component
{
    public:
    ComponentManager::ComponentType type;
};


class Transform : public Component
{
    public:
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
    sf::Vector2f velocity;
    float maxSpeed = 500.0f;
    bool jumping = false;
    sf::Vector2f acceleration = sf::Vector2f(0.0f, 320.0f);
    bool right = false;
    bool left = false;
    void setSpawnPoint();
    Transform();
};

class Collision : public Component
{
    public:
    bool deathZone;
    bool sideScroll;
    Collision(bool deathZone, bool sideScroll);
};

class Sprite : public Component
{
    public:
    sf::Shape * shape;
    ComponentManager::Shapes shapeType;
    bool player = false;
    std::string texture;
    Sprite(ComponentManager::Shapes shapeType, float radius, float width, float height, std::string texture, bool player);
};

class Render : public Component
{
    public:
    Render();
};

class pathFollow : public Component
{
    public:
    std::vector<sf::Vector2f> targetPositions;
    sf::Vector2f currentTarget;
    int currentIndex;

    void addPathPoint(sf::Vector2f point);
    void setNextTarget();
    sf::Vector2f getTarget();

    pathFollow();
};

class playerController : public Component
{
    public:
    int id = -1;
    zmq::socket_t *socket;

    playerController( int id, zmq::socket_t *socket);
};

class Spawn : public Component
{
    public:
    Spawn();
};

class EventRaiser : public Component
{
    public:
    EventRaiser();
};

#endif