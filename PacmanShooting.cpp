/**********************************
 CTIS164 - Homework 2
-----------------------------------
NAME SURNAME : Zeynep Arslan
 Additional Functions:

	1- When the remaining time is less than 5 seconds the ghosts will go faster

	2- The score is shown on the screen

	3- ESC to quit

	4- The color of the ghost is also randomized

The program runs smoothly without encountering any problems
-----------------------------------
**********************************/

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAX_GHOST 5// 5 fires at a time.
#define WINDOW_WIDTH  650
#define WINDOW_HEIGHT 650
#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_PERIOD2  20 // Period for the animation.
#define TIMER_ON         1 // 0:Disable timer, 1:Enable timer
#define PI 3.141592654
#define D2R 0.01745329252
#define TARGET_RADIUS 20
// GLOBAL VARIABLES and CONSTANTS
#define  START 0
#define  RUN   1
#define  DURATION 20

bool appState = false;
bool inStartButton = false;
bool activeTimer = false;
bool spacebar = false;
bool  fire = false;      // Flag to show if it is fired/active
bool collision = false;  // Flag to show if it hit the target
int  winWidth, winHeight; // Window width and height
int timeCounter = DURATION;

typedef struct {
	int r, g, b;
} color_t;

typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t ghXY;
	color_t color;
	bool isActive;
} ghost_t;

ghost_t ghost[5];
bool nextGhost = true;
int pcX = -270, pcY = 30; //initial coordinates of the pacman
int fireX = -280, fireY = 0; //initial coordinates of fire
int ghostSp = 8; //initial speed of the ghost
int score = 0;
int ghostCount = 0, ghostTimer = 0;

// Display text with variables
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 0.2);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	glPopMatrix();
}

// Display text with variables
void vprint(int x, int y, void* font, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, str[i]);
}

void circle(int x, int y, int r) {
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void pie_filled(float x, float y, float r, float start, float end) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (float angle = start; angle < end; angle += 3)
		glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
	glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
	glEnd();
}

// initializing the first ghost set
void initGhost()
{
	for (int i = 0; i < MAX_GHOST; i++)
	{
		ghost[i].isActive = false;
		ghost[i].color = { 150,150,150 };
		ghost[i].ghXY = { 215,-250 };
		ghost[i].isActive = false;
	}
}

// displaying the created ghosts
void displayGhost()
{
	if (ghostCount < MAX_GHOST && nextGhost)
	{
		int i = 0;
		while (ghost[i].isActive)
			i++;
		ghost[i].isActive = true;
		//randomly generated
		float xPos = rand() % 325;
		ghost[i].ghXY = { xPos, -300 };
		ghost[i].color.r = rand() % 256;
		ghost[i].color.g = rand() % 256;
		ghost[i].color.b = rand() % 256;

		ghostCount++;
		nextGhost = false;
	}
}

//start screen
void display_start() {
	glColor3ub(255, 255, 255);
	vprint2(-190, 240, 0.2, "00:00");
	vprint2(145, 240, 0.2, "-");
	vprint(25, -280, GLUT_BITMAP_9_BY_15, "Press F1 to Start a New Game");
}

//run screen
void display_run() {
	glColor3ub(255, 255, 255);
	vprint2(-190, 240, 0.2, "00:%02d", timeCounter);
	vprint2(145, 240, 0.2, "%d", score);
	vprint(50, -280, GLUT_BITMAP_9_BY_15, "Press F1 to Pause/Resume");
	vprint(-170, -310, GLUT_BITMAP_8_BY_13, "Press Up/Down Arrows to Move the Pacman");
	int width = (DURATION - timeCounter) * 300 / DURATION;

}

void drawPacman()
{
	// outer line
	glColor3f(0, 0, 0);
	pie_filled(-270, pcY - 30, 32, 20, 320);

	// tongue
	glBegin(GL_POLYGON);
	glColor3ub(255, 0, 0);
	glVertex2f(-270, pcY - 30);
	glVertex2f(-250, pcY - 40);
	glVertex2f(-250, pcY - 55);
	glVertex2f(-260, pcY - 40);
	glEnd();

	// pacman
	glColor3f(1, 1, 0);
	pie_filled(-270, pcY - 30, 30, 20, 320);

	// eyes
	glColor3ub(200, 200, 200);
	circle(-265, pcY - 15, 5);

	glColor3ub(255, 255, 255);
	circle(-265, pcY - 15, 4);

	glColor3ub(0, 0, 0);
	circle(-263, pcY - 15, 3);
}

