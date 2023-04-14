#include"CSCG.h"
using namespace std;


void OutputMasterProblem(All_Values& Values, All_Lists& Lists) {

	ofstream dataFile;
	dataFile.open("Master Problem.txt", ios::app);

	int all_cols_num = Lists.model_matrix.size();
	int all_rows_num = Values.item_types_num;

	dataFile << endl;
	dataFile << "MP-" << Values.iter << endl;

	for (int col = 0; col < all_cols_num; col++) {
		dataFile << "x" << col + 1 << "\t";
	}
	dataFile << endl;


	for (int col = 0; col < all_cols_num; col++) {
		dataFile << ("-----------");
	}
	dataFile << endl;

	for (int row = 0; row < all_rows_num; row++) {
		for (int col = 0; col < all_cols_num; col++) {
			dataFile << int(Lists.model_matrix[col][row]) << "\t";
		}
		dataFile << ">=" << "\t" << int(Lists.all_item_types_list[row].demand);
		dataFile << endl;
	}
	dataFile.close();
}


void OutputDualMasterProblem(All_Values& Values, All_Lists& Lists) {

	ofstream dataFile;
	dataFile.open("Dual Master Problem.txt", ios::app);

	int all_rows_num = Lists.model_matrix.size();
	int all_cols_num = Values.item_types_num;

	dataFile << endl;
	dataFile << "MP-" << Values.iter << endl;

	for (int col = 0; col < all_cols_num; col++) {
		dataFile << "c" << col + 1 << "\t";
	}
	dataFile << endl;

	for (int col = 0; col < all_cols_num; col++) {
		dataFile << ("-----------");
	}
	dataFile << endl;

	for (int col = 0; col < all_cols_num; col++) {
		dataFile << int(Lists.all_item_types_list[col].demand) << "\t";
	}
	dataFile << endl;

	for (int col = 0; col < all_cols_num; col++) {
		dataFile << ("-----------");
	}
	dataFile << endl;

	for (int row = 0; row < all_rows_num; row++) {
		for (int col = 0; col < all_cols_num; col++) {
			dataFile << int(Lists.model_matrix[row][col]) << "\t";
		}
		dataFile << ("<=\t1  x") << row + 1 << endl;;
	}
	dataFile << endl;
}

