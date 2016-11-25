#include "Edge.h"
#include "Node.h"

/** Default constructor */
Node::Node()
	: GraphElement()
{ }

/** Constructor with basic initialization */
Node::Node(const std::string& name, bool encloseNameInDoubleQuotes)
	: GraphElement()
	, _name(name)
	, _encloseNameInDoubleQuotes(encloseNameInDoubleQuotes)
{ }

/** Copy constructor */
Node::Node(const Node& src)
	: GraphElement(src)
	, _edges(src._edges)
	, _name(src._name)
	, _encloseNameInDoubleQuotes(src._encloseNameInDoubleQuotes)
{ }

/** Move constructor */
Node::Node(Node&& src)
	: GraphElement(src)
	, _edges(std::move(src._edges))
	, _name(std::move(src._name))
	, _encloseNameInDoubleQuotes(std::move(src._encloseNameInDoubleQuotes))
{ }

/** Destructor */
Node::~Node()
{
	_edges.Clear();
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
		_edges.Clear();
		_edges = std::move(src._edges);
		_name = std::move(src._name);
		_encloseNameInDoubleQuotes = std::move(src._encloseNameInDoubleQuotes);
	}
	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void Node::Copy(const Node& src)
{
	_edges = src._edges;
	_name = src._name;
	_encloseNameInDoubleQuotes = src._encloseNameInDoubleQuotes;
}

/** Adds the given edge to the list of edges connected to this node */
Node& Node::AddEdge(Edge* edge)
{
	// Check if the edge is alredy present
	if (GetEdgeIndex(edge) == -1)
		_edges.Add(edge);
	return *this;
}

/** Removes the given edge from the list of edges connected to this node */
Node& Node::RemoveEdge(Edge* edge)
{
	// Get the edge index inside of the list
	int index = GetEdgeIndex(edge);

	// Check if the edge was found and remove it from the list
	if (index >= 0)
		_edges.RemoveAt(index);

	return *this;
}

/** Removes the edge from the list of edges connected to this node at the given index */
Node& Node::RemoveEdgeAt(int index)
{
	// Check if the index is valid and then remove the element from the list
	if (index >= 0 && index < _edges.GetSize())
		_edges.RemoveAt(index);
	return *this;
}

/**
* Returns true or false depending if this node is the start node of the given edge
* This also returns false if this node isn't connected with the given edge
*/
bool Node::IsStartingNode(Edge* edge) const
{
	// Check if the edge is valid and then if this node is the starting node of the edge
	// Is it sufficient to check if the starting node of the edge is this node to also ensure
	// that this node is connected to the edge (it is ensured by the Graph class)
	if (edge != nullptr)
		return edge->GetStartNode() == this;
	return false;
}

/**
* Returns true or false depending if this node is the end node of the given edge
* This also returns false if this node isn't connected with the given edge
*/
bool Node::IsEndingNode(Edge* edge) const
{
	// Check if the edge is valid and then if this node is the ending node of the edge
	// Is it sufficient to check if the ending node of the edge is this node to also ensure
	// that this node is connected to the edge (it is ensured by the Graph class)
	if (edge != nullptr)
		return edge->GetEndNode() == this;
	return false;
}

/** Returns true or false depending if this node is or isn't connected with the given edge */
bool Node::IsConnectedWithEdge(Edge* edge) const
{
	// Check if the edge is valid and then if this node is the ending node of the edge
	if (edge != nullptr)
		return (edge->GetStartNode() == this || edge->GetEndNode() == this);
	return false;
}

/**
* Returns the index of the given edge inside the list of edges connected with this node
* If the given edge isn't connected with this node this function returns -1
*/
int Node::GetEdgeIndex(Edge* edge) const
{
	// Here is used List::Find since it's faster that looping through the list
	// since every List::GetAt cycles through the items of the list
	return _edges.Find(edge);

	// This is the slow version
	//
	// int edgesCount = _edges.GetSize();
	// for (int i = 0; i < edgesCount; i++)
	// {
	// 	if (_edges[i] == edge)
	// 		return i;
	// }
	// return -1;
}