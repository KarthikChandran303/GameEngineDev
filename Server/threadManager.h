#ifndef H_THREADMANAGER
#define H_THREADMANAGER
#include "shapes.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <zmq.hpp>
#include <unordered_map>
#include "Systems.h"

class ThreadManager
{
public: 
    ThreadManager();
    static std::vector<std::thread *> threads;
    static void addThread(std::thread *t);
    static void joinThreads();
    static void freeThreads();
};

class ThreadContainer
{
    public:
    ThreadContainer(int i, std::mutex *_mutex, std::condition_variable *_condition_variable, float deltaTime);
    bool busy;                                    // a member variable used to indicate thread "status"
    int i;                                        // an identifier
    std::mutex *_mutex;                           // the object for mutual exclusion of execution
    std::condition_variable *_condition_variable; // for messaging between threads
    float deltaTime;
    bool *isOpen;
    virtual void process() = 0;
    virtual void run();
};

class PlatformThread : public ThreadContainer
{
    public:
    PlatformThread(int i, std::mutex *_mutex, std::condition_variable *_condition_variable, float deltaTime, MovingPlatform *platform);
    MovingPlatform *platform;
    void process();
};

class PlayerThread : public ThreadContainer
{
    public:
    playerControllerSystem * system;
    std::string GUID;
    zmq::socket_t *socket;
    PlayerThread(playerControllerSystem * system, std::string GUID, zmq::socket_t *socket);    
    void process();
};

void threadFunction(ThreadContainer *threadContainer);
    
#endif