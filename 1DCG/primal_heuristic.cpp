// 2023-03-16

#include "CSCG.h"
using namespace std;

void PrimalHeuristic(All_Values& Values, All_Lists& Lists) {

	int N_num = Values.item_types_num;;
	int J_num = N_num;

	for (int col = 0; col < J_num; col++) {
		vector<double> temp_col;
		for (int row = 0; row < N_num; row++) {
			if (row == col) {
				double temp_val = 0;
				temp_val = Values.stock_length / Lists.all_item_types_list[row].length;
				temp_col.push_back(temp_val);
			}
			else {
				temp_col.push_back(0);
			}
		}
		Lists.model_matrix.push_back(temp_col);
	}
	cout << endl;
}
