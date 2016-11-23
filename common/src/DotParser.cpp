#include "Graph.h"
#include "DotParser.h"

#include <cctype>

/**
* Utility function to test if a character is alphabetical.
* A character is considered alphabetical if it is contained
* between A-Z and a-z (65-91 and 97-122 inside the ASCII table)
*/
static bool IsAlphabetic(int c)
{	
	return ((c >= 65 && c <= 91) || (c >= 97 && c <= 122));
}

/**
* Utility function to test if a character is a digit.
* A character is considered alphabetical if it is contained
* between 0-9 (48-57 inside the ASCII table)
*/
static bool IsDigit(int c)
{
	return (c >= 48 && c <= 57);
}

/** Default constructor */
DotParser::DotParser()
	: _lineNumber(0)
	, _columnNumber(0)
{ }

/** Copy constructor */
DotParser::DotParser(const DotParser& src)
	: _lineNumber(src._lineNumber)
	, _columnNumber(src._columnNumber)
{ }

/** Move constructor */
DotParser::DotParser(DotParser&& src)
	: _lineNumber(std::move(src._lineNumber))
	, _columnNumber(std::move(src._columnNumber))
{ }

/** Destructor */
DotParser::~DotParser()
{ }

/** Assign operator */
DotParser& DotParser::operator=(const DotParser& src)
{
	if (this != &src)
	{
		_lineNumber = src._lineNumber;
		_columnNumber = src._columnNumber;
	}

	return *this;
}

/** Move operator */
DotParser& DotParser::operator=(DotParser&& src)
{
	if (this != &src)
	{
		_lineNumber = std::move(src._lineNumber);
		_columnNumber = std::move(src._columnNumber);
	}

	return *this;
}

Graph DotParser::Parse(const std::string& dotDefinition)
{
	const char* dotDefinitionData = dotDefinition.data();
	int dotDefinitionDataLength = (int)dotDefinition.length();

	int parseIndex = 0;
	std::string tmpStr;

	Graph graph;
	
	// Parse the first token. Could be strict, graph or digraph
	Token tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength);

	// strict isn't supported so report a warning and parse a second token which must be graph or digraph
	if (tk == TOK_Strict)
	{
		std::cerr << "WARNING: the 'strict' directive is not supported" << std::endl;
		tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength);
	}

	if (tk == TOK_Graph)
		graph.SetGraphType(Graph::GT_Simple);
	else if (tk == TOK_Digraph)
		graph.SetGraphType(Graph::GT_Directed);
	else
		return graph;

	// We now look for a name for the graph or an open bracket
	tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength);

	if (tk == TOK_Id)
		graph.SetName(tmpStr);
	// If the token wasn't an ID then it must be an open bracket otherwise we have an error
	else if (tk != TOK_OpenBracket)
		return graph;

	// Proceed reading the statement lists of the DOT graph
	ParseStatementList(graph, tk == TOK_OpenBracket, parseIndex, dotDefinitionData, dotDefinitionDataLength);

	return graph;
}

