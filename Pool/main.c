/*  modified smr  - animate with stop s and Start S

moon
*/

/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 *  planet.c
 *  This program shows how to composite modeling transformations
 *  to draw translated and rotated models.
 *  Interaction:  pressing the d D and y Y keys (day and year)
 *  alters the rotation of the planet around the sun.
 *  ESC key exits
 *
 *  modified smr  - animate with stop s and Start S
 */

/*
* Patches:
* - The stick cannot hit the cue ball when it's moving anymore.
* - Created an array of 3 balls.
*/

// If using MAC, compile as follows:
//  gcc planet_moon.c -framework GLUT -framework OpenGL -framework Cocoa -o planet_moon

#define TIMERMSECS 1
#define NUMHOLES 6
#define MAX_USER_INPUT 256

#include <stdio.h>
#include <stdlib.h>

#include "physics.h"
#include "drawing_objects.h"
#include "collision_detection.h"
#include "camera.h"
//#include "console.h"

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)
#include <gl/freeglut.h>		// For windows
#ifdef _WIN64
//define something for Windows (64-bit only)
#endif
#elif __APPLE__
#include <GLUT/glut.h>			// for Mac OS
#endif

struct CueForce
{
    float min; //Minimum amount of force the cue ball can shoot.
    float max; //How much force can be added on top of minForce.
    //Real maximum force output is minForce + maxForce.
} cueForce = {0.1, 4.9};

GLfloat light1_position[] = {0.f, 5.f, -1.f, 1.0f};
GLfloat light2_position[] = {0.f, 5.f, 1.f, 1.0f};

int hit = 0; //Boolean for whether the stick has hit the cue ball.
int paused = 0; //Boolean for whether the game is paused.
int win = 0; //Boolean for whether the game is won.
int is_control_inverted = 0;
int overlay_on = 1;

float power = 0.f; //Power representing how hard the cue will hit between [0.0, 1.0]. Use calculatePower() to update value.

float friction = 200.f; //Friction of the balls.

//Table Length and wide
float tableLength = 2.54f;
float tableWidth = 1.27f;

int windowWidth = 500;
int windowHeight = 500;
Point2 mousepos;

enum CameraType
{
    CAM_TOPDOWN,
    CAM_FOLLOWCUE,
    CAM_SIDEANGLE
};
int activeCameraType = CAM_TOPDOWN;
float cameraRotation;
Camera camera;

static Vector3 gravity = {0.f, -9.8f, 0.f};

static float previousTime;
static float currentTime;
static float deltaTime;

PowerBar bar;

CueStick stick;

Ball cue;

Color *colors = 0;
Ball *balls = 0;

int NUMBALLS = 0;

//Color colors[NUMBALLS];

int base = 7;
int width = 5;
int length = 4;

//Holes Array
Hole holes[NUMHOLES];

Cubic table;

// Variables used in keyboard_console
char inputBuffer[MAX_USER_INPUT] = "\0"; //Buffer to store user input from the console.
int is_command_entered = 0; //Keeps track whether the first token in a command is entered. Used for text coloring.

//Color escape codes.
//Removed right now because it is not supported on all terminals.
const char* OUT_YELLOW = ""; // \033[0;33m
const char* OUT_GREEN = ""; // \033[0;32m
const char* OUT_RESET = ""; // \033[0m


void setDefaults(); //Resets properties that can be changed through the console.

void init(void);

void initLight();

void setLevel(char levelType);

void initCue(void);

void initHolePostion(Hole* holes, float wide, float length, float holeSize);

void initTable(float height);

void initCamera(enum CameraType type);

void positionPowerBar(void); //Sets the power bar position to the right side of the window.

void calculatePower(); //Calculates a float between [0.0, 1.0] based on mouse y value. Stores it in power.

float calculateRotation();

static void animate(/*int value*/);

void displayObjects();

void displayUI();

void display(void);

void reshape (int w, int h);

void keyboard_world (unsigned char key, int x, int y);

void keyboard_console (unsigned char key, int x, int y);

void mouse(int button, int state, int x, int y);

void passiveMouse(int x, int y);


