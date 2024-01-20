#include "shapes.h"
#include "vector.h"
#include "threadManager.h"
#include "Timeline.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include "nlohmann/json.hpp"
#include <unordered_map>
#include "Setup.h"
#include "sole.hpp"
#include "Entity.h"
#include "Component.h"
#include "Systems.h"
#include "EventManager.h"
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n) Sleep(n)
#endif

using json = nlohmann::json;

void createScene()
{
    // Player
    std::string GUID = GameObjectManager::generateGUID();
    GameObject *player = GameObjectManager::createGameObject(GUID);
    Transform *transform = new Transform();
    transform->position = sf::Vector2f(100, 100);
    Render *render = new Render();
    Sprite *sprite = new Sprite(ComponentManager::Shapes::Circle, 40, 0, 0, "none", true);
    // GameObjectManager::addComponent(GUID, transform);
    // GameObjectManager::addComponent(GUID, render);
    // GameObjectManager::addComponent(GUID, sprite);

    // Moving Platform
    GUID = GameObjectManager::generateGUID();
    GameObject *platform = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(400, 300);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 190, 15, "color", false);
    Collision *collision = new Collision(false, false);
    pathFollow *path = new pathFollow();
    path->addPathPoint(sf::Vector2f(700, 300));
    path->addPathPoint(sf::Vector2f(150, 300));
    path->setNextTarget();
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, path);
    GameObjectManager::addComponent(GUID, collision);

    // Moving Platform 2
    GUID = GameObjectManager::generateGUID();
    GameObject *platform2 = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(1200, 300);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 190, 15, "color", false);
    path = new pathFollow();
    collision = new Collision(false, false);
    path->addPathPoint(sf::Vector2f(1200, 500));
    path->addPathPoint(sf::Vector2f(1200, 50));
    path->setNextTarget();
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, path);
    GameObjectManager::addComponent(GUID, collision);

    // Static Platform
    GUID = GameObjectManager::generateGUID();
    GameObject *staticlatform = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(200, 500);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 100, 100, "mine", false);
    collision = new Collision(false, false);
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    // Ground
    GUID = GameObjectManager::generateGUID();
    GameObject *ground = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(500, 580);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 200, 50, "none", false);
    collision = new Collision(false, false);
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    // Ground 2
    GUID = GameObjectManager::generateGUID();
    GameObject *ground2 = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(100, 580);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 400, 50, "none", false);
    collision = new Collision(false, false);
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    // Ground 3
    GUID = GameObjectManager::generateGUID();
    GameObject *ground3 = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(1200, 580);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 900, 50, "none", false);
    collision = new Collision(false, false);
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    //  Wall
    GUID = GameObjectManager::generateGUID();
    GameObject *Wall = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(0, 400);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 50, 800, "none", false);
    collision = new Collision(false, false);
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    //  Death Zone
    GUID = GameObjectManager::generateGUID();
    GameObject *death = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(400, 800);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 190, 50, "none", false);
    collision = new Collision(true, false);
    GameObjectManager::addComponent(GUID, transform);
    // GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    //  Death Zone 2
    GUID = GameObjectManager::generateGUID();
    GameObject *death2 = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(700, 750);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 190, 50, "none", false);
    collision = new Collision(true, false);
    GameObjectManager::addComponent(GUID, transform);
    // GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    //  Side Scroll
    GUID = GameObjectManager::generateGUID();
    GameObject *sideScroll = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(800, 400);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 10, 800, "none", false);
    collision = new Collision(false, true);
    GameObjectManager::addComponent(GUID, transform);
    // GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, collision);

    // Spawn Point
    GUID = GameObjectManager::generateGUID();
    GameObject *spawn = GameObjectManager::createGameObject(GUID);
    transform = new Transform();
    transform->position = sf::Vector2f(100, 50);
    render = new Render();
    sprite = new Sprite(ComponentManager::Shapes::Rectangle, 0, 50, 50, "none", false);
    Spawn *spawnPoint = new Spawn();
    GameObjectManager::addComponent(GUID, transform);
    // GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, spawnPoint);

}

void createPlayer(int id, zmq::socket_t *sock)
{
    // Player
    std::string GUID = GameObjectManager::generateGUID();
    GameObject *player = GameObjectManager::createGameObject(GUID);
    Transform *transform = new Transform();
    transform->setSpawnPoint();
    Render *render = new Render();
    Sprite *sprite = new Sprite(ComponentManager::Shapes::Circle, 40, 0, 0, "none", true);
    Collision *collision = new Collision(false, false);
    playerController *controller = new playerController(id, sock);
    EventRaiser *eventRaiser = new EventRaiser();
    GameObjectManager::addComponent(GUID, transform);
    GameObjectManager::addComponent(GUID, render);
    GameObjectManager::addComponent(GUID, sprite);
    GameObjectManager::addComponent(GUID, controller);
    GameObjectManager::addComponent(GUID, collision);
    GameObjectManager::addComponent(GUID, eventRaiser);

}

