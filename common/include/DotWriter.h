#pragma once

#include "Graph.h"

//#include <fstream>
//#include <sstream>
//#include <iostream>

class DotWriter
{
public:
	/** Default constructor */
	DotWriter() { }
	
	/** Destructor */
	~DotWriter() { }
		
public:
	static bool Write(const Graph& graph, std::ostream& output);
};