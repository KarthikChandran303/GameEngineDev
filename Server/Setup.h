#ifndef H_SETUP
#define H_SETUP
#include "shapes.h"
#include <SFML/Graphics.hpp>

// define a 120x50 rectangle
MovingPlatform movingPlat(190.0f, 15.0f);
StationaryPlatform staticPlat(100.0f, 100.0f);
StationaryPlatform ground(800.0f, 50.0f);

Player player(40.0f);

#endif