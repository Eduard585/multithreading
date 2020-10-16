#ifndef QSORTMPI
#define QSORTMPI

#include <iostream>
#include <math.h>
#include <chrono>
#include <fstream>
#include <Windows.h>
#include <fstream>
#include <iostream>



const int MAX_SIZE = 500000;

int read_file(std::ifstream& input, int* result) {
	int i = 0;
	while (input >> result[i] && i < MAX_SIZE)
	{
		i++;
	}
	return i;
}

void write_file(std::ofstream& output, int arrSize, int* arr) {
	if (output.is_open()) {
		for (int i = 0; i < arrSize; i++)
			output << arr[i] << " ";
		output.close();
	}
	else
		std::cout << "Unable to open an output file.\n";
}



int lomuto_partition(int* A, int lo, int hi) {
	int pivot = A[hi];
	int i = lo - 1;
	for (int j = lo; j < hi; j++) {
		if (A[j] <= pivot) {
			i++;
			int temp = A[i];
			A[i] = A[j];
			A[j] = temp;
		}
	}
	int temp = A[i + 1];
	A[i + 1] = A[hi];
	A[hi] = temp;
	return i + 1;
}



int partition(int* A, int lo, int hi) {
	int pivot = A[lo];
	int i = lo - 1;
	int j = hi + 1;
	while (true) {
		do
			i = i + 1;
		while (A[i] < pivot);

		do
			j = j - 1;
		while (A[j] > pivot);

		if (i >= j)
			return j;

		int temp = A[i];
		A[i] = A[j];
		A[j] = temp;
	}
}



void quicksort(int* A, int lo, int hi) {
	if (lo < hi) {
		int p = partition(A, lo, hi);
		quicksort(A, lo, p);
		quicksort(A, p + 1, hi);
	}
}


int sort_recursive(int* arr, int arrSize, int currProcRank, int maxRank, int rankIndex) {
	MPI_Status status;

	
	int shareProc = currProcRank + pow(2, rankIndex);
	rankIndex++;

	
	if (shareProc > maxRank) {
		quicksort(arr, 0, arrSize - 1);
		return 0;
	}

		
	int j = 0;
	int pivotIndex;
	do {
		pivotIndex = lomuto_partition(arr, j, arrSize - 1);
		j++;
	} while (pivotIndex == j - 1);

		
	if (pivotIndex <= arrSize - pivotIndex) {
		MPI_Send(arr, pivotIndex - 1, MPI_INT, shareProc, pivotIndex, MPI_COMM_WORLD);
		sort_recursive((arr + pivotIndex + 1), (arrSize - pivotIndex - 1), currProcRank, maxRank, rankIndex);
		MPI_Recv(arr, pivotIndex - 1, MPI_INT, shareProc, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
	else {
		MPI_Send((arr + pivotIndex + 1), arrSize - pivotIndex - 1, MPI_INT, shareProc, pivotIndex + 1, MPI_COMM_WORLD);
		sort_recursive(arr, (pivotIndex + 1), currProcRank, maxRank, rankIndex);
		MPI_Recv((arr + pivotIndex + 1), arrSize - pivotIndex - 1, MPI_INT, shareProc, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
}

void generateArray(const std::string& xsFilename, int length) {
	std::ofstream xsFile(xsFilename, std::ios_base::trunc);
	xsFile << length << ' ';
	for (int i = 0; i < length; i++) {
		xsFile << std::rand() << ' ';
	}
	xsFile.close();
}


#endif