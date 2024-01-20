#include "Systems.h"
#include "Component.h"
#include "Entity.h"
#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
#include <zmq.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "threadManager.h"
#include "EventManager.h"

using json = nlohmann::json;

RenderSystem::RenderSystem(zmq::socket_t &publisher) : publisher(publisher) {}

void System::handleEvent(Event e)
{
}

void RenderSystem::update()
{
    // create json objects to send
    json entitiesToRender;
    entitiesToRender["Objects"];

    // iterate through map
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> renderMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Render);
    std::unordered_map<std::string, Component *>::iterator it = renderMap.begin();
    while (it != renderMap.end())
    {
        json Entity;
        Entity["player"] = false;
        Entity["id"] = -1;
        Entity["shape"] = "none";
        Entity["texture"] = "none";
        Entity["radius"] = 0;
        Entity["sizeX"] = 0;
        Entity["sizeY"] = 0;
        Entity["position"]["X"] = 0;
        Entity["position"]["Y"] = 0;
        Entity["velocity"]["X"] = 0;
        Entity["velocity"]["Y"] = 0;
        Entity["jumping"] = 0;
        Entity["right"] = false;
        Entity["left"] = false;

        Render *renderer = (Render *)it->second;
        Sprite *sprite = (Sprite *)gm.getComponent(it->first, ComponentManager::ComponentType::Sprite);
        Transform *transform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
        if (sprite != NULL)
        {
            Entity["player"] = sprite->player;
            Entity["texture"] = sprite->texture;
            Entity["position"]["X"] = transform->position.x;
            Entity["position"]["Y"] = transform->position.y;

            switch (sprite->shapeType)
            {
            case ComponentManager::Shapes::Circle:
                Entity["shape"] = "circle";
                Entity["radius"] = (*((sf::CircleShape *)sprite->shape)).getRadius();
                break;
            case ComponentManager::Shapes::Rectangle:
                Entity["shape"] = "rectangle";
                Entity["sizeX"] = (*((sf::RectangleShape *)sprite->shape)).getSize().x;
                Entity["sizeY"] = (*((sf::RectangleShape *)sprite->shape)).getSize().y;
                break;
            }

            if (sprite->player)
            {
                Entity["velocity"]["X"] = transform->velocity.x;
                Entity["velocity"]["Y"] = transform->velocity.y;
                Entity["jumping"] = transform->jumping;
                Entity["right"] = transform->right;
                Entity["left"] = transform->left;
                transform->right = false;
                transform->left = false;
                playerController *controller = (playerController *)gm.getComponent(it->first, ComponentManager::ComponentType::playerController);
                if (controller != NULL)
                {
                    Entity["id"] = controller->id;
                }
            }
            entitiesToRender["Objects"].push_back(Entity);
        }
        ++it;
    }

    // send render data to clients
    zmq::message_t broadcast(entitiesToRender.dump().size() + 1);

    snprintf((char *)broadcast.data(), entitiesToRender.dump().size() + 1, "%s", entitiesToRender.dump().c_str());

    publisher.send(broadcast, zmq::send_flags::none);

    // std::cout << entitiesToRender << std::endl;
}

pathFollowSystem::pathFollowSystem(float deltaTime)
{
    this->deltaTime = deltaTime;
}

void pathFollowSystem::update()
{
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> pathFollowMap = gm.getAllComponentsofType(ComponentManager::ComponentType::pathFollow);
    std::unordered_map<std::string, Component *>::iterator it = pathFollowMap.begin();
    while (it != pathFollowMap.end())
    {
        pathFollow *pathFollow = (class pathFollow *)it->second;
        Transform *transform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);

        if (transform != NULL)
        {
            if (length(transform->position - pathFollow->currentTarget) < 5.0f)
            {
                pathFollow->setNextTarget();
            }

            transform->velocity = pathFollow->currentTarget - transform->position;
            normalize(transform->velocity);
            transform->velocity = transform->velocity * 150.0f;
            transform->position = (transform->position + transform->velocity * deltaTime);
        }
        ++it;
    }
}

