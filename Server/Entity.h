#ifndef H_ENTITY
#define H_ENTITY
#include "Component.h"
#include <unordered_map>
#include <string>

class GameObject
{
public:
    std::string GUID;
    GameObject(std::string id);
};

class GameObjectManager
{
public:
    static std::string generateGUID();
    static GameObject *createGameObject(std::string id);
    static std::unordered_map<std::string, std::vector<Component *> *>components;
    static Component *createComponent(ComponentManager::ComponentType type);
    static void addComponent(std::string id, Component *component);
    static void removeComponent(std::string id, ComponentManager::ComponentType type);
    Component * getComponent(std::string id, ComponentManager::ComponentType type);
    std::unordered_map<std::string, Component *> getAllComponentsofType(ComponentManager::ComponentType type);
};

#endif