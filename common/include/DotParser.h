#pragma once

#include "Graph.h"

/**
* The parser is desinged after a subset of the specification
* found at the page http://www.graphviz.org/doc/info/lang.html
*/
class DotParser
{
	enum Token
	{
		TOK_Strict,
		TOK_Graph,
		TOK_Digraph,
		TOK_Node,
		TOK_Edge,
		TOK_Id,
		TOK_OpenBracket,
		TOK_ClosedBraket,
		TOK_OpenSquareBracket,
		TOK_ClosedSquareBraket,
		TOK_Semicolon,
		TOK_Comment,
		TOK_Equal,
		TOK_SimpleEdgeSymbol,
		TOK_DirectedEdgeSymbol,
		TOK_Coma,
		TOK_NotValid
	};

public:
	/** Default constructor */
	DotParser() { }
	
	/** Destructor */
	~DotParser() { }
		
public:
	static bool Parse(Graph& resultGraph, const std::string& dotDefinition);

private:
	static bool ParseID(std::string& id, bool& enclosedWithDoubleQuotes, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);
	static bool ParseStatementList(Graph& graph, bool bracketAlreadyFound, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static bool MoveToFirstOccurenceOfChar(char c, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static void RemoveSpaces(int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static Token ParseToken(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static void ReadUntilSpaces(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static bool ReadComment(std::string& result, bool singleLine, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static bool ParseAttributesList(GraphElement* singleElement, List<GraphElement*>* elements, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);

	static bool ParseEdgeList(Graph& graph, const std::string& firstNodeId, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber);
};