void drawFires() {

	glColor3ub(250, 100, 0);
	circle(fireX, fireY, 6);

	glColor3ub(240, 140, 0);
	circle(fireX - 10, fireY, 4);

	glColor3ub(250, 200, 0);
	circle(fireX - 20, fireY, 2);

}

void drawGhost()
{
	for (int i = 0; i < MAX_GHOST; i++)
	{
		if (ghost[i].isActive)
		{	// body
			glColor3ub(ghost[i].color.r, ghost[i].color.g, ghost[i].color.b);
			circle(ghost[i].ghXY.x - 15, ghost[i].ghXY.y + 10, 20);
			glRectf(ghost[i].ghXY.x + 5, ghost[i].ghXY.y + 10, ghost[i].ghXY.x - 35, ghost[i].ghXY.y - 15);
			circle(ghost[i].ghXY.x - 15, ghost[i].ghXY.y - 15, 5);
			circle(ghost[i].ghXY.x, ghost[i].ghXY.y - 15, 5);
			circle(ghost[i].ghXY.x - 30, ghost[i].ghXY.y - 15, 5);

			// eyes
			glColor3ub(250, 250, 250);
			circle(ghost[i].ghXY.x - 25, ghost[i].ghXY.y + 15, 5);
			circle(ghost[i].ghXY.x - 10, ghost[i].ghXY.y + 15, 5);

			glColor3ub(0, 0, 100);
			circle(ghost[i].ghXY.x - 28, ghost[i].ghXY.y + 15, 3);
			circle(ghost[i].ghXY.x - 12, ghost[i].ghXY.y + 15, 3);
		}
	}
}

