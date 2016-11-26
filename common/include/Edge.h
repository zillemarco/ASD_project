#pragma once

#include "List.h"
#include "GraphElement.h"

// Forward declaration of used types
class Node;
class Graph;

class Edge : public GraphElement
{
	friend Graph;

protected:
	/**
	* Make the constructors private to block the creation of edges from unknown places.
	* The only way to create an edge is by using Graph::AddEdge
	*/

	/** Default constructor */
	Edge();

	/**
	* Constructor for initialization
	* startNode: is the starting node of this edge. Needs to be non-nullptr
	* endNode: is the ending node of this edge. Needs to be non-nullptr
	*/
	Edge(Node* startNode, Node* endNode);

public:
	/** Copy constructor */
	Edge(const Edge& src);

	/** Move constructor */
	Edge(Edge&& src);

	/** Destructor */
	~Edge();

public:
	/** Assign operator */
	Edge& operator=(const Edge& src);

	/** Move operator */
	Edge& operator=(Edge&& src);

private:
	/** Utility function to use with copy constructor and assign operator */
	void Copy(const Edge& src);

private:
	/** Sets the start node of this edge */
	inline Edge& SetStartNode(Node* node) { _startNode = node; return *this; }

	/** Sets the end node of this edge */
	inline Edge& SetEndNode(Node* node) { _endNode = node; return *this; }

	/** Sets both the start and end nodes of this edge */
	Edge& SetNodes(Node* startNode, Node* endNode);

public:
	/** Returns a pointer to the start node of this edge */
	inline const Node* GetStartNode() const { return _startNode; }
		
	/** Returns a pointer to the end node of this edge */
	inline const Node* GetEndNode() const { return _endNode; }
	
	/**
	* Returns true or false depending if this edge is valid or not.
	* To be valid the edge needs to have both start and end node different and as non-nullptr
	*/
	inline bool IsValid() const { return _startNode != nullptr && _endNode != nullptr && _startNode != _endNode; }

private:
	/** Pointer to the start node of this edge */
	Node* _startNode;

	/** Pointer to the end node of this edge */
	Node* _endNode;
};

template<> struct ContainerElementDefaultValue<Edge*> { static Edge* Value() { return nullptr; } };