#include <windows.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const GLfloat SKY_COLOR[] = {0.4f, 0.6f, 1.0f};
const GLfloat BRICK_COLOR[] = {0.91f, 0.47f, 0.2f};
const GLfloat BRICK_SHADOW_COLOR[] = {0.81f, 0.37f, 0.1f};
const GLfloat BRICK_HIGHLIGHT_COLOR[] = {1.0f, 0.57f, 0.3f};
const GLfloat CLOUD_WHITE[] = {1.0f, 1.0f, 1.0f};
const GLfloat CLOUD_BLUE[] = {0.6f, 0.8f, 1.0f};
const GLfloat BUSH_GREEN_LIGHT[] = {0.2f, 0.8f, 0.2f};
const GLfloat BUSH_GREEN_DARK[] = {0.0f, 0.5f, 0.0f};

const int BRICK_ROWS = 2;
const int BRICK_COLUMNS = 16;

float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.3f;

void drawCircle(float cx, float cy, float r) {
    int num_segments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.14159f * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawCloud(float x, float y, float pixelSize, bool isSmall) {
    glColor3fv(CLOUD_WHITE);
    float r = isSmall ? pixelSize * 2.5f : pixelSize * 3.5f;
    float spacing = r * 0.9f;

    drawCircle(x + spacing * 0, y, r);
    drawCircle(x + spacing * 1, y + r * 0.3f, r * 1.1f);
    drawCircle(x + spacing * 2, y, r);

    if (!isSmall) {
        drawCircle(x + spacing * 1.5f, y - r * 0.4f, r * 0.8f);
        drawCircle(x + spacing * 0.5f, y - r * 0.4f, r * 0.8f);
    }
}

void drawBush(float x, float y, float scale, int size) {
    float r = 10 * scale;  // Reduced radius
    float spacing = r * 2.5f; // Increased spacing for more separation

    // Main bush color
    glColor3fv(BUSH_GREEN_LIGHT);

    // Draw the first bush (center)
    drawCircle(x, y, r);

    // Draw the second bush (left)
    drawCircle(x - spacing, y, r * 0.9f);

    // Draw the third bush (right)
    drawCircle(x + spacing, y, r * 0.9f);

    if (size > 0) {
        // Draw a far left bush if size is greater than 0
        drawCircle(x - spacing * 2, y, r * 0.7f);
    }
    if (size > 1) {
        // Draw a far right bush if size is greater than 1
        drawCircle(x + spacing * 2, y, r * 0.7f);
    }

    // Draw base (rounded rectangle shape)
    glColor3fv(BUSH_GREEN_DARK);
    glBegin(GL_QUADS);
    glVertex2f(x - spacing * (2 + size * 0.6f), y - r * 0.3f);
    glVertex2f(x + spacing * (2 + size * 0.6f), y - r * 0.3f);
    glVertex2f(x + spacing * (2 + size * 0.6f), y);
    glVertex2f(x - spacing * (2 + size * 0.6f), y);
    glEnd();
}

void drawSeparateBushes(float startX, float startY, float scale, int size) {
    float r = 10 * scale;
    float spacing = r * 3.5f; // Increased spacing to separate the bushes more

    // Draw each bush as a separate entity
    drawBush(startX, startY, scale, size);
    drawBush(startX + spacing, startY, scale, size); // Draw second bush offset to the right
    drawBush(startX - spacing, startY, scale, size); // Draw third bush offset to the left
}



void drawPixel(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}

void drawBrick(float x, float y, float width, float height) {
    glColor3fv(BRICK_COLOR);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + width, y);
    glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glEnd();

    glColor3fv(BRICK_SHADOW_COLOR);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + width, y);
    glVertex2f(x + width, y + height * 0.1f); glVertex2f(x, y + height * 0.1f);
    glVertex2f(x + width * 0.9f, y); glVertex2f(x + width, y);
    glVertex2f(x + width, y + height); glVertex2f(x + width * 0.9f, y + height);
    glEnd();

    glColor3fv(BRICK_HIGHLIGHT_COLOR);
    glBegin(GL_QUADS);
    glVertex2f(x, y + height * 0.9f); glVertex2f(x + width, y + height * 0.9f);
    glVertex2f(x + width, y + height); glVertex2f(x, y + height);
    glVertex2f(x, y); glVertex2f(x + width * 0.1f, y);
    glVertex2f(x + width * 0.1f, y + height); glVertex2f(x, y + height);
    glEnd();

    glColor3fv(BRICK_SHADOW_COLOR);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + height * 0.5f); glVertex2f(x + width, y + height * 0.5f);
    for (int i = 1; i < 4; i++) {
        float xPos = x + (width * i) / 4.0f;
        glVertex2f(xPos, y); glVertex2f(xPos, y + height * 0.5f);
        xPos = x + (width * (i - 0.5f)) / 4.0f;
        glVertex2f(xPos, y + height * 0.5f); glVertex2f(xPos, y + height);
    }
    glEnd();
}

void display() {
    glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float pixelSize = WINDOW_WIDTH / 150.0f;

    float cloud1X = fmod(cloudOffset + WINDOW_WIDTH * 0.15f, WINDOW_WIDTH + pixelSize * 40) - pixelSize * 40;
    float cloud2X = fmod(cloudOffset + WINDOW_WIDTH * 0.45f, WINDOW_WIDTH + pixelSize * 30) - pixelSize * 30;
    float cloud3X = fmod(cloudOffset + WINDOW_WIDTH * 0.75f, WINDOW_WIDTH + pixelSize * 40) - pixelSize * 40;

    drawCloud(cloud1X, WINDOW_HEIGHT * 0.75f, pixelSize, false);
    drawCloud(cloud2X, WINDOW_HEIGHT * 0.65f, pixelSize, true);
    drawCloud(cloud3X, WINDOW_HEIGHT * 0.75f, pixelSize, false);

    float brickHeight = (float)WINDOW_WIDTH / BRICK_COLUMNS;
    float bushY = brickHeight * BRICK_ROWS;

    drawBush(WINDOW_WIDTH * 0.05f, bushY, pixelSize * 2.0f, 2);
    drawBush(WINDOW_WIDTH * 0.38f, bushY, pixelSize * 1.6f, 0);
    drawBush(WINDOW_WIDTH * 0.65f, bushY, pixelSize * 1.8f, 1);

    float brickWidth = (float)WINDOW_WIDTH / BRICK_COLUMNS;
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLUMNS; col++) {
            float x = col * brickWidth;
            float y = row * brickHeight;
            drawBrick(x, y, brickWidth, brickHeight);
        }
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    cloudOffset += CLOUD_SPEED;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Super Mario Bros Scene");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
