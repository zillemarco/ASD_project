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
	return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));
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

/** Utility struct to hold a function operator to be used by ParseAttributeList */
struct AddAttribute
{
	AddAttribute(const std::string& attributeName, const std::string& attributeValue)
		: _name(attributeName)
		, _value(attributeValue)
	{ }

	inline bool operator()(GraphElement* element)
	{
		if (element != nullptr)
			element->SetAttribute(_name, _value);
		return true;
	}

	std::string _name;
	std::string _value;
};

bool DotParser::Parse(Graph& resultGraph, const std::string& dotDefinition)
{
	const char* dotDefinitionData = dotDefinition.data();
	int dotDefinitionDataLength = (int)dotDefinition.length();

	int parseIndex = 0;
	int lineNumber = 0;
	int columnNumber = 0;
	std::string tmpStr;
		
	// Parse the first token. Could be strict, graph or digraph
	Token tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength, lineNumber, columnNumber);

	// strict isn't supported so report a warning and parse a second token which must be graph or digraph
	if (tk == TOK_Strict)
	{
		std::cerr << "WARNING: the 'strict' directive is not supported" << std::endl;
		tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength, lineNumber, columnNumber);
	}

	if (tk == TOK_Graph)
		resultGraph.SetGraphType(Graph::GT_Simple);
	else if (tk == TOK_Digraph)
		resultGraph.SetGraphType(Graph::GT_Directed);
	else
		return false;

	// We now look for a name for the graph or an open bracket
	tk = ParseToken(tmpStr, parseIndex, dotDefinitionData, dotDefinitionDataLength, lineNumber, columnNumber);

	if (tk == TOK_Id)
		resultGraph.SetName(tmpStr);
	// If the token wasn't an ID then it must be an open bracket otherwise we have an error
	else if (tk != TOK_OpenBracket)
		return false;

	// Proceed reading the statement lists of the DOT graph
	return ParseStatementList(resultGraph, tk == TOK_OpenBracket, parseIndex, dotDefinitionData, dotDefinitionDataLength, lineNumber, columnNumber);
}

