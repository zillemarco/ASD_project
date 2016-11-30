#include "Graph.h"
#include "RandomGraphGenerator.h"
#include "RandomGenerator.h"
#include "ASDProjectSolver.h"
#include "ASDProjectTimeTester.h"
#include "DotWriter.h"

#include <fstream>
#include <ctime>

void PrintUsage();
bool ParseArgs(int argc, char *argv[], int& numberOfGraphs, int& numberOfNodes, int& edgeChance, double& seed, double& delta, int& iterations, double& distribution, std::string& generatedGraphsFolderPath, std::string& solutionGraphsFolderPath);

int main(int argc, char *argv[])
{
	int numberOfGraphs = 1;
	int numberOfNodes = 10;
	int edgeChance = 15;
	double seed = 0;
	double delta = 0.2;
	int iterations = 7;
	double distribution = 1.96;
	std::string generatedGraphsFolderPath = "";
	std::string solutionGraphsFolderPath = "";

	if (!ParseArgs(argc, argv, numberOfGraphs, numberOfNodes, edgeChance, seed, delta, iterations, distribution, generatedGraphsFolderPath, solutionGraphsFolderPath))
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

	double minimumTime = ASDProjectTimeTester::GetMinimumTime();
	double totalTime = 0.0;
	int totalEdges = 0;

	if (seed < 0.0)
		RandomGenerator::SetSeed((double)clock());
	else if(seed > 0.0)
		RandomGenerator::SetSeed(seed);

	for (int i = 0; i < numberOfGraphs; i++)
	{
		std::string graphName = "G" + std::to_string(i);
		
		Graph graph;
		graph.SetName(graphName, false);

		if (RandomGraphGenerator::CreateRandomGraph(numberOfNodes, edgeChangePercentage, graph))
		{
			if (saveGeneratedGraphs)
			{
				std::ofstream outputFile(generatedGraphsFolderPath + graphName + ".dot");
				if (outputFile.is_open())
					DotWriter::Write(graph, outputFile);
				outputFile.close();
			}

			double time = ASDProjectTimeTester::ComputeAlgorithmTime(graph, iterations, distribution, minimumTime, delta);
			int edges = graph.GetEdges().GetSize();

			std::cout << "Time for the first graph (" << numberOfNodes << " nodes, " << edges << " edges): " << time << " seconds" << std::endl;
			totalTime += time;
			totalEdges += edges;

			if (saveSolutionGraphs)
			{
				Graph result;
				ASDProjectSolver::ProcessData(graph, result);

				std::ofstream outputFile(solutionGraphsFolderPath + "out_" + graphName + ".dot");
				if (outputFile.is_open())
					DotWriter::Write(result, outputFile);
				outputFile.close();
			}
		}
	}

	std::cout << std::endl << "Average execution time for the algorithm on graphs with " << numberOfNodes << " nodes and an average of " << (totalEdges / numberOfGraphs) << " edges is " << (totalTime / ((double)numberOfGraphs)) << " seconds" << std::endl << std::endl;

	return 0;
}

void PrintUsage()
{
	std::cout
		<< "USAGE:" << std::endl
		<< "time_tester[.exe] -ng NumberOfGraphs -nn NumberOfNodes -ec EdgeChance [-seed Seed] [-delta Delta] [-it Iterations] [-nordist Distribution] [-g GeneratedGraphsFolderPath] [-s SolutionsGraphsFolderPath]" << std::endl << std::endl
		<< "Parameters:" << std::endl
		<< "\t-ng: the number of graphs to generate for the test" << std::endl
		<< "\t-nn: the number of nodes for each generated graph" << std::endl
		<< "\t-ec: the chance of adding an edge, expressed in percentage (in the range [0..100])" << std::endl
		<< "\t-seed: the seed used to generate the random graphs (< 0: use random seed; 0: use default seed; > 0: use given seed)" << std::endl
		<< "\t-delta: maximum error threshold. When we find a measurement with an error below this number we treat it as valid" << std::endl
		<< "\t-it: number of times to run the algorithm each time until enough data is collected for each graph" << std::endl
		<< "\t-nordist: normal distribution value" << std::endl
		<< "\t-g: the folder where the generated graphs will be saved" << std::endl
		<< "\t-s: the folder where the solution graphs will be saved" << std::endl;
}

bool ParseArgs(
	int argc,
	char *argv[],
	int& numberOfGraphs,
	int& numberOfNodes,
	int& edgeChance,
	double& seed,
	double& delta,
	int& iterations,
	double& distribution,
	std::string& generatedGraphsFolderPath,
	std::string& solutionGraphsFolderPath)
{
	// An unsufficient number of arguments were given
	if (argc < 7)
		return false;

	numberOfGraphs = 1;
	numberOfNodes = 10;
	edgeChance = 15;
	seed = 0;
	delta = 0.2;
	iterations = 7;
	distribution = 1.96;
	generatedGraphsFolderPath = "";
	solutionGraphsFolderPath = "";

	bool foundNumberOfGraphs = false;
	bool foundNumberOfNodes = false;
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
		else if (strOption == "-ec")
		{
			// If the value starts with a '-' then it is an option
			if (strValue[0] == '-')
				return false;

			edgeChance = atoi(strValue.c_str());
			foundEdgeChance = true;
		}
		else if (strOption == "-seed")
			seed = atof(strValue.c_str());
		else if (strOption == "-delta")
			delta = atof(strValue.c_str());
		else if (strOption == "-it")
			iterations = atoi(strValue.c_str());
		else if (strOption == "-nordist")
			distribution = atof(strValue.c_str());
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

	return foundNumberOfGraphs && foundNumberOfNodes && foundEdgeChance;
}