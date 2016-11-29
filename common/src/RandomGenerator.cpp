#include "RandomGenerator.h"
#include <cmath>

/** Initialize the seed to be used if the user doesn't call SetSeed */
double RandomGenerator::s_seed = 62413904;

/** Gets a new randon number in the range [0..1] */
double RandomGenerator::GetRandom()
{
	static const int a = 16087;
	static const int m = 2147483647;
	static const int q = 127773;
	static const int r = 2836;

	double lo, hi, test;

	hi = ceil(s_seed / q);
	lo = s_seed - q * hi;
	test = a * lo - r * hi;

	if (test < 0.0)
		s_seed = test + m;
	else
		s_seed = test;

	return s_seed / m;
}

/** Gets a new randon number in the range [min..max] given */
double RandomGenerator::GetRandom(double min, double max)
{
	return (max - min + 1) * GetRandom() + min;
}