void playerControllerSystem::handleEvent(Event e)
{
    // Iterate through event data

    if (e.Type == "CONTROL")
    {
        // std::cout << "Test" << std::endl;
        GameObjectManager gm;
        std::string GUID = *(e.getData("GUID").m_asStringId);
        Transform *transform = (Transform *)gm.getComponent(GUID, ComponentManager::ComponentType::Transform);
        Sprite *sprite = (Sprite *)gm.getComponent(GUID, ComponentManager::ComponentType::Sprite);

        if (e.getData("left").m_asBool)
        {
            transform->velocity = sf::Vector2f(-200.0f, transform->velocity.y);
        }
        else if (e.getData("right").m_asBool)
        {
            transform->velocity = sf::Vector2f(200.0f, transform->velocity.y);
        }
        else
        {
            transform->velocity = sf::Vector2f(0.0f, transform->velocity.y);
        }
 
        if (e.getData("space").m_asBool && !transform->jumping)
        {
            std::cout << "Jumping" << std::endl;
            transform->velocity= sf::Vector2f(transform->velocity.y, -340.0f);
            transform->jumping = true;
        }

        // float velocityX = e.getData("velocityX").m_asFloat;
        // float velocityY = e.getData("velocityY").m_asFloat;
        // bool jumping = e.getData("jumping").m_asBool;
        // // std::cout << "GUID: " << GUID << " velocityX: " << velocityX << " velocityY: " << velocityY << " jumping: " << jumping << std::endl;

        

        // transform->velocity = sf::Vector2f(velocityX, velocityY);
        // transform->jumping = jumping;
        transform->velocity += transform->acceleration * deltaTime;
        if (length(transform->velocity) >= transform->maxSpeed)
        {
            normalize(transform->velocity);
            transform->velocity = sf::Vector2f(transform->velocity.x * transform->maxSpeed, transform->velocity.y * transform->maxSpeed);
        }
        transform->position = (transform->position + transform->velocity * deltaTime);
        sprite->shape->setPosition(transform->position);
    }
}

void playerControllerSystem::handlePlayerMovement(std::string GUID, zmq::socket_t *socket)
{
    GameObjectManager gm;
    Transform *transform = (Transform *)gm.getComponent(GUID, ComponentManager::ComponentType::Transform);
    Sprite *sprite = (Sprite *)gm.getComponent(GUID, ComponentManager::ComponentType::Sprite);

    transform->right = false;
    transform->left = false;

    zmq::message_t update;
    socket->recv(update, zmq::recv_flags::none);
    std::string updateString = std::string(static_cast<char *>(update.data()), update.size());
    json updateJson = json::parse(updateString);
    if (updateJson["closed"])
    {
        gm.removeComponent(GUID, ComponentManager::ComponentType::playerController);
        gm.removeComponent(GUID, ComponentManager::ComponentType::Render);
        return;
    }

    Event controllerEvent = Event("CONTROL", this->timeline->getTime(), 0.0);
    std::string *GUIDptr = new std::string(GUID);
    controllerEvent.addData("GUID", GUIDptr);
    controllerEvent.addData("right", (bool)updateJson["rightArrow"]);
    controllerEvent.addData("left", (bool)updateJson["leftArrow"]);
    controllerEvent.addData("space", (bool)updateJson["space"]);
    
    controllerEvent.addData("deltaTime", (float)deltaTime);
    {
        std::unique_lock<std::mutex> lock(*_mutex);
        EventManager::raiseEvent(controllerEvent);
    }

    // transform->velocity = sf::Vector2f(updateJson["X"], updateJson["Y"]);
    // transform->jumping = updateJson["jumping"];
    // transform->velocity += transform->acceleration * deltaTime;
    // if (length(transform->velocity) >= transform->maxSpeed)
    // {
    //     normalize(transform->velocity);
    //     transform->velocity = sf::Vector2f(transform->velocity.x * transform->maxSpeed, transform->velocity.y * transform->maxSpeed);
    // }
    // transform->position = (transform->position + transform->velocity * deltaTime);
    // sprite->shape->setPosition(transform->position);
    // std::cout << "Event: " << controllerEvent.getData("velocityX").m_asFloat << controllerEvent.getData("velocityY").m_asFloat << controllerEvent.getData("jumping").m_asBool << std::endl;
}

void playerControllerSystem ::update()
{
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> playerControllerMap = gm.getAllComponentsofType(ComponentManager::ComponentType::playerController);
    std::unordered_map<std::string, Component *>::iterator it = playerControllerMap.begin();

    while (it != playerControllerMap.end())
    {
        playerController *playerController = (class playerController *)it->second;
        this->id = playerController->id;

        if (*init || ((*items)[this->id + 1].revents & ZMQ_POLLIN))
        {
            PlayerThread *playerThread = new PlayerThread(this, it->first, playerController->socket);
            playerThread->run();
            if (this->id == numOfPlayers - 1)
            {
                *init = true;
            }
        }
        ++it;
    }
}

