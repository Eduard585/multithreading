#include "Util.h"


using namespace std;

void Util::createMatrixFile(string name, int rows, int columns) {
	std::ofstream f1(name);
	srand(time(NULL));
	f1.setf(std::ios_base::binary, std::ios_base::trunc);
	f1 << rows << " " << columns << std::endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			int value = rand() % 100 + 1;
			f1 << value << " ";
		}
		f1 << std::endl;
	}
	f1.close();
}

vector<vector<double>> Util::readMatrix(const string &filename) {
	vector<vector<double>> matrix;
	int64_t rows, columns;
	ifstream file(filename);
	if (!file) {
		cerr << "Error opening file: " << filename << ".\n";
		return matrix;
	}
	file >> rows >> columns;
	if (rows < 1 || columns < 1) {
		cerr << "The number of rows and columns must be greater than 0.\n";
		return matrix;
	}
	matrix.resize(rows);
	for (auto& row : matrix) {
		row.resize(columns);
	}
	for (auto& row : matrix) {
		for (auto& num : row) {
			file >> num;
		}
	}
	file.close();
	return matrix;
}
void Util::writeMatrixInFile(vector<vector<double>> result) {
	int64_t rows = result.size();
	int64_t columns = result[0].size();
	ofstream fileout("result.txt", ios_base::trunc);
	fileout << rows << " " << columns << endl;
	for (int64_t i = 0; i < rows; i++) {
		for (int64_t j = 0; j < columns; j++) {
			fileout << result[i][j] << " ";
		}
		fileout << std::endl;
	}
	fileout.close();
}

void Util::writeResult(char* name, int result) {
	std::ofstream f1(name, std::ios::out | std::ios::app);
	f1 << result << " ";
	f1.close();
}

int** Util::createEmptyMatrix(int rows, int columns) {
	int** result;
	result = new int* [rows];
	for (int row = 0; row < rows; row++) {
		result[row] = new int[columns];
		for (int column = 0; column < columns; column++) {
			result[row][column] = 0;
		}		
	}
	return result;
}

int* Util::readApproximation(char* fileName) {
	std::ifstream file(fileName);
	int length;
	int* result;
	file >> length;
	result = new int[length];
	for (int i = 0; i < length; i++) {
		file >> result[i];
	}
	file.close();
	return result;
}

void Util::writeLogTime(string& str) {
	std::ofstream filelog("Log.txt", std::ios::out | std::ios::app);
	filelog << str;
	filelog.close();
}

void Util::printMatrix(std::vector<std::vector<double>> matrix) {
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix.size(); j++) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Util::writeInFileResult(vector<vector<double>> result) {
	int64_t rows = result.size();
	int64_t columns = result[0].size();
	ofstream fileout("result.txt", ios_base::trunc);
	fileout << rows << " " << columns << endl;
	for (int64_t i = 0; i < rows; i++) {
		for (int64_t j = 0; j < columns; j++) {
			fileout << result[i][j] << " ";
		}
		fileout << std::endl;
	}
	fileout.close();
}