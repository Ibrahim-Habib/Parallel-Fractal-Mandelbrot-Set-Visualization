#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <iostream>
#include <math.h>
#include <complex>
#include <cstdio>
#include <vector>
#include <omp.h>
#include <amp.h>

using namespace std;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

int fractal_area_depth[WINDOW_HEIGHT][WINDOW_WIDTH];

void InitGraphics(int argc, char *argv[]);
double zoom=1.0;
int DEPTH_THRESHOLD=30;

int f(complex<double>c)
{
	complex<double>z = c;
	int depth=1;
	while(z.real()*z.real()+z.imag()*z.imag() <= 4)
	{
		depth++;
		z = z*z + c;
		if(depth==DEPTH_THRESHOLD)break;
	}
	return depth;
}

void SetTransformations() 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-WINDOW_WIDTH/2.0, WINDOW_WIDTH/2.0, -WINDOW_HEIGHT/2.0, WINDOW_HEIGHT/2.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
}

void updateFractals()
{
	memset(fractal_area_depth,0,sizeof(fractal_area_depth));
	double t = omp_get_wtime();
	for(int i = 0 ; i<WINDOW_HEIGHT ; i++)
		for(int j = 0 ; j<WINDOW_WIDTH ; j++)
		{
			int d = f(complex<double>((j-WINDOW_WIDTH/2)/256.0,(i-WINDOW_HEIGHT/2)/256.0));
			fractal_area_depth[i][j]= (d<DEPTH_THRESHOLD) ? d : 0;
		}
		char fpsString[128];
		sprintf_s(fpsString, "%f fps", 1/(omp_get_wtime()-t));
		glutSetWindowTitle(fpsString);			
}


void OnDisplay() 
{
	updateFractals();

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	glBegin(GL_POINTS);

	for(int i = 0 ; i<WINDOW_HEIGHT ; i++)
		for(int j = 0 ; j<WINDOW_WIDTH ; j++)
			if(fractal_area_depth[i][j])
			{
				double colorShade = (double)fractal_area_depth[i][j]/(double)DEPTH_THRESHOLD;
				glColor3d(colorShade,0,colorShade);
				glVertex2d(j-WINDOW_WIDTH/2, i-WINDOW_HEIGHT/2);
			}
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
	glutCreateWindow("Mandelbrot Fractals Visualization");
	glutDisplayFunc(OnDisplay);

	glutMainLoop();
}


int main(int argc, char* argv[]) 
{
	InitGraphics(argc, argv);
}