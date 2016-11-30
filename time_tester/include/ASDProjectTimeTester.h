#pragma once

#include "Graph.h"

class ASDProjectTimeTester
{
public:
	/**
	* Returns a double precision floating point time value.
	* You can use this like this to know how many seconds passed between two GetTime() calls:
	*	double t0 = GetTime(); 
	*	...
	*	do stuff
	*	...
	*	double t1 = GetTime();
	*	double elapsed = t1 - t0;
	*/
	static double GetTime();

	/** Returns the granularity of the system (the minimum measurable time span) */
	static double GetGranularity();

	/**
	* Returns the minimun time that the system needs to give a valid result
	* tollerance: the tollerance that the system will support (expressed in a percentage inside the intervall [0..1])
	* If tollerance is minor or equal to 0 then it defaults to 0.05
	*/
	static double GetMinimumTime(double tollerance = 0.05);

	/**
	* Returns the number of times the preparations have to be made to compute a valid time result (that meets the tollerance used to compute the minimum time)
	* The only preparation it has to be done is to copy the input graph into the result graph
	*/
	static int GetRepetitionsCountForPreparation(const Graph& inputGraph, double minimumTime);

	/**
	* Returns the number of times the algorithm has to be launched to compute a valid time result (that meets the tollerance used to compute the minimum time)
	*/
	static int GetRepetitionsCountForWholeAlgorithm(const Graph& inputGraph, double minimumTime);

	/**
	* Computes the average time that the algorithm takes to give a result,
	* stripping away the time needed to prepare the input
	*/
	static double ComputeAverageCleanTime(const Graph& inputGraph, double minimumTime);

	/**
	* Computes the time needed to run the algorithm given
	* inputGraph: the input graph
	* n: the number of times to run the algorithm each time until we have collected enough data
	* za: normal distribution
	* minimumTime: the minimun time that the system needs to give a valid result
	* delta: maximum error from the expected result
	*/
	static double ComputeAlgorithmTime(const Graph& inputGraph, int n, double za, double minimumTime, double delta);
};