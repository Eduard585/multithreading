#include <iostream>
#include <tuple>
#include <vector>
#include "utils.h"
#include "Jacobi.hpp"
#include "mpi.h"


using namespace std;

int main(int argc, char* argv[])
{
	utils utils;
	if (argc == 3) {
		utils::generateMatrix(argv[1], atoi(argv[2]));
	}
	else if (argc == 4) {
		utils::generateInitX(argv[1], atoi(argv[2]), atof(argv[3]));
	}
	else if (argc != 5) {
		cout << "Not all arguments specified!" << endl;
		return (1);
	}
	else {
		double wtime;
		int rowsCount, nBar;
		int* countsALocal, * countsBLocal;      /* размеры фрагментов массива */
		int* displacementsALocal, * displacementsBLocal; /* смещения фрагментов массива */
		double* aLocal, * bLocal;
		double* matrixBuffer = nullptr, * tempVector = nullptr;
		double* vectorBuffer = nullptr;

		string matrixFile(argv[1]);
		string xInputFile(argv[2]);
		double e(atof(argv[3]));
		string xOutputFile(argv[4]);


		MPI_Init(&argc, &argv);
		int rank, numprocs;
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);

		/*if (numprocs <= 1) {		
			throw "Need more than 1 process for completejob.";
			MPI_Abort(MPI_COMM_WORLD, 1);
		}*/

		if (rank == 0) {
			tie(matrixBuffer, tempVector, vectorBuffer, rowsCount) = utils::load(matrixFile, xInputFile);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		wtime = MPI_Wtime();

		MPI_Bcast(&rowsCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (rank != 0) {
			vectorBuffer = new double[rowsCount];
		}
		MPI_Bcast(vectorBuffer, rowsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		nBar = rowsCount / numprocs;
		int residue = rowsCount % numprocs;
		countsALocal = new int[numprocs];
		displacementsALocal = new int[numprocs];

		for (int p = 0; p < numprocs; p++) {
			countsALocal[p] = nBar * rowsCount;
			displacementsALocal[p] = p * nBar * rowsCount;
			if (p < residue) {
				countsALocal[p] += rowsCount;
				displacementsALocal[p] += (p + 1) * rowsCount;
			}
			
		}	

		aLocal = new double[countsALocal[rank]];
		countsBLocal = new int[numprocs];
		displacementsBLocal = new int[numprocs];
		for (int p = 0; p < numprocs; p++) {
			countsBLocal[p] = nBar;
			displacementsBLocal[p] = p * nBar;
			if (p < residue) {
				countsBLocal[p] += 1;
				displacementsBLocal[p] += p + 1;
			}
			
		}
		
		bLocal = new double[countsBLocal[rank]];
		MPI_Scatterv(matrixBuffer, countsALocal, displacementsALocal, MPI_DOUBLE,
			aLocal, countsALocal[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

		MPI_Scatterv(tempVector, countsBLocal, displacementsBLocal, MPI_DOUBLE,
			bLocal, countsBLocal[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

		jacobi::jacobi(rowsCount, countsBLocal[rank], aLocal, bLocal, vectorBuffer, e, rank, numprocs, countsBLocal, displacementsBLocal);

		double* result = new double[rowsCount];
		MPI_Gatherv(vectorBuffer, countsBLocal[rank], MPI_DOUBLE, result, countsBLocal, displacementsBLocal, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		wtime = MPI_Wtime() - wtime;

		if (rank == 0) {
			
			char buf[1000];
			int res = -1;
			res = snprintf(buf, sizeof(buf), "Number of processes = %d, matrix A: order of system = %d, spent %f\n", numprocs, rowsCount, wtime);
			string str = "error!";
			if (res >= 0 && res < sizeof(buf))
				str = buf;
			std::cout << str << std::endl;
			utils::writeLogTime(str);
			utils::printResultInFile(xOutputFile, result, rowsCount);

		}
		MPI_Finalize();
	}
	return 0;
}