void setDefaults()
{
    base = 7;
    width = 5;
    length = 4;
    friction = 500.f;
    cueForce.min = 0.1;
    cueForce.max = 4.9;
    is_control_inverted = 0;
    overlay_on = 1;
}

void init(void)
{

    glClearColor (0.0, 0.0, 0.0, 0.0);

    initLight();

    setLevel('t');

    initTable(0.2f);
    //setTriangleFormation(&balls, -1.0f); //Position balls into triangular formation (drawing_objects.h).
    //setRectangularFormation(&balls, -1.0f); //Position balls into rectangular formation (drawing_objects.h).

    //Six hole on the table
    //Setting position for the holes by using length and wide of the table, last parameter is for setting size of the hole
    initHolePostion(&holes[0],tableWidth,tableLength,0.2);
    initHolePostion(&holes[1],tableWidth,0.1,0.2);
    initHolePostion(&holes[2],tableWidth,-tableLength + 0.2,0.2);
    initHolePostion(&holes[3],-tableWidth + 0.2,tableLength,0.2);
    initHolePostion(&holes[4],-tableWidth + 0.2,0.1,0.2);
    initHolePostion(&holes[5],-tableWidth + 0.2,-tableLength + 0.2,0.2);

    stick.length = 2.f; //Length of the stick.
    stick.angle = 20.0f; //Angle between the stick and the ground.
    stick.maxDistance = 1.f; //Maximum distance between the stick and the cue ball.

    stick.rotation = 180.f; //Rotation of the stick.
    stick.distance = 0.f; //How far back the stick is.


    //Setup the power bar properties.
    bar.width = 20;
    bar.height = 350;
    bar.outlineWidth = 2;
    positionPowerBar();

    glEnable(GL_DEPTH_TEST);
    //glShadeModel (GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Sets the default blending mode (used for transparent objects).
}

void initLight()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT0, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT0, GL_POSITION, light2_position);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //glColor

    glEnable(GL_DEPTH_TEST);
}

void setLevel(char levelType)
{
    win = 0; //Game is not won yet.

    if(levelType == 't' || levelType == 'T')
    {
        NUMBALLS = calculateTotalFromBase(base);

    }
    else if(levelType == 'r' || levelType == 'R')
    {
        NUMBALLS = calculateTotalFromLengthAndWidth(width, length);
    }


//    printf("Number of balls: %d\n", NUMBALLS);
    if(balls != 0)
        free(balls);
    if(colors != 0)
        free(colors);
    balls = (Ball*)malloc(NUMBALLS * sizeof(Ball));
    initBalls(balls, NUMBALLS);
    colors = (Color*)malloc(NUMBALLS * sizeof(Color));

    if(levelType == 't' || levelType == 'T')
        setTriangularFormation(balls, base); //Position balls into triangular formation (drawing_objects.h).
    else if(levelType == 'r' || levelType == 'R')
        setRectangularFormation(balls, width, length); //Position balls into rectangular formation (drawing_objects.h).

//    // Assign a color for each ball, looping through RGB/CMY and skipping B/W.
//    int n = 0; //Number of iterations.
//    int c = 0; //Number of valid colors assigned.
//    while(c<NUMBALLS)
//    {
//        if( !( (n%8 == 0) || (n%8 == 7) ) ) //If it's not the 1st or 8th term (is not black or white)...
//        {
//            colors[c][0] = n%2 == 0;
//            colors[c][1] = n%4 < 2;
//            colors[c][2] = n%8 < 4;
//            c++;
//        }
//        n++;
//    }
    initCamera(CAM_TOPDOWN);
    cue.is_scored = 0;
    initCue();
}

void initCue(void)
{
    cue = setVelocity(cue, 0.f, 0.f, 0.f); //Initialise the velocity of the cue ball (geometry.h).
    cue = setPosition(cue, 0.f, 1.f, 1.693f); //Initialise the position of the cue ball (geometry.h).
    cue.radius = 0.057f;
    cue.mass = 50.f;
    cue.is_scored = 0;
    return;
}

