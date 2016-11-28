#pragma once

#include "Graph.h"

#include <fstream>
#include <sstream>
#include <iostream>

class ASDProjectSolver
{
public:
	static bool ProcessData(Graph& result, int argc, char *argv[]);
	static bool ProcessData(Graph& result, const std::string& dotFileContent);

private:
	static Node* FindBestRoot(Graph& graph, int& addedEdges);

	static int AddEdgesToRoot(Graph& graph, Node* root, Graph::NodePointersList& unreachableNodes);
	
public:
	/** Method that gets the input file content (both from standard input or from the first argument of the process) */
	static std::string GetInputFileContent(int argc, char *argv[]);

	/** Method that returns a string which contains all the content of 'input' */
	static std::string ReadFile(std::istream& input);
};