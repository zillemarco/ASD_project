#include "Graph.h"

/**
* Comparator used by Graph::AddEdge to check if an edge with the
* given start and end nodes exists
*/ 
struct EdgeComparator
{
	EdgeComparator(const Node& startNode, const Node& endNode, Graph::GraphType graphType)
		: _startNode(startNode)
		, _endNode(endNode)
		, _graphType(graphType)
	{ }

	inline bool operator()(const Edge& edge)
	{
		// If the graph is directed two edges as equal only if the start and end node are the same
		if(_graphType == Graph::GT_Directed)
			return (*edge.GetStartNode()) == _startNode && (*edge.GetEndNode()) == _endNode;
		// If the graph is not directed there start and end node don't exist so two edges are equal
		// also if the start and end node are the same
		else
		{
			return ((*edge.GetStartNode()) == _startNode && (*edge.GetEndNode()) == _endNode) || ((*edge.GetStartNode()) == _endNode && (*edge.GetEndNode()) == _startNode);
		}
		return false;
	}

	const Node& _startNode;
	const Node& _endNode;
	Graph::GraphType _graphType;
};

/** Comparator used to check if an edge with the given start and end nodes exists */
struct EdgeComparatorWithNodesPointer
{
	EdgeComparatorWithNodesPointer(const Node* startNode, const Node* endNode, Graph::GraphType graphType)
		: _startNode(startNode)
		, _endNode(endNode)
		, _graphType(graphType)
	{ }

	inline bool operator()(const Edge& edge)
	{
		// If the graph is directed two edges as equal only if the start and end node are the same
		if (_graphType == Graph::GT_Directed)
			return edge.GetStartNode() == _startNode && edge.GetEndNode() == _endNode;
		// If the graph is not directed there start and end node don't exist so two edges are equal
		// also if the start and end node are the same
		else
		{
			return (edge.GetStartNode() == _startNode && edge.GetEndNode() == _endNode) || (edge.GetStartNode() == _endNode && edge.GetEndNode() == _startNode);
		}
		return false;
	}

	const Node* _startNode;
	const Node* _endNode;
	Graph::GraphType _graphType;
};

/** Comparator used by Graph::GetNode to get a node from a given name */
struct NodeComparator
{
	NodeComparator(const std::string& name)
		: _name(name)
	{ }

	inline bool operator()(const Node& node)
	{
		return node.GetName() == _name;
	}

	std::string _name;
};

/** Default constructor */
Graph::Graph()
	: _name("")
	, _encloseNameInDoubleQuotes(false)
	, _graphType(GT_NotValid)
{ }

/** Constructor for basic initialization */
Graph::Graph(const std::string& name, bool encloseNameInDoubleQuotes, GraphType type)
	: _name(name)
	, _encloseNameInDoubleQuotes(encloseNameInDoubleQuotes)
	, _graphType(type)
{ }

/** Copy constructor */
Graph::Graph(const Graph& src)
{
	Copy(src);
}

/** Move constructor */
Graph::Graph(Graph&& src)
	: _edges(std::move(src._edges))
	, _nodes(std::move(src._nodes))
	, _name(std::move(src._name))
	, _encloseNameInDoubleQuotes(std::move(src._encloseNameInDoubleQuotes))
	, _graphType(std::move(src._graphType))
{ }

/** Destructor */
Graph::~Graph()
{
	_edges.Clear();
	_nodes.Clear();
	_name = "";
	_graphType = GT_NotValid;
}

/** Assign operator */
Graph& Graph::operator=(const Graph& src)
{
	if (this != &src)
		Copy(src);
	return *this;
}

