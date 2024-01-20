#ifndef H_EVENTMANAGER
#define H_EVENTMANAGER
#include <unordered_map>
#include "Systems.h"
#include <vector>
#include <queue>
#include <string>

enum type
{
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING_ID,
    TYPE_COUNT, // number of unique types
    TYPE_ERROR // used for error checking
};

struct Variant
{
    Variant();
    Variant(int value);
    Variant(float value);
    Variant(bool value);
    Variant(std::string * value);
    type m_type;
    union
    {
        int m_asInteger;
        float m_asFloat;
        bool m_asBool;
        std::string * m_asStringId;
    };

    
    // Variant(float value) : m_type(TYPE_FLOAT) { m_asFloat = value; }
    // Variant(bool value) : m_type(TYPE_BOOL) { m_asBool = value; }
    // Variant(std::string * value) : m_type(TYPE_STRING_ID) { m_asStringId = value; }
};

class Event
{
    // enum type
public:
    std::string Type;
    float timeStamp;
    float priority;
    // std::unordered_map<std::string, argumentData> data;
    std::unordered_map<std::string, Variant> data;
    //   std::unordered_map<std::string, Variant> data;

    void addData(std::string key, int num);
    void addData(std::string key, float num);
    void addData(std::string key, bool num);
    void addData(std::string key, std::string * num);
    Variant getData(std::string key);

    Event(std::string Type, float timeStamp, float priority);
};

class Compare
{
public:
    bool operator()(Event e1, Event e2);
};

class EventManager
{
private:
    EventManager();
    EventManager *instance = NULL;
public:
    static std::vector<std::string> EventTypes;
    static std::unordered_map<std::string, std::vector<System *>> registeredHandlers;
    static std::priority_queue<Event, std::vector<Event>, Compare> eventQueue;
    static void registerEvent(std::string, System *);
    // one or more event types associated to an event 		type
    static void unregister(std::string, System *); // optional
    static void raiseEvent(Event e);               // immediate action or put into queue
    static void handleEvents();                    // maybe to handle all events
    static void printEventQueue();
    EventManager *getInstance();
};

#endif
