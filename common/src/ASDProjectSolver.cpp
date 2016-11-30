#include "ASDProjectSolver.h"
#include "DotParser.h"

bool ASDProjectSolver::ProcessData(Graph& result, int argc, char *argv[])
{
	return ProcessData(result, GetInputFileContent(argc, argv));
}

bool ASDProjectSolver::ProcessData(Graph& result, const std::string& dotFileContent)
{
	Graph inputGraph;

	// Parse the DOT file to get the starting graph
	if (DotParser::Parse(inputGraph, dotFileContent) == false)
		return false;

	return ProcessData(inputGraph, result);
}

/** Process the given graph and modifies it to solve the project */
bool ASDProjectSolver::ProcessData(const Graph& inputGraph, Graph& result)
{
	result = inputGraph;

	// Rename the graph
	result.SetName("out_" + result.GetName(), result.EncloseNameInDoubleQuotes());

	// Make sure that the graph hasn't got any cycle
	if (result.IsCyclic())
	{
		std::cout << "ERROR: the graph contains cycles!";
		return false;
	}

	// Loop through the possible roots of the graph to find the one that adds the least amount of edges
	Node* root = nullptr;
	int addedEdges = 0;

	if ((root = FindBestRoot(result, addedEdges)) == nullptr)
	{
		std::cout << "ERROR: couldn't find the best root";
		return false;
	}

	// Label the root to show the number of added edges
	root->SetAttribute("label", "root = " + root->GetName() + "; |E| - |E'| = " + std::to_string(addedEdges), false, true);

	// Compute the best paths from the root
	result.ComputeBestPathsFromRoot(root);

	return true;
}

Node* ASDProjectSolver::FindBestRoot(Graph& graph, int& addedEdges)
{
	Graph bestGraph;
	std::string bestRootName = "";
	int bestAddedEdges = -1;
	
	// Get all the nodes that are a valid candidate for the root
	// The possible candidates are all the nodes that don't have any entrant edge
	Graph::NodePointersList possibleRoots = graph.GetNonEntrantNodes();

	// Should never reach here because if there isn't any node without edges or with only exiting edges there must be a cycle
	if (possibleRoots.GetSize() == 0)
	{
		std::cout << "ERROR: the graph doesn't contain any node without edges or with only entrant edges so there isn't any valid node for the root!";
		return nullptr;
	}

	Graph::NodePointersList::Iterator possibleRootsIt = possibleRoots.Begin();
	Graph::NodePointersList::Iterator possibleRootsEnd = possibleRoots.End();
		
	for (; possibleRootsIt && possibleRootsIt != possibleRootsEnd; ++possibleRootsIt)
	{
		// Get all the unreachable nodes from the candidate root
		Graph::NodePointersList unreachableNodes = graph.GetUnreachableNodes(*possibleRootsIt, true);

		// If all the nodes can be reached from the given root then set the local graph ast the result and return
		if (unreachableNodes.GetSize() == 0)
		{
			addedEdges = 0;
			return *possibleRootsIt;
		}

		int localAddedEdges = AddEdgesToRoot(graph, *possibleRootsIt, unreachableNodes);

		if (localAddedEdges >= 0 && (bestAddedEdges == -1 || localAddedEdges < bestAddedEdges))
		{
			bestGraph = graph;
			bestRootName = possibleRootsIt->GetName();
			bestAddedEdges = localAddedEdges;
		}
		
		// Remove the edges added from the current possible root
		// If the current possible root is the best, the complete graph with all the added edges is stored inside 'bestGraph' any way
		graph.RemoveEdgesAddedByASDProjectSolver();
	}

	// If bestAddedEdges is still negative then we couldn't find any valid root
	if (bestAddedEdges == -1)
		return nullptr;
	
	// Move the best graph to the result and return true
	// Using std::move to save up some loops inside the graph copy ctor
	graph = bestGraph;
	addedEdges = bestAddedEdges;

	return graph.GetNode(bestRootName);
}

int ASDProjectSolver::AddEdgesToRoot(Graph& graph, Node* root, Graph::NodePointersList& unreachableNodes)
{
	int addedEdgesCount = 0;

	while (unreachableNodes.GetSize() > 0)
	{
		Graph::NodePointersList::Iterator it = unreachableNodes.Begin();
		Graph::NodePointersList::Iterator end = unreachableNodes.End();

		Node* bestNodeToAdd = nullptr;
		int unreachableNodesCount = unreachableNodes.GetSize();

		// Loop through the unreachable nodes and add the missing edges, checking that we're not creating any cycle
		for (; it && it != end; ++it)
		{
			Edge* addedEdge = graph.AddEdge(root, *it);

			// Make sure the edge was successfully added
			if (addedEdge)
			{
				// Mark the edge as added by the ASDProjectSolver
				addedEdge->SetAddedBySolver(true);

				// Check if the addition of the edge has created any cycle, in which case remove the edge
				if (graph.IsCyclic(true) == false)
				{
					int localUnreachableNodesCount = graph.GetUnreachableNodesCount(root, true);

					// If we have added an edge that makes the root able to reach all the nodes we can stop right here
					if (localUnreachableNodesCount == 0)
					{
						// Add the red colored attribute to the edge as the problem says
						addedEdge->SetAttribute("color", "red", false, false);

						return ++addedEdgesCount;
					}
					
					if (bestNodeToAdd == nullptr || localUnreachableNodesCount < unreachableNodesCount)
					{
						bestNodeToAdd = *it;
						unreachableNodesCount = localUnreachableNodesCount;
					}
				}

				graph.RemoveEdge(addedEdge);
			}
		}

		// If we couldn't find any node that could be reached by the root without creating a cycle return immediately
		if (bestNodeToAdd == nullptr)
			return -1;
		
		// Add the edge with the best node (the one that minimizes the unreachable nodes from the root)
		Edge* addedEdge = graph.AddEdge(root, bestNodeToAdd);

		// Add the red colored attribute to the edge as the problem says
		addedEdge->SetAttribute("color", "red", false, false);

		// Mark the edge as added by the ASDProjectSolver
		addedEdge->SetAddedBySolver(true);

		// Increment the number of added edges
		addedEdgesCount++;

		// Get the new list of unreachable nodes
		unreachableNodes = graph.GetUnreachableNodes(root, true);
	}

	return addedEdgesCount;
}

/** Method that gets the input file content (both from standard input or from the first argument of the process) */
std::string ASDProjectSolver::GetInputFileContent(int argc, char *argv[])
{
	std::string dotFileContent = "";

	// If the number of arguments is more than 1 then the path of the input file was given
	if (argc > 1)
	{
		// Try to open the file
		std::ifstream file(argv[1]);

		// If the file was opened then read its content and create the graph
		if (file)
			dotFileContent = ReadFile(file);
		else
			std::cerr << "Invalid file name passed as first argument" << std::endl;
	}
	// If there are no arguments use the standard input to read the graph data
	else
		dotFileContent = ReadFile(std::cin);

	return dotFileContent;
}

/** Method that returns a string which contains all the content of 'input' */
std::string ASDProjectSolver::ReadFile(std::istream& input)
{
	std::ostringstream std_input;
	std_input << input.rdbuf();

	return std_input.str();
}