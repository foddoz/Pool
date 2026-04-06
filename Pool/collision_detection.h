#ifndef COLLISION_DETECTION_H_INCLUDED
#define COLLISION_DETECTION_H_INCLUDED

#include <math.h>
#include "geometry.h"
#include "physics.h"

float calculateDistance(Point3 p1, Point3 p2); //Calculates the distance between two points.

int collisionCheckBalls(Ball B1, Ball B2); //Calculates whether two balls have collided or not.

void penetrationResolution(Ball *B1, Ball *B2);

#endif // COLLISION_DETECTION_H_INCLUDED