/** Move operator */
Graph& Graph::operator=(Graph&& src)
{
	if (this != &src)
	{
		_edges.Clear(0, true);
		_nodes.Clear(0, true);
		_name = "";
		_graphType = GT_NotValid;

		_edges = std::move(src._edges);
		_nodes = std::move(src._nodes);
		_name = std::move(src._name);
		_encloseNameInDoubleQuotes = std::move(src._encloseNameInDoubleQuotes);
		_graphType = std::move(src._graphType);
	}
	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void Graph::Copy(const Graph& src)
{
	_name = src._name;
	_encloseNameInDoubleQuotes = src._encloseNameInDoubleQuotes;
	_graphType = src._graphType;

	// Clear the edges and the nodes
	_edges.Clear();
	_nodes.Clear();
	
	// Add all the nodes
	NodeList::ConstIterator nodesIt = src._nodes.Begin();
	NodeList::ConstIterator nodesEnd = src._nodes.End();
	for (; nodesIt && nodesIt != nodesEnd; ++nodesIt)
	{
		Node node = Node((*nodesIt).GetName(), (*nodesIt).EncloseNameInDoubleQuotes());
		node.CopyAttributes(*nodesIt);

		_nodes.Add(node);
	}

	// Add all the edges
	EdgeList::ConstIterator edgesIt = src._edges.Begin();
	EdgeList::ConstIterator edgesEnd = src._edges.End();
	for (; edgesIt && edgesIt != edgesEnd; ++edgesIt)
	{
		const std::string& startNodeName = (*edgesIt).GetStartNode()->GetName();
		const std::string& endNodeName = (*edgesIt).GetEndNode()->GetName();

		Edge* addedEdge = AddEdgeNoCheck(startNodeName, endNodeName);
		addedEdge->CopyAttributes(*edgesIt);
	}
}

/**
* Creates a new edge with the given nodes as starting and ending nodes and adds it
* to the list of edges of this grpah. The given nodes must be valid (non-nullptr).
* This function also adds the newly created edge to the list of edges of the given nodes.
* Returns a pointer to the newly created edge or nullptr if errors occurs.
* Possible errors are:
*  - one or both the nodes are not valid
*  - an edge with startNode as starting node and endNode as ending node already exists
* Derived classes that override this method must ensure that the previous errors are checked
*/
Edge* Graph::AddEdge(Node* startNode, Node* endNode)
{
	if (startNode == nullptr || endNode == nullptr)
	{
		std::cerr << "Graph error [AddEdge]: adding an edge with no start or end node" << std::endl;
		return nullptr;
	}

	if (_edges.Find(EdgeComparator(*startNode, *endNode, _graphType)) != -1)
	{
		std::cerr << "Graph error [AddEdge]: an edge with the given nodes already exists" << std::endl;
		return nullptr;
	}

	// Create the new edge and add it to the list of edges of the graph
	_edges.Add(Edge(startNode, endNode));

	// If the graph is directed add the end node to the adjacency list of the start node
	if (_graphType == GT_Directed)
		startNode->AddAdjacentNode(endNode);
	// If the graph is not directed then both the nodes can be reached through one or the other
	// so add both to the adjacency list of the other one
	else
	{
		startNode->AddAdjacentNode(endNode);
		endNode->AddAdjacentNode(startNode);
	}

	// Return the last edge inside the list which is the one which has been added
	return &_edges.Back();
}

/**
* Creates a new edge with the given nodes names as starting and ending nodes and adds it
* to the list of edges of this grpah. The given nodes must be valid (non-empty).
* If the nodes with the given names don't exist they're added automatically.
* This function also adds the newly created edge to the list of edges of the given nodes.
* Returns a pointer to the newly created edge or nullptr if errors occurs.
* Possible errors are:
*  - one or both the nodes names are not valid
*  - an edge with startNode as starting node and endNode as ending node already exists
* Derived classes that override this method must ensure that the previous errors are checked
*/
Edge* Graph::AddEdge(const std::string& startNodeName, const std::string& endNodeName, bool encloseStartNodeNameInDoubleQuotes, bool encloseEndNodeNameInDoubleQuotes)
{
	if (startNodeName.empty())
	{
		std::cerr << "Graph error [AddEdge]: the start node name is not valid" << std::endl;
		return nullptr;
	}

	if (endNodeName.empty())
	{
		std::cerr << "Graph error [AddEdge]: the end node name is not valid" << std::endl;
		return nullptr;
	}

	// Searche the nodes inside the ones already present
	Node* startNode = GetNode(startNodeName);
	Node* endNode = GetNode(endNodeName);

	// If the start node wasn't found we add it and check if there are errors
	if (startNode == nullptr)
	{
		startNode = AddNode(startNodeName, encloseStartNodeNameInDoubleQuotes);
		
		if(startNode == nullptr)
		{
			std::cerr << "Graph error [AddEdge]: unable to add the starting node named " << startNodeName << std::endl;
			return nullptr;
		}
	}

	// If the end node wasn't found we add it and check if there are errors
	if (endNode == nullptr)
	{
		endNode = AddNode(endNodeName, encloseEndNodeNameInDoubleQuotes);

		if (endNode == nullptr)
		{
			std::cerr << "Graph error [AddEdge]: unable to add the ending node named " << startNodeName << std::endl;
			return nullptr;
		}
	}

	return AddEdge(startNode, endNode);
}

/**
* Adds a new edge without doing any check. (Utility for the copy operator and constructor)
* Returns a pointer to the new edge.
*/
Edge* Graph::AddEdgeNoCheck(const std::string& startNodeName, const std::string& endNodeName)
{
	// Searche the nodes inside the ones already present
	Node* startNode = GetNode(startNodeName);
	Node* endNode = GetNode(endNodeName);

	// Create the new edge and add it to the list of edges of the graph
	_edges.Add(Edge(startNode, endNode));

	// If the graph is directed add the end node to the adjacency list of the start node
	if (_graphType == GT_Directed)
		startNode->AddAdjacentNode(endNode);
	// If the graph is not directed then both the nodes can be reached through one or the other
	// so add both to the adjacency list of the other one
	else
	{
		startNode->AddAdjacentNode(endNode);
		endNode->AddAdjacentNode(startNode);
	}

	// Return the last edge inside the list which is the one which has been added
	return &_edges.Back();
}

/** Removes the edge at the given index from the edges of the graph */
Graph& Graph::RemoveEdge(int edgeIndex)
{
	// Make sure the index is valid
	if (edgeIndex >= 0 && edgeIndex < _edges.GetSize())
	{
		EdgeList::Iterator it = _edges.GetIteratorAt(edgeIndex);
		return RemoveEdgeWithIterator(it);
	}
	return *this;
}

/** Removes the given edge from the edges of the graph */
Graph& Graph::RemoveEdge(Edge* edge)
{
	// Make sure the edge is valid
	if (edge == nullptr)
		return *this;

	// Find the edge index and then call the other RemoveEdge method
	return RemoveEdge(_edges.Find(*edge));
}

/** Removes the given edge from the edges of the graph */
Graph& Graph::RemoveEdgeWithIterator(EdgeList::Iterator& it)
{
	// Make sure the iterator is valid
	if (it && it.IsOfList(&_edges))
	{
		// Remove the adjacent node from the edge start node
		(*it).GetStartNode()->RemoveAdjacentNode((*it).GetEndNode());

		// If the graph isn't directed then there is also the adjacent node from the end to the start node to remove
		if (_graphType != GT_Directed)
			(*it).GetEndNode()->RemoveAdjacentNode((*it).GetStartNode());

		// Then remove the edge from the edge list
		_edges.Remove(it);
	}

	return *this;
}

/**
* Gets the edge at the given index from inside the list of edges of this graph
* If the given index is not valid returns nullptr
*/
Edge* Graph::GetEdge(int index)
{
	// Check if the index is valid
	if (index >= 0 && index < _edges.GetSize())
		return &_edges.GetAt(index);
	return nullptr;
}

/**
* Creates a new node with the given name or returns an existing one if it already exists a node with the given name.
* Returns nullptr if errors occurs.
* Possible errors are:
*  - the name is empty
* Derived classes that override this method must ensure that the previous errors are checked
*/
Node* Graph::AddNode(const std::string& name, bool encloseNodeNameInDoubleQuotes)
{
	if (name.empty())
	{
		std::cerr << "Graph error [AddNode]: adding a node without name" << std::endl;
		return nullptr;
	}

	// Try to find the node
	Node* node = GetNode(name);

	// If the node exists return it
	if (node != nullptr)
		return node;
	else
	{
		// Otherwise create the new node, add it to the list of nodes of the graph and return it
		_nodes.Add(Node(name, encloseNodeNameInDoubleQuotes));

		// Returns the last node inside the list which is the one that has been just added
		return &_nodes.Back();
	}
}

/**
* Gets the node at the given index from inside the list of nodes of this graph
* If the given index is not valid returns nullptr
*/
Node* Graph::GetNode(int index)
{
	// Check if the index is valid
	if (index >= 0 && index < _nodes.GetSize())
		return &_nodes.GetAt(index);
	return nullptr;
}

/**
* Gets the node at the given index from inside the list of nodes of this graph
* If the given index is not valid returns nullptr
*/
const Node* Graph::GetNode(int index) const
{
	// Check if the index is valid
	if (index >= 0 && index < _nodes.GetSize())
		return &_nodes.GetAt(index);
	return nullptr;
}

/** Gets the node that has the given name. If the node isn't found returns nullptr */
Node* Graph::GetNode(const std::string& nodeName)
{
	bool found = false;
	Node* result = &_nodes.FindElement(NodeComparator(nodeName), found);

	if (found == true)
		return result;
	return nullptr;
}

/** Gets the node that has the given name. If the node isn't found returns nullptr */
const Node* Graph::GetNode(const std::string& nodeName) const
{
	bool found = false;
	const Node* result = &_nodes.FindElement(NodeComparator(nodeName), found);

	if (found == true)
		return result;
	return nullptr;
}

/**
* Applies the DSF algorithm to see if the graph contains cycles inside.
* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
*/
bool Graph::IsCyclic(bool setNodesColorToWhiteAtStart, bool revertAllNodesToPreviousColor)
{
	// Get the values here to save up some function calls
	int nodesCount = _nodes.GetSize();
	int edgesCount = _edges.GetSize();

	// If the graph has no edges then it cannot contain a cycle
	if (edgesCount == 0)
		return false;

	// If the graph has no nodes it cannot contain a cycle (also should't reach here... without edges there shouldn't be nodes...)
	if (nodesCount == 0)
		return false;

	// If the graph has edge and is not directed it can only have cycles
	if (_graphType != GT_Directed)
		return true;

	// Otherwise we need to use DSF to check
	
	Node::NodeColor* nodesColor = nullptr;
	
	NodeList::Iterator it = _nodes.Begin();
	NodeList::Iterator end = _nodes.End();

	// Create the current nodes color array
	if (revertAllNodesToPreviousColor)
	{
		nodesColor = (Node::NodeColor*)malloc(sizeof(Node::NodeColor) * _nodes.GetSize());
		memset(nodesColor, 0, sizeof(Node::NodeColor) * _nodes.GetSize());
	}

	// If we have to restore the node's colors and set them to white at start we do so
	if(revertAllNodesToPreviousColor || setNodesColorToWhiteAtStart)
	{
		for (int index = 0; it && it != end; index++, it++)
		{
			// Memorize the current node color to restore is later
			if (revertAllNodesToPreviousColor)
				nodesColor[index] = (*it).GetColor();

			if(setNodesColorToWhiteAtStart)
				(*it).SetColor(Node::NodeColor::NC_White);
		}
	}

	// Loop through the nodes of the graph until we finish them or we find a cycle
	bool result = false;
	for (; it && it != end && result == false; it++)
	{
		// Do a DSF visit only if the node is 'white'
		if ((*it).GetColor() == Node::NodeColor::NC_White)
		{
			// Do the DSF visit to see if we can find a cycle starting from the node 'it'
			// MarkReachableNodes returns false when it finds a cycle
			result = (MarkReachableNodes(&(*it)) == false);
		}
	}

	// Revert the nodes color if the user wanted to
	if (revertAllNodesToPreviousColor && nodesColor != nullptr)
	{
		// Reset the iterator to start over again
		it = _nodes.Begin();

		for (int index = 0; it && it != end; index++, it++)
			(*it).SetColor(nodesColor[index]);

		// Free the temporary node's colors array
		free(nodesColor);
	}

	// If result is still false the graph doesn't contain ant cycle
	return result;
}

/**
* Runs through the entire list of nodes of this graph and returns a list of
* the nodes that are not reachable from the given node.
* node: the node from where to start the search
* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
*/
Graph::NodePointersList Graph::GetUnreachableNodes(Node* node, bool setNodesColorToWhiteAtStart, bool revertAllNodesToPreviousColor)
{
	NodePointersList result;

	// Make sure we have a valid node as input
	if (node == nullptr)
		return result;

	Node::NodeColor* nodesColor = nullptr;

	NodeList::Iterator it = _nodes.Begin();
	NodeList::Iterator end = _nodes.End();

	// Create the current nodes color array
	if (revertAllNodesToPreviousColor)
	{
		nodesColor = (Node::NodeColor*)malloc(sizeof(Node::NodeColor) * _nodes.GetSize());
		memset(nodesColor, 0, sizeof(Node::NodeColor) * _nodes.GetSize());
	}
	
	// If we have to restore the node's colors and set them to white at start we do so
	if (revertAllNodesToPreviousColor || setNodesColorToWhiteAtStart)
	{
		for (int index = 0; it && it != end; index++, it++)
		{
			// Memorize the current node color to restore is later
			if (revertAllNodesToPreviousColor)
				nodesColor[index] = (*it).GetColor();

			if (setNodesColorToWhiteAtStart)
				(*it).SetColor(Node::NodeColor::NC_White);
		}
	}
	
	// Mark all the nodes that are reachable from the given node
	MarkReachableNodes(node);

	// Loop through the nodes and see if there are white-colored nodes, which means that they couldn't be reached from the given node
	it = _nodes.Begin();

	for (; it && it != end; it++)
	{
		// If the node is white-colord then it couldn't be reached from the given node
		if ((*it).GetColor() == Node::NodeColor::NC_White)
			result.Add(&(*it));
	}

	// Revert the nodes color if the user wanted to
	if (revertAllNodesToPreviousColor && nodesColor != nullptr)
	{
		// Reset the iterator to start over again
		it = _nodes.Begin();

		for (int index = 0; it && it != end; index++, it++)
			(*it).SetColor(nodesColor[index]);

		// Free the temporary node's colors array
		free(nodesColor);
	}

	return result;
}

/**
* Runs through the entire list of nodes of this graph and returns a list of
* the nodes that are not reachable from the given node.
* nodeName: the name of the node node from where to start the search
* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
*/
Graph::NodePointersList Graph::GetUnreachableNodes(const std::string& nodeName, bool setNodesColorToWhiteAtStart, bool revertAllNodesToPreviousColor)
{
	Node* node = GetNode(nodeName);

	// GetUnreachableNodes checks if the node is not valid
	return GetUnreachableNodes(node, setNodesColorToWhiteAtStart, revertAllNodesToPreviousColor);
}

/**
* Returns the number of nodes that the given node cannot reach.
* node: the node from where to start the search
* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
*/
int Graph::GetUnreachableNodesCount(Node* node, bool setNodesColorToWhiteAtStart, bool revertAllNodesToPreviousColor)
{
	int result = 0;

	// Make sure we have a valid node as input
	if (node == nullptr)
		return result;

	Node::NodeColor* nodesColor = nullptr;

	NodeList::Iterator it = _nodes.Begin();
	NodeList::Iterator end = _nodes.End();

	// Create the current nodes color array
	if (revertAllNodesToPreviousColor)
	{
		nodesColor = (Node::NodeColor*)malloc(sizeof(Node::NodeColor) * _nodes.GetSize());
		memset(nodesColor, 0, sizeof(Node::NodeColor) * _nodes.GetSize());
	}

	// If we have to restore the node's colors and set them to white at start we do so
	if (revertAllNodesToPreviousColor || setNodesColorToWhiteAtStart)
	{
		for (int index = 0; it && it != end; index++, it++)
		{
			// Memorize the current node color to restore is later
			if (revertAllNodesToPreviousColor)
				nodesColor[index] = (*it).GetColor();

			if (setNodesColorToWhiteAtStart)
				(*it).SetColor(Node::NodeColor::NC_White);
		}
	}

	// Mark all the nodes that are reachable from the given node
	MarkReachableNodes(node);

	// Loop through the nodes and see if there are white-colored nodes, which means that they couldn't be reached from the given node
	it = _nodes.Begin();

	for (; it && it != end; it++)
	{
		// If the node is white-colord then it couldn't be reached from the given node
		if ((*it).GetColor() == Node::NodeColor::NC_White)
			result++;
	}

	// Revert the nodes color if the user wanted to
	if (revertAllNodesToPreviousColor && nodesColor != nullptr)
	{
		// Reset the iterator to start over again
		it = _nodes.Begin();

		for (int index = 0; it && it != end; index++, it++)
			(*it).SetColor(nodesColor[index]);

		// Free the temporary node's colors array
		free(nodesColor);
	}

	return result;
}

/**
* This method iterates over the list of adjacent nodes of the given node
* in a DFS way, checking that they're not gray-colored (in which case there is a cycle so we cannot continue and return false).
* If they are not gray-colored they're set as so and calls MarkReachableNodes on the node. If all the adjacent nodes are ok the node
* is set as black-colored.
* At the end all the nodes of the graph which contains the given node will be either white or black. Is a node is white then it couldn't be reached by the node
* given in input to the first call to MarkReachableNodes.
* Returns false if a cycle is found.
*/
bool Graph::MarkReachableNodes(Node* node)
{
	// Mark the node so that we know it is being processed
	node->SetColor(Node::NodeColor::NC_Gray);

	const List<Node*>& adjacentNodes = node->GetAdjacentNodes();

	List<Node*>::ConstIterator it = adjacentNodes.Begin();
	List<Node*>::ConstIterator end = adjacentNodes.End();

	for (; it && it != end; it++)
	{
		// If the node is being processed by the DFS then we have found a cycle inside the graph
		if (it->GetColor() == Node::NodeColor::NC_Gray)
			return false;

		// If the node hasn't been processed we run IsCyclicUtility on it to see if we can find a loop starting from it
		if (it->GetColor() == Node::NodeColor::NC_White && MarkReachableNodes(*it) == false)
			return false;
	}

	// Mark the node so that we know it has been processed correctly
	node->SetColor(Node::NodeColor::NC_Black);

	// We haven't found a cycle starting from the given node
	// so the nodes were marked correctly
	return true;
}

/** Returns true if the node hasn't got any entrant edge */
bool Graph::IsNonEntrantNode(const Node* node) const
{
	// Check if the node is valid
	if (node == nullptr)
		throw "Graph error [IsNonEntrantNode]: node not valid.";

	EdgeList::ConstIterator it = _edges.Begin();
	EdgeList::ConstIterator end = _edges.End();

	// Loop through all the edges of the graph.
	// As soon as we find an edge which end node is the one given as input we have found that the node has an entrant edge
	for (; it && it != end; it++)
	{
		if ((*it).GetEndNode() == node)
			return false;
	}

	// There wasn't any edge which has the node given as input as end node so se node is non-entrant
	return true;
}

/** Returns a list containing the pointer to all the nodes that don't have any edge or only have entrant edges */
Graph::NodePointersList Graph::GetNonEntrantNodes()
{
	Graph::NodePointersList result;

	NodeList::Iterator it = _nodes.Begin();
	NodeList::Iterator end = _nodes.End();

	// Loop through the nodes. If a node hasn't got any adjacent node then is doe
	for (; it && it != end; it++)
	{
		// Do this to save one method call
		Node* node = &(*it);

		if (IsNonEntrantNode(node) == true)
			result.Add(node);
	}

	return result;
}

/**
* Given a node as root, computes the best paths that connects the root to all the nodes that it can reach. Works event if there is a cycle
* root: the node which is to be used as root
* setupInitialDistances: if this is true an additional loop is added at the beginning to setup the initial distances. Pass this to true if you don't know if a previous call to ComputeBestPathsFromRoot was made
*/
void Graph::ComputeBestPathsFromRoot(Node* root, bool setupInitialDistances)
{
	// Make sure the given root is valid
	if (root == nullptr)
		return;

	if (setupInitialDistances)
	{
		NodeList::Iterator it = _nodes.Begin();
		NodeList::Iterator end = _nodes.End();

		// Loop through the nodes. If a node hasn't got any adjacent node then is doe
		for (; it && it != end; it++)
			(*it).SetDistanceFromRoot(-1);
	}

	// The root has distance 0 to itself
	root->SetDistanceFromRoot(0);

	const List<Node*>& adjacentNodes = root->GetAdjacentNodes();

	List<Node*>::ConstIterator adjacentNodesIt = adjacentNodes.Begin();
	List<Node*>::ConstIterator adjacentNodesEnd = adjacentNodes.End();

	for (; adjacentNodesIt && adjacentNodesIt != adjacentNodesEnd; ++adjacentNodesIt)
		FindBestPath(root, root, *adjacentNodesIt, 1);
}

/**
* Traverse the graph updating the best path that connects the node 'start' to the node 'node'
* root: the root from where the path started
* start: the node previous to 'node' inside the path
* node: the node we are checking to see if we find a path shorter then the one it currently has
* distance: the distance from the node 'start' from the root node
*/
void Graph::FindBestPath(const Node* root, Node* start, Node* node, int distance)
{
	// Make sure the nodes are valid
	if (start == nullptr || node == nullptr)
		return;

	if (node->GetDistanceFromRoot() == -1)
	{
		// Find the edge that connects start with node
		bool found = false;
		Edge* connectingEdge = &_edges.FindElement(EdgeComparatorWithNodesPointer(start, node, _graphType), found);

		// If the edge was found set it as the best edge for node and set the node's distance from the root
		if (found)
		{
			connectingEdge->SetAttribute("style", "dashed", false, false);

			node->SetEdgeForBestPath(connectingEdge);
			node->SetDistanceFromRoot(distance);

			// Set the label for the node
			node->SetAttribute("label", "d(" + root->GetName() + "," + node->GetName() + " = " + std::to_string(distance) + ")", false, true);
		}

		// Iterator over the adjacent nodes of node to adjust the distances from them to the root
		const List<Node*>& adjacentNodes = node->GetAdjacentNodes();

		List<Node*>::ConstIterator adjacentNodesIt = adjacentNodes.Begin();
		List<Node*>::ConstIterator adjacentNodesEnd = adjacentNodes.End();

		for (; adjacentNodesIt && adjacentNodesIt != adjacentNodesEnd; ++adjacentNodesIt)
			FindBestPath(root, node, *adjacentNodesIt, distance + 1);
	}
	else if (distance < node->GetDistanceFromRoot())
	{
		// Find the edge that connects start with node
		bool found = false;
		Edge* connectingEdge = &_edges.FindElement(EdgeComparatorWithNodesPointer(start, node, _graphType), found);

		// If the edge was found set it as the best edge for node and set the node's distance from the root
		if (found)
		{
			// If a node aready had a best edge and it is different from the current one, remove the dashed mark from that edge
			Edge* currentEdgeForBestPath = node->GetEdgeForBestPath();
			
			if (currentEdgeForBestPath != nullptr)
			{
				// If the edge that connects start with node is different then remove the dashed mark from that edge,
				// add the dashed mark to the new edge and set it as the best edge from start to node
				if (currentEdgeForBestPath != connectingEdge)
				{
					currentEdgeForBestPath->RemoveAttribute("style");
					connectingEdge->SetAttribute("style", "dashed", false, false);

					node->SetEdgeForBestPath(connectingEdge);
				}
			}
			else
			{
				node->SetEdgeForBestPath(connectingEdge);
				connectingEdge->SetAttribute("style", "dashed", false, false);
			}

			node->SetDistanceFromRoot(distance);

			// Set the label for the node
			node->SetAttribute("label", "d(" + root->GetName() + "," + node->GetName() + " = " + std::to_string(distance) + ")", false, true);

			// Iterator over the adjacent nodes of node to adjust the distances from them to the root
			const List<Node*>& adjacentNodes = node->GetAdjacentNodes();

			List<Node*>::ConstIterator adjacentNodesIt = adjacentNodes.Begin();
			List<Node*>::ConstIterator adjacentNodesEnd = adjacentNodes.End();

			for (; adjacentNodesIt && adjacentNodesIt != adjacentNodesEnd; ++adjacentNodesIt)
				FindBestPath(root, node, *adjacentNodesIt, distance + 1);
		}
	}
}

/** Removes all the edges marked as added by ASDProjectSolver */
Graph& Graph::RemoveEdgesAddedByASDProjectSolver()
{
	EdgeList::Iterator it = _edges.Begin();
	EdgeList::Iterator end = _edges.End();

	// Loop through all the edges
	while (it && it != end)
	{
		// If the edge was added by ASDPorjectSolver remove it. The Remove method moves 'it' forward
		if ((*it).IsAddedBySolver())
			RemoveEdgeWithIterator(it);
		// Otherwise move to the next edge
		else
			it++;
	}

	return *this;
}