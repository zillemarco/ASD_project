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
		TOK_NotValid
	};

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

private:
	bool ParseID(std::string& id, bool& enclosedWithDoubleQuotes, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);
	bool ParseStatementList(Graph& graph, bool bracketAlreadyFound, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	bool MoveToFirstOccurenceOfChar(char c, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	void RemoveSpaces(int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	Token ParseToken(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	void ReadUntilSpaces(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	bool ReadComment(std::string& result, bool singleLine, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	bool ParseAttributesList(GraphElement* element, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

	bool ParseEdgeList(Graph& graph, const std::string& firstNodeId, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength);

private:
	/** The line where the parser is currently at during parsing. Useful for error logs */
	int _lineNumber;

	/** The character of the line where the parser is currently at during parsing. Useful for error logs */
	int _columnNumber;
};