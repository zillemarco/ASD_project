#include "Graph.h"
#include "RandomGraphGenerator.h"
#include "ASDProjectSolver.h"
#include "DotWriter.h"

#include <fstream>

void PrintUsage();
bool ParseArgs(int argc, char *argv[], int& numberOfGraphs, int& numberOfNodes, int& numberOfEdges, int& edgeChance, std::string& generatedGraphsFolderPath, std::string& solutionGraphsFolderPath);

int main(int argc, char *argv[])
{
	int numberOfGraphs;
	int numberOfNodes;
	int numberOfEdges;
	int edgeChance;
	std::string generatedGraphsFolderPath;
	std::string solutionGraphsFolderPath;

	if (!ParseArgs(argc, argv, numberOfGraphs, numberOfNodes, numberOfEdges, edgeChance, generatedGraphsFolderPath, solutionGraphsFolderPath))
	{
		PrintUsage();
		return -1;
	}

	double edgeChangePercentage = (double)edgeChance / 100.0;
	bool saveGeneratedGraphs = generatedGraphsFolderPath.empty() == false;
	bool saveSolutionGraphs = solutionGraphsFolderPath.empty() == false;

	if (saveGeneratedGraphs && generatedGraphsFolderPath[generatedGraphsFolderPath.length() - 1] != '\\')
		generatedGraphsFolderPath += "\\";

	if (saveSolutionGraphs && solutionGraphsFolderPath[solutionGraphsFolderPath.length() - 1] != '\\')
		solutionGraphsFolderPath += "\\";

	for (int i = 0; i < numberOfGraphs; i++)
	{
		std::string graphName = "G" + std::to_string(i);
		
		Graph graph;
		graph.SetName(graphName, false);

		if (RandomGraphGenerator::CreateRandomGraph(numberOfNodes, numberOfEdges, edgeChangePercentage, graph))
		{
			if (saveGeneratedGraphs)
			{
				std::ofstream outputFile(generatedGraphsFolderPath + graphName + ".dot");
				if (outputFile.is_open())
					DotWriter::Write(graph, outputFile);
				outputFile.close();
			}

			if (ASDProjectSolver::ProcessData(graph) && saveSolutionGraphs)
			{
				std::ofstream outputFile(solutionGraphsFolderPath + "out_" + graphName + ".dot");
				if (outputFile.is_open())
					DotWriter::Write(graph, outputFile);
				outputFile.close();
			}
		}
	}

	return 0;
}

void PrintUsage()
{
	std::cout
		<< "USAGE:" << std::endl
		<< "time_tester[.exe] -ng NumberOfGraphs -nn NumberOfNodes -ne NumberOfEdges -ec EdgeChance [-g GeneratedGraphsFolderPath] [-s SolutionsGraphsFolderPath]" << std::endl << std::endl
		<< "Parameters:" << std::endl
		<< "\tng: the number of graphs to generate for the test" << std::endl
		<< "\tnn: the number of nodes for each generated graph" << std::endl
		<< "\tne: the number of edges for each generated graph" << std::endl
		<< "\tec: the chance of adding an edge, expressed in percentage (in the range [0..100])" << std::endl
		<< "\tg: the folder where the generated graphs will be saved" << std::endl
		<< "\ts: the folder where the solution graphs will be saved";
}

bool ParseArgs(int argc, char *argv[], int& numberOfGraphs, int& numberOfNodes, int& numberOfEdges, int& edgeChance, std::string& generatedGraphsFolderPath, std::string& solutionGraphsFolderPath)
{
	// An unsufficient number of arguments were given
	if (argc < 9)
		return false;

	bool foundNumberOfGraphs = false;
	bool foundNumberOfNodes = false;
	bool foundNumberOfEdges = false;
	bool foundEdgeChance = false;
	
	// Loop adding always 2 (option + value)
	for (int i = 1; (i + 1) < argc; i += 2)
	{
		std::string strOption = argv[i];
		std::string strValue = argv[i + 1];

		if (strOption == "-ng")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			numberOfGraphs = atoi(strValue.c_str());
			foundNumberOfGraphs = true;
		}
		else if(strOption == "-nn")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			numberOfNodes = atoi(strValue.c_str());
			foundNumberOfNodes = true;
		}
		else if (strOption == "-ne")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			numberOfEdges = atoi(strValue.c_str());
			foundNumberOfEdges = true;
		}
		else if (strOption == "-ec")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			edgeChance = atoi(strValue.c_str());
			foundEdgeChance = true;
		}
		else if (strOption == "-g")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			generatedGraphsFolderPath = strValue;
		}
		else if (strOption == "-s")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			solutionGraphsFolderPath = strValue;
		}
	}

	return foundNumberOfGraphs && foundNumberOfNodes && foundNumberOfEdges && foundEdgeChance;
}