bool DotParser::ParseID(std::string& id, bool& enclosedWithDoubleQuotes, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	id = "";

	RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength);
	
	if (parseIndex < dotDefinitionLength)
	{
		// Check if the ID starts with a double quote since it means that the end of the ID is marked by
		// another double quote not escaped
		enclosedWithDoubleQuotes = *dotDefinition == '\"';

		// The first character can be either a double quote, an alphabetic one or an underscore
		if (enclosedWithDoubleQuotes || IsAlphabetic(*dotDefinition) || *dotDefinition == '_')
		{
			// If the first character was a double quote we don't add it to the ID itself so move on
			if (enclosedWithDoubleQuotes)
			{
				parseIndex++;
				dotDefinition++;

				_columnNumber++;
			}

			while (parseIndex < dotDefinitionLength)
			{
				// If the character is a backslash we need to handle it in a special way
				// to check if it is escaping a double quote
				if (*dotDefinition == '\\')
				{
					// If we have no more characters after the backslash then we have
					// reached the end of the DOT while parsing an ID and it is impossible
					if ((parseIndex + 1) >= dotDefinitionLength)
						return false;
					else
					{
						// Check if the character next to the backslash is a double quote.
						if (*(dotDefinition + 1) == '\"')
						{
							// If the ID started with a double quote then the backslash escapes another one
							// so add a double quote to the ID and move to the next character
							if (enclosedWithDoubleQuotes)
							{
								id += "\"";

								// Add 2 since we have the backslash and the double quote to remove
								parseIndex += 2;
								dotDefinition += 2;

								_columnNumber += 2;
							}
							// Otherwise we are trying to escape a quote in the middle of an ID 
							// which wasn't started with a double quote, so the DOT is not valid
							else
								return false;
						}
						else
						{
							id += *dotDefinition;
							parseIndex++;
							dotDefinition++;

							_columnNumber++;
						}
					}
				}
				// If the character is a double quote and we have found it here we are sure
				// that it wasn't esacaped (the if before would have got that) so we need to do some checks
				else if (*dotDefinition == '\"')
				{
					// If the ID started with a double quote then this double quote is closing the ID
					if (enclosedWithDoubleQuotes)
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						return true;
					}
					// Otherwise we have found a double quote in the middle of an ID not encolsed
					// by double quotes, which is an error
					else
						return false;
				}
				// If we have found a space character we need to check if the ID is valid or not
				else if (std::isspace(*dotDefinition))
				{
					// If the ID started with a double quote then this space can go inside the ID
					if (enclosedWithDoubleQuotes)
					{
						id += *dotDefinition;

						if (*dotDefinition == '\n')
						{
							_lineNumber++;
							_columnNumber = 0;
						}
						else
							_columnNumber++;

						parseIndex++;
						dotDefinition++;

						return true;
					}
					// Otherwise the space means that the ID is finished
					else
						return true;
				}
				// If the character is alphabetic or an underscore we add it to the ID
				else if (IsAlphabetic(*dotDefinition) || IsDigit(*dotDefinition) || *dotDefinition == '_')
				{
					id += *dotDefinition;

					parseIndex++;
					dotDefinition++;

					_columnNumber++;
				}
				// We have found a character not valid for the ID
				else
					return false;
			}
		}
	}

	return false;
}

bool DotParser::ParseStatementList(Graph& graph, bool bracketAlreadyFound, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	Token tk = TOK_NotValid;
	std::string tmpStr = "";

	// If an open bracket wasn't already found we check if it is present
	// otherwise we have an error
	if (bracketAlreadyFound == false)
	{
		tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength);

		if (tk != TOK_OpenBracket)
			return false;
	}

	// Parse until we finish off the DOT buffer
	while (parseIndex < dotDefinitionLength)
	{
		// Get the next token
		tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength);

		// These three tokens aren't handled to print a warning but continue parsing
		if (tk == TOK_Edge || tk == TOK_Node || tk == TOK_Graph)
		{
			if (tk == TOK_Edge)
				std::cerr << "WARNING: the 'edge' directive is not supported [" << _lineNumber << ", " << _columnNumber << std::endl;
			else if(tk == TOK_Node)
				std::cerr << "WARNING: the 'node' directive is not supported [" << _lineNumber << ", " << _columnNumber << std::endl;
			else if (tk == TOK_Graph)
				std::cerr << "WARNING: the 'graph' directive is not supported [" << _lineNumber << ", " << _columnNumber << std::endl;

			// Get the next token which must be an open square bracket
			if (ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength) != TOK_OpenSquareBracket)
				return false;

			// Now we have to parse the attribute list, which could also be empty
			if (ParseAttributesList(nullptr, parseIndex, dotDefinition, dotDefinitionLength) == false)
				return false;
		}
		// We have found a valid ID so we could be reading an edge or a node declaration
		else if (tk == TOK_Id)
		{
			// Save the ID we have just read
			std::string objectID = tmpStr;

			// Parse the next token to understand if we are looking at an edge or node declaration
			tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength);

			// We've read a node declaration which has no attributes so add it to the graph
			if (tk == TOK_Semicolon)
			{
				// Make sure the node is added to the graph successfully
				if (graph.AddNode(objectID) == nullptr)
					return false;
			}
			// We've read a node declaration which has some attributes so add
			// the node to the graph and read the attributes to it
			else if (tk == TOK_OpenSquareBracket)
			{
				Node* node = graph.AddNode(objectID);

				// Make sure the node is added to the graph successfully
				if (node == nullptr)
					return false;

				// Parse the list of attributes and add them to the node
				if (ParseAttributesList(node, parseIndex, dotDefinition, dotDefinitionLength) == false)
					return false;
			}
			// We have found an edge declaration
			else if (tk == TOK_SimpleEdgeSymbol || tk == TOK_DirectedEdgeSymbol)
			{
				// Make sure the edge declaration is consistent with the graph type
				if (tk == TOK_SimpleEdgeSymbol && graph.GetGraphType() != Graph::GT_Simple)
				{
					std::cerr << "ERROR: found simple edge (--) on a non-simple graph [" << _lineNumber << ", " << _columnNumber << std::endl;
					return false;
				}
				else if(tk == TOK_DirectedEdgeSymbol && graph.GetGraphType() != Graph::GT_Directed)
				{
					std::cerr << "ERROR: found directed edge (->) on a non-directed graph [" << _lineNumber << ", " << _columnNumber << std::endl;
					return false;
				}

				// Parse the edge list and add it to the graph
				if (ParseEdgeList(graph, objectID, parseIndex, dotDefinition, dotDefinitionLength) == false)
					return false;
			}
			// Might have found an expression like ID = ID
			else if(tk == TOK_Equal)
			{
				// If the next token isn't an ID then there is an error
				if (ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength) != TOK_Id)
					return false;
			}
		}
		// If we have found a semicolon or a comment just ignore it
		else if (tk == TOK_Semicolon || tk == TOK_Comment)
		{
			continue;
		}
		// We have found an unexpected token so there is an error
		else
		{
			return false;
		}
	}
}