void initHolePostion(Hole* holes, float wide, float length, float holeSize)
{
    //Coordinate of hole
    //Top left
    holes->Point[0][0] = wide;
    holes->Point[0][1] = length;
    //Bottom right
    holes->Point[1][0] = wide - holeSize;
    holes->Point[1][1] = length - holeSize;
}

void initTable(float height)
{
    //Set up size for the table
    table.vertices[0][0] = tableWidth;
    table.vertices[0][1] = height;
    table.vertices[0][2] = tableLength;

    table.vertices[1][0] = tableWidth;
    table.vertices[1][1] = height;
    table.vertices[1][2] = -tableLength;

    table.vertices[2][0] = -tableWidth;
    table.vertices[2][1] = height;
    table.vertices[2][2] = tableLength;

    table.vertices[3][0] = -tableWidth;
    table.vertices[3][1] = height;
    table.vertices[3][2] = -tableLength;
}

void initCamera(enum CameraType type)
{
    switch(type)
    {
    case CAM_TOPDOWN:
        camera.up[0] = 0.f; //Set the camera's up vector.
        camera.up[1] = 0.f;
        camera.up[2] = -1.f;
        camera = setCamPosition(camera, 0.f, 4.f, 0.f); //Set the camera position.
        camera = setLookAt(camera, 0.f, 0.f, 0.f); //Set where the camera looks at.
        cameraRotation = 180.f;
        activeCameraType = type;
        break;
    case CAM_FOLLOWCUE:
        camera.up[0] = sin(stick.rotation * 3.14/180); //Set the camera's up vector.
        camera.up[1] = 0.f;
        camera.up[2] = cos(stick.rotation * 3.14/180);
        Point3 offset = {0.0, 4.0, 0.0};
        camera = setCamPosition(camera, cue.position[0] + offset[0], cue.position[1] + offset[1], cue.position[2] + offset[2]); //Set the camera position.
        camera = setLookAt(camera, cue.position[0], cue.position[1], cue.position[2]); //Set where the camera looks at.
        cameraRotation = 180.f;
        activeCameraType = type;
        break;
    case CAM_SIDEANGLE:
        camera.up[0] = 0.f; //Set the camera's up vector.
        camera.up[1] = 1.f;
        camera.up[2] = 0.f;
        camera = setCamPosition(camera, 1.f, 2.f, 3.f); //Set the camera position.
        camera = setLookAt(camera, 0.f, 0.f, 0.f); //Set where the camera looks at.
        cameraRotation = 180.f;
        activeCameraType = type;
        return;
    }
}

void positionPowerBar(void)
{
    bar.position[0] = windowWidth - bar.width - 20;
    bar.position[1] = windowHeight/2;
    return;
}

void calculatePower()
{
    float activeInputArea = 0.5; //Defines the percentage of the window used to determine power.

    float mouse_y = is_control_inverted ? windowHeight - mousepos[1] : mousepos[1]; //Invert mouse y if setting is enabled.
    float offset = (1 - activeInputArea) * windowHeight / 2; //Height of the top and bottom areas where power will be clamped.
    power = 1 - (mouse_y - offset) / ( windowHeight * activeInputArea);
    power = power < 0 ? 0 : power; //Clamp value below 0.
    power = power > 1 ? 1 : power; //Clamp value above 1.
}


float calculateRotation()
{
    float activeInputArea = 0.8; //Defines the percentage of the window used to determine rotation.

    float mouse_x = is_control_inverted ? windowHeight - mousepos[0] : mousepos[0]; //Invert mouse y if setting is enabled.
    float offset = (1 - activeInputArea) * windowWidth / 2; //Width of the left/right areas where power will be clamped.
    float amount = 1 - (mouse_x - offset) / ( windowWidth*activeInputArea );
    amount = amount < 0 ? 0 : amount; //Clamp value below 0.
    amount = amount > 1 ? 1 : amount; //Clamp value above 1.
    return amount * 360.f;
}

