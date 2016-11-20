#pragma once

#include "List.h"

#include <string>

// Forward declaration of used types
class Edge;
class Graph;

class Node
{
	friend Graph;

protected:
	/**
	* Make the constructors private to block the creation of nodes from unknown places.
	* The only way to create a node is by using Graph::AddNode
	*/

	/** Default constructor */
	Node();

	/** Constructor with basic initialization */
	Node(const std::string& name);

public:
	/** Copy constructor */
	Node(const Node& src);

	/** Move constructor */
	Node(Node&& src);

	/** Destructor */
	~Node();

public:
	/** Assign operator */
	Node& operator=(const Node& src);

	/** Move operator */
	Node& operator=(Node&& src);

private:
	/** Utility function to use with copy constructor and assign operator */
	void Copy(const Node& src);

	/**
	* Adds the given edge to the list of edges connected to this node
	* It checks if the edge is alreay inside the list of edges connected with this node
	*/
	Node& AddEdge(Edge* edge);

	/** Removes the given edge from the list of edges connected to this node */
	Node& RemoveEdge(Edge* edge);

	/** Removes the edge from the list of edges connected to this node at the given index */
	Node& RemoveEdgeAt(int index);
	
public:
	/** Returns the name of this node */
	const std::string GetName() const { return _name; }

	/** Returns the list of edges connected to this node */
	const List<Edge*>& GetEdges() const { return _edges; }

	/**
	* Returns true or false depending if this node is the start node of the given edge
	* This also returns false if this node isn't connected with the given edge
	*/
	bool IsStartingNode(Edge* edge) const;

	/**
	* Returns true or false depending if this node is the end node of the given edge
	* This also returns false if this node isn't connected with the given edge
	*/
	bool IsEndingNode(Edge* edge) const;

	/** Returns true or false depending if this node is or isn't connected with the given edge */
	bool IsConnectedWithEdge(Edge* edge) const;

	/**
	* Returns the index of the given edge inside the list of edges connected with this node
	* If the given edge isn't connected with this node this function returns -1
	*/
	int GetEdgeIndex(Edge* edge) const;

private:
	/** Represents the name of the node */
	std::string _name;

	/** List of edges connected to this node */
	List<Edge*> _edges;
};