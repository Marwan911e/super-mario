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
const GLfloat CLOUD_WHITE[] = {1.0f, 1.0f, 1.0f};
const GLfloat CLOUD_BLUE[] = {0.6f, 0.8f, 1.0f}; // Light blue for cloud detail
const GLfloat BUSH_GREEN_LIGHT[] = {0.2f, 0.8f, 0.2f}; // Light green for bushes
const GLfloat BUSH_GREEN_DARK[] = {0.0f, 0.5f, 0.0f}; // Dark green for bush shadows

// Brick dimensions and layout
const int BRICK_ROWS = 2;
const int BRICK_COLUMNS = 16;

// Animation variables
float cloudOffset = 0.0f;
const float CLOUD_SPEED = 0.3f; // Speed of cloud movement

// Cloud patterns - Exact pixel map of Super Mario Bros clouds
// For large cloud (16x11)
const int LARGE_CLOUD_WIDTH = 16;
const int LARGE_CLOUD_HEIGHT = 11;
const char LARGE_CLOUD[LARGE_CLOUD_HEIGHT][LARGE_CLOUD_WIDTH+1] = {
    "    ####        ",
    "   ######       ",
    "  ########      ",
    " ##########     ",
    "############    ",
    "##############  ",
    "################",
    "################",
    "################",
    "################",
    "    #    #    # "
};

const char LARGE_CLOUD_DETAIL[LARGE_CLOUD_HEIGHT][LARGE_CLOUD_WIDTH+1] = {
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "bbbbbbbbbbbbbbbb",
    "                "
};

// For small cloud (10x8)
const int SMALL_CLOUD_WIDTH = 10;
const int SMALL_CLOUD_HEIGHT = 8;
const char SMALL_CLOUD[SMALL_CLOUD_HEIGHT][SMALL_CLOUD_WIDTH+1] = {
    "  ####    ",
    " ######   ",
    "########  ",
    "######### ",
    "##########",
    "##########",
    "##########",
    "  #   #   "
};

const char SMALL_CLOUD_DETAIL[SMALL_CLOUD_HEIGHT][SMALL_CLOUD_WIDTH+1] = {
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "bbbbbbbbbb",
    "          "
};

// Bush patterns - Pixel maps for Super Mario Bros bushes (updated to match the image)
// For large bush (16x10, representing the hill-like bush on the left)
const int LARGE_BUSH_WIDTH = 16;
const int LARGE_BUSH_HEIGHT = 10;
const char LARGE_BUSH[LARGE_BUSH_HEIGHT][LARGE_BUSH_WIDTH+1] = {
    "      #         ",
    "     ###        ",
    "    #####       ",
    "   #######      ",
    "  #########     ",
    " ###########    ",
    "#############   ",
    "##############  ",
    "############### ",
    "################"
};

const char LARGE_BUSH_DETAIL[LARGE_BUSH_HEIGHT][LARGE_BUSH_WIDTH+1] = {
    "                ",
    "                ",
    "                ",
    "                ",
    "       s        ",
    "      s s       ",
    "    s  s  s     ",
    "   s    s    s  ",
    "  s s  s  s s   ",
    " s   s    s   s "
};

// For medium bush (12x8, representing the medium bush on the right)
const int MEDIUM_BUSH_WIDTH = 12;
const int MEDIUM_BUSH_HEIGHT = 8;
const char MEDIUM_BUSH[MEDIUM_BUSH_HEIGHT][MEDIUM_BUSH_WIDTH+1] = {
    "    ##      ",
    "   ####     ",
    "  ######    ",
    " ########   ",
    "##########  ",
    "########### ",
    "############",
    "############"
};

const char MEDIUM_BUSH_DETAIL[MEDIUM_BUSH_HEIGHT][MEDIUM_BUSH_WIDTH+1] = {
    "            ",
    "            ",
    "      s     ",
    "    s  s    ",
    "   s    s   ",
    " s   s   s  ",
    "s  s  s  s  ",
    "  s    s    "
};

// For small bush (8x6, representing the small bush in the middle)
const int SMALL_BUSH_WIDTH = 8;
const int SMALL_BUSH_HEIGHT = 6;
const char SMALL_BUSH[SMALL_BUSH_HEIGHT][SMALL_BUSH_WIDTH+1] = {
    "  ##    ",
    " ####   ",
    "#####   ",
    "######  ",
    "####### ",
    "########"
};

const char SMALL_BUSH_DETAIL[SMALL_BUSH_HEIGHT][SMALL_BUSH_WIDTH+1] = {
    "        ",
    "   s    ",
    "  s     ",
    " s  s   ",
    "s    s  ",
    "  s   s "
};

void drawPixel(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}