bool DotParser::MoveToFirstOccurenceOfChar(char c, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	// Move forward inside the DOT definition string until we reach
	// a character that is not the character we are looking for
	while (parseIndex < dotDefinitionLength && *dotDefinition != c)
	{
		if (*dotDefinition == '\n')
		{
			_lineNumber++;
			_columnNumber = 0;
		}
		else
			_columnNumber++;

		parseIndex++;
		dotDefinition++;
	}

	// We have found the character we were looking for so move the buffer forward
	if (*dotDefinition == c)
	{
		// Even if we were looking for a new line we need to make sure
		// the line and column attributes are correct
		if (*dotDefinition == '\n')
		{
			_lineNumber++;
			_columnNumber = 0;
		}
		else
			_columnNumber++;

		parseIndex++;
		dotDefinition++;

		return true;
	}

	return false;
}

void DotParser::RemoveSpaces(int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	// Move forward inside the DOT definition string until we reach a character that is not a white space
	while (parseIndex < dotDefinitionLength && std::isspace(*dotDefinition))
	{
		if (*dotDefinition == '\n')
		{
			_lineNumber++;
			_columnNumber = 0;
		}
		else
			_columnNumber++;

		parseIndex++;
		dotDefinition++;
	}
}

DotParser::Token DotParser::ParseToken(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength);

	while (parseIndex < dotDefinitionLength)
	{
		int localParseIndex = parseIndex;
		const char* localDotDefinition = dotDefinition;

		// First of all try to parse a valid ID
		bool idEnclosedWithDoubleQuotes = false;
		if (ParseID(result, idEnclosedWithDoubleQuotes, localParseIndex, localDotDefinition, dotDefinitionLength))
		{
			// Since we have found a valid ID we copy the local values of the parse
			// index and the DOT definition buffer to the real ones
			parseIndex = localParseIndex;
			dotDefinition = localDotDefinition;

			// If the ID we have found is not enclosed in double quotes we might have found
			// a token such as "edge", "node" or "graph" so we test it
			if (result == "edge")
				return TOK_Edge;
			else if (result == "node")
				return TOK_Node;
			else if (result == "graph")
				return TOK_Graph;
			else if (result == "digraph")
				return TOK_Digraph;
			else if (result == "strict")
				return TOK_Strict;
			// Otherwise we have simply found a valid ID
			else
				return TOK_Id;
		}
		// If a valid ID wasn't found we check for other tokens such as semicolons, brackets, etc.
		else
		{
			// Found an open bracked
			if (*dotDefinition == '{')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_OpenBracket;
			}
			// Found a closed bracked
			else if (*dotDefinition == '}')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_ClosedBraket;
			}
			// Found an open square bracked
			else if (*dotDefinition == '[')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_OpenSquareBracket;
			}
			// Found a closed square bracked
			else if (*dotDefinition == ']')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_ClosedSquareBraket;
			}
			// Found a semicolon
			else if (*dotDefinition == ';')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_Semicolon;
			}
			// Found an equal
			else if (*dotDefinition == '=')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				return TOK_Equal;
			}
			// We might have found an edge symbol
			else if (*dotDefinition == '-')
			{
				// If we don't have characters left then this is an error
				if ((parseIndex + 1) >= dotDefinitionLength)
					return TOK_NotValid;
				else
				{
					parseIndex++;
					dotDefinition++;

					_columnNumber++;

					// We have found a symple edge symbol
					if (*dotDefinition == '-')
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;
						
						return TOK_SimpleEdgeSymbol;
					}
					// We have found a directed edge symbol
					else if (*dotDefinition == '>')
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						return TOK_DirectedEdgeSymbol;
					}
					// This isn't an edge symbol so there is an error
					else
					{
						return TOK_NotValid;
					}
				}
			}
			// We might have found a comment or an error
			else if (*dotDefinition == '/')
			{
				// If we don't have characters left then this is an error
				if ((parseIndex + 1) >= dotDefinitionLength)
					return TOK_NotValid;
				else
				{
					parseIndex++;
					dotDefinition++;

					_columnNumber++;

					// If instead we have room for other characters
					// we can check if the comment is a single or multiple
					// one and if it is a comment
					if (*dotDefinition == '/')
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						// Found a single line comment so read the rest of the line
						ReadComment(result, true, parseIndex, dotDefinition, dotDefinitionLength);

						return TOK_Comment;
					}
					else if (*dotDefinition == '*')
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						// Found a multiple line comment so read until we find "*/"
						ReadComment(result, false, parseIndex, dotDefinition, dotDefinitionLength);

						return TOK_Comment;
					}
					else
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						// Found a non-valid token
						return TOK_NotValid;
					}
				}
			}
			// Found an unknown token
			else
				return TOK_NotValid;
		}

		RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength);
	}
}

