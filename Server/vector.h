#ifndef H_VEC
#define H_VEC

#include <SFML/Graphics.hpp>
#include <cmath>

float length (const sf::Vector2f &vec);

void normalize (sf::Vector2f &vec);

void print(const sf::Vector2f &vec);

sf::Vector2f orientationToVector(double orientation);

float dot(const sf::Vector2f &v1, const sf::Vector2f &v2);
#endif