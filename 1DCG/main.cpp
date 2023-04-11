// 2023-03-16

#include "CSCG.h"
using namespace std;

int main() {

	All_Values Values;
	All_Lists Lists;

	ReadData(Values, Lists);
	PrimalHeuristic(Values, Lists);
	ColumnGeneration(Values, Lists);

	system("pause");
	return 0;
}