playerControllerSystem::playerControllerSystem(float deltaTime, bool *init, std::vector<zmq_pollitem_t> *items, int numOfPlayers, std::mutex *mutex, Timeline *timeline)
{
    this->deltaTime = deltaTime;
    this->init = init;
    this->items = items;
    this->numOfPlayers = numOfPlayers;
    this->id = 0;
    this->_mutex = mutex;
    this->timeline = timeline;
}

playerControllerSystem::playerControllerSystem()
{
}

collisionSystem::collisionSystem(Timeline *timeline)
{
    this->timeline = timeline;
}

void collisionSystem::handleEvent(Event e)
{
    GameObjectManager gm;
    if (e.Type == "COLLISION")
    {
        std::string playerID = *(e.getData("player").m_asStringId);
        std::string colliderID = *(e.getData("collider").m_asStringId);
        // std::cout << "Collision between " << playerID << " and " << colliderID << std::endl;
        Transform *playerTransform = (Transform *)gm.getComponent(playerID, ComponentManager::ComponentType::Transform);
        Sprite *collisionSprite = (Sprite *)gm.getComponent(colliderID, ComponentManager::ComponentType::Sprite);
        Sprite *playerSprite = (Sprite *)gm.getComponent(playerID, ComponentManager::ComponentType::Sprite);

        if (playerTransform->position.x > collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width)
        {
            playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width + ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
        }
        else if (playerTransform->position.x < collisionSprite->shape->getGlobalBounds().left)
        {
            playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left - ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
        }
        else if (playerTransform->position.y > collisionSprite->shape->getGlobalBounds().top + collisionSprite->shape->getGlobalBounds().height)
        {
            playerTransform->velocity = sf::Vector2f(playerTransform->velocity.x, 0.0f);
            playerTransform->position = sf::Vector2f(playerTransform->position.x, collisionSprite->shape->getGlobalBounds().top + collisionSprite->shape->getGlobalBounds().height + ((sf::CircleShape *)(playerSprite->shape))->getRadius());
        }
        else if (playerTransform->position.y < collisionSprite->shape->getGlobalBounds().top)
        {
            playerTransform->jumping = false;
            playerTransform->velocity = sf::Vector2f(playerTransform->velocity.x, 0.0f);
            playerTransform->position = sf::Vector2f(playerTransform->position.x, collisionSprite->shape->getGlobalBounds().top - ((sf::CircleShape *)(playerSprite->shape))->getRadius());
        }
    }
    else if (e.Type == "DEATH")
    {
        std::cout << "REACHED DEATH ZONE" << std::endl;
        std::string playerID = *(e.getData("player").m_asStringId);
        std::string colliderID = *(e.getData("collider").m_asStringId);

        std::unordered_map<std::string, Component *> spawnMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Spawn);
        Transform *playerTransform = (Transform *)gm.getComponent(playerID, ComponentManager::ComponentType::Transform);
        std::unordered_map<std::string, Component *>::iterator it = spawnMap.begin();
        float minDistance = 1000000.0f;
        sf::Vector2f spawnPos = sf::Vector2f(0.0, 0.0);
        while (it != spawnMap.end())
        {
            Spawn *spawn = (class Spawn *)it->second;
            Transform *spawnTransform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
            if (spawnTransform != NULL)
            {
                if (length(spawnTransform->position - playerTransform->position) < minDistance)
                {
                    minDistance = length(spawnTransform->position - playerTransform->position);
                    spawnPos = spawnTransform->position;
                }
            }
            ++it;
        }
        playerTransform->position = spawnPos;

        Event spawnEvent = Event("SPAWN", this->timeline->getTime(), 0);
        std::string *playerIDPtr = new std::string(playerID);
        spawnEvent.addData("player", playerIDPtr);
        EventManager::raiseEvent(spawnEvent);
    }
    else if (e.Type == "SIDESCROLL")
    {
        std::cout << "REACHED SIDESCROLL ZONE" << std::endl;
        std::string playerID = *(e.getData("player").m_asStringId);
        std::string colliderID = *(e.getData("collider").m_asStringId);

        Transform *playerTransform = (Transform *)gm.getComponent(playerID, ComponentManager::ComponentType::Transform);
        Sprite *collisionSprite = (Sprite *)gm.getComponent(colliderID, ComponentManager::ComponentType::Sprite);
        Sprite *playerSprite = (Sprite *)gm.getComponent(playerID, ComponentManager::ComponentType::Sprite);

        if (playerTransform->position.x > collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width)
        {
            playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left - ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
            playerTransform->left = true;
        }
        else if (playerTransform->position.x < collisionSprite->shape->getGlobalBounds().left)
        {
            playerTransform->right = true;
            playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width + ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
        }
        else
        {
            playerTransform->right = false;
            playerTransform->left = false;
        }
    }
    else if (e.Type == "SPAWN")
    {
        std::cout << "REACHED SPAWN" << std::endl;
        std::string playerID = *(e.getData("player").m_asStringId);
        Sprite *playerSprite = (Sprite *)gm.getComponent(playerID, ComponentManager::ComponentType::Sprite);
        if( playerSprite->texture == "none") {
            playerSprite->texture = "player2";
        }
        else {
            playerSprite->texture = "none";
        }
        
        // playerSprite->shape->setScale(0.5f, 0.5f);
        // playerSprite->shape->setOutlineThickness(5.0f);
    }

}

