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
const GLfloat MARIO_RED[] = {1.0f, 0.0f, 0.0f};
const GLfloat MARIO_BLUE[] = {0.0f, 0.0f, 1.0f};
const GLfloat MARIO_SKIN[] = {1.0f, 0.8f, 0.6f};
const GLfloat COIN_COLOR[] = {1.0f, 0.84f, 0.0f};

const int BRICK_ROWS = 2;
const int BRICK_COLUMNS = 16;

float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.3f;
const float MARIO_SPEED = 2.0f;
const float JUMP_VELOCITY = 10.0f;
const float GRAVITY = 0.5f;
const float COIN_ROTATION_SPEED = 5.0f;
const int JUMP_INTERVAL = 100;

float pixelSize;

// Mario state
struct Mario {
    float x;
    float y;
    float velocityY;
    bool isJumping;
    int runFrame;

    Mario() : x(0.0f), y(0.0f), velocityY(0.0f), isJumping(false), runFrame(0) {}
} mario;

// Floating block structure
struct Block {
    float x, y, width, height;
};

// Coin structure
struct Coin {
    float x, y;
    float rotation;
    bool active;

    Coin() : x(0.0f), y(0.0f), rotation(0.0f), active(true) {}
};

// Initialize blocks and coins explicitly
Block blocks[3];
Coin coins[3];

void initBlocksAndCoins() {
    // Floating blocks
    blocks[0].x = WINDOW_WIDTH * 0.3f;
    blocks[0].y = WINDOW_HEIGHT * 0.3f;
    blocks[0].width = 50.0f;
    blocks[0].height = 50.0f;

    blocks[1].x = WINDOW_WIDTH * 0.5f;
    blocks[1].y = WINDOW_HEIGHT * 0.4f;
    blocks[1].width = 50.0f;
    blocks[1].height = 50.0f;

    blocks[2].x = WINDOW_WIDTH * 0.7f;
    blocks[2].y = WINDOW_HEIGHT * 0.3f;
    blocks[2].width = 50.0f;
    blocks[2].height = 50.0f;

    // Coins above blocks
    coins[0].x = WINDOW_WIDTH * 0.32f;
    coins[0].y = WINDOW_HEIGHT * 0.38f;

    coins[1].x = WINDOW_WIDTH * 0.52f;
    coins[1].y = WINDOW_HEIGHT * 0.48f;

    coins[2].x = WINDOW_WIDTH * 0.72f;
    coins[2].y = WINDOW_HEIGHT * 0.38f;
}

void drawCircle(float cx, float cy, float r) {
    int num_segments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.14159265359f * i / num_segments;
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
    float r = 10 * scale;
    float spacing = r * 2.5f;

    glColor3fv(BUSH_GREEN_LIGHT);
    drawCircle(x, y, r);
    drawCircle(x - spacing, y, r * 0.9f);
    drawCircle(x + spacing, y, r * 0.9f);

    if (size > 0) drawCircle(x - spacing * 2, y, r * 0.7f);
    if (size > 1) drawCircle(x + spacing * 2, y, r * 0.7f);

    glColor3fv(BUSH_GREEN_DARK);
    glBegin(GL_QUADS);
    glVertex2f(x - spacing * (2 + size * 0.6f), y - r * 0.3f);
    glVertex2f(x + spacing * (2 + size * 0.6f), y - r * 0.3f);
    glVertex2f(x + spacing * (2 + size * 0.6f), y);
    glVertex2f(x - spacing * (2 + size * 0.6f), y);
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

void drawMario(float x, float y, float size, int frame) {
    float unit = size * 0.5f;

    glColor3fv(MARIO_BLUE);
    glBegin(GL_QUADS);
    glVertex2f(x - unit, y); glVertex2f(x, y); glVertex2f(x, y + unit); glVertex2f(x - unit, y + unit);
    glVertex2f(x, y); glVertex2f(x + unit, y); glVertex2f(x + unit, y + unit); glVertex2f(x, y + unit);
    glEnd();

    glColor3fv(MARIO_RED);
    glBegin(GL_QUADS);
    glVertex2f(x - unit, y + unit); glVertex2f(x + unit, y + unit);
    glVertex2f(x + unit, y + 3 * unit); glVertex2f(x - unit, y + 3 * unit);
    glEnd();

    glColor3fv(MARIO_SKIN);
    drawCircle(x, y + 4 * unit, unit);

    glColor3fv(MARIO_RED);
    glBegin(GL_QUADS);
    glVertex2f(x - unit * 1.5f, y + 4 * unit); glVertex2f(x + unit * 0.5f, y + 4 * unit);
    glVertex2f(x + unit * 0.5f, y + 5 * unit); glVertex2f(x - unit * 1.5f, y + 5 * unit);
    glEnd();

    glColor3fv(MARIO_SKIN);
    if (frame == 0) {
        glBegin(GL_QUADS);
        glVertex2f(x - unit * 1.5f, y + unit); glVertex2f(x - unit, y + unit);
        glVertex2f(x - unit, y + 2 * unit); glVertex2f(x - unit * 1.5f, y + 2 * unit);
        glVertex2f(x + unit, y + unit); glVertex2f(x + unit * 1.5f, y + unit);
        glVertex2f(x + unit * 1.5f, y + 2 * unit); glVertex2f(x + unit, y + 2 * unit);
        glEnd();
    } else if (frame == 1) {
        glBegin(GL_QUADS);
        glVertex2f(x - unit * 2, y + 2 * unit); glVertex2f(x - unit, y + 2 * unit);
        glVertex2f(x - unit, y + 3 * unit); glVertex2f(x - unit * 2, y + 3 * unit);
        glVertex2f(x + unit, y + unit); glVertex2f(x + unit * 1.5f, y + unit);
        glVertex2f(x + unit * 1.5f, y + 2 * unit); glVertex2f(x + unit, y + 2 * unit);
        glEnd();
    } else if (frame == 2) {
        glBegin(GL_QUADS);
        glVertex2f(x - unit * 1.5f, y + unit); glVertex2f(x - unit, y + unit);
        glVertex2f(x - unit, y + 2 * unit); glVertex2f(x - unit * 1.5f, y + 2 * unit);
        glVertex2f(x + unit, y + 2 * unit); glVertex2f(x + unit * 2, y + 2 * unit);
        glVertex2f(x + unit * 2, y + 3 * unit); glVertex2f(x + unit, y + 3 * unit);
        glEnd();
    }
}

void drawCoin(float x, float y, float rotation, float size) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    glColor3fv(COIN_COLOR);
    glBegin(GL_QUADS);
    glVertex2f(-size, -size); glVertex2f(size, -size);
    glVertex2f(size, size); glVertex2f(-size, size);
    glEnd();
    glPopMatrix();
}

