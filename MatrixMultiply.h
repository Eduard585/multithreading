#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <windows.h>

using namespace std;

class MatrixMultiply
{
public:
	vector<vector<double>> multiplyInOneThead(vector<vector<double>>& a, vector<vector<double>>& b);
	vector<vector<double>> multiplyParallelStatic(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize);
	vector<vector<double>> multiplyParallelDynamic(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize);
	vector<vector<double>> multiplyParallelGuided(vector<vector<double>>& a, vector<vector<double>>& b, int chunkSize);
};


