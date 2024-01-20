#include "EventManager.h"
#include "Systems.h"
#include <unordered_map>
#include <iostream>

std::unordered_map<std::string, std::vector<System *> > EventManager::registeredHandlers;
std::priority_queue<Event, std::vector<Event>, Compare> EventManager::eventQueue;
std::vector<std::string> EventManager::EventTypes = {"COLLISION", "DEATH", "SPAWN", "CONTROL", "SIDESCROLL"};

Variant::Variant()
{
    m_type = TYPE_ERROR;
}

Variant::Variant(int value) : m_type(TYPE_INTEGER)
{
    m_asInteger = value;
}

Variant::Variant(float value) : m_type(TYPE_FLOAT)
{
    m_asFloat = value;
}

Variant::Variant(bool value) : m_type(TYPE_BOOL)
{
    m_asBool = value;
}

Variant::Variant(std::string *value) : m_type(TYPE_STRING_ID)
{
    m_asStringId = value;
}

Event::Event(std::string Type, float timeStamp, float priority)
{
    this->Type = Type;
    this->timeStamp = timeStamp;
    this->priority = priority;
    data = std::unordered_map<std::string, Variant>();
}

void Event::addData(std::string key, int val)
{
    data[key] = Variant(val);
}

void Event::addData(std::string key, float val)
{
    data[key] = Variant(val);
}

void Event::addData(std::string key, bool val)
{
    data[key] = Variant(val);
}

void Event::addData(std::string key, std::string *val)
{
    data[key] = Variant(val);
}

Variant Event::getData(std::string key)
{
    if (data.find(key) != data.end())
    {
        return data[key];
    }
    else
    {
        return Variant();
    }
}

bool Compare::operator() (Event e1, Event e2)
{
    {
        if(e1.timeStamp == e2.timeStamp)
        {
            return e1.priority > e2.priority;
        }
        else {
            return e1.timeStamp > e2.timeStamp;
        }
    }
}

void EventManager::registerEvent(std::string EventType, System *system)
{
    // check if event type exists
    bool found = false;
    for(int i = 0; i < EventTypes.size(); i++)
    {
        if(EventTypes.at(i) == EventType)
        {
            found = true;
            break;
        }
    }

    if(!found)
    {
        std::cout << "Invalid event type" << std::endl;
        return;
    }

    if(registeredHandlers.find(EventType) == registeredHandlers.end())
    {
        std::vector<System *> systems;
        systems.push_back(system);
        registeredHandlers[EventType] = systems;
    }
    else
    {
        registeredHandlers[EventType].push_back(system);
    }
}

void EventManager::unregister(std::string EventType, System *system)
{
    if(registeredHandlers.find(EventType) != registeredHandlers.end())
    {
        std::vector<System *> systems = registeredHandlers[EventType];
        for(int i = 0; i < systems.size(); i++)
        {
            if(systems.at(i) == system)
            {
                systems.erase(systems.begin() + i);
                break;
            }
        }
    }
}

void EventManager::raiseEvent(Event e)
{
    bool valid = false;
    for(int i = 0; i < EventTypes.size(); i++)
    {
        if(EventTypes.at(i) == e.Type)
        {
            valid = true;
            break;
        }
    }
    if(!valid)
    {
        std::cout << "Invalid event type" << std::endl;
        return;
    }
    eventQueue.push(e);
}

void EventManager::handleEvents()
{
    while(!eventQueue.empty())
    {
        Event e = eventQueue.top();
        eventQueue.pop();
        std::vector<System *> systems = registeredHandlers[e.Type];
        for(int i = 0; i < systems.size(); i++)
        {
            systems.at(i)->handleEvent(e);
        }
        //free string pointer in event
        for(auto it = e.data.begin(); it != e.data.end(); it++)
        {
            if(it->second.m_type == TYPE_STRING_ID)
            {
                delete it->second.m_asStringId;
            }
        }
    }
}

void EventManager::printEventQueue()
{
    std::priority_queue<Event, std::vector<Event>, Compare> temp = eventQueue;
    while(!temp.empty())
    {
        Event e = temp.top();
        temp.pop();
        std::unordered_map<std::string, Variant> data = e.data;
        // iteratre through data
        for(auto it = data.begin(); it != data.end(); it++)
        {
            std::cout << it->first << ": ";
            if(it->second.m_type == TYPE_INTEGER)
            {
                std::cout << it->second.m_asInteger << std::endl;
            }
            else if(it->second.m_type == TYPE_FLOAT)
            {
                std::cout << it->second.m_asFloat << std::endl;
            }
            else if(it->second.m_type == TYPE_BOOL)
            {
                std::cout << it->second.m_asBool << std::endl;
            }
            else if(it->second.m_type == TYPE_STRING_ID)
            {
                std::cout << *(it->second.m_asStringId) << std::endl;
            }
        }

        std::cout << e.Type << " " << e.timeStamp << " " << e.priority << std::endl;
    }
}

EventManager::EventManager()
{
    
}

EventManager *EventManager::getInstance()
{
    if(instance == NULL)
    {
        instance = new EventManager();
    }
    return instance;
}
