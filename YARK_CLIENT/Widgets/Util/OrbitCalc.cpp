//stollen from KSP itself
#include "OrbitCalc.h"

double sign(double input) {
	if (input > 0) {
		return 1;
	}
	else if (input < 0) {
		return -1;
	}
	return 0;
}

//ecentric -> mean
double GetMeanAnomaly(OrbitData* o, double E)
{
	if (o->e < 1.0)
		return E - o->e * sin(E);
	if (isinf(E))
		return E;
	return o->e * sinh(E) - E;
}

//true -> ecentric
double GetEccentricAnomaly(OrbitData* o, double trueAnomaly)
{
	double num1 = cos(trueAnomaly / 2.0);
	double num2 = sin(trueAnomaly / 2.0);
	double num3;
	if (o->e < 1.0)
	{
		num3 = 2.0 * atan2(sqrt(1.0 - o->e) * num2, sqrt(1.0 + o->e) * num1);
	}
	else
	{
		double num4 = sqrt((o->e - 1.0) / (o->e + 1.0)) * num2 / num1;
		num3 = num4 < 1.0 ? (num4 > -1.0 ? log((1.0 + num4) / (1.0 - num4)) : -INFINITY) : INFINITY;
	}
	return num3;
}

double GetTrueAnomaly(OrbitData* o, double E)
{
	double num1;
	if (o->e < 1.0)
	{
		double num2 = cos(E / 2.0);
		num1 = 2.0 * atan2(sqrt(1.0 + o->e) * sin(E / 2.0), sqrt(1.0 - o->e) * num2);
	}
	else
		num1 = !(E == INFINITY) ? (!(E == -INFINITY) ? 2.0 * atan2(sqrt(o->e + 1.0) * sinh(E / 2.0), sqrt(o->e - 1.0) * cosh(E / 2.0)) : -acos(-1.0 / o->e)) : acos(-1.0 / o->e);
	return num1;
}

//stollen from KSP itself
double solveEccentricAnomaly(double M, double ecc, double maxError, int maxIterations)
{
	if (ecc >= 1.0)
		return  solveEccentricAnomalyHyp(M, ecc, maxError);
	if (ecc < 0.8)
		return  solveEccentricAnomalyStd(M, ecc, maxError);
	return  solveEccentricAnomalyExtremeEcc(M, ecc, maxIterations);
}

//stollen from KSP itself
double solveEccentricAnomalyStd(double M, double ecc, double maxError)
{
	double num1 = 1.0;
	double num2 = M + ecc * sin(M) + 0.5 * ecc * ecc * sin(2.0 * M);
	while (abs(num1) > maxError)
	{
		double num3 = num2 - ecc * sin(num2);
		num1 = (M - num3) / (1.0 - ecc * cos(num2));
		num2 += num1;
	}
	return num2;
}

//stollen from KSP itself
double solveEccentricAnomalyExtremeEcc(double M, double ecc, int iterations)
{
	double num1 = M + 0.85 * ecc * (double)sign(sin(M));
	for (int index = 0; index < iterations; ++index)
	{
		double num2 = ecc * sin(num1);
		double num3 = ecc * cos(num1);
		double num4 = num1 - num2 - M;
		double num5 = 1.0 - num3;
		double num6 = num2;
		num1 += -5.0 * num4 / (num5 + (double)sign(num5) * sqrt(abs(16.0 * num5 * num5 - 20.0 * num4 * num6)));
	}
	return num1;
}

//stollen from KSP itself
double solveEccentricAnomalyHyp(double M, double ecc, double maxError)
{
	if (M == INFINITY)
		return M;
	double num1 = 1.0;
	double num2 = 2.0 * M / ecc;
	double num3 = log(sqrt(num2 * num2 + 1.0) + num2);
	while (abs(num1) > maxError)
	{
		num1 = (ecc * sinh(num3) - num3 - M) / (ecc * cosh(num3) - 1.0);
		num3 -= num1;
	}
	return num3;
}

glm::dvec3 kep_2_cart(double semi_latus_rectum, double anomaly, double eccentricity, float inclination, float arg_PE, double long_AN) {
	glm::dvec3 posPQW_;
	glm::dvec3 posECI_;
	posPQW_ = glm::vec3(semi_latus_rectum * glm::cos(anomaly) / (1 + eccentricity * glm::cos(anomaly)), semi_latus_rectum * glm::sin(anomaly) / (1 + eccentricity * glm::cos(anomaly)), 0);

	double cO = glm::cos(arg_PE); double sO = glm::sin(arg_PE);
	double co = glm::cos(long_AN); double so = glm::sin(long_AN);
	double ci = glm::cos(inclination); double si = glm::sin(inclination);
	glm::dmat3 dcmPQW2ECI_ = glm::dmat3(cO * co - sO * ci * so, -cO * so - sO * ci * co, sO * si,
		sO * co + cO * ci * so, -sO * so + cO * ci * co, -cO * si,
		si * so, si * co, ci);

	posECI_ = dcmPQW2ECI_ * posPQW_;
	return glm::vec3(-posECI_.x, posECI_.y, posECI_.z);
}