int main()
{
    createScene();

    bool initialization = false;

    movingPlat.setPosition(sf::Vector2f(400, 300));
    movingPlat.addPathPoint(sf::Vector2f(700.0f, 300.0f));
    movingPlat.addPathPoint(sf::Vector2f(100.0f, 300.0f));
    movingPlat.setNextTarget();

    staticPlat.setPosition(200.0f, 500.0f);

    ground.setPosition(400.0f, 580.0f);

    player.setPosition(450.0f, 100.0f);

    sf::Clock clock;
    sf::Vector2f add;

    std::vector<sf::RectangleShape *> objects;

    sf::Vector2f target(700.0f, 300.0f);
    player.jumping = false;

    // sf::Vector2f velocity = sf::Vector2f(0.0f, 0.0f);

    float deltaTime = 0.0f;
    float timeScale = 1.5f;
    int framesPerSecond = 30;

    // Mutex to handle locking, condition variable to handle notifications between threads
    std::mutex m;
    std::condition_variable cv;

    // Create a global timeline
    Timeline *GlobalTimeline = new Timeline(NULL, 0);
    Timeline *GameTimeline = new Timeline(GlobalTimeline, timeScale);


    System * playerCont = new playerControllerSystem();
    EventManager::registerEvent("CONTROL", playerCont);
    System * colSys = new collisionSystem(GameTimeline);
    EventManager::registerEvent("COLLISION", colSys);
    EventManager::registerEvent("SPAWN", colSys);
    EventManager::registerEvent("DEATH", colSys);
    EventManager::registerEvent("SIDESCROLL", colSys);

    // Timeline *AnotherTimeline = new Timeline(GameTimeline, 1.5f);

    float last_time = GameTimeline->getTime();

    // netowrk thread
    //   Prepare our context and socket
    zmq::context_t context(2);
    zmq::socket_t replier(context, zmq::socket_type::rep);
    replier.bind("tcp://*:5555");

    //  Connect as publisher
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5556");
    // return 0;

    // zmq::socket_t *playerSockets[3] = {&player0, &player1, &player2};

    std::vector<zmq::socket_t *> playerSockets;
    int id = 0;
    std::vector<Player *> players;

    //  Initialize poll set
    // zmq::pollitem_t items[] = {
    //     {replier, 0, ZMQ_POLLIN, 0}, {player0, 0, ZMQ_POLLIN, 0}, {player1, 0, ZMQ_POLLIN, 0}, {player2, 0, ZMQ_POLLIN, 0}};

    std::vector<zmq::pollitem_t> items;
    items.push_back({replier, 0, ZMQ_POLLIN, 0});
    // covert vector to array

    json updateData;
    json startUpData;
    startUpData["Players"] = json::array();

    std::unordered_map<int, Player *> playerMap;
    int pok = 0;
    while (true)
    {
        bool updatedPlayer = false;
        zmq::message_t request;

        zmq::poll(&items[0], items.size(), std::chrono::milliseconds(0));

        if (items[0].revents & ZMQ_POLLIN)
        {
            //  Wait for next request from client
            replier.recv(request, zmq::recv_flags::none);
            std::cout << "Received Connection" << std::endl;

            zmq::socket_t *newSock = new zmq::socket_t(context, ZMQ_PULL);
            // Find a socket to bind to
            char port[1024]; // make this sufficiently large.
                             // otherwise an error will be thrown because of invalid argument.
            size_t size = sizeof(port);
            try
            {
                (*newSock).bind("tcp://*:*");
            }
            catch (zmq::error_t &e)
            {
                std::cerr << "Error: could not bind to socket " << e.what();
                return e.num();
            }
            (*newSock).getsockopt(ZMQ_LAST_ENDPOINT, &port, &size);
            playerSockets.push_back(newSock);
            items.push_back({*newSock, 0, ZMQ_POLLIN, 0});

            updateData["id"] = id;
            updateData["port"] = std::string(port);

            std::string updateDataString = updateData.dump();

            zmq::message_t reply(updateDataString.size() + 1);
            snprintf((char *)reply.data(), updateDataString.size() + 1, "%s", updateDataString.c_str());
            // std::cout << "Sending Reply Data" << (char *)reply.data() << std::endl;
            replier.send(reply, zmq::send_flags::none);

            // Create player
            createPlayer(id, newSock);

            last_time = GameTimeline->getTime();

            id++;
            initialization = false;
            zmq::poll(&items[0], items.size(), std::chrono::milliseconds(0));
        }

        // Do work only if there are clients
        if (id > 0)
        {
            float current_time = GameTimeline->getTime();
            float delta_time = current_time - last_time;
            last_time = current_time;
            int numOfPlayers = id;
            ((playerControllerSystem *) (playerCont))->deltaTime = delta_time; // set delta time for event handling system

            // Raise player controller events
            playerControllerSystem *playerControllerSystem = new class playerControllerSystem(delta_time, &initialization, &items, numOfPlayers, &m, GameTimeline);
            playerControllerSystem->update();
            
            // std::cout << "Player Controller System" << std::endl;
            ThreadManager::joinThreads();
            ThreadManager::freeThreads();

            // Handle player controller events
            EventManager::handleEvents();

            // Raise collision events
            collisionRaiserSystem *collisionRaiser = new collisionRaiserSystem(GameTimeline);
            collisionRaiser->update();

            System *pathFollowSystem = new class pathFollowSystem(delta_time);
            pathFollowSystem->update();

            // Handle collision events
            EventManager::handleEvents();

            System *renderSystem = new RenderSystem(publisher);
            renderSystem->update();
            delete renderSystem;

            // std::cout << "Finished Updating" << std::endl;
            

            sf::sleep(sf::seconds(1.0f / framesPerSecond));
            //std::cout << "Waiting.." << std::endl;
        }
    }

    return 0;
}