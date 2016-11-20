#include "Edge.h"
#include "Node.h"
#include "Graph.h"

/**
* Element destructor used by the list of edges of Graph.
* Deletes the edge and sets it to nullptr
*/
struct EdgeDestructor
{
	inline void operator()(Edge*& edge)
	{
		// Make sure the edge is valid and then destroy it
		if (edge != nullptr)
		{
			delete edge;
			edge = nullptr;
		}
	}
};

/**
* Element destructor used by the list of nodes of Graph.
* Deletes the node and sets it to nullptr
*/
struct NodeDestructor
{
	inline void operator()(Node*& node)
	{
		// Make sure the node is valid and then destroy it
		if (node != nullptr)
		{
			delete node;
			node = nullptr;
		}
	}
};

/**
* Comparator used by Graph::AddEdge to check if an edge with the
* given start and end nodes exists
*/ 
struct EdgeComparator
{
	EdgeComparator(const Node* startNode, const Node* endNode)
		: _startNode(startNode)
		, _endNode(endNode)
	{ }

	inline bool operator()(const Edge* edge)
	{
		return edge->GetStartNode() == _startNode && edge->GetEndNode() == _endNode;
	}

	const Node* _startNode;
	const Node* _endNode;
};


/** Default constructor */
Graph::Graph()
{ }

/** Copy constructor */
Graph::Graph(const Graph& src)
{
	Copy(src);
}

/** Move constructor */
Graph::Graph(Graph&& src)
	: _edges(src._edges)
	, _nodes(src._nodes)
{
	src._edges.Clear();
	src._nodes.Clear();
}

/** Destructor */
Graph::~Graph()
{
	_edges.Clear();
	_nodes.Clear();
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
		Copy(src);

		src._edges.Clear();
		src._nodes.Clear();
	}
	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void Graph::Copy(const Graph& src)
{
	_edges = src._edges;
	_nodes = src._nodes;
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

	if (_edges.Find(EdgeComparator(startNode, endNode)) != nullptr)
	{
		std::cerr << "Graph error [AddEdge]: an edge with the given start and end nodes already exists" << std::endl;
		return nullptr;
	}

	// Create the new edge and add it to the list of edges of the graph
	Edge* edge = new Edge(startNode, endNode);
	_edges.Add(edge);

	// Add the new edge to the list of connected edges of startNode and endNode
	startNode->AddEdge(edge);
	endNode->AddEdge(edge);

	return edge;
}

/**
* Gets the edge at the given index from inside the list of edges of this graph
* If the given index is not valid returns nullptr
*/
Edge* Graph::GetEdge(int index)
{
	// Check if the index is valid
	if (index >= 0 && index < _edges.GetSize())
		return _edges.GetAt(index);
	return nullptr;
}

/**
* Creates a new node with the given name
* Returns a pointer to the newly created node or nullptr if errors occurs
* Possible errors are:
*  - the name is empty
* Derived classes that override this method must ensure that the previous errors are checked
*/
Node* Graph::AddNode(const std::string& name)
{
	if (name.empty())
	{
		std::cerr << "Graph error [AddNode]: adding a node without name" << std::endl;
		return nullptr;
	}

	// Create the new node and add it to the list of nodes of the graph
	Node* node = new Node(name);
	_nodes.Add(node);

	return node;
}

/**
* Gets the node at the given index from inside the list of nodes of this graph
* If the given index is not valid returns nullptr
*/
Node* Graph::GetNode(int index)
{
	// Check if the index is valid
	if (index >= 0 && index < _nodes.GetSize())
		return _nodes.GetAt(index);
	return nullptr;
}