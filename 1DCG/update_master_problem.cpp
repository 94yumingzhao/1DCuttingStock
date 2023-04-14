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

	OutputMasterProblem(Values, Lists);
	OutputDualMasterProblem(Values, Lists);

	int N_num = Values.item_types_num;
	int J_num = Lists.model_matrix.size();

	// set the obj para of the new col
	IloNum obj_para = 1;
	IloNumColumn CplexCol = Obj_MP(obj_para);

	// add the new col ro the model of MP 
	for (int row = 0; row < N_num; row++) {
		IloNum row_para = Lists.new_col[row];
		CplexCol += Cons_MP[row](row_para);
	}

	// var >= 0
	string var_name = "X_" + to_string(J_num + 1); // var name
	IloNum var_min = 0; // var LB
	IloNum var_max = IloInfinity; // var UB
	IloNumVar X_Var(CplexCol, var_min, var_max, ILOFLOAT, var_name.c_str());
	Vars_MP.add(X_Var); // add new var

	CplexCol.end(); // end this IloNumColumn object

	// solve the new updated MP
	printf("\n\n################ MP-%d CPLEX SOLVING START ################\n\n", Values.iter);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("Update Master Problem.lp");
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
					printf("\t var_x_%d = %f int\n", col + 1, soln_val);
				}
			}
			else {
				printf("\t var_x_%d = %f\n", col + 1, soln_val);
			}
		}
	}

	// print and store dual-prices of MP cons
	Lists.dual_prices_list.clear();
	printf("\n\t DUAL PRICES of MP cons: \n\n");

	for (int row = 0; row < N_num; row++) {
		double dual_val = MP_cplex.getDual(Cons_MP[row]); // get dual-prices of all cons
		printf("\t dual_r_%d = %f\n", row + 1, dual_val);
		Lists.dual_prices_list.push_back(dual_val);
	}

	printf("\n\t MP-%d:", Values.iter);
	printf("\n\t Lower Bound = %f", MP_cplex.getValue(Obj_MP));
	printf("\n\t NUM of all solns = %d", all_cols_num);
	printf("\n\t NUM of fsb-solns = %d", fsb_num);
	printf("\n\t NUM of int-solns = %d", int_num);

	MP_cplex.end();
	return MP_flag;
}

bool SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP) {

	OutputMasterProblem(Values, Lists);
	OutputDualMasterProblem(Values, Lists);

	int N_num = Values.item_types_num;
	int J_num = Lists.model_matrix.size();

	IloNumArray  con_min(Env_MP);
	IloNumArray  con_max(Env_MP);
	for (int row = 0; row < N_num; row++) {
		int demand =
			Lists.all_item_types_list[row].demand; // con >= demand
		con_min.add(IloNum(demand)); // con LB
		con_max.add(IloNum(IloInfinity));  // con UB
	}
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);
	con_min.end();
	con_max.end();

	// 列建模
	for (int col = 0; col < J_num; col++) {
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para);
		for (int row = 0; row < N_num; row++) {
			IloNum row_para = Lists.model_matrix[row][col];
			CplexCol += Cons_MP[row](row_para);
		}
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		string var_name = "X_" + to_string(col + 1); // var name
		IloNumVar Var(CplexCol, var_min, var_max, ILOINT, var_name.c_str()); 		// var >= 0
		Vars_MP.add(Var);

		CplexCol.end(); // end this IloNumColumn object
	}

	// solve model
	printf("\n\n################  MP-Final CPLEX SOLVING START ################\n");
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("Final Master Problem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("################  MP-Final CPLEX SOLVING OVER ##################\n");

	int fsb_num = 0;
	int int_num = 0;

	// judge model feasibility
	if (MP_flag == 0) {
		printf("\n\t MP-Final is NOT FEASIBLE\n");
	}
	else {
		printf("\n\t MP-Final is FEASIBLE\n");
		printf("\n\t Obj = %f\n\n", MP_cplex.getValue(Obj_MP));
		printf("\n\t X Vars: \n\n");

		for (int col = 0; col < J_num; col++) {
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			if (soln_val > 0) // feasible soln > 0
			{
				fsb_num++;
				int soln_int_val = int(soln_val);
				if (soln_int_val == soln_val) {
					if (soln_int_val >= 1) {
						int_num++;
						printf("\t var_x_%d = %f int\n", col + 1, soln_val);
					}
				}
				else {
					printf("\t var_x_%d = %f\n", col + 1, soln_val);
				}
			}
		}

		printf("\n\t MP-Final:\n");
		printf("\n\t Lower Bound = %f", MP_cplex.getValue(Obj_MP));
		printf("\n\t NUM of all solns = %d", J_num);
		printf("\n\t NUM of fsb solns = %d", fsb_num);
		printf("\n\t NUM of int solns = %d", int_num);
	}

	MP_cplex.end();
	return MP_flag;

}

