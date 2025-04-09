#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <cmath>

// Window size
// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Colors
const GLfloat SKY_COLOR[] = {0.4f, 0.6f, 1.0f}; // Blue sky color
const GLfloat BRICK_COLOR[] = {0.91f, 0.47f, 0.2f}; // More accurate brick color
const GLfloat BRICK_SHADOW_COLOR[] = {0.81f, 0.37f, 0.1f}; // Darker shade for brick shadows
const GLfloat BRICK_HIGHLIGHT_COLOR[] = {1.0f, 0.57f, 0.3f}; // Lighter shade for highlights

// Brick dimensions and layout
const int BRICK_ROWS = 2;
const int BRICK_COLUMNS = 16;

void drawBrick(float x, float y, float width, float height) {
    // Main brick body
    glColor3fv(BRICK_COLOR);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Bottom and right shadows
    glColor3fv(BRICK_SHADOW_COLOR);
    glBegin(GL_QUADS);
    // Bottom shadow
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height * 0.1f);
    glVertex2f(x, y + height * 0.1f);

    // Right shadow
    glVertex2f(x + width * 0.9f, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x + width * 0.9f, y + height);
    glEnd();

    // Top and left highlights
    glColor3fv(BRICK_HIGHLIGHT_COLOR);
    glBegin(GL_QUADS);
    // Top highlight
    glVertex2f(x, y + height * 0.9f);
    glVertex2f(x + width, y + height * 0.9f);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);

    // Left highlight
    glVertex2f(x, y);
    glVertex2f(x + width * 0.1f, y);
    glVertex2f(x + width * 0.1f, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Separator lines
    glColor3fv(BRICK_SHADOW_COLOR);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Horizontal center line
    glVertex2f(x, y + height * 0.5f);
    glVertex2f(x + width, y + height * 0.5f);

    // Vertical lines for brick pattern
    for (int i = 1; i < 4; i++) {
        float xPos = x + (width * i) / 4.0f;
        glVertex2f(xPos, y);
        glVertex2f(xPos, y + height * 0.5f);

        // Offset vertical lines for bottom row
        xPos = x + (width * (i - 0.5f)) / 4.0f;
        glVertex2f(xPos, y + height * 0.5f);
        glVertex2f(xPos, y + height);
    }
    glEnd();
}

void display() {
    // Clear screen with sky color
    glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the ground bricks at the bottom of the screen
    float brickWidth = (float)WINDOW_WIDTH / BRICK_COLUMNS;
    float brickHeight = brickWidth; // Square bricks
    float groundY = 0.0f; // Put bricks at the bottom

    // Draw each brick
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLUMNS; col++) {
            float x = col * brickWidth;
            float y = groundY + (row * brickHeight);
            drawBrick(x, y, brickWidth, brickHeight);
        }
    }

    glutSwapBuffers();
}


void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height); // Origin at bottom-left
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Super Mario Bros Scene");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
