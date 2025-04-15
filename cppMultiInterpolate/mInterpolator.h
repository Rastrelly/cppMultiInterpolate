#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/math/interpolators/cardinal_cubic_b_spline.hpp>

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
	mInterpolator(double vx1, double vx2, int vres, int vnres, pVec givenData);
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
	mInterpLinear(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
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
	mInterpLagrange(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
	{
		callInterpolate();
	}
};

//newton interpolator class
class mInterpNewton : public mInterpolator
{

public:
	double newtonInterp(double sx);
	void callInterpolate();
	mInterpNewton(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		callInterpolate();
	}
	mInterpNewton(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
	{
		callInterpolate();
	}
};

//newton interpolator class
class mInterpSpline : public mInterpolator
{
private:
	boost::math::interpolators::cardinal_cubic_b_spline<double> *spline;
public:
	void prepareSpline();
	double splineInterp(double sx);
	void callInterpolate();
	mInterpSpline(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		prepareSpline();
		callInterpolate();
	}
	mInterpSpline(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
	{
		prepareSpline();
		callInterpolate();
	}
	~mInterpSpline()
	{
		delete(spline);
	}
};


//least squares linear
class mInterpLSLinear : public mInterpolator
{
private:
	double sxi, syi, sxi2, sxyi;

public:
	void calcInterpCoeffs();
	double lsqLinInterp(double x, double a, double b);
	void callInterpolate();
	mInterpLSLinear(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		callInterpolate();
	}
	mInterpLSLinear(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
	{
		callInterpolate();
	}
};


//least squares linear
class mInterpLSCubic : public mInterpolator
{
private:
	double sxi, syi, sxi2, sxyi, sx2yi, sxi3, sxi4;

public:
	void calcInterpCoeffs();
	double lsqCubInterp(double x, double a0, double a1, double a2, double a3);
	void callInterpolate();
	mInterpLSCubic(double vx1, double vx2, int vres, int vnres) :mInterpolator(vx1, vx2, vres, vnres)
	{
		callInterpolate();
	}
	mInterpLSCubic(double vx1, double vx2, int vres, int vnres, pVec dat) :mInterpolator(vx1, vx2, vres, vnres, dat)
	{
		callInterpolate();
	}
};