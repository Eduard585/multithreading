//#include "stdafx.h"
#include "mpi.h"
#include "header.h"


int read_file(std::ifstream& input, int* result);
int partition(int* A, int lo, int hi);
void quicksort(int* A, int lo, int hi);
int lomuto_partition(int* A, int lo, int hi);
int sort_recursive(int* arr, int arrSize, int currProcRank, int maxRank, int rankIndex);
void write_file(std::ofstream& output, int arrSize, int* arr);
void generateArray(const std::string& xsFilename, int rows, double initNum);

int main(int argc, char* argv[]) {
	if (argc == 3) {		
		generateArray(argv[1], atoi(argv[2]));
		return 0;
	}
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	

	if (argc != 4) { std::cout << "Usage: enter input and output filenames, number of iterations to perform sorting.\n"; return 1; }
	int iter_count = atoi(argv[3]);
	
	for (int iter = 0; iter < iter_count; iter++) {
		
		int rankPower = 0;
		while (pow(2, rankPower) <= rank)
			rankPower++;

		if (rank == 0) {
			auto start = std::chrono::high_resolution_clock::now();

			std::ifstream input(argv[1]);
			if (!input) { std::cout << "Input file not found.\n"; return 1; }

			
			int* numbers = (int*)malloc(MAX_SIZE * sizeof(int));
			int arraySize = read_file(input, numbers);

			sort_recursive(numbers, arraySize, rank, size - 1, rankPower);

			std::ofstream output(argv[2]);
			write_file(output, Multi, numbers);
			auto finish = std::chrono::high_resolution_clock::now();
			std::cout << double(std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()) / 1000 << std::endl;
		}
		else {
			MPI_Status status;
			int subarray_size;
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			MPI_Get_count(&status, MPI_INT, &subarray_size);

			int source_process = status.MPI_SOURCE;
			int* subarray = (int*)malloc(subarray_size * sizeof(int));

			MPI_Recv(subarray, subarray_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sort_recursive(subarray, subarray_size, rank, size - 1, rankPower);
			MPI_Send(subarray, subarray_size, MPI_INT, source_process, 0, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}