void collisionSystem::update()
{
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> collisionMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Collision);
    std::unordered_map<std::string, Component *>::iterator it = collisionMap.begin();
    std::vector<std::string> players;
    std::vector<std::string> collisionEntities;
    while (it != collisionMap.end())
    {
        Collision *collision = (class Collision *)it->second;
        Transform *transform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
        Sprite *sprite = (Sprite *)gm.getComponent(it->first, ComponentManager::ComponentType::Sprite);

        if (transform != NULL && sprite != NULL)
        {
            if (sprite->player)
            {
                players.push_back(it->first);
            }
            else
            {
                collisionEntities.push_back(it->first);
            }
        }
        ++it;
    }

    for (int i = 0; i < players.size(); i++)
    {
        Transform *playerTransform = (Transform *)gm.getComponent(players[i], ComponentManager::ComponentType::Transform);
        Sprite *playerSprite = (Sprite *)gm.getComponent(players[i], ComponentManager::ComponentType::Sprite);
        playerSprite->shape->setPosition(playerTransform->position);
        for (int j = 0; j < collisionEntities.size(); j++)
        {
            Transform *collisionTransform = (Transform *)gm.getComponent(collisionEntities[j], ComponentManager::ComponentType::Transform);
            Sprite *collisionSprite = (Sprite *)gm.getComponent(collisionEntities[j], ComponentManager::ComponentType::Sprite);
            Collision *collisionComponent = (Collision *)gm.getComponent(collisionEntities[j], ComponentManager::ComponentType::Collision);
            collisionSprite->shape->setPosition(collisionTransform->position);
            if (playerSprite->shape->getGlobalBounds().intersects(collisionSprite->shape->getGlobalBounds()))
            {
                if (collisionComponent->deathZone)
                {
                    std::unordered_map<std::string, Component *> spawnMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Spawn);
                    std::unordered_map<std::string, Component *>::iterator it = spawnMap.begin();
                    float minDistance = 1000000.0f;
                    sf::Vector2f spawnPos = sf::Vector2f(0.0, 0.0);
                    while (it != spawnMap.end())
                    {
                        Spawn *spawn = (class Spawn *)it->second;
                        Transform *spawnTransform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
                        if (spawnTransform != NULL)
                        {
                            if (length(spawnTransform->position - playerTransform->position) < minDistance)
                            {
                                minDistance = length(spawnTransform->position - playerTransform->position);
                                spawnPos = spawnTransform->position;
                            }
                        }
                        ++it;
                    }
                    playerTransform->position = spawnPos;
                }
                else if (collisionComponent->sideScroll)
                {
                    if (playerTransform->position.x > collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width)
                    {
                        playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left - ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
                        playerTransform->left = true;
                    }
                    else if (playerTransform->position.x < collisionSprite->shape->getGlobalBounds().left)
                    {
                        playerTransform->right = true;
                        playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width + ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
                    }
                    else
                    {
                        playerTransform->right = false;
                        playerTransform->left = false;
                    }
                }

                else
                {
                    // jumping = false;
                    if (playerTransform->position.x > collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width)
                    {
                        playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left + collisionSprite->shape->getGlobalBounds().width + ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
                    }
                    else if (playerTransform->position.x < collisionSprite->shape->getGlobalBounds().left)
                    {
                        playerTransform->position = sf::Vector2f(collisionSprite->shape->getGlobalBounds().left - ((sf::CircleShape *)(playerSprite->shape))->getRadius(), playerTransform->position.y);
                    }
                    else if (playerTransform->position.y > collisionSprite->shape->getGlobalBounds().top + collisionSprite->shape->getGlobalBounds().height)
                    {
                        playerTransform->velocity = sf::Vector2f(playerTransform->velocity.x, 0.0f);
                        playerTransform->position = sf::Vector2f(playerTransform->position.x, collisionSprite->shape->getGlobalBounds().top + collisionSprite->shape->getGlobalBounds().height + ((sf::CircleShape *)(playerSprite->shape))->getRadius());
                    }
                    else if (playerTransform->position.y < collisionSprite->shape->getGlobalBounds().top)
                    {
                        playerTransform->jumping = false;
                        playerTransform->velocity = sf::Vector2f(playerTransform->velocity.x, 0.0f);
                        playerTransform->position = sf::Vector2f(playerTransform->position.x, collisionSprite->shape->getGlobalBounds().top - ((sf::CircleShape *)(playerSprite->shape))->getRadius());
                    }
                }
            }
        }
    }
}

