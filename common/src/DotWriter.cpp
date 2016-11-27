#include "DotWriter.h"

struct AttributesWriter
{
	AttributesWriter(std::ostream* output)
		: _output(output)
	{ }

	bool operator()(const GraphElement::Attribute& attribute, int index, bool lastElement)
	{
		// Write the attribute name
		if (attribute._encloseNameInDoubleQuotes)
			(*_output) << "\"";
		(*_output) << attribute._name;
		if (attribute._encloseNameInDoubleQuotes)
			(*_output) << "\"";

		(*_output) << "=";

		// Write the attribute value
		if (attribute._encloseValueInDoubleQuotes)
			(*_output) << "\"";
		(*_output) << attribute._value;
		if (attribute._encloseValueInDoubleQuotes)
			(*_output) << "\"";

		// Add a coma if this is not the last element
		if (lastElement == false)
			(*_output) << ", ";

		return true;
	}

	std::ostream* _output;
};

struct NodesWriter
{
	NodesWriter(std::ostream* output)
		: _output(output)
	{ }

	bool operator()(const Node& node, int index, bool lastElement)
	{
		// If the node has no connected edges or has at least one attribute then we must write it
		// otherwise the node will be written by the edge connected to the node and we can save space here
		if(node.HasAdjacentNodes() == false || node.HasAttributes())
		{
			// Write the node name
			if (node.EncloseNameInDoubleQuotes())
				(*_output) << "\"";
			(*_output) << node.GetName();
			if (node.EncloseNameInDoubleQuotes())
				(*_output) << "\"";

			if (node.HasAttributes() == false)
				(*_output) << ";" << std::endl;
			else
			{
				(*_output) << " [";

				node.GetAttributes().ForEach(AttributesWriter(_output));

				(*_output) << "];" << std::endl;
			}
		}

		return true;
	}

	std::ostream* _output;
};

struct EdgesWriter
{
	EdgesWriter(std::ostream* output, const std::string& edgeSymbol)
		: _output(output)
		, _edgeSymbol(edgeSymbol)
	{ }

	bool operator()(const Edge& edge, int index, bool lastElement)
	{
		// There two must be valid and this is ensured by the Graph class
		const Node* startNode = edge.GetStartNode();
		const Node* endNode = edge.GetEndNode();

		// Write the start node name
		if (startNode->EncloseNameInDoubleQuotes())
			(*_output) << "\"";
		(*_output) << startNode->GetName();
		if (startNode->EncloseNameInDoubleQuotes())
			(*_output) << "\"";

		// Write the connection symbol
		(*_output) << _edgeSymbol;

		// Write the end node name
		if (endNode->EncloseNameInDoubleQuotes())
			(*_output) << "\"";
		(*_output) << endNode->GetName();
		if (endNode->EncloseNameInDoubleQuotes())
			(*_output) << "\"";

		if (edge.HasAttributes() == false)
			(*_output) << ";" << std::endl;
		else
		{
			(*_output) << " [";

			edge.GetAttributes().ForEach(AttributesWriter(_output));

			(*_output) << "];" << std::endl;
		}

		return true;
	}

	std::ostream* _output;
	std::string _edgeSymbol;
};

bool DotWriter::Write(const Graph& graph, std::ostream& output)
{
	// Cannot write a graph if it is not valid or if the output stream is not valid
	if (!output || graph.GetGraphType() == Graph::GT_NotValid)
		return false;

	bool directedGraph = graph.GetGraphType() == Graph::GT_Directed;

	// Write the graph declaration
	if (directedGraph)
		output << "digraph ";
	else
		output << "graph ";

	// Write the graph name
	if (graph.EncloseNameInDoubleQuotes())
		output << "\"";
	output << graph.GetName();

	if (graph.EncloseNameInDoubleQuotes())
		output << "\"";
	output << " {" << std::endl;

	// First write all the nodes
	graph.GetNodes().ForEach(NodesWriter(&output));

	// Then write all the edges
	graph.GetEdges().ForEach(EdgesWriter(&output, directedGraph ? " -> " : " -- "));

	// Close the graph
	output << "}";

	return true;
}