static void animate(/*int value*/)
{

    //Set up the next timer (do this first)
    glutTimerFunc(TIMERMSECS, animate, 0);

    currentTime = glutGet(GLUT_ELAPSED_TIME)/1000.f; //Gets the time.
    deltaTime = currentTime - previousTime; //Gets the change in time.
    previousTime = currentTime;

    if(paused) return; //Don't animate if paused.

    for(int i = 0; i < NUMHOLES; i++)
    {
        if(cue.position[0] <= holes[i].Point[0][0] && cue.position[0]>= holes[i].Point[1][0] &&
                cue.position[2] <= holes[i].Point[0][1] && cue.position[2] >= holes[i].Point[1][1])
        {
            cue.is_scored = 1;
        }
    }

    if(cue.is_scored == 0)
    {
        if(cue.position[1] > cue.radius) //If cue is not on the floor...
            cue = ballFall(cue, gravity, deltaTime); //Calculates the gravitational pull of the cue ball (physics.h).
        cue = ballBounce(cue); //Bounces the cue ball if it's on the table (physics.h).
        cue = ballBounceEdge(cue,table);
        cue = ballFriction(cue, friction, deltaTime); //Calculates the friction on the cue ball if it's moving horinzontally (physics.h).
    }
    else
    {
        cue.velocity[0] = 0;
        cue.velocity[2] = 0;
        if(cue.position[1] - cue.radius > -2) //If cue is not on the floor (bottom of hole)...
        {
            cue = ballFall(cue, gravity, deltaTime); //Fall into the hole.
        }
        else
        {
            initCue();
        }
    }

    // int ballsFallCheck[NUMBALLS];
    //int ballFallCheck[NUMBALLS];

    int score = 0; //How many balls are in the hole.

    for(int i = 0; i < NUMBALLS; i++)
    {
        //ballsFallCheck[NUMBALLS] = 0;
        for(int j = 0; j < NUMHOLES; j++)
        {
            if(balls[i].position[0] <= holes[j].Point[0][0] && balls[i].position[0]>= holes[j].Point[1][0] &&
                    balls[i].position[2] <= holes[j].Point[0][1] && balls[i].position[2] >= holes[j].Point[1][1])
            {
                balls[i].is_scored = 1;
            }
        }

        if(balls[i].is_scored) //If ball is in the hole...
        {
            score++;
        }

        if(balls[i].is_scored == 0)
        {
            if(balls[i].position[1] > balls[i].radius ) //If ball is not on ground...
                balls[i] = ballFall(balls[i], gravity, deltaTime); //Calculates the gravitational pull of the ball (physics.h).
                balls[i] = ballBounceEdge(balls[i],table);
                balls[i] = ballBounce(balls[i]);  //Bounces the ball if it's on the ground (physics.h).

                balls[i] = ballFriction(balls[i], friction, deltaTime); //Calculates the friction on the ball if it's moving horinzontally (physics.h).
            // Check collision with cue ball.
            if(collisionCheckBalls(balls[i], cue) == 1) //If the ball have collided with cue (physics.h).
            {
                penetrationResolution(&balls[i], &cue);
                collide(&balls[i], &cue); //Calculates the balls bouncing off each other (physics.h).
            }

            // Check collision with every other ball.
            if(i == NUMBALLS)  //Skip checking for collisions if this is the last ball (it's already been checked against every other ball).
            {
                continue;
            }
            for(int k = i + 1; k < NUMBALLS; k++) //For every other ball... (we want to check for collisions)
            {
                if(collisionCheckBalls(balls[i], balls[k]) == 1) //If the balls have collided (physics.h).
                {
                    penetrationResolution(&balls[i], &balls[k]);
                    collide(&balls[i], &balls[k]); //Calculates the balls bouncing off each other (physics.h).
                }
            }
        }
        else
        {
            balls[i].velocity[0] = 0;
            balls[i].velocity[2] = 0;
            if(balls[i].position[1] - balls[i].radius > -1.99) //If ball is not on ground...
                balls[i] = ballFall(balls[i], gravity, deltaTime); //Apply gravity.
            balls[i] = ballBounceFloor(balls[i]);//Bounces the cue ball if it's on the ground
        }
    }

    if(!win && score >= NUMBALLS) //If win message isn't printed out yet and all the balls are scored...
    {
        win = 1;
        printf("%sYou win!%s\n", OUT_YELLOW, OUT_RESET);
    }

    if(hit == 0) //If cue is not moving...
    {
        if(!isBallMoving(cue)) //If cue is not moving...
        {
            calculatePower(); //Stores value in power.
            stick.distance = -(power*stick.maxDistance + cue.radius); //Update distance between the stick and the cue ball.
            stick.rotation = calculateRotation();
        }
    }
    else //If the cue ball has been hit...
    {
        stick.distance += deltaTime * 2; //Stick will travel towards the cue ball.
        if(stick.distance >= 0) //When the stick hits the ball...
        {
            float force = cueForce.min + cueForce.max*power;
            cue.velocity[0] += force * sin(stick.rotation * 3.14/180); //Calculate the velocity of the cue ball towards the x axis.
            cue.velocity[2] += force * cos(stick.rotation * 3.14/180); //Calculate the velocity of the cue ball towards the z axis.
            hit = 0; //Cue ball has not been hit anymore.
        }
    }

    if(activeCameraType == CAM_FOLLOWCUE) //If camera is following the cue...
        initCamera(CAM_FOLLOWCUE); //Update camera position.


    glutPostRedisplay();
}

