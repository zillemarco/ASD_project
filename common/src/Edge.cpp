#include "Node.h"
#include "Edge.h"

/** Default constructor */
Edge::Edge()
	: _startNode(nullptr)
	, _endNode(nullptr)
{ }

/**
* Constructor for initialization
* startNode: is the starting node of this edge. Needs to be non-nullptr
* endNode: is the ending node of this edge. Needs to be non-nullptr
*/
Edge::Edge(Node* startNode, Node* endNode)
	: _startNode(startNode)
	, _endNode(endNode)
{
	if (_startNode == nullptr)
	{
		// Output the error to the standard output and throw an exception to stop the execution
		std::cerr << "Edge error [ctor]: invalid start node. It was given as nullptr" << std::endl;
		throw "Edge error [ctor]: invalid start node. It was given as nullptr";
	}

	if (_endNode == nullptr)
	{
		// Output the error to the standard output and throw an exception to stop the execution
		std::cerr << "Edge error [ctor]: invalid end node. It was given as nullptr" << std::endl;
		throw "Edge error [ctor]: invalid start end. It was given as nullptr";
	}
}

/** Copy constructor */
Edge::Edge(const Edge& src)
	: _startNode(nullptr)
	, _endNode(nullptr)
{
	Copy(src);
}

/** Move constructor */
Edge::Edge(Edge&& src)
	: _startNode(std::move(src._startNode))
	, _endNode(std::move(src._endNode))
{
	src._startNode = nullptr;
	src._endNode = nullptr;
}

/** Destructor */
Edge::~Edge()
{
	_startNode = nullptr;
	_endNode = nullptr;
}

/** Assign operator */
Edge& Edge::operator=(const Edge& src)
{
	if (this != &src)
		Copy(src);
	return *this;
}

/** Move operator */
Edge& Edge::operator=(Edge&& src)
{
	if (this != &src)
	{
		Copy(src);

		src._startNode = nullptr;
		src._endNode = nullptr;
	}
	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void Edge::Copy(const Edge& src)
{
	if(src._startNode != nullptr)
		SetStartNode(std::move(src._startNode));

	if(src._endNode != nullptr)
		SetEndNode(std::move(src._endNode));
}

/** Sets both the start and end nodes of this edge */
Edge& Edge::SetNodes(Node* startNode, Node* endNode)
{
	// Check if the given nodes are valid
	if (startNode == nullptr)
	{
		std::cerr << "Edge error [SetNodes]: invalid start node. It was given as nullptr" << std::endl;
		return *this;
	}

	if (endNode == nullptr)
	{
		std::cerr << "Edge error [SetNodes]: invalid end node. It was given as nullptr" << std::endl;
		return *this;
	}

	if (startNode == endNode)
	{
		std::cerr << "Edge error [SetNodes]: start node and end node are the same" << std::endl;
		return *this;
	}

	// Only if the given nodes are valid save them into the edge
	_startNode = startNode;
	_endNode = endNode;

	return *this;
}