collisionRaiserSystem::collisionRaiserSystem(Timeline *timeline)
{
    this->timeline = timeline;
}

void collisionRaiserSystem::update()
{
    GameObjectManager gm;
    std::unordered_map<std::string, Component *> eventMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Event);
    std::unordered_map<std::string, Component *>::iterator eventIt = eventMap.begin();
    std::unordered_map<std::string, Component *> collisionMap = gm.getAllComponentsofType(ComponentManager::ComponentType::Collision);
    // print collisionMap
    int count = 0;
    while (eventIt != eventMap.end())
    {
        // std::cout << count++ << std::endl;
        //  Get Components pertaining to event
        Sprite *eventSprite = (Sprite *)gm.getComponent(eventIt->first, ComponentManager::ComponentType::Sprite);
        Transform *eventTransform = (Transform *)gm.getComponent(eventIt->first, ComponentManager::ComponentType::Transform);
        Collision *eventCollision = (Collision *)gm.getComponent(eventIt->first, ComponentManager::ComponentType::Collision);
        eventSprite->shape->setPosition(eventTransform->position);
        std::string *playerID;
        if (eventSprite != NULL && eventSprite->player && eventCollision != NULL)
        {
            std::unordered_map<std::string, Component *>::iterator it = collisionMap.begin();
            std::string *collisonEntity;
            while (it != collisionMap.end())
            {

                Transform *transform = (Transform *)gm.getComponent(it->first, ComponentManager::ComponentType::Transform);
                Sprite *sprite = (Sprite *)gm.getComponent(it->first, ComponentManager::ComponentType::Sprite);
                Collision *collision = (Collision *)gm.getComponent(it->first, ComponentManager::ComponentType::Collision);

                sprite->shape->setPosition(transform->position);
                if (transform != NULL && sprite != NULL)
                {
                    // std::cout << "test" << std::endl;
                    if (!sprite->player && sprite->shape->getGlobalBounds().intersects(eventSprite->shape->getGlobalBounds()))
                    {

                        playerID = new std::string(eventIt->first);
                        collisonEntity = new std::string(it->first);

                        if (collision->deathZone)
                        {
                            Event deathEvent = Event("DEATH", this->timeline->getTime(), 0);
                            deathEvent.addData("player", playerID);
                            deathEvent.addData("collider", collisonEntity);
                            EventManager::raiseEvent(deathEvent);
                        }
                        else if (collision->sideScroll)
                        {
                            Event sideScrollEvent = Event("SIDESCROLL", this->timeline->getTime(), 0);
                            sideScrollEvent.addData("player", playerID);
                            sideScrollEvent.addData("collider", collisonEntity);
                            EventManager::raiseEvent(sideScrollEvent);
                        }
                        else
                        {
                            Event collisionEvent = Event("COLLISION", this->timeline->getTime(), 0);
                            collisionEvent.addData("player", playerID);
                            collisionEvent.addData("collider", collisonEntity);
                            EventManager::raiseEvent(collisionEvent);
                        }
                        // std::cout << "PLAYER: " << *( collisionEvent.getData("player").m_asStringId ) << " COLLIDER: " << *(collisionEvent.getData("collider").m_asStringId) << std::endl;
                    }
                }
                ++it;
            }
        }
        ++eventIt;
    }
}

// void controllerRaiserSystem::update()
// {
//     GameObjectManager gm;
//     std::unordered_map<std::string, Component *> playerControllerMap = gm.getAllComponentsofType(ComponentManager::ComponentType::playerController);
//     std::unordered_map<std::string, Component *>::iterator it = playerControllerMap.begin();

//     while (it != playerControllerMap.end())
//     {
//         playerController *playerController = (class playerController *)it->second;
//         this->id = playerController->id;

//         if (*init || ((*items)[this->id + 1].revents & ZMQ_POLLIN))
//         {
//             PlayerThread *playerThread = new PlayerThread(this, it->first, playerController->socket);
//             playerThread->run();
//             if (this->id == numOfPlayers - 1)
//             {
//                 *init = true;
//             }
//         }
//         ++it;
//     }
// }