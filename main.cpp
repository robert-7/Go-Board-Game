// OLD INCLUDES/USINGS
//#include <Windows.h>
//#include <GL\glew.h>
//#include <GL\freeglut.h>
//#include <iostream>

//using namespace std;

// NEW INCLUDES/USINGS

//#pragma comment(lib, "DevIL.lib")
//#pragma comment(lib, "ILU.lib")
//#pragma comment(lib, "ILUT.lib")

#include <iostream>
using namespace std;

//For Sleep in Windows
//#include <windows.h>
//For Sleep in Linux
#include <iostream>
#include <unistd.h>

// includes, graphics
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

ILuint il[3] = { 0, 1, 2 };
GLuint texture[3] = { 0, 1, 2 };
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// For the queue
#include <queue>
#include <list>
#include <vector>

#define GL_CLAMP_TO_EDGE 0x812F
typedef queue<int>  INTQUEUE;

// Window settings
int windowID;                // Glut window ID (for display)

							 // Camera methods
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

// Camera settings
bool updateCamZPos = false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.01;

GLdouble camXPos = 0.0;
GLdouble camYPos = 0.0;
GLdouble camZPos = -1.5;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP = 0.1;
const GLdouble FAR_CLIP = 1000.0;

// global variables
int wireframe = 0;
int lighting = 1;
int material = 1;
int pauseTeapot_y = 1;
int pauseTeapot_x = 1;
int pauseLighting = 0;
int depthTest = 1;
int cullFace = 1;
int smooth = 1;
int enabletexture = 0;
float angle_y = 0;
float angle_x = 45;
float angle2 = 0; // Useless
float translatelight = 0;

// Behind the Scenes variables
int placex = 0;
int placey = 0;
int col = 1;
float t = 0;
float inc = 0.002; // Depicts how fast time increments.

				   // Behind the Scenes
int board_status[19][19];
int liberties_status[19][19];
int restart_option = 0;
INTQUEUE rm_queue;
list< vector<int> > L;

//float rm_array[1083];     // Holds the objects that will be removed.
//int p = 0;                // Always points to the last index of rm_array.

// declaration, forward
void display();
void keyboard(unsigned char, int, int);
void init();
void idle();
void SetImage();
void DrawSphere(int color);
void DrawUnitCube(int color);
void ApplyTransformations(float indx, float indy, float z);
void ApplyTransformationGeneral(float indx, float indy, float z);

// Behind the Scenes
void init_board();
void clear_liberties();
int make_move(int x, int y, int piece);
int check_liberties(int x, int y, int originx, int originy, int piece);
void remove_block(int x, int y, int piece);
void jump_off(int x0, int y0, int color);

