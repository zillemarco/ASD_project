#pragma once

#include "Graph.h"

#include <fstream>
#include <sstream>
#include <iostream>

class ASDProjectSolver
{
public:
	/** Process the data reading the DOT file from the standard input or the input file and returns the result into the 'result' graph */
	static bool ProcessData(Graph& result, int argc, char *argv[]);

	/** Process the data reading the DOT data from the given dotFileContent and returns the result into the 'result' graph */
	static bool ProcessData(Graph& result, const std::string& dotFileContent);

	/** Process the given graph and modifies it to solve the project */
	static bool ProcessData(Graph& result);
	
private:
	static Node* FindBestRoot(Graph& graph, int& addedEdges);

	static int AddEdgesToRoot(Graph& graph, Node* root, Graph::NodePointersList& unreachableNodes);
	
public:
	/** Method that gets the input file content (both from standard input or from the first argument of the process) */
	static std::string GetInputFileContent(int argc, char *argv[]);

	/** Method that returns a string which contains all the content of 'input' */
	static std::string ReadFile(std::istream& input);
};