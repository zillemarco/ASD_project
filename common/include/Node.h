#pragma once

class Edge;

class Node
{
public:
	/** Default constructor */
	Node();

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

private:
	/** Represents the name of the node */
	const char* _name;
};