#include "Graph.h"

#include "Stack.h"

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
	_edges.Clear(0, true);
	_nodes.Clear(0, true);
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
	_edges = src._edges;
	_nodes = src._nodes;
	_name = src._name;
	_encloseNameInDoubleQuotes = src._encloseNameInDoubleQuotes;
	_graphType = src._graphType;
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
		result;

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