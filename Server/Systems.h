#ifndef H_SYSTEMS
#define H_SYSTEMS
#include "Component.h"
#include <unordered_map>
#include <string>
#include <zmq.hpp>
#include "Timeline.h"

class Event;

class System
{
    public:
    virtual void handleEvent(Event e);
    virtual void update() = 0;
};

class RenderSystem : public System
{
    public:
    zmq::socket_t &publisher;

    RenderSystem(zmq::socket_t &publisher);

    void update();
};

class pathFollowSystem : public System
{
    public:
    float deltaTime;

    void update();

    pathFollowSystem(float deltaTime);
};

class playerControllerSystem : public System
{
    public:
    float deltaTime;
    int id;
    bool *init;
    std::vector<zmq_pollitem_t> *items;
    std::mutex *_mutex;
    int numOfPlayers;
    Timeline *timeline;

    void update();
    void handlePlayerMovement(std::string GUID, zmq::socket_t *socket);
    void handleEvent(Event e);

    playerControllerSystem(float deltaTime, bool *init, std::vector<zmq_pollitem_t> *items, int numOfPlayers, std::mutex *_mutex, Timeline *timeline);
    playerControllerSystem();
};

class collisionSystem : public System
{
    public:
    Timeline *timeline;

    void update();
    void handleEvent(Event e);
    collisionSystem(Timeline *timeline);
};

class collisionRaiserSystem : public System
{
    public:
    Timeline *timeline;
    void update();
    collisionRaiserSystem(Timeline *timeline);
};

class controllerRaiserSystem : public System
{
    public:
    float timeStep;
    int id;
    bool *init;
    std::vector<zmq_pollitem_t> *items;
    int numOfPlayers;

    void update();
    controllerRaiserSystem(float timeStep, bool *init, std::vector<zmq_pollitem_t> *items, int numOfPlayers);
};

#endif