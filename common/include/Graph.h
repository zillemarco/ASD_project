#pragma once

#include "List.h"
#include "Edge.h"
#include "Node.h"

#include <string>

/**
* Element destructor used by the list of edges of Graph.
* Deletes the edge and sets it to nullptr
*/
struct EdgeListDestructor
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
struct NodeListDestructor
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

class Graph
{
	typedef List<Edge*, EdgeListDestructor> EdgeList;
	typedef List<Node*, NodeListDestructor> NodeList;

public:
	enum GraphType
	{
		GT_Simple,
		GT_Directed,
		GT_NotValid
	};

public:
	/** Default constructor */
	Graph();

	/** Constructor for basic initialization */
	Graph(const std::string& name, GraphType type);
	
	/** Copy constructor */
	Graph(const Graph& src);

	/** Move constructor */
	Graph(Graph&& src);

	/** Destructor */
	~Graph();

public:
	/** Assign operator */
	Graph& operator=(const Graph& src);

	/** Move operator */
	Graph& operator=(Graph&& src);

private:
	/** Utility function to use with copy constructor and assign operator */
	void Copy(const Graph& src);

public:
	/** Sets the type of this graph as the one given */
	Graph& SetGraphType(GraphType type) { _graphType = type; return *this; }

	/** Returns the type of this graph */
	inline GraphType GetGraphType() const { return _graphType; }

	/** Sets the name of this graph to the one given */
	Graph& SetName(const std::string& name) { _name = name; return *this; }

	/** Returns the name of this graph */
	inline const std::string& GetName() const { return _name; }

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
	virtual Edge* AddEdge(Node* startNode, Node* endNode);

	/**
	* Gets the edge at the given index from inside the list of edges of this graph
	* If the given index is not valid returns nullptr
	*/
	Edge* GetEdge(int index);
	
	/** Returns the list of edges of this graph */
	const EdgeList& GetEdges() const { return _edges; }
	
	/**
	* Creates a new node with the given name
	* Returns a pointer to the newly created node or nullptr if errors occurs
	* Possible errors are:
	*  - the name is empty
	* Derived classes that override this method must ensure that the previous errors are checked
	*/
	virtual Node* AddNode(const std::string& name);

	/**
	* Gets the node at the given index from inside the list of nodes of this graph
	* If the given index is not valid returns nullptr
	*/
	Node* GetNode(int index);

	/** Returns the list of edges of this graph */
	const NodeList& GetNodes() const { return _nodes; }
	
private:
	/** The type of this graph */
	GraphType _graphType;

	/** The name of this graph */
	std::string _name;

	/** List of all the edges of this graph */
	EdgeList _edges;

	/** List of all the nodes of this graph */
	NodeList _nodes;
};