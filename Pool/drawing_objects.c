#include "drawing_objects.h"

void drawPlane(float width, float length)
{
    Point3 vertices[4] =
    {
        {-width, 0, -length}, //Sets the vertices of the plane with the given dimensions.
        {-width, 0, length},
        {width, 0, length},
        {width, 0, -length}
    };

    glBegin(GL_POLYGON);
    //glColor3f(1, 1, 1);
    for(int k = 0; k < 4; k++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(vertices[k]);
        glPopMatrix();
    }
    glEnd();
}

void drawBalls(const Ball* balls, int count)
{
    for(int i = 0; i < count; i++) //Draw each ball.
    {
        glPushMatrix();

        glColor3fv(balls[i].color); //Colour
        glTranslatef(balls[i].position[0], balls[i].position[1], balls[i].position[2]); //Position Ball.
        glutSolidSphere(balls[i].radius, 20, 16);   //Draw Ball.

        glPopMatrix();
    }
}

void drawCueStick(CueStick stick)
{
    glRotatef(stick.rotation, 0, 1, 0); //Rotate stick around the cue ball.

    glPushMatrix();
    glRotatef(-90, 0, 1, 0); //I genuinely have no idea why this needs to be rotated for the shadow to display properly.
    glRotatef(90, 1, 0, 0);
    glTranslatef(0.0, 0.001, 0.0);
    float part = cos(stick.angle * 3.14/180);
    Rect2 shadow = {stick.distance * part, -0.02, (stick.distance - stick.length) * part, 0.02}; //Define the shadow below the stick.
    glColor3f(0.2, 0.3, 0.2); //Green-ish gray.
    drawRect2(shadow); //Draw shadow.

    glColor3f(1, 1, 0.2); //Yellow.
    Rect2 path = {0.1 - stick.distance/stick.maxDistance*0.2, -0.05, 0.0, 0.05}; //Define the path the cue will take.
    drawRect2(path);

    glPopMatrix();

    glColor3f(1.0, 1.0, 1.0); //White.
    glRotatef(stick.angle, 1, 0, 0); //Angle towards the cue ball.
    glTranslatef(0.f, 0.f, stick.distance - stick.length); //Position the stick.
    gluCylinder(gluNewQuadric(), 0.05f, 0.02f, stick.length, 20, 16); //Draw the stick.
}

void initBalls(Ball * b, int size)
{
    for(int i = 0; i < size; i++)
    {
        b[i] = setVelocity(b[i], 0.f, 0.f, 0.f); //Initialise the velocity of the ball (geometry.h).
        b[i].radius = 0.1f;
//        b[i].mass = 50.f; //Mass is set when setting formation instead.
        b[i].is_scored = 0;
    }
}

int calculateTotalFromBase(int numBallsAtBase)
{
    int totalNumBalls = 0;
    for(int i = numBallsAtBase; i > 0; i--)
    {
        totalNumBalls += i;
    }

    return totalNumBalls;
}

int calculateTotalFromLengthAndWidth(int width, int length)
{
    return width * length;
}

void getColorFromHue(Color C, float hue)
{
    hue -= (int)hue; //Only get fractional part.
    float red;
    float green;
    float blue;

    float six1 = 1.f / 6.f; // 1 sixth
    float six2 = 2.f / 6.f; // 2 sixths
    float six3 = 3.f / 6.f; // 3 sixths
    float six4 = 4.f / 6.f; // 4 sixths
    float six5 = 5.f / 6.f; // 5 sixths

    if(hue < six1)
    {
        red = 1;
        green = hue / six1;
        blue = 0;
    }
    else if(hue < six2)
    {
        red = 1 - ((hue - six1) / six1);
        green = 1;
        blue = 0;
    }
    else if(hue < six3)
    {
        red = 0;
        green = 1;
        blue = (hue - six2) / six1;
    }
    else if(hue < six4)
    {
        red = 0;
        green = 1 - ((hue - six3) / six1);
        blue = 1;
    }
    else if(hue < six5)
    {
        red = (hue - six4) / six1;
        green = 0;
        blue = 1;
    }
    else
    {
        red = 1;
        green = 0;
        blue = 1 - ((hue - six5) / six1);
    }

//    float red = ((percentage < 0.5)? ((0.5 - percentage) / 0.5) : 0); //Red = 1 when percentage = 0, Red approaches 0 as percentage approaches 1.
//
//    float green = (percentage < 0.3)? (percentage/0.3) : ((1 - percentage) / 0.7); //Green = 1 when percentage = 0.5, Green approaches 0 as percentage approaches 0 / 1.
//
//    float blue = ((percentage > 0.5)? ((percentage - 0.5) / 0.5) : 0) ; //Blue = 1 when percentage = 1, Blue approaches 0 as percentage approaches 0.

    C[0] = red;
    C[1] = green;
    C[2] = blue;

//    printf("h %f, r %f, g %f, b %f\n", hue, red, green, blue);

    return;
}

