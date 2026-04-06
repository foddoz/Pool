#ifndef DRAWING_OBJECTS_H_INCLUDED
#define DRAWING_OBJECTS_H_INCLUDED

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit)
#include <gl/freeglut.h> // For windows
#ifdef _WIN64
//define something for Windows (64-bit only)
#endif
#elif __APPLE__
#include <GLUT/glut.h> // for Mac OS
#endif

#include "geometry.h"

#include <math.h>

// To DO: (done :P)
// Separate visuals for cuestick from power.
//  Should have similar input as power bar going from 0.0 to 1.0.
//  Defines value for how long the stick can go back for.
// Make animation of stick tilting slightly upwards after hitting ball (and fade away?).
// Make datastructure to sture max power, min power, actual power.
// Make power based on the power bar.

typedef struct CueStick{
    float length; //Length of the cue.
    float angle; //Angle from the ground.
    float maxDistance; //How far back the stick should be at maximum power.
    float rotation; //Rotation around the origin.
    float distance; //How wound back the cue is.

}CueStick;

typedef struct PowerBar{
    float width;
    float height;
    float outlineWidth;
    float power; //How full the bar is. Meant to be used within the range [0.0, 1.0].
    Point2 position; //Position defines the middle of the bar.
} PowerBar;

void drawHole(Hole* holes, float width, float length, float holeSize);
// These functions draw 3D objects.
void drawBalls(const Ball* balls, int count); //Draw each ball in the array.

void drawPlane(float width, float height); //Draw of 2D plane with given dimensions.

void drawCueStick(CueStick stick); //Draw the cue stick object.

void initBalls(Ball * b, int size);

int calculateTotalFromBase(int numBallsAtBase);

int calculateTotalFromLengthAndWidth(int width, int length);

void getColorFromHue(Color C, float hue);

void setTriangularFormation(Ball * b, int numBallsAtBase);

void setRectangularFormation(Ball * b, int width, int length);

// These functions draw 2D elements.
void drawRect2(const Rect2 rect); //Draw a rectangle defined by a Rect2.

void drawRect2Outline(const Rect2 rect, float size); //Draw only the outline of a rectangle, with thickness specified by size. It draws an outer outline.

void drawPowerBar(PowerBar bar, int disabled); //Draw the power bar UI element.

void drawAngleDisplay(float angle, float x, float y, float width, int disabled);

void drawTableLeg(float width, float length, float topRadius, float baseRadius);//Draw legs for the pool table

void drawTableEdges(float width, float length, float edgeWide, float edgeHeight);

void drawCircle(int triangleAmount,float radius);

#endif // DRAWING_OBJECTS_H_INCLUDED
