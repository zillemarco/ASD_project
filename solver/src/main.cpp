#include "Graph.h"

#include <fstream>
#include <sstream>
#include <iostream>

void ReadFile(std::istream& input);

int main(int argc, char *argv[])
{
	// If the number of arguments is more than 1 then the path of the input file was given
	if (argc > 1)
	{
		// Try to open the file
		std::ifstream file(argv[1]);

		// If the file was opened then read its content and create the graph
		if (file)
			ReadFile(file);
		else
		{
			std::cerr << "Invalid file name passed as first argument" << std::endl;
			return 0;
		}
	}
	// If there are no arguments use the standard input to read the graph data
	else
	{
		ReadFile(std::cin);
	}

	return 0;
}

void ReadFile(std::istream& input)
{
	std::ostringstream std_input;
	std_input << input.rdbuf();

	std::string readString = std_input.str();

	std::cout << "READ" << std::endl << readString;
	std::cerr << "No errors";
}