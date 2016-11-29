#include "RandomGraphGenerator.h"
#include "RandomGenerator.h"

/**
* Creates a new random directed graph that doesn't contain any cycle
* numberOfNodes: the number of nodes that will be contained inside the graph
* numberOfEdges: the number of edges that will be contained inside the graph
* result: the resulting graph will be stored here
* Returns false if this method can't add the number of nodes and edges as requested
*/
bool RandomGraphGenerator::CreateRandomGraph(int numberOfNodes, int numberOfEdges, double edgeChance, Graph& result)
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

	// Graph::NodePointersList addedNodes(numberOfNodes);
	// int addedNodesCount = 0;
	// int addedEdgesCount = 0;
	// 
	// // Calculate the minimum number of nodes to add each cycle (plus 1 to never have 0)
	// double minNodesToAddPerCycle = (numberOfNodes / 10) + 1;
	// 
	// while (addedNodesCount < numberOfNodes)
	// {
	// 	// Add an amount of nodes that is between the minimum previously calculated and the number of nodes that are left to complete the graph
	// 	int nodesToAddCount = (int)RandomGenerator::GetRandom(minNodesToAddPerCycle, numberOfNodes - addedNodesCount);
	// 
	// 	// Used to memorize the nodes that will be added during this iteration
	// 	Graph::NodePointersList newlyAddedNodes(nodesToAddCount);
	// 
	// 	// Add the nodes to the graph. Use the node index as name
	// 	for (int i = 0; i < nodesToAddCount; i++)
	// 		newlyAddedNodes.Add(result.AddNode(std::to_string(addedNodesCount + i), false));
	// 
	// 	Graph::NodePointersList::Iterator addedNodesIt = addedNodes.Begin();
	// 	Graph::NodePointersList::Iterator addedNodesEnd = addedNodes.End();
	// 
	// 	Graph::NodePointersList::Iterator newlyAddedNodesIt = newlyAddedNodes.Begin();
	// 	Graph::NodePointersList::Iterator newlyAddedNodesEnd = newlyAddedNodes.End();
	// 
	// 	// Start the loop to add the edges
	// 	for (; addedNodesIt && addedNodesIt != addedNodesEnd; ++addedNodesIt)
	// 	{
	// 		newlyAddedNodesIt = newlyAddedNodes.Begin();
	// 		newlyAddedNodesEnd = newlyAddedNodes.End();
	// 
	// 		for (; newlyAddedNodesIt && newlyAddedNodesIt != newlyAddedNodesEnd; ++newlyAddedNodesIt)
	// 		{
	// 			// Connect the 'old' nodes with the ones just created based on a percentual chance
	// 			if (addedEdgesCount < numberOfEdges && RandomGenerator::GetRandom() < edgeChance)
	// 			{
	// 				if (result.AddEdge(*addedNodesIt, *newlyAddedNodesIt) != nullptr)
	// 					addedEdgesCount++;
	// 			}
	// 		}
	// 	}
	// 
	// 	// Move the newly added nodes to the 'old' added nodes
	// 	newlyAddedNodesIt = newlyAddedNodes.Begin();
	// 	newlyAddedNodesEnd = newlyAddedNodes.End();
	// 
	// 	for (; newlyAddedNodesIt && newlyAddedNodesIt != newlyAddedNodesEnd; ++newlyAddedNodesIt)
	// 		addedNodes.Add(*newlyAddedNodesIt);
	// 
	// 	addedNodesCount += nodesToAddCount;
	// }
	// 
	// // Return true if we could meet the called requirements
	// return addedEdgesCount == numberOfEdges && addedNodesCount == numberOfNodes;
}