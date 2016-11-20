#pragma once

#include "Graph.h"

/**
* The parser is desinged after a subset of the specification
* found at the page http://www.graphviz.org/doc/info/lang.html
*/
class DotParser
{
public:
	/** Default constructor */
	DotParser();
	
	/** Copy constructor */
	DotParser(const DotParser& src);

	/** Move constructor */
	DotParser(DotParser&& src);

	/** Destructor */
	~DotParser();

public:
	/** Assign operator */
	DotParser& operator=(const DotParser& src);

	/** Move operator */
	DotParser& operator=(DotParser&& src);
	
public:
	Graph Parse(const std::string& dotDefinition);
};