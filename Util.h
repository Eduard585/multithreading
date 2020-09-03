#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <windows.h>

using namespace std;
#pragma once
class Util
{
public:
	static void createMatrixFile(string name, int rows, int columns);
	vector<vector<double>> readMatrix(const string& name);
	void writeMatrixInFile(vector<vector<double>> result);
	int** createEmptyMatrix(int rows, int columns);
	int* readApproximation(char* name);
	void writeResult(char* name,int result);
	void writeLogTime(string& str);
	void printMatrix(std::vector<std::vector<double>> matrix);
	void writeInFileResult(vector<vector<double>> result);
	Util() {}
};

