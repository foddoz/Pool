#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

typedef float Point2[2];
typedef float Point3[3];
typedef Point3 Vector3;     // Vector in 3D
typedef Point3 Color;

typedef float Rect2[4]; //Value at Index 0 = left x, 1 = bottom y, 2 = right x, 3 = top y

typedef struct Ball
{
    Vector3 velocity;
    Point3 position;
    float radius;
    float mass;
    int is_scored;
    Color color;
}Ball;

//Define table hole, each hole has four coordinates
//We only need two, the top left and the bottom right
typedef struct Hole
{
    Point2 Point[2];
}Hole;

//Define wall and table
typedef struct Cubic
{
    Point3 vertices[4];
}Cubic;

Ball setPosition(Ball b, float x, float y, float z); //Sets the position of a ball.

Ball setVelocity(Ball b, float x, float y, float z); //Sets the velocity of a ball.

typedef float Vertex[4];

typedef int Triangle[3];

typedef struct Object3D
{
    int nvert;
    int ntri;
    int nedges;

    Vertex *v_list;
    Triangle *t_list;

}Object3D;

#endif // GEOMETRY_H_INCLUDED
