#pragma once

class RandomGenerator
{
public:
	/** Sets the seed used to generate the pseudo-number */
	static void SetSeed(double seed) { s_seed = seed; }

	/** Gets the seed used to generate the pseudo-number */
	static double GetSeed() { return s_seed; }

	/** Gets a new randon number in the range [0..1] */
	static double GetRandom();

	/** Gets a new randon number in the range [min..max] given */
	static double GetRandom(double min, double max);

private:
	static double s_seed;
};