#pragma once

#include "List.h"
#include "GraphElement.h"

// Forward declaration of used types
class Node;
class Graph;
class ASDProjectSolver;

class Edge : public GraphElement
{
	friend ContainerElementDefaultValue<Edge>;
	friend Graph;
	friend ASDProjectSolver;

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

	/** Equality operator. Checks the nodes of the edge */
	friend bool operator==(const Edge& lhs, const Edge& rhs)
	{
		if (lhs._startNode != rhs._startNode || lhs._endNode != rhs._endNode)
			return false;
		return true;
	}

	/** Inequality operator. Checks the nodes of the edge */
	friend bool operator!=(const Edge& lhs, const Edge& rhs) { return (lhs == rhs) == false; }

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
	inline Node* GetStartNode() { return _startNode; }

	/** Returns a pointer to the start node of this edge */
	inline const Node* GetStartNode() const { return _startNode; }
	
	/** Returns a pointer to the end node of this edge */
	inline Node* GetEndNode() { return _endNode; }

	/** Returns a pointer to the end node of this edge */
	inline const Node* GetEndNode() const { return _endNode; }
	
	/**
	* Returns true or false depending if this edge is valid or not.
	* To be valid the edge needs to have both start and end node different and as non-nullptr
	*/
	inline bool IsValid() const { return _startNode != nullptr && _endNode != nullptr && _startNode != _endNode; }

	/** Method to check is this edge was added by the ASDProjectSolver */
	inline bool IsAddedBySolver() const { return _addedBySolver; }

private:
	/** Method to set the value of _addedBySolver. Only ASDPorjectSolver may call this method */
	inline void SetAddedBySolver(bool addedBySolver) { _addedBySolver = addedBySolver; }

private:
	/** Pointer to the start node of this edge */
	Node* _startNode;

	/** Pointer to the end node of this edge */
	Node* _endNode;

	/** Flag set from ASDProjectSolver only to mark the edge if it was added */
	bool _addedBySolver;
};

template<> struct ContainerElementDefaultValue<Edge> { static Edge Value() { return Edge(); } };
template<> struct ContainerElementDefaultValue<Edge*> { static Edge* Value() { return nullptr; } };