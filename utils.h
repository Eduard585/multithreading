#include <tuple>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <climits>
#include <omp.h>
#include <algorithm>

using namespace std;
using graph = vector<vector<pair<int, int>>>;
class utils // Or whatever the name is...
{

	// ...
public:
	static void createFileAdjacencyMatrix(string filename, int nodesSize, int minBorder, int maxBorder);
	static void loadMatrix(const string filename, graph& graph);
	static void printVector(const vector<int>& distances);
	static void printAnswer(int root, const string& outFile, const vector<int>& distances);
	static vector<int> dijkstraStatic(graph graph, long startNode, long chunkSize);
	static vector<int> dijkstraDynamic(graph graph, long startNode, long chunkSize);
	static vector<int> dijkstraReduction(graph graph, int startNode, int nodes);
	static void writeLogTime(string& str);
};