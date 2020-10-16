#include <iostream>
#include "utils.h"
#include <fstream>
#include <windows.h>
#include <omp.h>
#include <vector>

using graph = vector<vector<pair<int, int>>>;
int main()
{
	utils util;
	graph gr;
	util.loadMatrix("input1.txt", gr);
	long long startTime = GetTickCount64();
	long long &timeMultiply = 0;
	util.dijkstraStatic(gr, 0, 100);
	timeMultiply = GetTickCount64() - startTime;
	util.writeLogTime(to_string(timeMultiply));
	timeMultiply = 0;
	util.dijkstraDynamic(gr, 0, 100);
	util.writeLogTime(to_string(timeMultiply));
	util.dijkstraReduction(gr, 0, 100);
	util.writeLogTime(to_string(timeMultiply));
}