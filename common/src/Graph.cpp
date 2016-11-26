#include "Graph.h"

#include "Stack.h"

/** Extended node used by the Tarjan algorithm */
class TarjanNode : public Node
{
public:
	TarjanNode()
		: _index(0)
		, _lowLink(0)
	{ }

	TarjanNode(const Node& sourceNode, int index, int lowLink)
		: Node(sourceNode)
		, _index(index)
		, _lowLink(lowLink)
	{ }

	TarjanNode(const TarjanNode& src)
		: Node(src)
		, _index(src._index)
		, _lowLink(src._lowLink)
	{ }

	TarjanNode(TarjanNode&& src)
		: Node(std::move(src))
		, _index(std::move(src._index))
		, _lowLink(std::move(src._lowLink))
	{ }

	TarjanNode& operator=(const TarjanNode& src)
	{
		Node::operator=(src);

		if (this != &src)
		{
			_index = src._index;
			_lowLink = src._lowLink;
		}

		return *this;
	}

	TarjanNode& operator=(TarjanNode&& src)
	{
		Node::operator=(src);

		if (this != &src)
		{
			_index = std::move(src._index);
			_lowLink = std::move(src._lowLink);
		}

		return *this;
	}

public:
	/** Sets the index value */
	TarjanNode& SetIndex(int index) { _index = index; return *this; }

	/** Returns the index value */
	int SetIndex() const { return _index; }

	/** Sets the low link value */
	TarjanNode& SetLowLink(int lowLink) { _lowLink = lowLink; return *this; }

	/** Returns the low link value */
	int GetLowLink() const { return _index; }

private:
	int _index;
	int _lowLink;
};

template<> struct ContainerElementDefaultValue<TarjanNode> { static TarjanNode Value() { return TarjanNode(); } };

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
		if(edge != nullptr)
			return edge->GetStartNode() == _startNode && edge->GetEndNode() == _endNode;
		return false;
	}

	const Node* _startNode;
	const Node* _endNode;
};

/** Comparator used by Graph::GetNode to get a node from a given name */
struct NodeComparator
{
	NodeComparator(const std::string& name)
		: _name(name)
	{ }

	inline bool operator()(const Node* node)
	{
		if(node != nullptr)
			return node->GetName() == _name;
		return false;
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

	if (_edges.Find(EdgeComparator(startNode, endNode)) != -1)
	{
		std::cerr << "Graph error [AddEdge]: an edge with the given start and end nodes already exists" << std::endl;
		return nullptr;
	}

	// Create the new edge and add it to the list of edges of the graph
	Edge* edge = CreateEdge(startNode, endNode);

	// Add the edge only if it was created successfully
	if (edge != nullptr)
	{
		_edges.Add(edge);

		// Add the new edge to the list of connected edges of startNode and endNode
		startNode->AddEdge(edge);
		endNode->AddEdge(edge);
	}

	return edge;
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
		return _edges.GetAt(index);
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
		node = CreateNode(name, encloseNodeNameInDoubleQuotes);

		// Add the node only if it was created successfully
		if (node != nullptr)
			_nodes.Add(node);

		return node;
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
		return _nodes.GetAt(index);
	return nullptr;
}

/** Gets the node that has the given name. If the node isn't found returns nullptr */
Node* Graph::GetNode(const std::string& nodeName)
{
	bool found = false;
	return _nodes.FindElement(NodeComparator(nodeName), found);
}

/**
* Applies the Tarjan's altgorithm to return all the strongly connected components of this graph.
* Only works if the graph is directed.
* Returns false if there are errors.
*/
bool Graph::Tarjan(List<Graph>& resultGraphs) const
{
	// This algorithm works only for directed graphs
	if (_graphType != GT_Directed)
		return false;

	Stack<TarjanNode> stack;
	int index = 0;



	return true;
}