void displayObjects()
{

    drawBalls(balls, NUMBALLS);  // (drawing_objects.h)

    //Display the actual pool table.
    glColor3f(0.00, 0.40, 0.13); //Somewhat dark green.
    //drawPlane(tableWide, tableLength); //Draw the dimensions of the plane.
    drawPlane(tableWidth, tableLength); //Draw the dimensions of the plane.
    drawTableLeg(tableWidth, tableLength, 0.04, 0.05);//Draw Table leg
    drawTableEdges(tableWidth, tableLength, 0.2, 0.2);
    drawHole(holes, tableWidth, tableLength, 0.2);//Draw hole on table

    glColor3f (1.0, 1.0, 1.0); //White
    glTranslatef(cue.position[0], cue.position[1], cue.position[2]); //Position cue ball.
    glutSolidSphere(cue.radius, 20, 16);   //Draw cue ball.


    if(!isBallMoving(cue)) //If cue is not moving...
        drawCueStick(stick);
}

void displayUI()
{
    glDisable(GL_LIGHTING);
    if(overlay_on)
    {
        bar.power = power;
        float angle;
        if(is_control_inverted)
        {
            angle = stick.rotation;
        }
        else
        {
            angle = 360.f - stick.rotation;
        }

        if(isBallMoving(cue)) //If ball is moving...
        {
            //Draw UI with gray overlay.
            drawPowerBar(bar, 1);
            drawAngleDisplay(angle, windowWidth*0.1, 20, windowWidth*0.8, 1);
        }
        else
        {
            //Draw UI normally.
            drawPowerBar(bar, 0);
            drawAngleDisplay(angle, windowWidth*0.1, 20, windowWidth*0.8, 0);
        }
    }
    if(paused)
    {
        Rect2 overlay = {0, 0, windowWidth, windowHeight};
        glColor4f(0, 0, 0, 0.5); //Set color to transparent grey.
        drawRect2(overlay);
    }
    glEnable(GL_LIGHTING);
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position[0], camera.position[1], camera.position[2], //eye location
              camera.lookAt[0], camera.lookAt[1], camera.lookAt[2], //looking at
              camera.up[0], camera.up[1], camera.up[2] //up vector
             );

    displayObjects(); //Draw all 3D Objects.

    // Set up camera for 2D rendering.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0); //Set ortho projection with (0,0) at the bottom left of the screen.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); //Disable depth test so UI always appears on top.

    displayUI(); //Draw stuff that's projected straight onto the 2D screen.

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); //Reset the Projection Matrix back to the default perspective view.
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}


