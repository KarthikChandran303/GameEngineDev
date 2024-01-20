#include "Component.h"
#include <SFML/Graphics.hpp>
#include "vector.h"
#include <unordered_map>
#include <string>
#include "shapes.h"
#include "Entity.h"

void Transform::setSpawnPoint()
{
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> spawnMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Spawn);
    std::unordered_map<std::string, Component *>::iterator it = spawnMap.begin();
    // get the first spawn point
    Spawn * spawn = (Spawn *)it->second;
    Transform * spawnPos = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
    if(spawnPos != NULL)
    {
        this->position = spawnPos->position;
    }
}

Transform::Transform()
{
    type = ComponentManager::ComponentType::Transform;
    position = sf::Vector2f(0, 0);
    scale = sf::Vector2f(1, 1);
    sf::Vector2f velocity = sf::Vector2f(0, 0);
    rotation = 0;
    maxSpeed = 500.0f;
}

Collision::Collision(bool deathZone, bool sideScroll)
{
    type = ComponentManager::ComponentType::Collision;
    this->deathZone = deathZone;
    this->sideScroll = sideScroll;
}

Sprite::Sprite(ComponentManager::Shapes shapeType, float radius, float width, float height, std::string texture, bool player)
{
    type = ComponentManager::ComponentType::Sprite;
    this->shapeType = shapeType;
    this->texture = texture;
    this->player = player;
    switch (shapeType)
    {
    case ComponentManager::Shapes::Rectangle:
        shape = new sf::RectangleShape(sf::Vector2f(width, height));
        break;
    case ComponentManager::Shapes::Circle:
        shape = new sf::CircleShape(radius);
        break;
    }
    setOrginCenter(*shape);
}

Render::Render()
{
    type = ComponentManager::ComponentType::Render;
}

pathFollow::pathFollow()
{
    type = ComponentManager::ComponentType::pathFollow;
    currentIndex = -1;
}

void pathFollow::addPathPoint(sf::Vector2f point)
{
    targetPositions.push_back(point);
}

void pathFollow::setNextTarget()
{
    currentIndex++;
    if (currentIndex >= targetPositions.size())
    {
        currentIndex = 0;
    }
    currentTarget = targetPositions[currentIndex];
}

sf::Vector2f pathFollow::getTarget()
{
    return currentTarget;
}

playerController::playerController(int id, zmq::socket_t *socket)
{
    this->type = ComponentManager::ComponentType::playerController;
    this->id = id;
    this->socket = socket;
}

Spawn::Spawn()
{
    Spawn::type = ComponentManager::ComponentType::Spawn;
}

EventRaiser::EventRaiser()
{
    EventRaiser::type = ComponentManager::ComponentType::Event;
}

// Component * ComponentManager::createComponent(ComponentType type)
// {
//     switch (type)
//     {
//     case ComponentManager::Transform:
//         return new class Transform();
//         break;
//     case ComponentManager::Collision:
//         return new class Collision();
//         break;
//     case ComponentManager::Render:
//         return new class Render();
//         break;
//     case ComponentManager::Sprite:
//         return new class Sprite();
//         break;
//     default:
//         return nullptr;
//         break;
//     }
// }
