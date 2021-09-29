#include "../../Client/Client.h"
#include <glm/glm.hpp>
#define PI ((double)(3.14159265358979323846264338327950288))

double sign(double input);

double GetMeanAnomaly(OrbitData* o, double E);

double GetEccentricAnomaly(OrbitData* o, double trueAnomaly);

double GetTrueAnomaly(OrbitData* o, double E);

//stollen from KSP itself
double solveEccentricAnomaly(double M, double ecc, double maxError, int maxIterations);

//stollen from KSP itself
double solveEccentricAnomalyStd(double M, double ecc, double maxError);

//stollen from KSP itself
double solveEccentricAnomalyExtremeEcc(double M, double ecc, int iterations);

//stollen from KSP itself
double solveEccentricAnomalyHyp(double M, double ecc, double maxError);

glm::dvec3 kep_2_cart(double semi_latus_rectum, double anomaly, double eccentricity, float inclination, float arg_PE, double long_AN);