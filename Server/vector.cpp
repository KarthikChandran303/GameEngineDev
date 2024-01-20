#include "vector.h"
#include <cmath>
#include <iostream>

float length (const sf::Vector2f &vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

void normalize(sf::Vector2f& source)
{
    float length = std::hypot(source.x, source.y);
    if (length != 0) source /= length;
}


void print(const sf::Vector2f &vec) {
    std::cout<<"[X: " << vec.x << " Y: " << vec.y << "]" << std::endl;
}

sf::Vector2f orientationToVector(double orientation) {
    float remapOrient = fmod(orientation , 360);
    remapOrient = (remapOrient / 360 ) * (2 * M_PI);
    sf::Vector2f temp = sf::Vector2f(cos( remapOrient ), sin( remapOrient ));
    normalize(temp);
    return temp;
}

float dot(const sf::Vector2f &v1, const sf::Vector2f &v2) {
    return (v1.x * v2.x + v1.y * v2.y);
}