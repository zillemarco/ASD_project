#include "Edge.h"
#include "Node.h"

/** Comparator used by Node::RemoveAdjacentNode to check if an adjacent node exists inside the list of adjacent nodes of a node */
struct AdjacentNodeComparator
{
	AdjacentNodeComparator(const Node* adjacentNode)
		: _adjacentNode(adjacentNode)
	{ }

	inline bool operator()(const Node* node)
	{
		if (_adjacentNode == node)
			return true;
		return false;
	}

	const Node* _adjacentNode;
};
/** Default constructor */
Node::Node()
	: GraphElement()
{ }

/** Constructor with basic initialization */
Node::Node(const std::string& name, bool encloseNameInDoubleQuotes)
	: GraphElement()
	, _name(name)
	, _encloseNameInDoubleQuotes(encloseNameInDoubleQuotes)
	, _color(NodeColor::NC_White)
	, _distanceFromRoot(-1)
	, _edgeForBestPath(nullptr)
{ }

/** Copy constructor */
Node::Node(const Node& src)
	: GraphElement(src)
	, _adjacentNodes(src._adjacentNodes)
	, _name(src._name)
	, _encloseNameInDoubleQuotes(src._encloseNameInDoubleQuotes)
	, _color(src._color)
	, _distanceFromRoot(src._distanceFromRoot)
	, _edgeForBestPath(src._edgeForBestPath)
{ }

/** Move constructor */
Node::Node(Node&& src)
	: GraphElement(src)
	, _adjacentNodes(std::move(src._adjacentNodes))
	, _name(std::move(src._name))
	, _encloseNameInDoubleQuotes(std::move(src._encloseNameInDoubleQuotes))
	, _color(std::move(src._color))
	, _distanceFromRoot(std::move(src._distanceFromRoot))
	, _edgeForBestPath(std::move(src._edgeForBestPath))
{ }

/** Destructor */
Node::~Node()
{
	_adjacentNodes.Clear();
}

/** Assign operator */
Node& Node::operator=(const Node& src)
{
	GraphElement::operator=(src);

	if (this != &src)
		Copy(src);
	return *this;
}

/** Move operator */
Node& Node::operator=(Node&& src)
{
	GraphElement::operator=(src);

	if (this != &src)
	{
		_adjacentNodes.Clear();
		_adjacentNodes = std::move(src._adjacentNodes);
		_name = std::move(src._name);
		_encloseNameInDoubleQuotes = std::move(src._encloseNameInDoubleQuotes);
		_color = std::move(src._color);
		_distanceFromRoot = std::move(src._distanceFromRoot);
		_edgeForBestPath = std::move(src._edgeForBestPath);
	}
	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void Node::Copy(const Node& src)
{
	_adjacentNodes = src._adjacentNodes;
	_name = src._name;
	_encloseNameInDoubleQuotes = src._encloseNameInDoubleQuotes;
	_color = src._color;
	_distanceFromRoot = src._distanceFromRoot;
	_edgeForBestPath = src._edgeForBestPath;
}

/**
* Adds the given node to the list of adjacent nodes of this node.
* It checks if the ndode is alreay inside the list of adjacent nodes to not add it multiple times
*/
Node& Node::AddAdjacentNode(Node* adjacentNode)
{
	// Check if the edge is alredy present
	if (GetAdjacentNodeIndex(adjacentNode) == -1)
		_adjacentNodes.Add(adjacentNode);
	return *this;
}

/** Removes the given node from the list of adjacent nodes of this node */
Node& Node::RemoveAdjacentNode(const Node* adjacentNode)
{
	// Find the adjacent node inside the list of adjacent nodes of this node
	List<Node*>::Iterator it = _adjacentNodes.FindIterator(AdjacentNodeComparator(adjacentNode));

	// If the adjacent node was found remove it
	if(it)
		_adjacentNodes.Remove(it);

	return *this;
}

/**
* Returns the index of the given node inside the list of adjacent nodes of this node
* If the given node cannot be reached directly through this node returns -1
*/
int Node::GetAdjacentNodeIndex(Node* adjacentNode) const
{
	// Here is used List::Find since it's faster that looping through the list
	// since every List::GetAt cycles through the items of the list
	return _adjacentNodes.Find(adjacentNode);

	// This is the slow version
	//
	// int adjacentNodesCount = _adjacentNodes.GetSize();
	// for (int i = 0; i < adjacentNodesCount; i++)
	// {
	// 	if (_adjacentNodes[i] == adjacentNode)
	// 		return i;
	// }
	// return -1;
}