void setTriangularFormation(Ball * b, int numBallsAtBase)
{
    float mass_min = 30.f;
    float mass_max = 1000.f;

    int i = 0; //Go to the first row of the triangular formation.

    for(int row = 1; row <= numBallsAtBase; row++) //Go through each row of the triangular formation...
    {
        float percentage = (float)(row-1)/(numBallsAtBase-1); //How heavy the ball is.

        for(int k = 0; k < row; k++) //Go through each ball in the row...
        {
            b[i+k].mass = mass_min + percentage*(mass_max - mass_min); //Set mass so balls at the back are heavier.
//            printf("mass %f\n", b[i+k].mass);
            getColorFromHue(b[i+k].color, percentage * (4.4/6.f)); //Shifts color, heaviest ball is purple.

            float x = (row - 1) * (b[i+k].radius) - k * (b[i+k].radius * 2.1); //First part sets the starting ball to the right of the row. Second part sets each ball.
            float y = b[i+k].radius; //Places ball on top of table.
            float z = -1.2f + numBallsAtBase * b[i+k].radius - row * (1.6 * b[i+k].radius); //First positions the formation depending on how many balls are at the base. Second part sets the ball in the z position depending on which row its in; resides in the corners in between the surrounding rows.
            b[i+k] = setPosition(b[i+k], x, y, z); //Sets each dimension of the ball.
        }
        i += row; //Go to the next row.
    }
}

void setRectangularFormation(Ball * b, int width, int length)
{
    float mass_min = 30.f;
    float mass_max = 1000.f;

    for(int i = 0; i < width; i++) //Go through each column...
    {
        for(int k = 0; k < length; k++) //Go through each row...
        {
            float percentage = (float) k/(length-1); //How heavy the ball is.

            b[i*length+k].mass = mass_min + percentage*(mass_max - mass_min); //Set mass so balls at the back are heavier.
//            printf("mass %f\n", b[i*length+k].mass);
            getColorFromHue(b[i*length+k].color, percentage * (4.4/6.f)); //Shifts color, heaviest ball is purple.

            float x = ((width - 1) * (b[i*length+k].radius)) - i * (2 * b[i*length+k].radius);
            float y = b[i*length+k].radius; //Places ball on top of table.
            float z = -1.2f + length * b[i*length+k].radius - k * (2 * b[i*length+k].radius); //
            b[i*length+k] = setPosition(b[i*length+k], x, y, z); //Sets each dimension of the ball.
        }
    }
}

void drawRect2(const Rect2 rect)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glVertex2f(rect[0], rect[1]);
    glVertex2f(rect[2], rect[1]);
    glVertex2f(rect[2], rect[3]);
    glVertex2f(rect[0], rect[3]);
    glEnd();

    glEnable(GL_LIGHTING);
    return;
}

void drawRect2Outline(const Rect2 rect, float size)
{
    Rect2 outer =
    {
        rect[0] - size, rect[1] - size, //Bottom left point including outline.
        rect[2] + size, rect[3] + size
    }; //Top right point including outline.

    glBegin(GL_QUAD_STRIP);
    //Bottom section.
    glVertex2f(outer[0], outer[1]);
    glVertex2f(rect[0], rect[1]);
    glVertex2f(outer[3], outer[1]);
    glVertex2f(rect[2], rect[1]);
    //Right section.
    glVertex2f(outer[2], outer[3]);
    glVertex2f(rect[2], rect[3]);
    //Top section.
    glVertex2f(outer[0], outer[3]);
    glVertex2f(rect[0], rect[3]);
    //Left section.
    glVertex2f(outer[0], outer[1]);
    glVertex2f(rect[0], rect[1]);
    glEnd();

    return;
}