//! Program entry point
int main(int argc, char** argv) {

	// initialize GLUT
	glutInit(&argc, argv);
	// set visual
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// set physical window properties
	glutInitWindowSize(512, 512);
	// position window on the screen
	glutInitWindowPosition(100, 100);
	// create window
	windowID = glutCreateWindow("hello");

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	ilGenImages(2, il);
	glGenTextures(2, texture);

	// initialize OpenGL
	init();

	// register rendering mainloop
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMouseFunc(mouse);        // Call mouse whenever mouse button pressed
	glutMotionFunc(motion);      // Call motion whenever mouse moves while button pressed
								 // let's rock ...
	glutMainLoop();

	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	/*

	1 : Toggle specularity
	2 : Toggle lighting
	3 : Toggle wireframe
	4 : Toggle light animation
	5 : Toggle depth testing
	6 : Toggle culling (try disabling both depth testing and culling)
	7 : Toggle between smooth and flat shading
	8 : Toggle texture

	o,O : Toggle teapot rotation along the y-axis
	p,P : Toggle teapot rotation along the x-axis
	r,R : Activate "reset board option"
	y,Y : Confirm  "reset board option"
	n,N : Cancel   "reset board option"
	v,V : Set color to white (This makes testing a bit faster)
	b,B : Set color to black (This makes testing a bit faster)

	wasd  : Move a piece around the board
	enter : Place a piece on the board

	ON RIGHT CLICK AND HOLD MOUSE:
	slide left  : move camera left
	slide right : move camera right
	slide up    : zoom camera in
	slide down  : zoom camera out
	*/

	switch (key)
	{
	case '1':
		// Enable/disable wireframe mode
		wireframe = !wireframe;

		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		break;

	case '3':
		material = !material;

		break;

	case '5':
		depthTest = !depthTest;

		if (depthTest) {
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		break;

	case '6':
		cullFace = !cullFace;

		if (cullFace) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		break;

	case '7':
		smooth = !smooth;

		if (smooth) {
			glShadeModel(GL_SMOOTH);
		}
		else {
			glShadeModel(GL_FLAT);
		}

		break;

	case '8':
		enabletexture = !enabletexture;

		if (enabletexture) {
			glEnable(GL_TEXTURE_2D);
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}
		break;

	case 'o':
	case 'O':
		cout << "PRESSED O\n";
		pauseTeapot_y = !pauseTeapot_y;

		break;

	case 'p':
	case 'P':
		cout << "PRESSED P\n";
		pauseTeapot_x = !pauseTeapot_x;

		break;

	case 'a':
	case 'A':
		if (placex > -9) { placex -= 1; }

		break;

	case 'w':
	case 'W':
		if (placey < 9) { placey += 1; }

		break;
	case 's':
	case 'S':
		if (placey > -9) { placey -= 1; }

		break;
	case 'd':
	case 'D':
		if (placex < 9) { placex += 1; }

		break;

	case 13:
		if (board_status[placex + 9][placey + 9] != 0)
		{
			cout << "YOU CAN'T PLACE A PIECE HERE BECAUSE THERE ALREADY IS A PIECE HERE!!!\n";
		}
		else
		{
			cout << "ENTER KEY PRESSED!!!\n";
			make_move(placex + 9, placey + 9, col);
			if (col == 1) { col = 2; }
			else { col = 1; }
		}
		break;
	case 'r':
	case 'R':
		cout << "You pressed 'r', the restart button. Press 'y' to confirm restart. Press 'n' to cancel.\n";
		restart_option = 1;
		break;
	case 'n':
	case 'N':
		if (restart_option) {
			cout << "You pressed 'n'. Restart option cancelled.\n";
			restart_option = 0;
		}
		break;
	case 'y':
	case 'Y':
		if (restart_option) {
			cout << "You pressed 'y'. The game has been restarted.\n";
			init_board();
			col = 1;
		}
		break;

	case 'b':
	case 'B':
		col = 2; break;
	case 'v':
	case 'V':
		col = 1; break;

	}
}

// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if (updateCamZPos)
	{
		// Update camera z position
		camZPos += (y - lastY) * ZOOM_SCALE;
		camXPos += (x - lastX) * ZOOM_SCALE;
		lastX = x;
		lastY = y;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}

// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			lastX = x;
			lastY = y;
			updateCamZPos = true;
		}
		else
		{
			updateCamZPos = false;
		}
	}
}

void printMatrix()
{
	float matrixData[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, matrixData);

	cout << endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << matrixData[i + 4 * j] << ", ";
		}

		cout << endl;
	}
}

void idle()
{

	if (!pauseTeapot_y) {
		angle_y = angle_y + 0.5;

		if (angle_y > 360) {
			angle_y = angle_y - 360;
		}
	}

	if (!pauseTeapot_x) {
		angle_x = angle_x + 0.5;

		if (angle_x > 360) {
			angle_x = angle_x - 360;
		}
	}

	angle2 = angle2 + 0.5;

	if (angle2 > 360) {
		angle2 = angle2 - 360;
	}

	/*
	if (!pauseLighting) {
	translatelight = translatelight + 0.5;

	if (translatelight > 360) {
	translatelight = translatelight - 360;
	}
	}
	*/

	glutPostRedisplay();
}


void lightingFunc()
{

	// Specify light position
	GLfloat lightposition[4] = { 0.0, 0.0, -0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

	// Specify diffuse component.  Diffuse component is white.
	//GLfloat lightdiffuse[4] = {0.0, 0.0, 0.0, 1.0};
	GLfloat lightstrength[4] = { 2.0, 2.0, 2.0, 2.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightstrength);

	// Specify specular component.  Specular component is green.
	//GLfloat lightspecular[4] = {0.0, 0.0, 0.0, 1.0};
	GLfloat lightspecular[4] = { 0.0, 2.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightstrength);

	GLfloat spread = 90 - 90 * t;
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &spread);

	glEnable(GL_LIGHT0);
}

