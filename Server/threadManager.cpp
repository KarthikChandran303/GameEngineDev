#include "threadManager.h"
#include <iostream>
#include "nlohmann/json.hpp"
#include <zmq.hpp>
#include <unordered_map>
#include "Systems.h"

using json = nlohmann::json;

std::vector<std::thread *> ThreadManager::threads;

ThreadManager::ThreadManager()
{
    threads = std::vector<std::thread *>();
}

void ThreadManager::addThread(std::thread *t)
{
    threads.push_back(t);
}

void ThreadManager::joinThreads()
{
    for(int i = 0; i < threads.size(); i++) {
        threads[i]->join();
    }
}

void ThreadManager::freeThreads()
{
    for (std::thread *t : threads)
    {
        delete t;
    }
    threads.clear();
}

ThreadContainer::ThreadContainer(int i, std::mutex *_mutex, std::condition_variable *_condition_variable, float deltaTime)
{
    this->i = i; // set the id of this thread
    if (i == 0)
    {
        busy = true;
    }
    this->_mutex = _mutex;
    this->_condition_variable = _condition_variable;
    this->deltaTime = deltaTime;
}

PlatformThread::PlatformThread(int i, std::mutex *_mutex, std::condition_variable *_condition_variable, float deltaTime, MovingPlatform *platform) : ThreadContainer(i, _mutex, _condition_variable, deltaTime)
{
    this->platform = platform;
}

void PlatformThread::process()
{
    platform->move(deltaTime, _mutex);
}

PlayerThread::PlayerThread(playerControllerSystem * system, std::string GUID, zmq::socket_t * socket) : ThreadContainer(0, NULL, NULL, 0)
{
    this->system = system;
    this->GUID = GUID;
    this->socket = socket;
}

void PlayerThread::process()
{
    system->handlePlayerMovement(GUID, this->socket);
}

void threadFunction(ThreadContainer *threadContainer)
{
    threadContainer->process();
}

void ThreadContainer::run()
{
    std::thread *t = new std::thread(threadFunction, this);
    ThreadManager::addThread(t);
}