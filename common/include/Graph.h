#pragma once

#include "List.h"
#include "Edge.h"
#include "Node.h"

#include <string>

class Graph
{
public:
	typedef List<Edge> EdgeList;
	typedef List<Node> NodeList;
	typedef List<Node*, ContainerDefaultElementDestructor<Node*>, false> NodePointersList;

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
	Graph(const std::string& name, bool encloseNameInDoubleQuotes, GraphType type);
	
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
	Graph& SetName(const std::string& name, bool encloseNameInDoubleQuotes) { _name = name; _encloseNameInDoubleQuotes = encloseNameInDoubleQuotes; return *this; }

	/** Returns the name of this graph */
	inline const std::string& GetName() const { return _name; }

	/** Returns true if a write needs to enclose the name of this graph between double quotes */
	bool EncloseNameInDoubleQuotes() const { return _encloseNameInDoubleQuotes; }

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
	* Creates a new edge with the given nodes names as starting and ending nodes and adds it
	* to the list of edges of this grpah. The given nodes must be valid (non-empty).
	* If the nodes with the given names don't exist they're added automatically.
	* This function also adds the newly created edge to the list of edges of the given nodes.
	* Returns a pointer to the newly created edge or nullptr if errors occurs.
	* Possible errors are:
	*  - one or both the nodes names are not valid
	*  - an edge with startNode as starting node and endNode as ending node already exists
	*/
	Edge* AddEdge(const std::string& startNodeName, const std::string& endNodeName, bool encloseStartNodeNameInDoubleQuotes, bool encloseEndNodeNameInDoubleQuotes);

	/**
	* Gets the edge at the given index from inside the list of edges of this graph
	* If the given index is not valid returns nullptr
	*/
	Edge* GetEdge(int index);
	
	/** Returns the list of edges of this graph */
	const EdgeList& GetEdges() const { return _edges; }
	
	/**
	* Creates a new node with the given name or returns an existing one if it already exists a node with the given name.
	* Returns nullptr if errors occurs.
	* Possible errors are:
	*  - the name is empty
	* Derived classes that override this method must ensure that the previous errors are checked
	*/
	Node* AddNode(const std::string& name, bool encloseNodeNameInDoubleQuotes);

	/**
	* Gets the node at the given index from inside the list of nodes of this graph
	* If the given index is not valid returns nullptr
	*/
	Node* GetNode(int index);

	/**
	* Gets the node at the given index from inside the list of nodes of this graph
	* If the given index is not valid returns nullptr
	*/
	const Node* GetNode(int index) const;

	/** Gets the node that has the given name. If the node isn't found returns nullptr */
	Node* GetNode(const std::string& nodeName);

	/** Gets the node that has the given name. If the node isn't found returns nullptr */
	const Node* GetNode(const std::string& nodeName) const;

	/** Returns the list of edges of this graph */
	const NodeList& GetNodes() const { return _nodes; }

	/**
	* Applies the DSF algorithm to see if the graph contains cycles inside.
	* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
	* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
	*/
	bool IsCyclic(bool setNodesColorToWhiteAtStart = false, bool revertAllNodesToPreviousColor = false);

	/**
	* Runs through the entire list of nodes of this graph and returns a list of
	* the nodes that are not reachable from the given node.
	* node: the node from where to start the search
	* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
	* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
	*/
	NodePointersList GetUnreachableNodes(Node* node, bool setNodesColorToWhiteAtStart = false, bool revertAllNodesToPreviousColor = false);

	/**
	* Runs through the entire list of nodes of this graph and returns a list of
	* the nodes that are not reachable from the given node.
	* nodeName: the name of the node node from where to start the search
	* setNodesColorToWhiteAtStart: if the caller of this method is not sure that all the nodes are currently white-colord, pass this parameter as true
	* revertAllNodesToPreviousColor: if this is given as true then the color all the nodes of the graph is restored to the one previous to this call. It adds an extra loop through the node's list
	*/
	NodePointersList GetUnreachableNodes(const std::string& nodeName, bool setNodesColorToWhiteAtStart = false, bool revertAllNodesToPreviousColor = false);

	/** Returns true if the node hasn't got any entrant edge */
	bool IsNonEntrantNode(const Node* node) const;

	/** Returns a list containing the pointer to all the nodes that don't have any entrant edge */
	NodePointersList GetNonEntrantNodes();

protected:
	/**
	* This method iterates over the list of adjacent nodes of the given node
	* in a DFS way, checking that they're not gray-colored (in which case there is a cycle so we cannot continue and return false).
	* If they are not gray-colored they're set as so and calls MarkReachableNodes on the node. If all the adjacent nodes are ok the node
	* is set as black-colored.
	* At the end all the nodes of the graph which contains the given node will be either white or black. Is a node is white then it couldn't be reached by the node
	* given in input to the first call to MarkReachableNodes.
	* Returns false if a cycle is found.
	*/
	static bool MarkReachableNodes(Node* node);
		
private:
	/** The type of this graph */
	GraphType _graphType;

	/** The name of this graph */
	std::string _name;

	/** If true then a writer needs to enclose the name of this graph between double quotes */
	bool _encloseNameInDoubleQuotes;

	/** List of all the edges of this graph */
	EdgeList _edges;

	/** List of all the nodes of this graph */
	NodeList _nodes;
};