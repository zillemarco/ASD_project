#include "ASDProjectSolver.h"
#include "Graph.h"
#include "DotWriter.h"

int main(int argc, char *argv[])
{
	Graph graph;
	
	if(ASDProjectSolver::ProcessData(graph, argc, argv))
		DotWriter::Write(graph, std::cout);

	return 0;
}