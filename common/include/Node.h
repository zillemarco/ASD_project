#pragma once

#include "List.h"
#include "GraphElement.h"

#include <string>

// Forward declaration of used types
class Edge;
class Graph;

class Node : public GraphElement
{
	friend ContainerElementDefaultValue<Node>;
	friend Graph;

public:
	/**
	* Local enum to use DSF
	* Make it derive from char to save 3 bytes (instead of using 4 bytes from int)
	*/
	enum class NodeColor : char
	{
		NC_White = 0,
		NC_Gray = 1,
		NC_Black = 2
	};

protected:
	/**
	* Make the constructors private to block the creation of nodes from unknown places.
	* The only way to create a node is by using Graph::AddNode
	*/

	/** Default constructor */
	Node();

	/** Constructor with basic initialization */
	Node(const std::string& name, bool encloseNameIdDoubleQuotes);

public:
	/** Copy constructor */
	Node(const Node& src);

	/** Move constructor */
	Node(Node&& src);

	/** Destructor */
	virtual ~Node();

public:
	/** Assign operator */
	Node& operator=(const Node& src);

	/** Move operator */
	Node& operator=(Node&& src);

	/** Equality operator. Checks the name, color and adjacency list of the nodes */
	friend bool operator==(const Node& lhs, const Node& rhs)
	{
		// Check the adjacency list at last to speed things up
		if (lhs._name != rhs._name || lhs._encloseNameInDoubleQuotes != rhs._encloseNameInDoubleQuotes || lhs._color != rhs._color || lhs._adjacentNodes != rhs._adjacentNodes)
			return false;
		return true;
	}

	/** Inequality operator. Checks the name, color and adjacency list of the nodes */
	friend bool operator!=(const Node& lhs, const Node& rhs) { return (lhs == rhs) == false; }

private:
	/** Utility function to use with copy constructor and assign operator */
	void Copy(const Node& src);

	/**
	* Adds the given node to the list of adjacent nodes of this node.
	* It checks if the ndode is alreay inside the list of adjacent nodes to not add it multiple times
	*/
	Node& AddAdjacentNode(Node* adjacentNode);

	/** Removes the given node from the list of adjacent nodes of this node */
	Node& RemoveAdjacentNode(const Node* adjacentNode);
		
public:
	/** Returns the name of this node */
	const std::string GetName() const { return _name; }

	/** Returns true if a write needs to enclose the name of this node between double quotes */
	bool EncloseNameInDoubleQuotes() const { return _encloseNameInDoubleQuotes; }

	/** Returns the list of nodes that can be reached directlt through this node */
	const List<Node*>& GetAdjacentNodes() const { return _adjacentNodes; }

	/** Returns true if there are nodes that can be reached directly from this node */
	bool HasAdjacentNodes() const { return _adjacentNodes.GetSize() > 0; }

	/**
	* Returns the index of the given node inside the list of adjacent nodes of this node
	* If the given node cannot be reached directly through this node returns -1
	*/
	int GetAdjacentNodeIndex(Node* adjacentNode) const;

	/** Sets the color of this node for DSF */
	void SetColor(NodeColor color) { _color = color; }

	/** Returns the color of this node for DSF */
	NodeColor GetColor() const { return _color; }

private:
	/** Name of the node */
	std::string _name;

	/** If true then a writer needs to enclose the name of this node between double quotes */
	bool _encloseNameInDoubleQuotes;

	/** List of nodes that can be reached directly from this node */
	List<Node*> _adjacentNodes;

	/** The color of this node when using DSF */
	NodeColor _color;
};

template<> struct ContainerElementDefaultValue<Node> { static Node Value() { return Node(); } };
template<> struct ContainerElementDefaultValue<Node*> { static Node* Value() { return nullptr; } };