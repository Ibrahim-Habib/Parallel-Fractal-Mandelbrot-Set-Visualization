#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

void OnDisplay();
/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
*/
void InitGraphics(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	//Create an 800x600 window with its top-left corner at pixel (100, 100)
	glutInitWindowPosition(100, 100); //pass (-1, -1) for Window-Manager defaults
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL Lab");
	//OnDisplay will handle the paint event
	glutDisplayFunc(OnDisplay);
	glutMainLoop();
}
/**
Sets the logical coordinate system we will use to specify
our drawings.
*/
void SetTransformations() {
	//set up the logical coordinate system of the window: [-100, 100] x [-100, 100]
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-100, 100, -100, 100);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}
/**
Handles the paint event. This event is triggered whenever
our displayed graphics are lost or out-of-date.
ALL rendering code should be written here.
*/
void OnDisplay() {
	//set the background color to white
	glClearColor(1, 1, 1, 1);
	//fill the whole color buffer with the clear color
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	//ALL drawing code goes here
	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(-50, -50, 0);
	glVertex3f(50, -50, 0);
	glVertex3f(0, 50, 0);
	glEnd();
	//force previously issued OpenGL commands to begin execution
	glFlush();
}

int main(int argc, char* argv[])
{
	InitGraphics(argc, argv);
	return 0;
}