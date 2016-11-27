#include "Graph.h"
#include "DotParser.h"
#include "DotWriter.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::string ReadFile(std::istream& input);

int main(int argc, char *argv[])
{
	std::string dotFileContent = "";

	// If the number of arguments is more than 1 then the path of the input file was given
	if (argc > 1)
	{
		// Try to open the file
		std::ifstream file(argv[1]);

		// If the file was opened then read its content and create the graph
		if (file)
			dotFileContent = ReadFile(file);
		else
		{
			std::cerr << "Invalid file name passed as first argument" << std::endl;
			return 0;
		}
	}
	// If there are no arguments use the standard input to read the graph data
	else
	{
		dotFileContent = ReadFile(std::cin);
	}

	Graph result;
	if (DotParser::Parse(result, dotFileContent))
	{
		if(result.IsCyclic())
			std::cout << "The graph HAS cycles" << std::endl << std::endl;
		else
			std::cout << "The graph HAS NOT cycles" << std::endl << std::endl;

		DotWriter::Write(result, std::cout);
	}

	return 0;
}

std::string ReadFile(std::istream& input)
{
	std::ostringstream std_input;
	std_input << input.rdbuf();

	return std_input.str();
}