void updateMario() {
    mario.x += MARIO_SPEED;
    if (mario.x > WINDOW_WIDTH) mario.x = -pixelSize * 10;

    static int frameCounter = 0;
    frameCounter++;

    // Automatic jumping
    if (frameCounter % JUMP_INTERVAL == 0 && !mario.isJumping) {
        mario.isJumping = true;
        mario.velocityY = JUMP_VELOCITY;
        mario.runFrame = 1;
    }

    // Update running animation
    if (frameCounter % 10 == 0 && !mario.isJumping) {
        mario.runFrame = (mario.runFrame + 1) % 3;
    }

    if (mario.isJumping) {
        mario.y += mario.velocityY;
        mario.velocityY -= GRAVITY;
    }

    float brickHeight = (float)WINDOW_WIDTH / BRICK_COLUMNS;
    float groundLevel = brickHeight * BRICK_ROWS;

    float marioWidth = pixelSize * 2.0f;
    float marioHeight = pixelSize * 5.0f;
    bool onBlock = false;
    for (int i = 0; i < 3; i++) {
        Block& b = blocks[i];
        if (mario.x + marioWidth > b.x && mario.x < b.x + b.width &&
            mario.y <= b.y + b.height && mario.y + marioHeight >= b.y + b.height &&
            mario.velocityY <= 0) {
            mario.y = b.y + b.height;
            mario.isJumping = false;
            mario.velocityY = 0.0f;
            onBlock = true;
            break;
        }
    }

    if (!onBlock && mario.y <= groundLevel && mario.velocityY <= 0) {
        mario.y = groundLevel;
        mario.isJumping = false;
        mario.velocityY = 0.0f;
    }

    for (int i = 0; i < 3; i++) {
        Coin& c = coins[i];
        if (c.active && fabs(mario.x - c.x) < pixelSize * 2 && fabs(mario.y - c.y) < pixelSize * 5) {
            c.active = false;
        }
    }
}

void updateCoins() {
    for (int i = 0; i < 3; i++) {
        if (coins[i].active) {
            coins[i].rotation += COIN_ROTATION_SPEED;
            if (coins[i].rotation >= 360.0f) coins[i].rotation -= 360.0f;
        }
    }
}

void display() {
    glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    pixelSize = WINDOW_WIDTH / 150.0f;

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

    for (int i = 0; i < 3; i++) {
        drawBrick(blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].height);
    }

    for (int i = 0; i < 3; i++) {
        if (coins[i].active) {
            drawCoin(coins[i].x, coins[i].y, coins[i].rotation, pixelSize);
        }
    }

    drawMario(mario.x, mario.y, pixelSize * 2.0f, mario.runFrame);

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
    updateMario();
    updateCoins();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Super-Mario-Bros.-221003166-221001810");
    initBlocksAndCoins();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
