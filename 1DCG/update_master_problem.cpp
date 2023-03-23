// 2023-03-16

#include "CSCG.h"
using namespace std;

bool SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP) {

	// add new col to the matrix of MP
	Lists.model_matrix.push_back(Lists.new_col);

	// set the obj para of the new col
	IloNum obj_para = 1;
	IloNumColumn CplexCol = Obj_MP(obj_para);

	// add the new col ro the model of MP 
	int all_rows_num = Values.item_types_num;
	for (int row = 0; row < all_rows_num; row++) {
		IloNum row_para = Lists.new_col[row];
		CplexCol += Cons_MP[row](row_para);
	}

	// var >= 0
	IloNum var_min = 0; // var LB
	IloNum var_max = IloInfinity; // var UB
	IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT);
	Vars_MP.add(Var); // add new var

	CplexCol.end(); // end this IloNumColumn object

	// solve the new updated MP
	printf("\n\n################ MP-%d CPLEX SOLVING START ################\n\n", Values.iter);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("updateMasterProblem.lp");
	bool MP_flag = MP_cplex.solve(); // solve MP
	printf("\n################ MP-%d CPLEX SOLVING OVER ##################\n", Values.iter);

	printf("\n	OBJ of MP-%d is %f\n\n", Values.iter, MP_cplex.getValue(Obj_MP));

	// print fsb-solns of the updated MP
	int fsb_num = 0;
	int int_num = 0;
	int all_cols_num = Lists.model_matrix.size();

	printf("\n	FEASIBLE SOLNS of MP: \n\n");
	for (int col = 0; col < all_cols_num; col++) {
		IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
		if (soln_val > 0) {// feasible soln > 0
			fsb_num++;
			int soln_int_val = int(soln_val);
			if (soln_int_val == soln_val) {
				if (soln_int_val >= 1) {
					int_num++;
					printf("	var_x_%d = %f int\n", col + 1, soln_val);
				}
			}
			else {
				printf("	var_x_%d = %f\n", col + 1, soln_val);
			}
		}
	}

	// print and store dual-prices of MP cons
	Lists.dual_prices_list.clear();
	printf("\n	DUAL PRICES of MP cons: \n\n");

	for (int row = 0; row < all_rows_num; row++) {
		double dual_val = MP_cplex.getDual(Cons_MP[row]); // get dual-prices of all cons
		printf("	dual_r_%d = %f\n", row + 1, dual_val);
		Lists.dual_prices_list.push_back(dual_val);
	}

	printf("\n	MP-%d:\n", Values.iter);
	printf("\n	Lower Bound = %f", MP_cplex.getValue(Obj_MP));
	printf("\n	NUM of all solns = %d", all_cols_num);
	printf("\n	NUM of fsb-solns = %d", fsb_num);
	printf("\n	NUM of int-solns = %d", int_num);

	MP_cplex.end();
	return MP_flag;
}

