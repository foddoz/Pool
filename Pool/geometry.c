#include "geometry.h"

Ball setPosition(Ball b, float x, float y, float z)
{
    b.position[0] = x;
    b.position[1] = y;
    b.position[2] = z;

    return b;
}

Ball setVelocity(Ball b, float x, float y, float z)
{
    b.velocity[0] = x;
    b.velocity[1] = y;
    b.velocity[2] = z;

    return b;
}
