#include "physics.h"

Ball ballFall(Ball b, const Vector3 g, const float time)
{
    for(int i = 0; i < 3; i++) //Go through each dimension of the ball's movement...
    {
        b.velocity[i] += (g[i] * time); //Calculates the ball's velocity.
        b.position[i] += (b.velocity[i] * time); //Calculates the ball's displacement.
    }
    return b; //Return the ball.
}

Ball ballBounce(Ball b)
{

    if(b.position[1] < b.radius) //If the ball has hit the floor...
    {
        b.position[1] = b.radius; //Set the position of the ball on the floor.
        b.velocity[1] *= -0.5; //Bounce the ball upwards of the half it's initial velocity.
    }

    return b; //Return the ball.
}

Ball ballBounceEdge(Ball b, Cubic table)
{
        if(b.position[0] < table.vertices[3][0] + b.radius) //If the ball has hit the floor...
    {
        b.position[0] = table.vertices[3][0] + b.radius; //Set the position of the ball on the floor.
        b.velocity[0] *= -0.5; //Bounce the ball upwards of the half it's initial velocity.
    }

    if(b.position[0] > table.vertices[0][0]  - b.radius) //If the ball has hit the floor...
    {
        b.position[0] = table.vertices[0][0]  - b.radius; //Set the position of the ball on the floor.
        b.velocity[0] *= -0.5; //Bounce the ball upwards of the half it's initial velocity.
    }

    if(b.position[2] < table.vertices[3][2]  + b.radius) //If the ball has hit the floor...
    {
        b.position[2] = table.vertices[3][2]  + b.radius; //Set the position of the ball on the floor.
        b.velocity[2] *= -0.5; //Bounce the ball upwards of the half it's initial velocity.
    }

    if(b.position[2] > table.vertices[0][2]  - b.radius) //If the ball has hit the floor...
    {
        b.position[2] = table.vertices[0][2]  - b.radius; //Set the position of the ball on the floor.
        b.velocity[2] *= -0.5; //Bounce the ball upwards of the half it's initial velocity.
    }
    return b; //Return the ball.
}

Ball ballBounceFloor(Ball b)
{

    float floor = -2;
    //if position of y axis of the ball is zero
    //Ball will bounce once y axis smaller than -2
    if(b.position[1] - b.radius < floor)
    {
        b.position[1] = floor + b.radius;
        b.velocity[1] *= -0.5;
    }
    return b;
}

Ball ballFriction(Ball b, const float friction, const float time)
{
    Vector3 Force; //The force at which friction is applied to.
    for(int i = 0; i < 3; i++) //For each dimension of the force...
    {
        Force[i] = b.mass * b.velocity[i] * time; //Calculate the dimensions of the force. F = ma, a = vt.
    }
/*
    b.velocity[0] -= (friction * Force[0]);
    b.velocity[2] -= (friction * Force[2]);
*/

    if(b.position[1] <= b.radius)
    {
        Force[0] = -(friction * Force[0]);
        Force[2] = -(friction * Force[2]);
    }
    else
    {
        Force[0] = 0.f;
        Force[1] = 0.f;
        Force[2] = 0.f;
    }

    Vector3 acceleration = {0.f, 0.f, 0.f};
    acceleration[0] = Force[0] / b.mass;
    acceleration[2] = Force[2] / b.mass;

    b.position[0] += b.velocity[0] * time + 0.5 * acceleration[0] * time * time; //Calculates the ball's displacement.
    b.position[2] += b.velocity[2] * time + 0.5 * acceleration[2] * time * time; //Calculates the ball's displacement.

    b.velocity[0] += (acceleration[0] * time);
    b.velocity[2] += (acceleration[2] * time);

    if(b.velocity[0] < 0.01 && b.velocity[0] > -0.01) //If the ball x velocity is very small...
         b.velocity[0] = 0.f; //Treat it as having stopped.
    if(b.velocity[2] < 0.01 && b.velocity[2] > -0.01) //If the ball z velocity is very small...
         b.velocity[2] = 0.f; //Treat it as having stopped.

    return b; //Return the ball.
}

int isBallMoving(const Ball b)
{
    int bool;
    //If the ball is significantly moving horizontally...
    if(b.velocity[0] > 0.01f || b.velocity[0] < -0.01f
       || b.velocity[2] > 0.01f || b.velocity[2] < -0.01f
       )
        bool = 1; //The ball is moving.
    else
        bool = 0; //The ball is not moving.

    return bool;
}

int isBallMovingInArray(const Ball* B, const int size)
{
    for(int i=0; i<size; i++)
    {
        if(isBallMoving(B[i])) //If there exists a ball that is moving...
            return 1; //Return true.
    }
    return 0; //Return false.
}

void collide(Ball *a, Ball *b)
{
    Vector3 relativeVelocity; //The relative velocity between the two balls.
    for(int i = 0; i < 3; i++) //For each dimension...
    {
        relativeVelocity[i] = a->velocity[i] - b->velocity[i]; //Calculate the difference between the velocities of the two balls.
    }

    Vector3 D; //Collision normal between the two balls.
    for(int i = 0; i < 3; i++) //For each dimension...
    {
        D[i] = a->position[i] - b->position[i]; //Calculate the difference between the positions of the two balls.
    }

    float D_norm = norm(D); //Calculate the distance between the two balls.
    for(int i = 0; i < 3; i++) //For each dimension...
    {
        D[i] = D[i] / D_norm; //Divide by the norm of the vector to obtain the unit vector.
    }

    float impulse = dotProduct(relativeVelocity, D); //Obtain the impulse by calculate the dot product between the relative velocity and the collision normal.
    impulse = (-1 * impulse); //Reverse the impulse.

    Vector3 impulseVector; //
    for(int i = 0; i < 3; i++)
    {
        impulseVector[i] = D[i] * impulse;
    }

    for(int i = 0; i < 3; i++)
    {
        a->velocity[i] += (impulseVector[i]);
        b->velocity[i] += (-1 * impulseVector[i]);
    }
}


float norm(Vector3 v)
{
    float d = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    return sqrt(d);
}

float dotProduct(Vector3 v1, Vector3 v2)
{
    return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

void crossProduct(Vector3 v1, Vector3 v2, Vector3 v)
{
    v[0] = v1[1]*v2[2] - v1[2]*v2[1];
    v[1] = v1[2]*v2[0] - v1[0]*v2[2];
    v[2] = v1[0]*v2[1] - v1[1]*v2[0];

    return;
}
