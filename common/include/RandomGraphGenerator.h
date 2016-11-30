#pragma once

#include "Graph.h"

class RandomGraphGenerator
{
public:
	/**
	* Creates a new random directed graph that doesn't contain any cycle
	* numberOfNodes: the number of nodes that will be contained inside the graph
	* numberOfEdges: the number of edges that will be contained inside the graph
	* edgeChance: change of adding an edge expressed in percentage (inside the intervall [0..1])
	* result: the resulting graph will be stored here
	* Returns false if this method can't add the number of nodes and edges as requested
	*/
	static bool CreateRandomGraph(int numberOfNodes, double edgeChance, Graph& result);
};