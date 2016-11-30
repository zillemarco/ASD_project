#include "RandomGraphGenerator.h"
#include "RandomGenerator.h"

/**
* Creates a new random directed graph that doesn't contain any cycle
* numberOfNodes: the number of nodes that will be contained inside the graph
* result: the resulting graph will be stored here
* Returns false if this method can't add the number of nodes and edges as requested
*/
bool RandomGraphGenerator::CreateRandomGraph(int numberOfNodes, double edgeChance, Graph& result)
{
	result.SetGraphType(Graph::GT_Directed);

	Graph::NodePointersList addedNodes(numberOfNodes);

	for (int i = 0; i < numberOfNodes; i++)
		addedNodes.Add(result.AddNode(std::to_string(i), false));

	Graph::NodePointersList::Iterator outerIt = addedNodes.Begin();
	Graph::NodePointersList::Iterator end = addedNodes.End();

	for (; outerIt && outerIt != end; ++outerIt)
	{
		Graph::NodePointersList::Iterator innerIt = outerIt + 1;

		for (; innerIt && innerIt != end; ++innerIt)
		{
			if (RandomGenerator::GetRandom() < edgeChance)
			{
				result.AddEdge(*outerIt, *innerIt);
			}
		}
	}

	return true;
}