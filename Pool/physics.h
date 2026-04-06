#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <math.h>

#include "geometry.h"

Ball ballFall(Ball b, const Vector3 g, const float time); //Calculates the gravitational pull of a ball.

Ball ballBounce(Ball b); //Bounces the ball if it's grounded.

Ball ballBounceEdge(Ball b, Cubic table);

Ball ballBounceFloor(Ball b); // Bounces the ball if fall down from table

Ball ballFriction(Ball b, const float friction, const float time); //Calculates the friction applied to a moving ball.

int isBallMoving(const Ball b); //Calculates whether the ball is moving.

int isBallMovingInArray(const Ball* b, const int size); //Calculates whether the ball is moving.

void collide(Ball *a, Ball *b); //Calculates the movement of two balls after they've collided.

float norm(Vector3 v); //Calculates the norm of a vector (also known as its magnitude).

float dotProduct(Vector3 v1, Vector3 v2); //Calculates the dot product of two vectors.

void crossProduct(Vector3 v1, Vector3 v2, Vector3 v); //Calculates the cross product of two vectors.


#endif // PHYSICS_H_INCLUDED