void reshape (int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(80.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    positionPowerBar();
}

void openConsole()
{
    paused = 1; //Pause the game.
    glutKeyboardFunc(keyboard_console); //Enable console input (and disable world input).
    glutMouseFunc((void*) 0); //Disable mouse input.
    printf("%sConsole open.%s\n", OUT_YELLOW, OUT_RESET);
}

void closeConsole()
{
    paused = 0;     //Un-pause the game.
    glutKeyboardFunc(keyboard_world); ////Enable world input (and disable console input).
    glutMouseFunc(mouse); //Enable mouse input.

    if(strcmp(inputBuffer, "\0") != 0) //If there's leftover user input in the console...
        printf("\n"); //Start on a new line.
    printf("%sConsole closed.%s\n", OUT_YELLOW, OUT_RESET);

    is_command_entered = 0; //Reset boolean.
    inputBuffer[0] = '\0'; //Reset input buffer.
}

void keyboard_world (unsigned char key, int x, int y)
{
    float speed = 0.1;

    switch (key)
    {

    // Camera movement keys.
    case 'A':
    case 'a': //Lower-case x to move camera around pool table clockwise.
        moveSideways(&camera, (-1*speed));
        break;
    case 'D':
    case 'd': //Upper-case X to move camera around pool table anti-clockwise.
        moveSideways(&camera, speed);
        break;
    case 'S':
    case 's': //Lower-case z to zoom camera out.
        moveForward(&camera, (-1*speed));
    break;
    case 'W':
    case 'w': //Upper-case Z to zoom camera in.
        moveForward(&camera, speed);
        break;
    case 'r': //R key to restart
    case 'R':
        setLevel('r');
        stick.rotation = 180.f;
        break;
    case 't': //T key to restart in triangular formation.
    case 'T':
        setLevel('t');
        stick.rotation = 180.f;
        break;
    case '1':
        initCamera(CAM_TOPDOWN);
        break;
    case '2':
        initCamera(CAM_FOLLOWCUE);
        break;
    case '3':
        initCamera(CAM_SIDEANGLE);
        break;
//    case 'a':
//    case 'A': //A key to rotate stick around cue ball clockwise.
//        if(!isBallMoving(cue))
//        {
//            stick.rotation -= 5; //Rotate the stick.
//            if(stick.rotation <= 0) //If the stick has completed a full revolution.
//                stick.rotation = 360; //Reset the angle of the stick.
//        }
//        break;
//    case 'd':
//    case 'D': //D key to rotate stick around cue ball anti-clockwise.
//        if(!isBallMoving(cue))
//        {
//            stick.rotation += 5; //Rotate the stick.
//            if(stick.rotation >= 360) //If the stick has completed a full revolution.
//                stick.rotation = 0; //Reset the angle of the stick.
//        }
//        break;

    case 27: //Esc key or
    case 13: //Enter key to bring up the console.
        openConsole();
        break;

    case 32: //Space bar.
        break;
    default:
        break;
    }
    // Keys to shoot the cue ball.


    glutPostRedisplay();
}

void keyboard_console(unsigned char key, int x, int y)
{
    if( (key >= 'A' && key <= 'Z' )     //If key is either [A-Z],
            || (key >= 'a' && key <= 'z' )  //or key is either [a-z],
            || (key >= '0' && key <= '9')   //or key is a digit,
            || (key  == '-')                //or key is minus sign,
            || (key  == '.')                //or key is fullstop,
            || (key  == ' ') )              //or key is space...
    {
        int len = strlen(inputBuffer);
        if(len < MAX_USER_INPUT - 1) //If there's space in the buffer...
        {
            //Add key to the user input buffer.
            inputBuffer[len] = key;
            inputBuffer[++len] = '\0';

            if(key == ' ') //If space key pressed...
            {
                is_command_entered = 1; //Command has been entered. (Stop displaying text as green).
            }
            if(is_command_entered)
            {
                printf("%s", OUT_YELLOW); //Set text color to yellow.
            }
            else
            {
                printf("%s", OUT_GREEN); //Set text color to green.
            }
            printf("%c", key); //Print keystroke.
            printf(""); //Reset text color.
        }

    }
    else if(key == 8) //Backspace
    {
        int len = strlen(inputBuffer);
        if(len > 0)
        {
            printf("\b \b"); //Delete previous character.
            inputBuffer[--len] = '\0';
        }
    }
    else if(key == 13) //Enter key - process user input.
    {

        char* command = strtok(inputBuffer, " "); //Get first token separated by a space.
        if(command == 0)//If no input entered...
        {
            closeConsole();
            return;
        }
        printf("\n");

        if(strcmp(command, "friction") == 0)
        {
            friction = atof(strtok(NULL, " "));
            printf("Friction set to: %f\n", friction);
        }
        else if(strcmp(command, "cueforcemin") == 0)
        {
            cueForce.min = atof(strtok(NULL, " "));
            printf("Cue force minimum set to: %f\n", cueForce.min);
        }
        else if(strcmp(command, "cueforcemax") == 0)
        {
            cueForce.max = atof(strtok(NULL, " "));
            printf("Cue force maximum set to: %f\n", cueForce.max);
        }
        else if(strcmp(command, "trianglebase") == 0)
        {
            base = atoi(strtok(NULL, " "));
            printf("Triangle base set to: %d\n", base);
        }
        else if(strcmp(command, "rectanglewidth") == 0)
        {
            width = atoi(strtok(NULL, " "));
            printf("Rectangle width set to: %d\n", width);
        }
        else if(strcmp(command, "rectanglelength") == 0)
        {
            length = atoi(strtok(NULL, " "));
            printf("Rectangle length set to: %d\n", length);
        }
        else if(strcmp(command, "invertcontrol") == 0)
        {
            is_control_inverted = is_control_inverted ? 0 : 1 ; //Flip inverted control setting.
            printf("Controls inverted.\n");
        }
        else if(strcmp(command, "overlay") == 0)
        {
            overlay_on = overlay_on ? 0 : 1 ; //Flip inverted control setting.
            if(overlay_on)
                printf("Overlay turned on.\n");
            else
                printf("Overlay turned off.\n");
        }
        else if(strcmp(command, "reset") == 0)
        {
            setDefaults();
            printf("Default settings applied.\n");
        }
        else if(strcmp(command, "exit") == 0)
        {
            printf("Closing program.\n");
            exit(0);
        }
        else if(strcmp(command, "help") == 0)
        {
            printf("List of commands:\n");
            printf("    %sfriction %s<value> %s- Set friction to the specified value.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %scueforcemin %s<value> %s- Set the minimum force of the cue stick.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %scueforcemax %s<value> %s- Set the maximum force of the cue stick.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %strianglebase %s<value> %s- Set the base size of the triangle formation.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %srectanglewidth %s<value> %s- Set the width of the rectangle formation.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %srectanglelength %s<value> %s- Set the length of the rectangle formation.\n", OUT_GREEN, OUT_YELLOW, OUT_RESET);
            printf("    %sinvertcontrol %s- Invert the control of the cue stick.\n", OUT_GREEN, OUT_RESET);
            printf("    %soverlay %s- Toggle the visibility of the UI overlay.\n", OUT_GREEN, OUT_RESET);
            printf("    %sreset %s- Reset all properties to the default value.\n", OUT_GREEN, OUT_RESET);
            printf("    %sexit %s- Exit the program.\n", OUT_GREEN, OUT_RESET);
            printf("    [esc] - Exit the console.\n");
        }
        else
        {
            printf("Unknown command. Enter 'help' for a list of commands.\n");

        }
        is_command_entered = 0;
        inputBuffer[0] = '\0';
    }
    else if(key == 27) //Esc key - exit the console.
    {
        closeConsole();
    }

    //Uncomment if the 3D view needs to be updated after running a command.
    //glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
{
    if(!isBallMoving(cue))
    {
        if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) //Left Mouse Button to hit the ball.
        {
            hit = 1; //The cue ball is being hit.
        }

    }

}

void passiveMouse(int x, int y)
{
    mousepos[0] = x;
    mousepos[1] = y;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (windowWidth, windowHeight);
    glutInitWindowPosition (400, 400);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard_world);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMouse);

    previousTime = glutGet(GLUT_ELAPSED_TIME)/1000.f;

    glutTimerFunc(TIMERMSECS, animate, 0); //after at least 33ms, the function animate() will be called.


    glutMainLoop();

    free(balls); //Once the program ends, the balls are deleted.
    free(colors);
    return 0;
}
