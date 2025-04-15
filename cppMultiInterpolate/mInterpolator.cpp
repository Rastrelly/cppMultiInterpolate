#include "mInterpolator.h"

mInterpolator::mInterpolator(double vx1, double vx2, int vres, int vnres)
{
	x1 = vx1;
	x2 = vx2;
	res = vres;
	newRes = vnres;
	buildFuncData();
}

mInterpolator::mInterpolator(double vx1, double vx2, int vres, int vnres, pVec givenData)
{
	x1 = vx1;
	x2 = vx2;
	res = vres;
	newRes = vnres;
	step = givenData[1].x - givenData[0].x;
	basePoints = givenData;
}

void mInterpolator::getDataArea(double &xMin, double &xMax, double &yMin, double &yMax, bool clambToBase)
{
	//parse base
	int l = basePoints.size();
	xMin = basePoints[0].x;
	xMax = basePoints[l - 1].x;
	yMin = basePoints[0].y;
	yMax = basePoints[0].y;

	for (int i = 1; i < l; i++)
	{
		if (basePoints[i].y > yMax) yMax = basePoints[i].y;
		if (basePoints[i].y < yMin) yMin = basePoints[i].y;
	}

	if (clambToBase) return;

	l = interpPoints.size();
	for (int i = 1; i < l; i++)
	{
		if (interpPoints[i].y > yMax) yMax = interpPoints[i].y;
		if (interpPoints[i].y < yMin) yMin = interpPoints[i].y;
	}
}

std::string mInterpolator::forceDecSep(double val, char forcedDecSep)
{
	std::string sVal = std::to_string(val);
	std::string sValClean = "";
	int sl = sVal.length();
	for (int i = 0; i < sl; i++)
	{
		char cSym = sVal[i];
		if (cSym == '0' || cSym == '1' || cSym == '2' ||
			cSym == '3' || cSym == '4' || cSym == '5' ||
			cSym == '6' || cSym == '7' || cSym == '8' || 
			cSym == '9' || cSym == '-' )
		{
			sValClean += cSym;
		}
		else
		{
			if (cSym == '.')
				sValClean += forcedDecSep;
			if (cSym == '-')
				sValClean += forcedDecSep;
		}
	}
	return sValClean;
}

void mInterpolator::printDataToCSV(std::string fileName)
{
	std::ofstream dataPrinter(fileName.c_str());

	int baseL = basePoints.size();
	int inteL = interpPoints.size();

	for (int i = 0; i < inteL; i++)
	{
		//setup basePoint
		if (i < baseL)
		{
			dataPrinter << "\"" << forceDecSep(basePoints[i].x,',') << "\"" << ";" <<
				"\"" << forceDecSep(basePoints[i].y, ',') << "\"" << ";";
			std::cout << "\"" << forceDecSep(basePoints[i].x, ',') << "\"" << ";" <<
				"\"" << forceDecSep(basePoints[i].y, ',') << "\"" << ";";
		}
		else
		{
			//print empty cells if out of scope
			dataPrinter << "\"" << "\"" << ";" <<
				"\"" << "\"" << ";";
			std::cout << "\"" << "\"" << ";" <<
				"\"" << "\"" << ";";
		}

		//setup interpolated point
		dataPrinter << "\"" << forceDecSep(interpPoints[i].x,',') << "\"" << ";" <<
			"\"" << forceDecSep(interpPoints[i].y,',') << "\"" << ";" << "\n";
		std::cout << "\"" << forceDecSep(interpPoints[i].x, ',') << "\"" << ";" <<
			"\"" << forceDecSep(interpPoints[i].y, ',') << "\"" << ";" << "\n";
	}

	dataPrinter.close();
}

void mInterpolator::buildFuncData()
{
	step = (x2 - x1) / res;
	basePoints.resize(res + 1);
	for (int i = 0; i <= res; i++)
	{
		double cx = x1 + i * step;
		basePoints[i].x = cx;
		basePoints[i].y = mathFunction(cx);
	}
}

