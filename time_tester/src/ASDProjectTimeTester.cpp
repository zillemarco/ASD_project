#include "ASDProjectTimeTester.h"
#include "ASDProjectSolver.h"

#include <ctime>
#include <cmath>

/**
* Method that returns a double precision floating point time value.
* You can use this like this to know how many seconds passed between two GetTime() calls:
*	double t0 = GetTime();
*	...
*	do stuff
*	...
*	double t1 = GetTime();
*	double elapsed = t1 - t0;
*/
double ASDProjectTimeTester::GetTime()
{
	return ((double)clock()) / CLOCKS_PER_SEC;
}

/** Returns the granularity of the system (the minimum measurable time span) */
double ASDProjectTimeTester::GetGranularity()
{
	double t0 = GetTime();
	double t1 = GetTime();

	while (t0 == t1)
		t1 = GetTime();

	return (t1 - t0);
}

/**
* Returns the minimun time that the system needs to give a valid result
* tollerance: the tollerance that the system will support (expressed in a percentage inside the intervall [0..1])
	* If tollerance is minor or equal to 0 then it defaults to 0.05
*/
double ASDProjectTimeTester::GetMinimumTime(double tollerance)
{
	if (tollerance <= 0.0)
		tollerance = 0.05;

	return GetGranularity() / tollerance;
}

/**
* Returns the number of times the preparations have to be made to compute a valid time result (that meets the tollerance used to compute the minimum time)
* The only preparation it has to be done is to copy the input graph into the result graph
*/
int ASDProjectTimeTester::GetRepetitionsCountForPreparation(const Graph& inputGraph, double minimumTime)
{
	// Intialize the times	
	double t0 = 0;
	double t1 = 0;

	int repetitions = 1;

	while ((t1 - t0) <= minimumTime)
	{
		// Exponential growth
		repetitions *= 2;

		t0 = GetTime();
		for (int i = 0; i < repetitions; i++)
			Graph tempResult = inputGraph;
		t1 = GetTime();
	}

	// Find the optimal repetitions count in a bisection fashion aproximating to 5
	int max = repetitions;
	int min = repetitions / 2;
	int wrongCycles = 5;

	while ((max - min) >= wrongCycles)
	{
		repetitions = (min + max) / 2;

		t0 = GetTime();
		for (int i = 0; i < repetitions; i++)
			Graph tempResult = inputGraph;
		t1 = GetTime();

		if ((t1 - t0) <= minimumTime)
			min = repetitions;
		else
			max = repetitions;
	}

	return max;
}

/**
* Returns the number of times the algorithm has to be launched to compute a valid time result (that meets the tollerance used to compute the minimum time)
*/
int ASDProjectTimeTester::GetRepetitionsCountForWholeAlgorithm(const Graph& inputGraph, double minimumTime)
{
	// Intialize the times	
	double t0 = 0;
	double t1 = 0;

	int repetitions = 1;

	while ((t1 - t0) <= minimumTime)
	{
		// Exponential growth
		repetitions *= 2;

		t0 = GetTime();
		for (int i = 0; i < repetitions; i++)
		{
			Graph tempResult;
			ASDProjectSolver::ProcessData(inputGraph, tempResult);
		}
		t1 = GetTime();
	}

	// Find the optimal repetitions count in a bisection fashion aproximating to 5
	int max = repetitions;
	int min = repetitions / 2;
	int wrongCycles = 5;

	while ((max - min) >= wrongCycles)
	{
		repetitions = (min + max) / 2;

		t0 = GetTime();
		for (int i = 0; i < repetitions; i++)
		{
			Graph tempResult;
			ASDProjectSolver::ProcessData(inputGraph, tempResult);
		}
		t1 = GetTime();

		if ((t1 - t0) <= minimumTime)
			min = repetitions;
		else
			max = repetitions;
	}

	return max;
}

/**
* Computes the average time that the algorithm takes to give a result,
* stripping away the time needed to prepare the input
*/
double ASDProjectTimeTester::ComputeAverageCleanTime(const Graph& inputGraph, double minimumTime)
{
	int preparationRepetitions = GetRepetitionsCountForPreparation(inputGraph, minimumTime);
	int algorithmRepetitions = GetRepetitionsCountForWholeAlgorithm(inputGraph, minimumTime);

	double t0 = 0;
	double t1 = 0;	
	
	// Compute the time to do the preparation
	t0 = GetTime();
	for (int i = 0; i < preparationRepetitions; i++)
		Graph tempResult = inputGraph;
	t1 = GetTime();

	double preparationTime = t1 - t0;

	// Compute the time to run the whole algorithm
	t0 = GetTime();
	for (int i = 0; i < algorithmRepetitions; i++)
	{
		Graph tempResult;
		ASDProjectSolver::ProcessData(inputGraph, tempResult);
	}
	t1 = GetTime();

	double algorithmTime = t1 - t0;

	// Return the average time to run just the algorithm, stripping away the preparation time
	return (algorithmTime / algorithmRepetitions) - (preparationTime / preparationRepetitions);
}

/**
* Computes the time needed to run the algorithm given
* inputGraph: the input graph
* n: the number of time to run the algoritm until we have enough data
* za: normal distribution
* minimumTime: the minimun time that the system needs to give a valid result
* delta: maximum error from the expected result
*/
double ASDProjectTimeTester::ComputeAlgorithmTime(const Graph& inputGraph, int n, double za, double minimumTime, double delta)
{
	double result = 0;
	double time = 0;
	double sumSquared = 0;
	int iterationsCount = 0;
	double localDelta = delta;

	do
	{
		for (int i = 0; i < n; i++)
		{
			double m = ComputeAverageCleanTime(inputGraph, minimumTime);
			time += m;
			sumSquared += (m * m);
		}

		iterationsCount += n;
		result = time / ((double)iterationsCount);

		double s = sqrt(sumSquared / ((double)iterationsCount) - (result * result));

		localDelta = 1.0 / sqrt((double)iterationsCount) * za * s;

	} while (localDelta >= delta);

	return result;
}