void materialFunc()
{
	// Add a diffuse component to our teapot.  The diffuse reflection constant
	// is white (white light source produces white reflection).
	GLfloat materialdiffuse[4] = { 0.5, 0.5, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialdiffuse);

	// Add a specular component to our teapot.  The specular reflection constant 
	// is white (white light source produces white reflection).
	GLfloat materialspecular[4] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialspecular);

	// Defines the shinyness (the exponent to the phong model).
	GLfloat materialshininess[1] = { 100.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialshininess);
}


void DrawSphere(int color)
{
	float colorNone[4] = { 0.0, 0.0, 0.0, 0.0 };

	if (color == 0)
	{
		float colorRed[4] = { 1.0, 0.0, 0.0, 0.0 };
		glColor4fv(colorRed);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorRed);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}

	if (color == 1)
	{
		float colorWhite[4] = { 1.0, 1.0, 1.0, 1.0 };
		glColor4fv(colorWhite);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorWhite);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}

	if (color == 2)
	{
		float colorBlack[4] = { 0.0, 0.0, 0.0, 0.0 };
		glColor4fv(colorBlack);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBlack);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}

	if (color == 3)
	{
		float colorGreen[4] = { 0.0, 0.0, 1.0, 0.0 };
		glColor4fv(colorGreen);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorGreen);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}

	if (color == 4)
	{
		float colorBlue[4] = { 0.0, 1.0, 0.0, 0.0 };
		glColor4fv(colorBlue);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBlue);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}



	glBegin(GL_TRIANGLES);
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();                    // Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);     // Create Smooth Normals
	gluQuadricTexture(quadratic, GL_TRUE);        // Create Texture Coords

	gluSphere(quadratic, 1.0f, 32, 32);
	glEnd();
}

