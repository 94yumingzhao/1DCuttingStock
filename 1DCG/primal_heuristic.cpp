// 2023-03-16

#include "CSCG.h"
using namespace std;

void InitModelMatrix(All_Values& Values, All_Lists& Lists)
{
	int item_types_num = Values.item_types_num;
	int all_rows_num = item_types_num;
	int all_cols_num = item_types_num;

	for (int col = 0; col < all_cols_num; col++)
	{
		vector<double> temp_col;
		for (int row = 0; row < all_rows_num; row++)
		{
			if (row == col)
			{
				double temp_val = 0;
				temp_val = Values.stock_length / Lists.all_item_types_list[row].item_type_length;
				temp_col.push_back(temp_val);
			}
			else
			{
				temp_col.push_back(0);
			}
		}
		Lists.model_matrix.push_back(temp_col);
	}
	cout << endl;
}
