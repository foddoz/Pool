#include "collision_detection.h"

#include <math.h>


float calculateDistance(Point3 p1, Point3 p2)
{
    float result = (p2[0] - p1[0]) * (p2[0] - p1[0]) + (p2[1] - p1[1]) * (p2[1] - p1[1]) + (p2[2] - p1[2]) * (p2[2] - p1[2]);
    return sqrt(result);
}

int collisionCheckBalls(Ball B1, Ball B2)
{
    int bool;

    float distance = calculateDistance(B1.position, B2.position); //Calculates the distance between the centres of the two spheres.
    if(distance <= B1.radius + B2.radius) //If the distance between the centres of the two spheres is less than the radii of the two spheres...
        bool = 1; //Balls have collided.
    else
        bool = 0; //Balls have not collided.

    return bool;
}

void penetrationResolution(Ball *B1, Ball *B2)
{
    Vector3 distance; //The distance vector between the two balls.
    for(int i = 0; i < 3; i++)
    {
        distance[i] = B1->position[i] - B2->position[i]; //Calculate the difference between the positions of the two balls.
    }

    float D_norm = norm(distance); //Calculate the normal of the distance.
    for(int i = 0; i < 3; i++) //For each dimension.
    {
        distance[i] = distance[i] / D_norm; //Convert distance to its unit vector.
    }

    float pene_depth = B1->radius + B2->radius - D_norm; //Calculate the penetration depth between the two balls.

    Vector3 pene_res; //Resolve the penetration.
    for(int i = 0; i < 3; i++)
    {
        pene_res[i] = distance[i] * (pene_depth / 2); //The relevant movement the two balls must make to not penetrate.
    }

    for(int i = 0; i < 3; i++)
    {
        B1->position[i] = B1->position[i] + pene_res[i]; //Move the first ball away.
        B2->position[i] = B2->position[i] + (-1 * pene_res[i]); //Move the second ball in the other direction.
    }

}