double mInterpolator::mathFunction(double x)
{
	return 50 * sin(x*3.14 / 180);
}

void mInterpolator::getNeighbourPoints(double x, mPoint &np1, mPoint &np2)
{
	int i = 0;
	mPoint np1c = { 0,0 }, np2c = { 0,0 };

	while (basePoints[i].x <= x)
	{

		np1c = basePoints[i];
		np2c = basePoints[i+1];

		i++;
		if (i > basePoints.size() - 2) break;

	}

	//std::cout << "np1 = " << np1c.x << "; np2 = " << np2c.x << " when x=" << x << "\n";

	np1 = np1c;
	np2 = np2c;
}

double mInterpLinear::linterp(mPoint p1, mPoint p2, double px)
{
	return p1.y + (((p2.y - p1.y) / (p2.x - p1.x)) * (px - p1.x));
}

void mInterpLinear::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);

	for (int i = 0; i <= getNewRes(); i++)
	{
		mPoint np1 = { 0,0 }, np2 = {0,0};
		double cx = getX1() + i * newStep;
		getNeighbourPoints(cx, np1, np2);
		double cy = linterp(np1,np2,cx);
		interpPoints[i] = {cx,cy};
		if (i % 100 == 0)
		std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}

double mInterpLagrange::lagrangeXY(double sx)
{
	int n = basePoints.size();

	double l = 1;
	double L = 0;

	for (int i = 0; i < n; i++)
	{
		double cy = basePoints[i].y;

		for (int j = 0; j < n; j++)
		{
			if (j != i)
			{
				l *= ((sx - basePoints[j].x) / (basePoints[i].x - basePoints[j].x));
			}
		}

		L += cy * l;
		l = 1;
	}

	return L;
}

void mInterpLagrange::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);

	for (int i = 0; i <= getNewRes(); i++)
	{
		double cx = getX1() + i * newStep;
		double cy = lagrangeXY(cx);
		interpPoints[i] = { cx,cy };
		if (i % 100 == 0)
			std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}


double mInterpNewton::newtonInterp(double sx)
{
	//source: https://gist.github.com/komasaru/6a10c02cf3f5e5fe411d7fb3987b36dd

	int l = basePoints.size();
	std::vector<double> w = {};
	std::vector<double> c = {};
	w.resize(l);
	c.resize(l);

	for (int i = 0; i < l; i++)
	{
		w[i] = basePoints[i].y;
		for (int j = i - 1; j >= 0; j--)
		{
			w[j] = (w[j + 1] - w[j]) / (basePoints[i].x - basePoints[j].x);
		}
		c[i] = w[0];
	}

	double s = c[l - 1];
	for (int i = l - 2; i >= 0; i--)
	{
		s = s * (sx - basePoints[i].x) + c[i];
	}

	return s;
}

void mInterpNewton::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);

	for (int i = 0; i <= getNewRes(); i++)
	{
		double cx = getX1() + i * newStep;
		double cy = newtonInterp(cx);
		interpPoints[i] = { cx,cy };
		if (i % 100 == 0)
			std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}

void mInterpSpline::prepareSpline()
{
	std::vector<double> yVals = {};
	yVals.resize(basePoints.size());
	
	for (int i = 0; i < basePoints.size(); i++)
		yVals[i] = basePoints[i].y;

	spline = new boost::math::interpolators::cardinal_cubic_b_spline<double>(yVals.begin(),yVals.end(),getX1(),getStep());
}

double mInterpSpline::splineInterp(double sx)
{
	return spline->operator()(sx);
}

void mInterpSpline::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);

	for (int i = 0; i <= getNewRes(); i++)
	{
		double cx = getX1() + i * newStep;
		double cy = splineInterp(cx);
		interpPoints[i] = { cx,cy };
		if (i % 100 == 0)
			std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}


