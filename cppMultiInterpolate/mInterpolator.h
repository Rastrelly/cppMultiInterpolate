#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

struct mPoint
{
	double x, y;
};

typedef std::vector<mPoint> pVec;

class mInterpolator
{
private:
	double x1, x2, step;
	double bYMin, bYMax, iYMin, iYMax;
	int res;
	int newRes;
	void buildFuncData();
	double mathFunction(double x);
	std::string forceDecSep(double val, char forcedDecSep);
public:
	pVec basePoints, interpPoints;
	void getDataArea(double &xMin, double &xMax, double &yMin, double &yMax, bool clambToBase);
	void printDataToCSV(std::string fileName);
	int getRes() { return res; }
	int getNewRes() { return newRes; }
	double getX1() { return x1; }
	double getX2() { return x2; }
	double getStep() { return step; }
	void setNewRes(int val) { newRes = val; }
	void getNeighbourPoints(double x, mPoint &np1, mPoint &np2);
	mInterpolator(double vx1, double vx2, int vres, int vnres);
	virtual void callInterpolate() = 0;
};

//linear interpolator class
class mInterpLinear : public mInterpolator
{
public:
	double linterp(mPoint p1, mPoint p2, double px);
	void callInterpolate();
	mInterpLinear(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		callInterpolate();
	}
};

//lagrange interpolator class
class mInterpLagrange : public mInterpolator
{
public:
	double lagrangeXY(double sx);
	void callInterpolate();
	mInterpLagrange(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		callInterpolate();
	}
};