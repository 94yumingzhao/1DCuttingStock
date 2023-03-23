// 2023-03-16

#include "CSCG.h"
using namespace std;

int main() {
	All_Values Values;
	All_Lists Lists;

	tuple<int, int, int> fileTxt;
	fileTxt = ReadData(Values, Lists);
	Values.stocks_num = get<0>(fileTxt);// number of all stocks
	Values.item_types_num = Lists.all_item_types_list.size(); // number of item types
	Values.stock_length = get<2>(fileTxt); // length of a stock

	InitModelMatrix(Values, Lists);
	ColumnGeneration(Values, Lists);

	int all_cols_num = Lists.model_matrix.size();

	system("pause");
	return 0;
}