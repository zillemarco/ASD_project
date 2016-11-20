#pragma once

#include "List.h"

#include <string>

// Forward declaration of used types
class Edge;
class Node;

struct EdgeDestructor;
struct NodeDestructor;

class Graph
{
	typedef List<Edge*, nullptr, EdgeDestructor> EdgeList;
	typedef List<Node*, nullptr, NodeDestructor> NodeList;

public:
	/** Default constructor */
	Graph();
	
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
	/** List of all the edges of this graph */
	EdgeList _edges;

	/** List of all the nodes of this graph */
	NodeList _nodes;
};