void DrawUnitCube(int color)
{
	if (color == 0) { glBindTexture(GL_TEXTURE_2D, texture[0]); }
	else if (color == 3)
	{
		float colorNone[4] = { 0.0, 0.0, 0.0, 0.0 };
		float colorGreen[4] = { 0.0, 0.0, 1.0, 0.0 };
		glColor4fv(colorGreen);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorGreen);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}
	else if (color == 4)
	{
		float colorNone[4] = { 0.0, 0.0, 0.0, 0.0 };
		float colorBlue[4] = { 0.0, 1.0, 0.0, 0.0 };
		glColor4fv(colorBlue);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colorBlue);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colorNone);
	}

	glBegin(GL_QUADS);

	// Front Face
	glNormal3f(0.0f, 0.0f, 2.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glNormal3f(0.0f, 0.0f, -2.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	// Bottom Face
	glNormal3f(0.0f, -2.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glNormal3f(2.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	// Left Face
	glNormal3f(-2.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);

	if (color == 0)
	{
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glBegin(GL_QUADS);
	}

	// Top Face
	glNormal3f(0.0f, 2.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);

	glEnd();

	// Windows
	//Sleep(3000);
	// Linux
	usleep(3000);
	
}

void display()
{
	// clear screen to background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw a white quad
	glColor4f(1.0, 1.0, 1.0, 1.0);

	// Push lighting bit; we'll pop it later so that the lighting state
	// isn't saved onto the next frame.  We do this so that we can disable
	// lighting more easily.
	glPushAttrib(GL_LIGHTING_BIT);

	glLoadIdentity();

	// Specify camera transformation
	glTranslatef(camXPos, camYPos, camZPos);

	// Specify the lighting we'll be using for this app.  Note that
	// the lights can be transformed using the usual translate, rotate,
	// and scale commands.
	glPushMatrix();

	//////////////////// MOVING LIGHT!!!!! ////////////////////
	glTranslatef(2 * sin(translatelight * 2 * 3.14 / 360), 0.0, 0.0);

	if (lighting) {
		lightingFunc();
	}

	glPopMatrix();


	glTranslatef(0.0, 0.0, -2.0);      // Move objects to viewing area.
	glRotatef(angle_x, 1.0, 0.0, 0.0); // To allow rotation on the x-axis
	glRotatef(angle_y, 0.0, 1.0, 0.0); // To allow rotation on the y-axis
	glPushMatrix();

	glScalef(1.0, 0.05, 1.0); // Flatten the Cube. 
							  // It should look like a board now.

							  // Specify material for the teapot we'll be drawing.
	if (material) {
		materialFunc();
	}

	DrawUnitCube(0);


	glPopMatrix();
	glPushMatrix();
	// Draw Beads

	glBindTexture(GL_TEXTURE_2D, 0);

	glScalef(0.0525, 0.0525, 0.0525); // Just the right size :)
	glTranslatef(0.0, 1.0, 0.0);      // The centre


	glPushMatrix();
	ApplyTransformations(placex, placey, 0);
	DrawSphere(0);
	glPopMatrix();

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if (board_status[i][j] != 0) {
				glPushMatrix();
				ApplyTransformations(i - 9, j - 9, 0);
				DrawSphere(board_status[i][j]);
				glPopMatrix();
			}
		}
	}

	// THE REMOVAL OF PIECE PORTION
	if (!L.empty()) {

		for (list<vector<int> >::iterator i = L.begin(); i != L.end(); ++i) {
			jump_off((*i)[0], (*i)[1], (*i)[2]);
		}
		t += (inc*L.size());
	}
	if (t > 1) {
		t = 0;
		L.clear();
	}

	glPopMatrix();
	glPopAttrib();

	// immediately process commands
	glFlush();

	// immediately call display loop again
	glutSwapBuffers();
}

void ApplyTransformations(float indx, float indy, float z)
{
	int i;
	if (indx < 0)
	{
		for (i = 0; i<abs(indx); i++)
		{
			glTranslatef(-2.0, 0.0, 0.0);
		}
	}
	else
	{
		for (i = 0; i<indx; i++)
		{
			glTranslatef(2.0, 0.0, 0.0);
		}
	}
	if (indy < 0)
	{
		for (i = 0; i<abs(indy); i++)
		{
			glTranslatef(0.0, 0.0, 2.0);
		}
	}
	else
	{
		for (i = 0; i<indy; i++)
		{
			glTranslatef(0.0, 0.0, -2.0);
		}
	}
}

/*
We can probably get rid of the first method,
but I don't feel like doing that.
*/

void ApplyTransformationGeneral(float indx, float indy, float z)
{
	glTranslatef(2 * indx, 10 * indy, 2 * z);
}

void SetImages()
{

	///////////////////// LOAD WOODEN TEXTURE /////////////////////
	ilBindImage(il[0]);
	ilLoadImage("images/wooden.jpg");
	ilConvertImage(IL_RGB, IL_UNSIGNED_SHORT);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		0,
		GL_RGB,
		GL_UNSIGNED_SHORT,
		ilGetData());


	///////////////////// LOAD CHECKERBOARD TEXTURE /////////////////
	ilBindImage(il[1]);
	ilLoadImage("images/checkerboard.png");
	ilConvertImage(IL_RGB, IL_UNSIGNED_SHORT);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		0,
		GL_RGB,
		GL_UNSIGNED_SHORT,
		ilGetData());


	glEnable(GL_NORMALIZE);

}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// BEHIND THE SCENES /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void init_board() {

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			board_status[i][j] = 0;
		}
	}
}

void clear_liberties() {
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			liberties_status[i][j] = -1;
		}
	}
}

int check_liberties(int x, int y, int originx, int originy, int piece) {
	if (x < 0 || x > 18 || y < 0 || y > 18) {
		return 0;
	}
	if (board_status[x][y] == 0) {
		return 1;
	}
	if (board_status[x][y] != piece) {
		return 0;
	}
	if (liberties_status[x][y] != -1) {
		return liberties_status[x][y];
	}
	int has_liberties = 0;
	int direcx = x - originx;
	int direcy = y - originy;
	if ((direcx >= 0 && check_liberties(x + 1, y, originx, originy, piece)) ||
		(direcx <= 0 && check_liberties(x - 1, y, originx, originy, piece)) ||
		(direcy >= 0 && check_liberties(x, y + 1, originx, originy, piece)) ||
		(direcy <= 0 && check_liberties(x, y - 1, originx, originy, piece))) {
		liberties_status[x][y] = 1;
	}
	else {
		liberties_status[x][y] = 0;
	}
	return liberties_status[x][y];
}

