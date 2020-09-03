#include <iostream>
#include <fstream>
#include <windows.h>
#include <omp.h>
#include <vector>

#include "Util.h"
#include "../MultiThreading/MatrixMultiply.h"

using namespace std;

//OPTIONS
ULONGLONG dwStart;
Util util;
MatrixMultiply multiply;
long long rows1 = 800;// Количество строк первой матрицы
long long columns2 = 1000;// Количество столбцов второй матрицы
long long inter21 = 900; // Количество столбцов первой матрицы и строк для второй
int threadsNum = 8;
int chunkSize = 1;

int main(int argc, char** argv)
{	
	if (argc < 3) {
		cout << "Not all arguments specified";
		return 1;
	}

	string file1(argv[1]);
	string file2(argv[2]);

	if (argc == 6) {
		util.createMatrixFile(file1, atoi(argv[3]), atoi(argv[4]));
		util.createMatrixFile(file2, atoi(argv[4]), atoi(argv[5]));
	}
	else {
		int maxThreadNum = omp_get_max_threads();
		omp_set_num_threads(maxThreadNum);

		vector<vector<double>> a = util.readMatrix(file1);
		vector<vector<double>> b = util.readMatrix(file2);
		
		int mode = argv[3] != nullptr ? atoi(argv[3]) : 1;
		int chunkSize = argv[4] != nullptr ? atoi(argv[4]) : 1;
		vector<vector<double>> result;
		int64_t rows = a.size();
		int64_t inter = b.size();
		int64_t columns = b[0].size();
		char buf[1000];
		int res = -1;
		std::cout << "Begin multiply" << std::endl;
		ULONGLONG startTime = GetTickCount64(), timeMultiply;

		switch (mode) {
		case(1): {
			result = multiply.multiplyInOneThead(a, b);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "ONE THREAD MODE: %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(2): {
			result = multiply.multiplyParallelStatic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "STATIC MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(3): {
			result = multiply.multiplyParallelDynamic(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "DYNAMIC MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		case(4): {
			result = multiply.multiplyParallelGuided(a, b, chunkSize);
			timeMultiply = GetTickCount64() - startTime;
			res = snprintf(buf, sizeof(buf), "GUIDED MODE (chunk size = %d) : %I64dx%I64d on %I64dx%I64d, spent %I64d milliseconds\n", chunkSize, rows, inter, inter, columns, timeMultiply);
			break;
		}
		default: break;
		}

		std::string str = "error!";
		if (res >= 0 && res < sizeof(buf))
			str = buf;
		std::cout << str << std::endl;
		util.writeLogTime(str);
		util.writeInFileResult(result);
	}
}