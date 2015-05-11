#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <cstdio>
#include <vector>
#include <omp.h>
#include <amp.h>

using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

void InitGraphics(int argc, char *argv[]);
void SetTransformations() 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-200, 200, -200, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
}


void OnDisplay() 
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(-50, -50, 0);
	glVertex3f(50, -50, 0);
	glVertex3f(0, 50, 0);
	glEnd();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void InitGraphics(int argc, char *argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100); 
	glutInitWindowSize(800, 800);
	glutCreateWindow("Mandelbrot Set");
	glutDisplayFunc(OnDisplay);
	
	glutMainLoop();
}


int main(int argc, char* argv[]) 
{
	InitGraphics(argc, argv);
}