void jump_off(int x0, int z0, int color) {
	// For now, let's just assume they're all jumping to point (-4,0,-4)

	int x1 = -4;            float xt = x0*(1 - t) + x1*t;
	int y0 = 9;            float yt = -y0*pow(t, 2) + y0*t + y0;
	int z1 = -4;            float zt = z0*(1 - t) + z1*t;

	glPushMatrix();
	ApplyTransformationGeneral((xt - 9), (yt - 9), -(zt - 9));
	DrawSphere(color);

	// Draw Thigh
	glPushMatrix();
	if (t < 0.1) { glRotatef(-80 + 80 * sinf(t * 10 * M_PI), 1.0, 0.0, 1.0); }
	else { glRotatef(-80, 1.0, 0.0, 1.0); }
	glScalef(2.0 / 5.0, 1.0, 2.0 / 5.0);
	glTranslatef(0.0, -1.0, 0.0);
	DrawUnitCube(3);
	glScalef(5.0 / 2.0, 1.0, 5.0 / 2.0);

	// Draw Shin
	glPushMatrix();

	glTranslatef(0, -0.8, 0);
	if (t < 0.1) { glRotatef(90 - 90 * sinf(t * 10 * M_PI), 1.0, 0.0, 1.0); }
	else { glRotatef(90, 1.0, 0.0, 1.0); }
	glScalef(1.0 / 5.0, 1.0, 1.0 / 5.0);
	glTranslatef(0, -1.0, 0);
	DrawUnitCube(4);
	glScalef(5.0, 1.0, 5.0);

	// Draw Ankle
	glPushMatrix();
	glTranslatef(0, -1.0, 0);
	if (t < 0.1) { glRotatef(-100 + 100 * sinf(t * 10 * M_PI), 1.0, 0.0, 1.0); }
	else { glRotatef(-100, 1.0, 0.0, 1.0); }
	glScalef(1.0 / 2.0, 2.0 / 3.0, 1.0 / 2.0);
	glTranslatef(0, -1.0, 0);
	DrawSphere(0);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void remove_block(int x, int y, int piece) {
	if (board_status[x][y] == piece) {
		board_status[x][y] = 0;
		cout << "Jump from "; cout << x; cout << y; cout << '\n';

		//     rm_queue.push(x);
		//     rm_queue.push(y);
		//     rm_queue.push(piece);

		vector<int> rm_piece;
		rm_piece.push_back(x);
		rm_piece.push_back(y);
		rm_piece.push_back(piece);
		L.push_back(rm_piece);

		remove_block(x - 1, y, piece);
		remove_block(x + 1, y, piece);
		remove_block(x, y - 1, piece);
		remove_block(x, y + 1, piece);
	}

}

int make_move(int x, int y, int piece) {
	board_status[x][y] = piece;
	clear_liberties();
	int other;
	if (piece == 1) {
		other = 2;
	}
	else {
		other = 1;
	}
	int has_liberties = 0;
	if (x > 0 && board_status[x - 1][y] != piece) {
		if (piece == 0) {
			has_liberties = 1;
		}
		else if (!check_liberties(x - 1, y, x - 1, y, other)) {
			has_liberties = 1;
			remove_block(x - 1, y, other);
		}
	}
	if (x < 18 && board_status[x + 1][y] != piece) {
		if (piece == 0) {
			has_liberties = 1;
		}
		else if (!check_liberties(x + 1, y, x + 1, y, other)) {
			has_liberties = 1;
			remove_block(x + 1, y, other);
		}
	}
	if (y > 0 && board_status[x][y - 1] != piece) {
		if (piece == 0) {
			has_liberties = 1;
		}
		else if (!check_liberties(x, y - 1, x, y - 1, other)) {
			has_liberties = 1;
			remove_block(x, y - 1, other);
		}
	}
	if (y < 18 && board_status[x][y + 1] != piece) {
		if (piece == 0) {
			has_liberties = 1;
		}
		else if (!check_liberties(x, y + 1, x, y + 1, other)) {
			has_liberties = 1;
			remove_block(x, y + 1, other);

		}
	}
	if (!has_liberties && !check_liberties(x, y, x, y, piece)) {
		remove_block(x, y, piece);
		return 0;
	}
	return 1;
}

void init()
{
	// select clearing color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// initialize projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double fovY = 45, aspect = 1.0, zNear = 0.1, zFar = 100;

	gluPerspective(fovY,    // Field of view
		aspect,  // Aspect ratio (width / height)
		zNear,	 // Near plane
		zFar);	 // Far plane

				 // initialize modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);   // enable lighting
	glEnable(GL_CULL_FACE);  // enable backface culling
	glEnable(GL_DEPTH_TEST); // enable depth testing
							 //glEnable(GL_TEXTURE_2D); // enable textures

	SetImages();

	init_board(); // Create the "Board"
}