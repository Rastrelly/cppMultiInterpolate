#include "mInterpolator.h"

mInterpolator::mInterpolator(double vx1, double vx2, int vres, int vnres)
{
	x1 = vx1;
	x2 = vx2;
	res = vres;
	newRes = vnres;
	buildFuncData();
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
			cSym == '6' || cSym == '7' || cSym == '8' || cSym == '9')
		{
			sValClean += cSym;
		}
		else
		{
			if (cSym == '.')
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
	return 10 * pow(x, 2) + 50 * sin(100 * x*3.14 / 180);
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
	}
}