void DotParser::ReadUntilSpaces(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	result = "";

	while (parseIndex < dotDefinitionLength && std::isspace(*dotDefinition) == false)
	{
		result += *dotDefinition;
		
		parseIndex++;
		dotDefinition++;

		_columnNumber++;
	}
}

bool DotParser::ReadComment(std::string& result, bool singleLine, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength)
{
	// Memorize the start of the comment
	const char* startOfComment = dotDefinition;

	if (singleLine)
	{
		// Read the rest of the buffer until we find a
		// new line character or we run out of buffer
		MoveToFirstOccurenceOfChar('\n', parseIndex, dotDefinition, dotDefinitionLength);

		// Store in result the content of the comment
		result = std::string(startOfComment, dotDefinition);

		return true;
	}
	else
	{
		// For multiple line comments it's more tricky since we have to look
		// for the first occurrence of the string "*/"

		while (parseIndex < dotDefinitionLength)
		{
			// If we find * we might be at the end of the comment
			if (*dotDefinition == '*')
			{
				parseIndex++;
				dotDefinition++;

				_columnNumber++;

				if (parseIndex < dotDefinitionLength)
				{
					// We have found the end of the comment
					if (*dotDefinition == '/')
					{
						parseIndex++;
						dotDefinition++;

						_columnNumber++;

						// Store in result the content of the comment
						result = std::string(startOfComment, dotDefinition - 2);

						return true;
					}
				}
				else
				{
					// We have reached the end of the buffer with the comment
					// still open so we have an error
					return false;
				}
			}
			// We are still reading the comment so move on
			else
			{
				if (*dotDefinition == '\n')
				{
					_lineNumber++;
					_columnNumber = 0;
				}
				else
					_columnNumber++;

				parseIndex++;
				dotDefinition++;
			}
		}

		// If we reach here then the end of the DOT buffer was reached without finding
		// the closing tag of the multiline comment so we have an error
		return false;
	}
}