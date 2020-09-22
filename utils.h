#include <tuple>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
class utils // Or whatever the name is...
{

	// ...
public:
	static void generateInitX(const string& xsFilename, int rows, double initNum);
	static void generateMatrix(const string& matrixFilename, int rows);
	static tuple<double*, double*, double*, int> load(const string& matrixFilename, const string& xsFilename);
	static void printVector(char* title, double xLocal[], int n);
	static void printResultInFile(const string& outFile, double* matrix, int n);
	static void writeLogTime(string& str);
//  ...
};
