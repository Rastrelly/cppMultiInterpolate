#include <iostream>
#include <GL/freeglut.h>
#include "mInterpolator.h"

double chartYB = 0, chartYT = 0, chartXL = 0, chartXR = 0;
pVec basePts = {};
pVec linPts = {};
pVec lagPts = {};
pVec nwtPts = {};
pVec splPts = {};

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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(3);

	glColor3f(0.0f, 0.0f, 1.0f);
	inGLChartVector(linPts);

	glColor3f(0.0f, 1.0f, 0.0f);
	inGLChartVector(lagPts);

	glColor3f(0.0f, 0.0f, 1.0f);
	inGLChartVector(nwtPts);

	glColor3f(1.0f, 1.0f, 0.0f);
	inGLChartVector(splPts);

	glLineWidth(1);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < basePts.size(); i++)
	{
		inGLDRawRect(basePts[i].x,basePts[i].y, yPropC, 1);
	}

	glutSwapBuffers();
}

void cbReshape(int w, int h)
{
	glViewport(0,0,w,h);
}


int main(int argc, char **argv)
{

	double x1 = -360, x2 = 360;
	int res = 100;
	int nRes = 1000;

	//interpolate stuff
	std::cout << "Linear...\n";
	mInterpLinear linInt(x1,x2,res,nRes);
	//linInt.printDataToCSV("linInterpOutput.csv");

	std::cout << "Lagrange...\n";
	mInterpLagrange lagInt(x1, x2, res, nRes);
	//lagInt.printDataToCSV("lagInterpOutput.csv");

	std::cout << "Newton...\n";
	mInterpNewton nwtInt(x1, x2, res, nRes);
	//nwtInt.printDataToCSV("newtInterpOutput.csv");

	std::cout << "Spline...\n";
	mInterpSpline splInt(x1, x2, res, nRes);
	//splInt.printDataToCSV("splInterpOutput.csv");

	std::cout << "All done, rendering...\n";

	//transfer data outside of calss objects
	basePts = linInt.basePoints;
	linPts = linInt.interpPoints;
	lagPts = lagInt.interpPoints;
	nwtPts = nwtInt.interpPoints;
	splPts = splInt.interpPoints;
	
	//discover render area
	lagInt.getDataArea(chartXL, chartXR, chartYB, chartYT, true);

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
