// 2023-03-16

#include "CSCG.h"
using namespace std;

int main() {

	All_Values Values;
	All_Lists Lists;

	ofstream dataFile1;
	dataFile1.open("Master Problem.txt", ios::out | ios::trunc);
	ofstream dataFile2;
	dataFile2.open("Dual Master Problem.txt", ios::out | ios::trunc);

	ReadData(Values, Lists);
	PrimalHeuristic(Values, Lists);
	ColumnGeneration(Values, Lists);

	dataFile1.close();
	dataFile2.close();
	system("pause");
	return 0;
}