void mInterpLSLinear::calcInterpCoeffs()
{
	sxi = 0;
	sxi2 = 0;
	syi = 0;
	sxyi = 0;

	for (int i = 0; i < basePoints.size(); i++)
	{
		sxi += basePoints[i].x;
		sxi2 += pow(basePoints[i].x, 2);
		syi += basePoints[i].y;
		sxyi += basePoints[i].x*basePoints[i].y;
	}
}


double mInterpLSLinear::lsqLinInterp(double x, double a , double b)
{
	int n = basePoints.size();
	return a * x + b;
}


void mInterpLSLinear::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);
	calcInterpCoeffs();
	std::cout << "sxi = " << sxi << "; syi = " << syi << "\n";
	double n = basePoints.size();
	double ca = (n*sxyi-sxi*syi)/(n*sxi2-pow(sxi,2));
	double cb = ((1/n)*syi)-((ca/n)*sxi);
	std::cout << "a = " << ca << "; b = " << cb << "\n";

	for (int i = 0; i <= getNewRes(); i++)
	{
		double cx = getX1() + i * newStep;
		double cy = lsqLinInterp(cx, ca, cb);
		interpPoints[i] = { cx,cy };
		if (i % 100 == 0)
			std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}


void mInterpLSCubic::calcInterpCoeffs()
{
	//sxi, syi, sxi2, sxyi, sx2yi, sxi3,sxi4;
	sxi = 0;
	sxi2 = 0;
	sxi3 = 0;
	sxi4 = 0;
	syi = 0;
	sxyi = 0;
	sx2yi = 0;

	for (int i = 0; i < basePoints.size(); i++)
	{
		sxi += basePoints[i].x;
		sxi2 += pow(basePoints[i].x, 2);
		sxi3 += pow(basePoints[i].x, 3);
		sxi4 += pow(basePoints[i].x, 4);
		syi += basePoints[i].y;
		sxyi += basePoints[i].x*basePoints[i].y;
		sx2yi += pow(basePoints[i].x,2)*basePoints[i].y;
	}
}


double mInterpLSCubic::lsqCubInterp(double x, double a0, double a1, double a2, double a3)
{
	return a2+a1*x+a0*pow(x,2);
}


void mInterpLSCubic::callInterpolate()
{
	double newStep = (getX2() - getX1()) / getNewRes();
	interpPoints.resize(getNewRes() + 1);
	calcInterpCoeffs();
	double n = basePoints.size();
	double ca0 = (sx2yi*pow(sxi,2)-sxyi*sxi*sxi2-sxi3*syi*sxi+syi*pow(sxi2,2)-n*sx2yi*sxi2+n*sxi3*sxyi)/
		(sxi4*pow(sxi,2)-2*sxi*sxi2*sxi3+pow(sxi2,3)-n*sxi4*sxi2+n*pow(sxi3,2));
	double ca1 = (pow(sxi2,2)*sxyi-n*sxi4*sxyi+n*sxi3*sx2yi-sxi*sxi2*sx2yi+sxi*sxi4*syi-sxi2*sxi3*syi)/
		(sxi4*pow(sxi,2)-2*sxi*sxi2*sxi3+pow(sxi2,3)-n*sxi4*sxi2+n*pow(sxi3,2));
	double ca2 = (sx2yi*pow(sxi2,2)-sxyi*sxi2*sxi3-sxi4*syi*sxi2+syi*pow(sxi3,2)-sxi*sx2yi*sxi3+sxi*sxi4*sxyi)/
		(sxi4*pow(sxi,2)-2*sxi*sxi2*sxi3+pow(sxi2,3)-n*sxi4*sxi2+n*pow(sxi3,2));
	std::cout << "a0 = " << ca0 << "; a1 = " << ca1 << "; a2 = " << ca2 << "\n";

	for (int i = 0; i <= getNewRes(); i++)
	{
		double cx = getX1() + i * newStep;
		double cy = lsqCubInterp(cx, ca0, ca1, ca2, 0);
		interpPoints[i] = { cx,cy };
		if (i % 100 == 0)
			std::cout << trunc(100 * ((double)i / (double)getNewRes())) << "\%\n";
	}
}