void display() {
	// Creating the window
	glClearColor(0.05, 0.02, 0.1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Screen
	glBegin(GL_POLYGON);
	glColor3ub(5, 10, 20);
	glVertex2f(325, 200);
	glColor3ub(5, 10, 20);
	glVertex2f(-325, 200);
	glColor3ub(40, 80, 150);
	glVertex2f(-325, -240);
	glColor3ub(40, 80, 150);
	glVertex2f(325, -240);
	glEnd();

	drawFires();
	// to cover up where the fire comes from 
	glBegin(GL_POLYGON);
	glColor3ub(5, 10, 20);
	glVertex2f(-260, 200);
	glColor3ub(5, 10, 20);
	glVertex2f(-325, 200);
	glColor3ub(40, 80, 150);
	glVertex2f(-325, -240);
	glColor3ub(40, 80, 150);
	glVertex2f(-260, -240);
	glEnd();

	drawGhost();
	drawPacman();

	glColor3ub(25, 10, 50);
	glRectf(-325, 200, 325, 600);
	glRectf(-325, -240, 325, -600);

	glEnd();
	glColor3ub(100, 100, 100);
	glRectf(-325, 200, 325, 210);
	glRectf(-325, -240, 325, -250);
	glEnd();

	glColor3ub(250, 250, 250);
	for (int x = -325; x <= 325; x += 20) {

		glRectf(x, 200, x + 10, 210);
		glRectf(x, -240, x + 10, -250);

	}

	glColor3ub(250, 250, 250);
	vprint(-220, 280, GLUT_BITMAP_9_BY_15, "Remaining Time:");
	vprint(100, 280, GLUT_BITMAP_9_BY_15, "Total Point:");

	vprint(-280, -280, GLUT_BITMAP_9_BY_15, "Press Sparebar to Fire");

	switch (appState)
	{
	case START:
		appState == 1;
		display_start();
		break;
	case RUN:
		appState == 2;
		display_run();
		break;
	}


	glutSwapBuffers();
}

void onResize(int w, int h) {
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // Refresh window.
}

void onKeyDown(unsigned char key, int x, int y) {

	// press space to fire
	if (key == ' ' && activeTimer)
	{
		fire = true;
		fireY = pcY - 30;
	}
	// To refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y) {

	// Exit when ESC is pressed
	if (key == 27)
		exit(0);

	// To refresh the window it calls display() function
	glutPostRedisplay();

}

void onSpecialKeyDown(int key, int x, int y) {

	// The game stops/starts
	if (key == GLUT_KEY_F1)
	{
		activeTimer = !activeTimer;

		if (timeCounter == DURATION)
			appState = !appState;
		else if (timeCounter == 0)
		{
			timeCounter = 20;
			appState = !appState;
		}
	}

	// Moving pacman
	if (key == GLUT_KEY_UP && appState == 1 && pcY < 195 && activeTimer) {
		pcY = pcY + 8;
	}
	if (key == GLUT_KEY_DOWN && appState == 1 && pcY > -175 && activeTimer) {
		pcY = pcY - 8;
	}

	// To refresh the window it calls display() function
	glutPostRedisplay();
}
#if TIMER_ON == 1
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// for the timer
	if (activeTimer)
	{
		if (timeCounter > 0)
		{
			timeCounter--;
		}
		// Time expires
		else if
			(timeCounter == 0)
		{
			activeTimer = !activeTimer;
			ghostSp = 8;
			appState = !appState;

			fireX = -280;
			score = 0;
			ghostCount = 0;
			ghostTimer = 0;
			nextGhost = true;
			initGhost();
		}
	}

	// To refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

#if TIMER_ON == 1
void onTimer2(int v) {
	glutTimerFunc(TIMER_PERIOD2, onTimer2, 0);
	if (activeTimer)
	{
		// When the remaining time is less than 5 seconds the ghosts will move faster
		if (timeCounter < 5 && timeCounter > 0)
			ghostSp = 12;
		if (timeCounter > 0)
		{
			displayGhost();

			for (int i = 0; i < MAX_GHOST; i++)
			{
				// Animate the ghosts
				ghost[i].ghXY.y += ghostSp;

				// collision detect
				//first part of the if checks the upper part of the ghost, the second part checks the lower part and the last part checks the middle
				if ((sqrtf(powf((fireY - ghost[i].ghXY.y - 10), 2) + powf((fireX - ghost[i].ghXY.x), 2)) < 26) || (sqrtf(powf((fireY - ghost[i].ghXY.y + 15), 2) + powf((fireX - ghost[i].ghXY.x + 15), 2)) < 11) || (fireX + 6 == ghost[i].ghXY.x + 5 && fireY< ghost[i].ghXY.y + 10 && fireY >ghost[i].ghXY.y + -15))
				{
					collision = !collision;
					ghost[i].ghXY.y = -260;
					// New target
					ghost[i].ghXY.x = rand() % 325; // Reset x-coordinate
					// Reset the colors
					ghost[i].color.r = rand() % 256;
					ghost[i].color.g = rand() % 256;
					ghost[i].color.b = rand() % 256;
				}

				//The objects start its movement again after reaching the end of the window
				if (ghost[i].ghXY.y > 325)
				{
					ghost[i].ghXY.y = -260;
					// New target
					ghost[i].ghXY.x = rand() % 325; // Reset x-coordinate
					// Reset the colors
					ghost[i].color.r = rand() % 256;
					ghost[i].color.g = rand() % 256;
					ghost[i].color.b = rand() % 256;
				}

				if (collision)
				{
					score++;
					collision = !collision;
				}

			}
			// timing the ghosts
			if (ghostTimer < 11)
			{
				ghostTimer = ghostTimer + 1.1;
			}
			else
			{
				nextGhost = true;
				ghostTimer = 0;
			}
		}
	}

	if (fire && activeTimer)
	{
		fireX = fireX + 10;
		//if the bullet reachs to the other side or hits the ghost it will be removed
		if (fireX > 325)
		{
			fire = !fire;
			fireX = -280;
			//fireY = pcY;
		}
	}
	else
	{
		fireY = pcY - 30;
	}

	// To refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif
void Init()
{
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("PACMAN SHOOTING GAME by Zeynep Arslan");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	// setting srand
	srand(time(NULL));

	// initializing the ghost array
	initGhost();

	// Keyboard Events
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(onSpecialKeyDown);

#if  TIMER_ON == 1
	// Timer Event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutTimerFunc(TIMER_PERIOD2, onTimer2, 0);
#endif

	Init();
	glutMainLoop();
	return(0);
}