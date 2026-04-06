#include "camera.h"

Camera setCamPosition(Camera c, float x, float y, float z)
{
    c.position[0] = x;
    c.position[1] = y;
    c.position[2] = z;

    return c;
}

Camera setLookAt(Camera c, float x, float y, float z)
{
    c.lookAt[0] = x;
    c.lookAt[1] = y;
    c.lookAt[2] = z;

    return c;
}

void pToQ(Camera cam, Vector3 v)
{
    v[0] = cam.lookAt[0] - cam.position[0];
    v[1] = cam.lookAt[1] - cam.position[1];
    v[2] = cam.lookAt[2] - cam.position[2];

    return;
}

void moveSideways(Camera *pcam, float speed)
{
    Vector3 X_dir;

    Vector3 p2q;
    pToQ(*pcam, p2q);

    crossProduct(p2q, pcam->up, X_dir);

    float d = norm(X_dir);

    pcam->position[0] += speed * X_dir[0]/d;
    pcam->position[1] += speed * X_dir[1]/d;
    pcam->position[2] += speed * X_dir[2]/d;

    return;
}

void moveForward(Camera* pcam, float speed)
{
    Vector3 p2q;
    pToQ(*pcam, p2q);

    float d = norm(p2q);

    pcam->position[0] += speed * p2q[0]/d;
    pcam->position[1] += speed * p2q[1]/d;
    pcam->position[2] += speed * p2q[2]/d;

    return;
}