bool DotParser::ParseID(std::string& id, bool& enclosedWithDoubleQuotes, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	id = "";

	RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);
	
	if (parseIndex < dotDefinitionLength)
	{
		// Check if the ID starts with a double quote since it means that the end of the ID is marked by
		// another double quote not escaped
		enclosedWithDoubleQuotes = *dotDefinition == '\"';

		// The first character can be either a double quote, an alphabetic or digit character, an underscore, a minus or a point
		if (enclosedWithDoubleQuotes || IsAlphabetic(*dotDefinition) || IsDigit(*dotDefinition) || *dotDefinition == '_' || *dotDefinition == '-' || *dotDefinition == '.')
		{
			// If the first character was a double quote we don't add it to the ID itself so move on
			if (enclosedWithDoubleQuotes)
			{
				parseIndex++;
				dotDefinition++;

				columnNumber++;
			}

			// Used to know if we are parsing a numeric ID
			bool parsingFirstChar = true;
			bool parsingNumeridID = false;
			bool foundPoint = false;

			while (parseIndex < dotDefinitionLength)
			{
				// If the character is a backslash we need to handle it in a special way
				// to check if it is escaping a double quote
				if (*dotDefinition == '\\')
				{
					parseIndex++;
					dotDefinition++;
					columnNumber++;

					// If we have no more characters after the backslash then we have
					// reached the end of the DOT while parsing an ID and it is impossible
					if (parseIndex >= dotDefinitionLength)
					{
						dotDefinition++;
						return false;
					}
					else
					{
						// Check if the character next to the backslash is a double quote.
						if (*dotDefinition == '\"')
						{
							parseIndex++;
							dotDefinition++;
							columnNumber++;

							// If the ID started with a double quote then the backslash escapes another one
							// so add a double quote to the ID and move to the next character
							if (enclosedWithDoubleQuotes)
								id += "\"";
							// Otherwise we are trying to escape a quote in the middle of an ID 
							// which wasn't started with a double quote, so the DOT is not valid
							else
								return false;
						}
						// The found backslash id just a backslash so add it to the ID
						else
							id += "\\";
					}
				}
				// If the character is a double quote and we have found it here we are sure
				// that it wasn't esacaped (the if before would have got that) so we need to do some checks
				else if (*dotDefinition == '\"')
				{
					parseIndex++;
					dotDefinition++;
					columnNumber++;

					// If the ID started with a double quote then this double quote is closing the ID
					if (enclosedWithDoubleQuotes)
						return true;
					// Otherwise we have found a double quote in the middle of an ID not encolsed
					// by double quotes, which is an error
					// This convers also the case if we are parsing a numerid ID
					else
						return false;
				}
				// If we have found a space character we need to check if the ID is valid or not
				else if (std::isspace(*dotDefinition))
				{
					parseIndex++;
					char space = *dotDefinition++;

					// If the ID started with a double quote then this space can go inside the ID
					if (enclosedWithDoubleQuotes)
					{
						id += space;

						if (space == '\n')
						{
							lineNumber++;
							columnNumber = 0;
						}
						else
							columnNumber++;

						return true;
					}
					// Otherwise the space means that the ID is finished
					// This convers also the case if we are parsing a numerid ID
					else
						return true;
				}
				// If we have found a semicolon, a coma, an open or closed square bracket or an equal and the ID isn't enclosed
				// by double quotes we have reached the end of the ID, otherwise it's just a character of the ID
				else if (*dotDefinition == ';' || *dotDefinition == ',' || *dotDefinition == '[' || *dotDefinition == ']' || *dotDefinition == '=')
				{
					if (enclosedWithDoubleQuotes)
					{
						id += *dotDefinition;

						parseIndex++;
						dotDefinition++;

						columnNumber++;
					}
					else
					{
						// Return without moving to the next character inside the buffer so
						// that it can be correctly treated like a token
						return true;
					}
				}
				// If we have found a minus, a point or a digit the ID could be a number
				if(*dotDefinition == '-' || *dotDefinition == '.' || IsDigit(*dotDefinition))
				{
					parseIndex++;
					columnNumber++;
					char symbol = *dotDefinition++;

					// If we are at the start of the ID then it is a numeric ID
					if (parsingFirstChar)
					{
						id += symbol;

						if (symbol == '.')
							foundPoint = true;

						parsingNumeridID = true;
					}
					// We're not parsing the first char but the string is enclosed with double quotes so there is no problem
					else if (enclosedWithDoubleQuotes == true)
						id += symbol;
					// We've found a minus inside an ID not enclosed in double quotes so there is an error
					else if (symbol == '-')
						return false;
					// We've found a point inside an ID not enclosed in double quotes so this could be either
					// a numeric ID or an error
					else if (symbol == '.')
					{
						// If we are parsing a number and we haven't already found a point
						// there is no problem
						if (parsingNumeridID == true && foundPoint == false)
						{
							id += symbol;
							foundPoint = true;
						}
						// Otherwise there is an error
						else
							return false;
					}
					// Otherwise we have read a digit
					else
						id += symbol;
				}
				// If the character is alphabetic or an underscore we add it to the ID
				else if (IsAlphabetic(*dotDefinition) || *dotDefinition == '_')
				{
					id += *dotDefinition;

					parseIndex++;
					dotDefinition++;

					columnNumber++;
				}
				// We have found a character not valid for the ID
				else
					return false;

				if(parsingFirstChar == true)
					parsingFirstChar = false;
			}
		}
	}

	return false;
}

