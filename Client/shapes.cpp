#include "shapes.h"
#include "vector.h"
#include <iostream>

void setOrginCenter(sf::Shape &shape) {
    sf::Vector2f center;
    center.x = shape.getLocalBounds().left + shape.getLocalBounds().width/2;
    center.y = shape.getLocalBounds().top + shape.getLocalBounds().height/2;
    shape.setOrigin(center);
}

Platform::Platform(float width, float height) : sf::RectangleShape(sf::Vector2f(width, height)){
    setOrginCenter(*this);
}

MovingPlatform::MovingPlatform(float width, float height) : Platform::Platform(width, height){
    currentIndex = -1;
}

void MovingPlatform::addPathPoint( sf::Vector2f point ){
    targetPositions.push_back(point);
}

void MovingPlatform::setNextTarget(){
    currentIndex++;
    if( currentIndex >= targetPositions.size() ) {
        currentIndex = 0;
    }
    currentTarget = targetPositions[currentIndex];
}

sf::Vector2f MovingPlatform::getTarget(){
    return currentTarget;
}

void MovingPlatform::moveToTarget(float deltaTime, std::mutex *_mutex){
    this->velocity = currentTarget - this->getPosition();
    normalize(velocity);
    velocity = velocity * 150.0f;
    std::unique_lock<std::mutex> lock(*_mutex);
    this->setPosition(this->getPosition() + velocity * deltaTime);  
}

void MovingPlatform::move(float deltaTime, std::mutex *_mutex){
    if( length(this->getPosition() - currentTarget ) < 5.0f ){
        setNextTarget();
    }
    moveToTarget(deltaTime, _mutex);
}

StationaryPlatform::StationaryPlatform(float width, float height) : Platform::Platform(width, height){
}

Player::Player(float radius) : sf::CircleShape(radius){
    setOrginCenter(*this);
    velocity = sf::Vector2f(0.0f, 0.0f);
    maxSpeed = 500.0f;
    acceleration = sf::Vector2f(0.0f, 320.0f);
}

void Player::updatePos(float deltaTime){
    // print(velocity);
    // print(acceleration);
    velocity += acceleration * deltaTime;
    if (length(velocity) >= maxSpeed)
    {
        normalize(velocity);
        velocity = sf::Vector2f(velocity.x * maxSpeed, velocity.y * maxSpeed);
    }
    this->setPosition(this->getPosition() + velocity * deltaTime);
}

sf::Vector2f Player::getVelocity(){
    return this->velocity;
}

void Player::setVelocity(sf::Vector2f newVelocity){
    this->velocity = newVelocity;
}

void Player::setMaxSpeed(float newMaxSpeed){
    this->maxSpeed = newMaxSpeed;
}