// Modified cloud drawing function that doesn't rely on array pointers
void drawCloud(float x, float y, float pixelSize, bool isSmall) {
    int width, height;

    if (isSmall) {
        // Draw small cloud using SMALL_CLOUD arrays
        width = SMALL_CLOUD_WIDTH;
        height = SMALL_CLOUD_HEIGHT;

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                float pixelX = x + col * pixelSize;
                float pixelY = y + (height - 1 - row) * pixelSize; // Invert to draw from top down

                if (SMALL_CLOUD[row][col] == '#') {
                    if (SMALL_CLOUD_DETAIL[row][col] == 'b') {
                        glColor3fv(CLOUD_BLUE); // Blue highlight
                    } else {
                        glColor3fv(CLOUD_WHITE); // White cloud
                    }
                    drawPixel(pixelX, pixelY, pixelSize);
                }
            }
        }
    } else {
        // Draw large cloud using LARGE_CLOUD arrays
        width = LARGE_CLOUD_WIDTH;
        height = LARGE_CLOUD_HEIGHT;

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                float pixelX = x + col * pixelSize;
                float pixelY = y + (height - 1 - row) * pixelSize; // Invert to draw from top down

                if (LARGE_CLOUD[row][col] == '#') {
                    if (LARGE_CLOUD_DETAIL[row][col] == 'b') {
                        glColor3fv(CLOUD_BLUE); // Blue highlight
                    } else {
                        glColor3fv(CLOUD_WHITE); // White cloud
                    }
                    drawPixel(pixelX, pixelY, pixelSize);
                }
            }
        }
    }
}

// Function to draw bushes of different sizes
// Function to draw bushes of different sizes
void drawBush(float x, float y, float pixelSize, int size) {
    int width, height;

    // Select the appropriate bush size
    switch (size) {
        case 0: // Small bush
            width = SMALL_BUSH_WIDTH;
            height = SMALL_BUSH_HEIGHT;
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    float pixelX = x + col * pixelSize;
                    float pixelY = y + (height - 1 - row) * pixelSize; // Invert to draw from top down

                    if (SMALL_BUSH[row][col] == '#') {
                        if (SMALL_BUSH_DETAIL[row][col] == 's') {
                            glColor3fv(BUSH_GREEN_DARK); // Shadow
                        } else {
                            glColor3fv(BUSH_GREEN_LIGHT); // Light green
                        }
                        drawPixel(pixelX, pixelY, pixelSize);
                    }
                }
            }
            break;

        case 1: // Medium bush
            width = MEDIUM_BUSH_WIDTH;
            height = MEDIUM_BUSH_HEIGHT;
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    float pixelX = x + col * pixelSize;
                    float pixelY = y + (height - 1 - row) * pixelSize;

                    if (MEDIUM_BUSH[row][col] == '#') {
                        if (MEDIUM_BUSH_DETAIL[row][col] == 's') {
                            glColor3fv(BUSH_GREEN_DARK); // Shadow
                        } else {
                            glColor3fv(BUSH_GREEN_LIGHT); // Light green
                        }
                        drawPixel(pixelX, pixelY, pixelSize);
                    }
                }
            }
            break;

        case 2: // Large bush
            width = LARGE_BUSH_WIDTH;
            height = LARGE_BUSH_HEIGHT;
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    float pixelX = x + col * pixelSize;
                    float pixelY = y + (height - 1 - row) * pixelSize;

                    if (LARGE_BUSH[row][col] == '#') {
                        if (LARGE_BUSH_DETAIL[row][col] == 's') {
                            glColor3fv(BUSH_GREEN_DARK); // Shadow
                        } else {
                            glColor3fv(BUSH_GREEN_LIGHT); // Light green
                        }
                        drawPixel(pixelX, pixelY, pixelSize);
                    }
                }
            }
            break;
    }
}

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

    // Set pixel size for sprites
    float pixelSize = WINDOW_WIDTH / 150.0f;

    // Draw clouds - positioned similar to the reference image with animation
    float cloud1X = fmod(cloudOffset + WINDOW_WIDTH * 0.15f, WINDOW_WIDTH + pixelSize * LARGE_CLOUD_WIDTH) - pixelSize * LARGE_CLOUD_WIDTH;
    float cloud2X = fmod(cloudOffset + WINDOW_WIDTH * 0.45f, WINDOW_WIDTH + pixelSize * SMALL_CLOUD_WIDTH) - pixelSize * SMALL_CLOUD_WIDTH;
    float cloud3X = fmod(cloudOffset + WINDOW_WIDTH * 0.75f, WINDOW_WIDTH + pixelSize * LARGE_CLOUD_WIDTH) - pixelSize * LARGE_CLOUD_WIDTH;

    // Draw the clouds with new positions
    drawCloud(cloud1X, WINDOW_HEIGHT * 0.75f, pixelSize, false); // Left large cloud
    drawCloud(cloud2X, WINDOW_HEIGHT * 0.65f, pixelSize, true);  // Middle small cloud
    drawCloud(cloud3X, WINDOW_HEIGHT * 0.75f, pixelSize, false); // Right large cloud

    // Draw bushes (static, not animated)
    // Positioned just above the bricks
    float brickHeight = (float)WINDOW_WIDTH / BRICK_COLUMNS;
    float bushY = brickHeight * BRICK_ROWS;

    // Draw three bushes of different sizes
    drawBush(WINDOW_WIDTH * 0.05f, bushY, pixelSize, 2);    // Large bush on left
    drawBush(WINDOW_WIDTH * 0.38f, bushY, pixelSize, 0);    // Small bush in middle
    drawBush(WINDOW_WIDTH * 0.65f, bushY, pixelSize, 1);    // Medium bush on right

    // Draw the ground bricks at the bottom of the screen
    float brickWidth = (float)WINDOW_WIDTH / BRICK_COLUMNS;
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

// Timer function for animation
void update(int value) {
    // Update cloud position
    cloudOffset += CLOUD_SPEED;

    // Trigger redisplay
    glutPostRedisplay();

    // Call update again after 16ms (approx. 60 FPS)
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Super Mario Bros Scene");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Set up the timer for animation
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