bool DotParser::ParseStatementList(Graph& graph, bool bracketAlreadyFound, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	Token tk = TOK_NotValid;
	std::string tmpStr = "";

	// If an open bracket wasn't already found we check if it is present
	// otherwise we have an error
	if (bracketAlreadyFound == false)
	{
		tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

		if (tk != TOK_OpenBracket)
		{
			std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a '{'" << std::endl;
			return false;
		}
	}

	// Parse until we finish off the DOT buffer
	while (parseIndex < dotDefinitionLength)
	{
		// Get the next token
		tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

		// These three tokens aren't handled to print a warning but continue parsing
		if (tk == TOK_Edge || tk == TOK_Node || tk == TOK_Graph)
		{
			if (tk == TOK_Edge)
				std::cerr << "WARNING: the 'edge' directive is not supported [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
			else if (tk == TOK_Node)
				std::cerr << "WARNING: the 'node' directive is not supported [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
			else if (tk == TOK_Graph)
				std::cerr << "WARNING: the 'graph' directive is not supported [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;

			// Get the next token which must be an open square bracket
			if (ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) != TOK_OpenSquareBracket)
			{
				std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a '['" << std::endl;
				return false;
			}

			// Now we have to parse the attribute list, which could also be empty
			if (ParseAttributesList(nullptr, nullptr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == false)
				return false;
		}
		// We have found a valid ID so we could be reading an edge or a node declaration
		else if (tk == TOK_Id)
		{
			// Save the ID we have just read
			std::string objectID = tmpStr;

			// Parse the next token to understand if we are looking at an edge or node declaration
			tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

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
				if (ParseAttributesList(node, nullptr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == false)
					return false;
			}
			// We have found an edge declaration
			else if (tk == TOK_SimpleEdgeSymbol || tk == TOK_DirectedEdgeSymbol)
			{
				// Make sure the edge declaration is consistent with the graph type
				if (tk == TOK_SimpleEdgeSymbol && graph.GetGraphType() != Graph::GT_Simple)
				{
					std::cerr << "ERROR: found simple edge (--) on a non-simple graph [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
					return false;
				}
				else if (tk == TOK_DirectedEdgeSymbol && graph.GetGraphType() != Graph::GT_Directed)
				{
					std::cerr << "ERROR: found directed edge (->) on a non-directed graph [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
					return false;
				}

				// Parse the edge list and add it to the graph
				if (ParseEdgeList(graph, objectID, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == false)
					return false;
			}
			// Might have found an expression like ID = ID
			else if (tk == TOK_Equal)
			{
				// If the next token isn't an ID then there is an error
				if (ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) != TOK_Id)
				{
					std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a valid ID" << std::endl;
					return false;
				}
			}
		}
		// If we have found a semicolon, a coma or a comment so just ignore it
		else if (tk == TOK_Semicolon || tk == TOK_Coma || tk == TOK_Comment)
			continue;
		// If we have found a closed bracket then the statements list is completed
		else if (tk == TOK_ClosedBraket)
			return true;
		// We have found an unexpected token so there is an error
		else
		{
			std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
			return false;
		}
	}

	return false;
}

bool DotParser::MoveToFirstOccurenceOfChar(char c, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	// Move forward inside the DOT definition string until we reach
	// a character that is not the character we are looking for
	while (parseIndex < dotDefinitionLength && *dotDefinition != c)
	{
		if (*dotDefinition == '\n')
		{
			lineNumber++;
			columnNumber = 0;
		}
		else
			columnNumber++;

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
			lineNumber++;
			columnNumber = 0;
		}
		else
			columnNumber++;

		parseIndex++;
		dotDefinition++;

		return true;
	}

	return false;
}

void DotParser::RemoveSpaces(int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	// Move forward inside the DOT definition string until we reach a character that is not a white space
	while (parseIndex < dotDefinitionLength && std::isspace(*dotDefinition))
	{
		if (*dotDefinition == '\n')
		{
			lineNumber++;
			columnNumber = 0;
		}
		else
			columnNumber++;

		parseIndex++;
		dotDefinition++;
	}
}

