#include "ASDProjectSolver.h"
#include "Graph.h"
#include "DotWriter.h"


int main(int argc, char *argv[])
{
	Graph graph;
	

	//if(graph.IsCyclic())
	//	std::cout << "The graph HAS cycles" << std::endl << std::endl;
	//else
	//	std::cout << "The graph HAS NOT cycles" << std::endl << std::endl;
	//	
	//const Graph::NodePointersList possibleRoots = graph.GetNonEntrantNodes();

	//Graph::NodePointersList::ConstIterator it = possibleRoots.Begin();
	//Graph::NodePointersList::ConstIterator end = possibleRoots.End();

	//std::cout << "Nodes that are possible roots:" << std::endl;

	//for (; it && it != end; it++)
	//	std::cout << "\t- " << it->GetName() << ": add " << graph.GetUnreachableNodes(*it, true).GetSize() << " edges" <<  std::endl;

	//std::cout << std::endl;

	if(ASDProjectSolver::ProcessData(graph, argc, argv))
		DotWriter::Write(graph, std::cout);

	return 0;
}