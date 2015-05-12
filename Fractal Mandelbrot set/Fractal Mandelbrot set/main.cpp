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

#include "mpi.h"

using namespace std;

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")


int num_of_processes;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024
#define minX -2
#define minY -2
#define maxX 2
#define maxY 2
//#define maxZoomLevel 100000
//#define minZoomLevel 0.1

int *fractal_area_depth;
void InitGraphics(int argc, char *argv[]);

double zoomLevel=1.0;
double centerX, centerY;

int DEPTH_THRESHOLD=50;

void keyPressed(unsigned char key, int x, int y)
{
	if(key == '+')
		zoomLevel += 0.1 * zoomLevel;
	if(key == '-')
		zoomLevel -= 0.1 * zoomLevel;
	if(key == 'r')
		DEPTH_THRESHOLD++;
	if(key == 't')
		DEPTH_THRESHOLD--;
	//validate that the zoomLevel is within the range
	//zoomLevel = min(zoomLevel, maxZoomLevel);
	//zoomLevel = max(zoomLevel, minZoomLevel);
	
	//double newCenterX = centerX, newCenterY = centerY;
	if(key == 'w')
		centerY += 0.1 * (1 / zoomLevel);
	if(key == 's')
		centerY -= 0.1 * (1 / zoomLevel);
	if(key == 'a')
		centerX -= 0.1 * (1 / zoomLevel);	
	if(key == 'd')
		centerX += 0.1 * (1 / zoomLevel);

	//if(newCenterX - 2 * (1 / zoomLevel) >= minX && newCenterX + 2 * (1 / zoomLevel) <= maxX && newCenterY - 2 * (1 / zoomLevel) >= minY && newCenterY + 2 * (1 / zoomLevel) <= maxY)
	//{
	//	centerX = newCenterX;
	//	centerY = newCenterY;
	//}

}


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

void updateFractals(int machineID, int *slave_buffer)
{
	double params[4]; 
	// 0: CENTERX
	// 1: CENTERY
	// 2: ZOOM_LVL
	// 3: DEPTH_THRESHOLD
	complex<double> center(params[0],params[1]);
	double zoomLevel=params[2];
	int depth_thres=params[3];

	MPI_Recv(&params, 4, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	int fraction_size = (WINDOW_HEIGHT*WINDOW_WIDTH)/(num_of_processes-1);
	int start_pos = (machineID-1) * fraction_size;

#pragma omp parallel for num_threads(16)
	for(int i=start_pos;i<start_pos+fraction_size;++i)
	{
		int y = i/WINDOW_WIDTH;
		int x = i%WINDOW_WIDTH;
		int depth = f(center+complex<double>((1 / zoomLevel) * (x-WINDOW_WIDTH/2)/(WINDOW_WIDTH/4.0), (1 / zoomLevel) * (y-WINDOW_HEIGHT/2)/(WINDOW_HEIGHT/4.0)));
		slave_buffer[i-start_pos]= (depth<depth_thres) ? depth : 0;
	}
	MPI_Send(slave_buffer, fraction_size, MPI_INT, 0, machineID, MPI_COMM_WORLD);
}


void OnDisplay() 
{
	//Sends a starting signal (current area of interest)to the slaves for the current frame computation
	double params[4];
	params[0] = centerX;// 0: CENTERX
	params[1] = centerY;// 1: CENTERY
	params[2] = zoomLevel;// 2: ZOOM_LVL
	params[3] = DEPTH_THRESHOLD;// 3: DEPTH_THRESHOLD
	double start_time = omp_get_wtime();

	for(int i= 1 ; i<num_of_processes ; i++)
		MPI_Send(params, 4, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SetTransformations();
	glBegin(GL_POINTS);

	int fraction_size = (WINDOW_HEIGHT*WINDOW_WIDTH)/(num_of_processes-1);
	for(int i= 1 ; i<num_of_processes ; i++)
		MPI_Recv((fractal_area_depth+(i-1)*fraction_size), fraction_size, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	char fpsString[128];
	sprintf(fpsString, "%f fps, Zoom Level:%fx, Center-x=%f, Center-Y=%f, DepthResolution=%d", 1/(omp_get_wtime()-start_time), zoomLevel , centerX , centerY,DEPTH_THRESHOLD);
	glutSetWindowTitle(fpsString);

	for(int i = 0 ; i<WINDOW_HEIGHT ; i++)
		for(int j = 0 ; j<WINDOW_WIDTH ; j++)
			if(fractal_area_depth[i*WINDOW_WIDTH+j])
			{
				double colorShade = (double)fractal_area_depth[i*WINDOW_WIDTH+j]/(double)DEPTH_THRESHOLD;
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
	glutCreateWindow("Parallel Mandelbrot Set Fractals Visualization");
	glutDisplayFunc(OnDisplay);
	glutKeyboardFunc(keyPressed);
	printf("funcitons registered\n");
	glutMainLoop();
}


int main(int argc, char* argv[]) 
{
	int myId;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);	
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	if(myId == 0)
	{
		printf("%d Processes\n", num_of_processes);
		if(num_of_processes < 2 )
		{
			printf("A Master needs at least one slave to satisfy him!\n");
			return 0;
		}
		else if(((num_of_processes-2) & (num_of_processes-1)) != 0)
		{
			printf("A Master needs a slave of power of 2 to divide the data evenly!\n");
			return 0;
		}
		else
		{
			fractal_area_depth = new int[WINDOW_HEIGHT*WINDOW_WIDTH];
			InitGraphics(argc, argv);
		}
	}
	else
	{
		int *slave_buffer = new int[(WINDOW_HEIGHT*WINDOW_WIDTH)/(num_of_processes-1)];
		while(1)
			updateFractals(myId, slave_buffer);
	}
	MPI_Finalize();
}