DotParser::Token DotParser::ParseToken(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

	while (parseIndex < dotDefinitionLength)
	{
		int localParseIndex = parseIndex;
		int localLineNumber = lineNumber;
		int localColumnNumber = columnNumber;
		const char* localDotDefinition = dotDefinition;

		// First of all try to parse a valid ID
		bool idEnclosedWithDoubleQuotes = false;
		if (ParseID(result, idEnclosedWithDoubleQuotes, localParseIndex, localDotDefinition, dotDefinitionLength, localLineNumber, localColumnNumber))
		{
			// Since we have found a valid ID we copy the local values of the parse
			// index and the DOT definition buffer to the real ones
			parseIndex = localParseIndex;
			dotDefinition = localDotDefinition;
			lineNumber = localLineNumber;
			columnNumber = localColumnNumber;

			// If the ID was enclosed in double quotes it is always a valid ID
			if (idEnclosedWithDoubleQuotes)
				return TOK_Id;
			else
			{
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

				columnNumber++;

				return TOK_OpenBracket;
			}
			// Found a closed bracked
			else if (*dotDefinition == '}')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_ClosedBraket;
			}
			// Found an open square bracked
			else if (*dotDefinition == '[')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_OpenSquareBracket;
			}
			// Found a closed square bracked
			else if (*dotDefinition == ']')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_ClosedSquareBraket;
			}
			// Found a semicolon
			else if (*dotDefinition == ';')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_Semicolon;
			}
			// Found an equal
			else if (*dotDefinition == '=')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_Equal;
			}
			// Found a coma
			else if (*dotDefinition == ',')
			{
				result = *dotDefinition;

				parseIndex++;
				dotDefinition++;

				columnNumber++;

				return TOK_Coma;
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

					columnNumber++;

					// We have found a symple edge symbol
					if (*dotDefinition == '-')
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;
						
						return TOK_SimpleEdgeSymbol;
					}
					// We have found a directed edge symbol
					else if (*dotDefinition == '>')
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;

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

					columnNumber++;

					// If instead we have room for other characters
					// we can check if the comment is a single or multiple
					// one and if it is a comment
					if (*dotDefinition == '/')
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;

						// Found a single line comment so read the rest of the line
						ReadComment(result, true, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

						return TOK_Comment;
					}
					else if (*dotDefinition == '*')
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;

						// Found a multiple line comment so read until we find "*/"
						ReadComment(result, false, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

						return TOK_Comment;
					}
					else
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;

						// Found a non-valid token
						return TOK_NotValid;
					}
				}
			}
			// Found an unknown token
			else
				return TOK_NotValid;
		}

		RemoveSpaces(parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);
	}

	return TOK_NotValid;
}

void DotParser::ReadUntilSpaces(std::string& result, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	result = "";

	while (parseIndex < dotDefinitionLength && std::isspace(*dotDefinition) == false)
	{
		result += *dotDefinition;
		
		parseIndex++;
		dotDefinition++;

		columnNumber++;
	}
}

bool DotParser::ReadComment(std::string& result, bool singleLine, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	// Memorize the start of the comment
	const char* startOfComment = dotDefinition;

	if (singleLine)
	{
		// Read the rest of the buffer until we find a
		// new line character or we run out of buffer
		MoveToFirstOccurenceOfChar('\n', parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

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

				columnNumber++;

				if (parseIndex < dotDefinitionLength)
				{
					// We have found the end of the comment
					if (*dotDefinition == '/')
					{
						parseIndex++;
						dotDefinition++;

						columnNumber++;

						// Store in result the content of the comment
						result = std::string(startOfComment, dotDefinition - 2);

						return true;
					}
				}
				else
				{
					// We have reached the end of the buffer with the comment
					// still open so we have an error
					std::cerr << "ERROR: reached the end of the file before closing the multiline comment" << std::endl;
					return false;
				}
			}
			// We are still reading the comment so move on
			else
			{
				if (*dotDefinition == '\n')
				{
					lineNumber++;
					columnNumber = 0;
				}
				else
					columnNumber++;

				parseIndex++;
				dotDefinition++;
			}
		}

		// If we reach here then the end of the DOT buffer was reached without finding
		// the closing tag of the multiline comment so we have an error
		std::cerr << "ERROR: reached the end of the file too soon" << std::endl;
		return false;
	}
}