void drawPowerBar(PowerBar bar, int disabled)
{
    //(x1, y1) is the bottom left.
    //(x2, y2) is the top right.
    //(o_x1, o_y1) is the bottom left including the border outline.
    //(o_x2, o_y2) is the top right including the border outline.
    //(p_y1) is the height of the power bar.
    float x1 = bar.position[0] - bar.width/2;
    float x2 = bar.position[0] + bar.width/2;
    float y1 = bar.position[1] - bar.height/2;
    float y2 = bar.position[1] + bar.height/2;
    float o_x1 = x1 - bar.outlineWidth;
    float o_y1 = y1 - bar.outlineWidth;
    float o_x2 = x2 + bar.outlineWidth;
    float o_y2 = y2 + bar.outlineWidth;
    float p_y1 = y1 + bar.height * bar.power;

    //glDisable(GL_LIGHTING);
    glBegin(GL_QUAD_STRIP);
    glColor3f(1.0, 1.0, 1.0); //White color.
    //Draw outline around bar.
    //Bottom section.
    glVertex2f(o_x1, o_y1);
    glVertex2f(x1, y1);
    glVertex2f(o_x2, o_y1);
    glVertex2f(x2, y1);
    //Right section.
    glVertex2f(o_x2, o_y2);
    glVertex2f(x2, y2);
    //Top section.
    glVertex2f(o_x1, o_y2);
    glVertex2f(x1, y2);
    //Left section.
    glVertex2f(o_x1, o_y1);
    glVertex2f(x1, y1);
    glEnd();


    glBegin(GL_QUADS);
    glColor3f(0.67, 0.13, 0.13); //Dark red color.
    //Draw base of the bar.
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glColor3f(0.96, 0.96, 0.25); //Yellow color.
    //Draw rectangle representing the power level.
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, p_y1);
    glVertex2f(x1, p_y1);
    glEnd();

    //Draw grey overlay if disabled.
    if(disabled)
    {
        glColor4f(0.1, 0.1, 0.1, 0.5); //Transparent grey.
        glBegin(GL_QUADS);
        glVertex2f(o_x1, o_y1);
        glVertex2f(o_x2, o_y1);
        glVertex2f(o_x2, o_y2);
        glVertex2f(o_x1, o_y2);
        glEnd();
    }

    //glEnable(GL_LIGHTING);

    return;
}

void drawAngleDisplay(float angle, float x, float y, float width, int disabled)
{
    float height = 20; //Height of the angle display.

    //Draw rectangle overlay.
    Rect2 overlay = {x, y, x+width, y+height};
    //glDisable(GL_LIGHTING);
    glColor4f(0.3, 0.1, 0.1, 0.8); //Transparent red.
    drawRect2(overlay);

    //Draw big markings (0, 90, 180, 270, 360).
    float half_width = 2; //Half the width of the marking.
    float gap = width/4;

    Rect2 marking = {x-half_width, y, x+half_width, y+height};

    glColor3f(1.f, 1.f, 1.f); //White.

    for(int i=0; i<5; i++)
    {
        drawRect2(marking);
        marking[0] += gap;
        marking[2] += gap;
    }

    //Draw smaller divisions between the big markings.
    half_width = 1;
    float offset = 2;
    gap = width/16;

    marking[0] = x - half_width;
    marking[1] = y + offset;
    marking[2] = x + half_width;
    marking[3] = y + height-offset;

    for(int i=1; i<17; i++)
    {
        drawRect2(marking);
        marking[0] += gap;
        marking[2] += gap;
    }

    //Draw current angle marker outline.
    half_width = 2;
    marking[0] = x - half_width + width*angle/360.f;
    marking[1] = y;
    marking[2] = x + half_width + width*angle/360.f;
    marking[3] = y + height;

    glColor4f(0.f, 0.f, 0.f, 0.5f); //transparent black.

    drawRect2(marking);

    //Draw current angle marker.
    half_width = 1;
    marking[0] = x - half_width + width*angle/360.f;
    marking[2] = x + half_width + width*angle/360.f;

    glColor3f(1.f, 1.f, 0.1f); //Yellow.

    drawRect2(marking);

    //Draw grey overlay if disabled.
    half_width = 1;

    if(disabled)
    {
        glColor4f(0.1, 0.1, 0.1, 0.5); //Transparent grey.
        overlay[0] -= half_width;
        overlay[2] += half_width;
        drawRect2(overlay);
    }
    //glEnable(GL_LIGHTING);

    return;
}

