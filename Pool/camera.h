#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "physics.h"

// Camera
typedef struct Camera
{
     Point3 position;
     Point3 lookAt;
     Vector3 up; // the up vector
} Camera;

Camera setCamPosition(Camera c, float x, float y, float z); //Set the position of the camera.
Camera setLookAt(Camera c, float x, float y, float z); //Sets where the camera looks at.

void pToQ(Camera cam, Vector3 v);

void moveSideways(Camera* pcam, float speed);

void moveForward(Camera* pcam, float speed);

#endif // CAMERA_H_INCLUDED
