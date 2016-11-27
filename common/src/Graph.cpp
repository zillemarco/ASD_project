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

/** Gets the node that has the given name. If the node isn't found returns nullptr */
Node* Graph::GetNode(const std::string& nodeName)
{
	bool found = false;
	Node* result = &_nodes.FindElement(NodeComparator(nodeName), found);

	if (found == true)
		return result;
	return nullptr;
}

/** Applies the DSF algorithm to see if the graph contains cycles inside */
bool Graph::IsCyclic() const
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
	
	// Create a copy of this graph to not modify it
	Graph tmpGraph(*this);

	NodeList::Iterator it = tmpGraph._nodes.Begin();
	NodeList::Iterator end = tmpGraph._nodes.End();

	// Loop through the nodes of the graph until we finish them or we find a cycle
	bool result = false;
	for (; it && it != end && result == false; it++)
	{
		// Do a DSF visit only if the node is 'white'
		if ((*it).GetColor() == Node::NodeColor::NC_White)
		{
			// Do the DSF visit to see if we can find a cycle starting from the node 'it'
			result = IsCyclicUtility(&(*it));
		}
	}

	// If result is still false the graph doesn't contain ant cycle
	return result;
}

bool Graph::IsCyclicUtility(Node* node)
{
	// Mark the node so that we know it is being processed
	node->SetColor(Node::NodeColor::NC_Gray);

	const List<Node*>& adjacentNodes = node->GetAdjacentNodes();

	List<Node*>::ConstIterator it = adjacentNodes.Begin();
	List<Node*>::ConstIterator end = adjacentNodes.End();

	for (; it && it != end; it++)
	{
		// If the node is being processed by the DFS then we have found a loop inside the graph
		if (it->GetColor() == Node::NodeColor::NC_Gray)
			return true;

		// If the node hasn't been processed we run IsCyclicUtility on it to see if we can find a loop starting from it
		if (it->GetColor() == Node::NodeColor::NC_White && IsCyclicUtility(*it))
			return true;
	}

	// Mark the node so that we know it has been processed correctly
	node->SetColor(Node::NodeColor::NC_Black);

	// We haven't found a cycle starting from the given node
	return false;
}