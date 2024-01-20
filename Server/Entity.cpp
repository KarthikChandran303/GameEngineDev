#include "Entity.h"
#include "Component.h"
#include <unordered_map>
#include <string>
#include "sole.hpp"
#include <iostream>

GameObject::GameObject(std::string id)
{
    GUID = id;
}

std::unordered_map<std::string, std::vector<Component *> *> GameObjectManager::components;

std::string GameObjectManager::generateGUID()
{
    sole::uuid u0 = sole::uuid0();
    std::string id = u0.str();
    return id;
}

GameObject *GameObjectManager::createGameObject(std::string id)
{
    GameObject *gameObject = new GameObject(id);
    return gameObject;
}

// Component *GameObjectManager::createComponent(ComponentManager::ComponentType type)
// {
//     Component *component = ComponentManager::createComponent(type);
//     return component;
// }

void GameObjectManager::addComponent(std::string id, Component *component)
{
    std::unordered_map<std::string, std::vector<Component *> *>::iterator it = components.find(id);
    if (it != components.end())
    {
        std::vector<Component *> *componentList = it->second;
        for(int i = 0; i < componentList->size(); i++)
        {
            if(componentList->at(i)->type == component->type)
            {
                std::cout << "Component already exists" << std::endl;
                return;
            }
        }
        componentList->push_back(component);
    }
    else
    {
        std::vector<Component *> *componentList = new std::vector<Component *>();
        componentList->push_back(component);
        components.insert(std::make_pair(id, componentList));
    }
}

void GameObjectManager::removeComponent(std::string id, ComponentManager::ComponentType type)
{
    std::unordered_map<std::string, std::vector<Component *> *>::iterator it = components.find(id);
    if (it != components.end())
    {
        std::vector<Component *> *componentList = it->second;
        for (int i = 0; i < componentList->size(); i++)
        {
            if (componentList->at(i)->type == type)
            {
                componentList->erase(componentList->begin() + i);
                break;
            }
        }
    }
}

Component * GameObjectManager::getComponent(std::string id, ComponentManager::ComponentType type)
{
    std::unordered_map<std::string, std::vector<Component *> *>::iterator it = components.find(id);
    if (it != components.end())
    {
        std::vector<Component *> *componentList = it->second;
        for (int i = 0; i < componentList->size(); i++)
        {
            if (componentList->at(i)->type == type)
            {
                return componentList->at(i);
            }
        }
    }
    return NULL;
}

std::unordered_map<std::string, Component *> GameObjectManager::getAllComponentsofType(ComponentManager::ComponentType type)
{
    std::unordered_map<std::string, Component *> componentMap;
    std::unordered_map<std::string, std::vector<Component *> *>::iterator it = components.begin();
    while (it != components.end())
    {
        std::vector<Component *> *componentList = it->second;
        for (int i = 0; i < componentList->size(); i++)
        {
            if (componentList->at(i)->type == type)
            {
                componentMap.insert(std::make_pair(it->first, componentList->at(i)));
            }
        }
        ++it;
    }
    return componentMap;
}