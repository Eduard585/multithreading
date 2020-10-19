#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "mpi.h"
#include "utils.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>


using namespace std;
using namespace utils;

int main(int argc, char** argv) {

    int* nums = nullptr;
    int numsSize = 0;

    if (argc == 4) {
        createFileOfNums(argv[1], atoi(argv[2]), -10000, 10000);
    }
    else if (argc != 3) {
        cout << "Not all arguments specified!" << endl;
		_CrtDumpMemoryLeaks();
        return (1);
    }
    else
    {
        string inFile(argv[1]);
        string outFile(argv[2]);
        MPI_Init(&argc, &argv);

        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {
            tie(nums, numsSize) = loadNums(inFile);
        }
        MPI_Bcast(&numsSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        double wtime = MPI_Wtime();

        if (size == 1) {
            quicksort(nums, numsSize);
        }
        else {
            quicksortMPI(nums, numsSize, rank, size);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        wtime = MPI_Wtime() - wtime;

        if (rank == 0) {
            char buf[1000];
            int res = -1;
            res = snprintf(buf, sizeof(buf), "%d Process: %d Size, %f Time\n", size, numsSize, wtime);
            string str = "error!";
            if (res >= 0 && res < sizeof(buf))
                str = buf;
            cout << str << endl;
            writeLogTime(str);
            printAnswer(nums, numsSize, outFile);
			delete[]nums;
        }
        MPI_Finalize();
    }
	_CrtDumpMemoryLeaks();
    return 0;
}