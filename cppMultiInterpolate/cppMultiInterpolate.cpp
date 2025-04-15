#include <iostream>
#include <ctime>
#include <GL/freeglut.h>
#include "mInterpolator.h"

double chartYB = 0, chartYT = 0, chartXL = 0, chartXR = 0;

pVec randData = {};

pVec basePts = {};
pVec linPts = {};
pVec lagPts = {};
pVec nwtPts = {};
pVec splPts = {};
pVec lslPts = {};
pVec lscPts = {};

void inGLDRawRect(double xc, double yc, double yCoeff, double r)
{
	glBegin(GL_QUADS);

	glVertex2d(xc - r, yc - r*yCoeff);
	glVertex2d(xc + r, yc - r*yCoeff);
	glVertex2d(xc + r, yc + r*yCoeff);
	glVertex2d(xc - r, yc + r*yCoeff);

	glEnd();
}

void inGLChartVector(pVec vec)
{
	int l = vec.size();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < l; i++)
	{
		glVertex2d(vec[i].x,vec[i].y);
	}
	glEnd();
}

void cbDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(chartXL, chartXR, chartYB, chartYT);

	double yPropC = (chartYT - chartYB) / (chartXR - chartXL);
	double sqSize = (chartXR - chartXL) / (4 * basePts.size());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(1);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < basePts.size(); i++)
	{
		inGLDRawRect(basePts[i].x, basePts[i].y, yPropC, sqSize);
	}

	glLineWidth(3);

	glColor3f(0.0f, 0.0f, 1.0f);
	inGLChartVector(linPts);

	glColor3f(0.0f, 1.0f, 0.0f);
	inGLChartVector(lagPts);

	glColor3f(0.0f, 0.0f, 1.0f);
	inGLChartVector(nwtPts);

	glColor3f(1.0f, 1.0f, 0.0f);
	inGLChartVector(splPts);

	glColor3f(0.0f, 1.0f, 1.0f);
	inGLChartVector(lslPts);

	glColor3f(1.0f, 0.0f, 1.0f);
	inGLChartVector(lscPts);

	glutSwapBuffers();
}

void cbReshape(int w, int h)
{
	glViewport(0,0,w,h);
}


int main(int argc, char **argv)
{
	srand(time(NULL));

	bool useRandData = true;
	int useBaseFunc = 2; //0 - pure noise, 1 - linear noised, 2 - parabola

	double x1 = -10, x2 = 10;
	int res = 20;
	int nRes = 100;

	if (useRandData)
	{
		randData.resize(res + 1);
		if (useBaseFunc==0)
		{
			for (int i = 0; i < randData.size(); i++)
			{
				randData[i].x = x1 + i * (x2 - x1) / res;
				randData[i].y = rand() % 1001 - 500;
			}
		}
		if (useBaseFunc==1)
		{
			for (int i = 0; i < randData.size(); i++)
			{
				float cx = x1 + i * (x2 - x1) / res;
				float cy = 10 * cx + 3 + (100*((float)rand()) / RAND_MAX) - 50;
				randData[i].x = cx;
				randData[i].y = cy;
			}
		}
		if (useBaseFunc == 2)
		{
			for (int i = 0; i < randData.size(); i++)
			{
				float cx = x1 + i * (x2 - x1) / res;
				float cy = pow(cx, 2) +(100 * ((float)rand()) / RAND_MAX) - 50;
				randData[i].x = cx;
				randData[i].y = cy;
			}
		}
	}

	//interpolate stuff
	mInterpLSLinear * linInt;
	std::cout << "Linear...\n";
	if (useRandData) linInt = new mInterpLSLinear(x1, x2, res, nRes, randData); else
	linInt = new mInterpLSLinear(x1,x2,res,nRes);
	//linInt.printDataToCSV("linInterpOutput.csv");

	mInterpLagrange * lagInt;
	std::cout << "Lagrange...\n";
	if (useRandData) lagInt = new mInterpLagrange(x1, x2, res, nRes, randData); else
		lagInt = new mInterpLagrange(x1, x2, res, nRes);
	//lagInt.printDataToCSV("lagInterpOutput.csv");

	mInterpNewton * nwtInt;
	std::cout << "Newton...\n";
	if (useRandData)  nwtInt = new mInterpNewton(x1, x2, res, nRes, randData); else
		nwtInt = new mInterpNewton(x1, x2, res, nRes);
	//nwtInt.printDataToCSV("newtInterpOutput.csv");

	mInterpSpline * splInt;
	std::cout << "Spline...\n";
	if (useRandData) splInt =  new mInterpSpline(x1, x2, res, nRes, randData); else
		splInt = new mInterpSpline(x1, x2, res, nRes);
	//splInt.printDataToCSV("splInterpOutput.csv");


	mInterpLSLinear * lslInt;
	std::cout << "Least squares linear...\n";
	if (useRandData) lslInt = new mInterpLSLinear(x1, x2, res, nRes, randData); else
		lslInt = new mInterpLSLinear(x1, x2, res, nRes);

	mInterpLSCubic * lscInt;
	std::cout << "Least squares linear...\n";
	if (useRandData) lscInt = new mInterpLSCubic(x1, x2, res, nRes, randData); else
		lscInt = new mInterpLSCubic(x1, x2, res, nRes);

	std::cout << "All done, rendering...\n";

	//transfer data outside of calss objects
	basePts = linInt->basePoints;
	linPts = linInt->interpPoints;
	lagPts = lagInt->interpPoints;
	nwtPts = nwtInt->interpPoints;
	splPts = splInt->interpPoints;
	lslPts = lslInt->interpPoints;
	lscPts = lscInt->interpPoints;
	
	//discover render area
	lagInt->getDataArea(chartXL, chartXR, chartYB, chartYT, true);

	//render stuff
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Chart render");
	glClearColor(0,0,0,1);

	glutDisplayFunc(cbDisplay);
	glutReshapeFunc(cbReshape);

	glutMainLoop();

	return 1;
}