void drawHole(Hole* holes, float width, float length, float holeSize)
{
    Point3 vertices[6] =
    {
        {-width+holeSize/1.5, 0.02, -length+holeSize/1.5},
        {-width+holeSize/1.5,0.02, length-holeSize/1.5},
        {-width+holeSize/1.5,0.02, 0},
        {width-holeSize/1.5,0.02, length-holeSize/1.5},
        {width-holeSize/1.5,0.02, -length+holeSize/1.5},
        {width-holeSize/1.5,0.02, 0}
    };

    for(int i = 0; i < 6; i++)
    {
        glPushMatrix();
        glTranslatef(vertices[i][0],vertices[i][1],vertices[i][2]);
        glRotated(90,1,0,0);
        drawCircle(20, holeSize/1.5);
        glPopMatrix();
    }
}

void drawTableLeg(float width, float length, float topRadius, float baseRadius)
{
    Point3 vertices[4] =
    {
        {-width, 0, -length},
        {-width, 0, length},
        {width, 0, length},
        {width, 0, -length}
    };

    glColor3f(0.5, 0.4, 0.4);

    for(int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(vertices[i][0],vertices[i][1],vertices[i][2]);
        glRotated(90,1,0,0);
        gluCylinder(gluNewQuadric(), baseRadius, topRadius, 2, 20, 16);
        glPopMatrix();
    }

}