bool DotParser::ParseAttributesList(GraphElement* singleElement, List<GraphElement*>* elements, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	Token tk = TOK_NotValid;
	std::string tmpStr = "";
	std::string attributeName = "";
	std::string attributeValue = "";
	bool foundAnAttribute = false;

	while (parseIndex < dotDefinitionLength)
	{
		// The first token must be an ID, a closed square bracket, a coma or a semicolon
		tk = ParseToken(attributeName, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

		// If we have found a closed square bracket the attributes list is finished
		if (tk == TOK_ClosedSquareBraket)
			return true;
		// If we have found a semicolon or a coma and we have alreay found an attribute
		// then there is no problem and we can go on, otherwise we have an error
		else if (tk == TOK_Semicolon || tk == TOK_Coma)
		{
			if (foundAnAttribute)
				continue;
			else
			{
				std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Cannot put semicolons or comas inside an empty attribute list" << std::endl;
				return false;
			}
		}
		// Found a valid ID which is the name of the attribute
		else if (tk == TOK_Id)
		{
			// Now there must be an equal
			if (ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == TOK_Equal)
			{
				// Now there must be another ID which is the value of the attribute
				if (ParseToken(attributeValue, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == TOK_Id)
				{
					// We have all the components to add the attribute
					if (elements != nullptr)
						elements->ForEach(AddAttribute(attributeName, attributeValue));
					else if(singleElement != nullptr)
						singleElement->SetAttribute(attributeName, attributeValue);

					if (foundAnAttribute == false)
						foundAnAttribute = true;
				}
				else
				{
					std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a valid ID" << std::endl;
					return false;
				}
			}
			else
			{
				std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Exprected a '='" << std::endl;
				return false;
			}
		}
	}

	return false;
}

bool DotParser::ParseEdgeList(Graph& graph, const std::string& firstNodeId, int& parseIndex, const char*& dotDefinition, int dotDefinitionLength, int& lineNumber, int& columnNumber)
{
	Token tk = TOK_NotValid;
	std::string tmpStr = "";
	std::string nodeId_1 = firstNodeId;
	std::string nodeId_2 = "";

	List<GraphElement*> addedEdges;

	// First check if there is at least one valid edge declaration
	if (parseIndex < dotDefinitionLength)
	{
		// The first token must be an ID
		tk = ParseToken(nodeId_2, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

		if (tk == TOK_Id)
		{
			// We have found a valid ID which is the second node ID so we can add the edge
			Edge* edge = graph.AddEdge(nodeId_1, nodeId_2);

			// If the edge has not been created there is an error
			if (edge == nullptr)
				return false;

			addedEdges.Add(edge);
		}
		else
		{
			std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a valid ID" << std::endl;
			return false;
		}
	}

	// Now go on and check if there are mode valid edge declarations or an attribute list
	while(parseIndex < dotDefinitionLength)
	{
		// Make the current second node of the edge the first of the next edge
		nodeId_1 = nodeId_2;
		nodeId_2 = "";

		// The first token must be a '--', a '->', a coma, a semicolon or an open square bracked
		tk = ParseToken(tmpStr, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);

		// If we have found a semicolon the edge list is finished
		if (tk == TOK_Semicolon)
			return true;
		else if (tk == TOK_SimpleEdgeSymbol || tk == TOK_DirectedEdgeSymbol)
		{
			// Make sure the edge declaration is consistent with the graph type
			if (tk == TOK_SimpleEdgeSymbol && graph.GetGraphType() != Graph::GT_Simple)
			{
				std::cerr << "ERROR: found simple edge (--) on a non-simple graph [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
				return false;
			}
			else if (tk == TOK_DirectedEdgeSymbol && graph.GetGraphType() != Graph::GT_Directed)
			{
				std::cerr << "ERROR: found directed edge (->) on a non-directed graph [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]" << std::endl;
				return false;
			}

			// Now we must find a second valid ID
			if (ParseToken(nodeId_2, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber) == TOK_Id)
			{
				// We have found a valid ID which is the second node ID so we can add the edge
				Edge* edge = graph.AddEdge(nodeId_1, nodeId_2);

				// If the edge has not been created there is an error
				if (edge == nullptr)
					return false;

				addedEdges.Add(edge);
			}
			else
			{
				std::cerr << "ERROR: unexpected symbol at [" << (lineNumber + 1) << ", " << (columnNumber + 1) << "]. Expected a valid ID" << std::endl;
				return false;
			}
		}
		// If we have found an open square bracket then the attribute list of the declared edge[s] has started
		// so the edge declarations are finisched
		else if (tk == TOK_OpenSquareBracket)
			return ParseAttributesList(nullptr, &addedEdges, parseIndex, dotDefinition, dotDefinitionLength, lineNumber, columnNumber);
		// Found an unexpected token
		else
			return false;
	}

	return false;
}