void drawTableEdges(float width, float length, float edgeWide, float edgeHeight)
{
    Point3 leftEdges[8] =
    {
        {width, 0, -length},
        {width, 0, length},
        {width + edgeWide, 0, length},
        {width + edgeWide, 0, -length},
        {width, edgeHeight, -length},
        {width, edgeHeight, length},
        {width + edgeWide, edgeHeight, length},
        {width + edgeWide, edgeHeight, -length}
    };

    Point3 rightEdges[8] =
    {
        {-width, 0, -length},
        {-width, 0, length},
        {-width - edgeWide, 0, length},
        {-width - edgeWide, 0, -length},
        {-width, edgeHeight, -length},
        {-width, edgeHeight, length},
        {-width - edgeWide, edgeHeight, length},
        {-width - edgeWide, edgeHeight, -length}
    };

    Point3 topEdges[8] =
    {
        {width + edgeWide, 0, length + edgeWide},
        {width + edgeWide, 0, length},
        {-width - edgeWide, 0, length},
        {-width - edgeWide, 0, length + edgeWide},
        {width + edgeWide, edgeHeight, length + edgeWide},
        {width + edgeWide, edgeHeight, length},
        {-width - edgeWide, edgeHeight, length},
        {-width - edgeWide, edgeHeight, length + edgeWide}
    };

    Point3 bottomEdges[8] =
    {
        {width + edgeWide, 0,-length - edgeWide},
        {width + edgeWide, 0, -length},
        {-width - edgeWide, 0, -length},
        {-width - edgeWide, 0, -length - edgeWide},
        {width + edgeWide, edgeHeight, -length - edgeWide},
        {width + edgeWide, edgeHeight, -length},
        {-width - edgeWide, edgeHeight, -length},
        {-width - edgeWide, edgeHeight, -length - edgeWide}
    };

    //Left Edge Bottom Side
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.4, 0.4);
    for(int i = 0; i < 4; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(leftEdges[i]);
        glPopMatrix();
    }
    glEnd();

        //Right Edge Bottom Side
    glBegin(GL_POLYGON);
    for(int i = 0; i < 4; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(rightEdges[i]);
        glPopMatrix();
    }
    glEnd();

        //Top Edge Bottom Side
    glBegin(GL_POLYGON);
    for(int i = 0; i < 4; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(topEdges[i]);
        glPopMatrix();
    }
    glEnd();

        //Bottom Edge Bottom Side
    glBegin(GL_POLYGON);
    for(int i = 0; i < 4; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(bottomEdges[i]);
        glPopMatrix();
    }
    glEnd();

    //Left Edge Top Side
    glBegin(GL_POLYGON);
    glColor3f(0.55, 0.4, 0.4);
    for(int i = 4; i < 8; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(leftEdges[i]);
        glPopMatrix();
    }
    glEnd();

    //Right Edge Top Side
    glBegin(GL_POLYGON);
    for(int i = 4; i < 8; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(rightEdges[i]);
        glPopMatrix();
    }
    glEnd();

    //Top Edge Top Side
    glBegin(GL_POLYGON);
    for(int i = 4; i < 8; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(topEdges[i]);
        glPopMatrix();
    }
    glEnd();

    //Bottom Edge Top Side
    glBegin(GL_POLYGON);
    for(int i = 4; i < 8; i++) //Draw the face of the plane using the list of vertices.
    {
        glPushMatrix();
        glVertex3fv(bottomEdges[i]);
        glPopMatrix();
    }
    glEnd();


    //Right Edge RIGHT SIDE
    glBegin(GL_POLYGON);
    glColor3f(0.6, 0.4, 0.4);
    glPushMatrix();
    glVertex3fv(rightEdges[0]);
    glVertex3fv(rightEdges[1]);
    glVertex3fv(rightEdges[5]);
    glVertex3fv(rightEdges[4]);
    glPopMatrix();
    glEnd();

    //Left Edge Right Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(leftEdges[0]);
    glVertex3fv(leftEdges[1]);
    glVertex3fv(leftEdges[5]);
    glVertex3fv(leftEdges[4]);
    glPopMatrix();
    glEnd();

    //Top Edge Right Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(topEdges[0]);
    glVertex3fv(topEdges[1]);
    glVertex3fv(topEdges[5]);
    glVertex3fv(topEdges[4]);
    glPopMatrix();
    glEnd();

        //Bottom Edge Right Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(bottomEdges[0]);
    glVertex3fv(bottomEdges[1]);
    glVertex3fv(bottomEdges[5]);
    glVertex3fv(bottomEdges[4]);
    glPopMatrix();
    glEnd();

    //Bottom Edge Left Side
    glBegin(GL_POLYGON);
     glColor3f(0.65, 0.4, 0.4);
    glPushMatrix();
    glVertex3fv(bottomEdges[2]);
    glVertex3fv(bottomEdges[3]);
    glVertex3fv(bottomEdges[7]);
    glVertex3fv(bottomEdges[6]);
    glPopMatrix();
    glEnd();

    //Right Edge Left Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(rightEdges[2]);
    glVertex3fv(rightEdges[3]);
    glVertex3fv(rightEdges[7]);
    glVertex3fv(rightEdges[6]);
    glPopMatrix();
    glEnd();

    //Left Edge Left Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(leftEdges[2]);
    glVertex3fv(leftEdges[3]);
    glVertex3fv(leftEdges[7]);
    glVertex3fv(leftEdges[6]);
    glPopMatrix();
    glEnd();

    //Top Edge Left Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(topEdges[2]);
    glVertex3fv(topEdges[3]);
    glVertex3fv(topEdges[7]);
    glVertex3fv(topEdges[6]);
    glPopMatrix();
    glEnd();

    //Top Edge Front Side
    glBegin(GL_POLYGON);
     glColor3f(0.35, 0.5, 0.5);
    glPushMatrix();
    glVertex3fv(topEdges[0]);
    glVertex3fv(topEdges[3]);
    glVertex3fv(topEdges[7]);
    glVertex3fv(topEdges[4]);
    glPopMatrix();
    glEnd();

    //Bottom Edge Front Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(bottomEdges[0]);
    glVertex3fv(bottomEdges[3]);
    glVertex3fv(bottomEdges[7]);
    glVertex3fv(bottomEdges[4]);
    glPopMatrix();
    glEnd();

    //Top Edge Back Side
    glBegin(GL_POLYGON);
    glColor3f(0.8, 0.4, 0.4);
    glPushMatrix();
    glVertex3fv(topEdges[1]);
    glVertex3fv(topEdges[2]);
    glVertex3fv(topEdges[6]);
    glVertex3fv(topEdges[5]);
    glPopMatrix();
    glEnd();

    //Bottom Edge Back Side
    glBegin(GL_POLYGON);
    glPushMatrix();
    glVertex3fv(bottomEdges[1]);
    glVertex3fv(bottomEdges[2]);
    glVertex3fv(bottomEdges[6]);
    glVertex3fv(bottomEdges[5]);
    glPopMatrix();
    glEnd();
}

void drawCircle(int triangleAmount,float radius)
{
    float PI = 3.14159;
    float twicePI = 2.0 * PI;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 0.0, 0.0);
    float x = 0;
    float y = 0;
    glVertex2f(x,y);
    for(int i = 0; i <= triangleAmount; i++)
    {
        glVertex2f(
               x + (radius * cos(i * twicePI / triangleAmount)),
               y + (radius * sin(i * twicePI / triangleAmount))
        );
    }
    glEnd();
}


