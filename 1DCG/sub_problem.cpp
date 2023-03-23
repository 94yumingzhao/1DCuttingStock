// 2023-03-16

#include "CSCG.h"
using namespace std;

bool SolveSubProblem(All_Values& Values, All_Lists& Lists) {
	bool optimal_flag = 0;

	IloEnv Env_SP;
	IloModel Model_SP(Env_SP);
	IloNumVarArray Vars_SP(Env_SP);

	// var >= 0
	IloNum var_min = 0; // var LB
	IloNum var_max = IloInfinity; // var UB

	// Init and set vars of SP
	int item_types_num = Values.item_types_num;
	for (int k = 0; k < item_types_num; k++) {
		IloNumVar var(Env_SP, var_min, var_max, ILOINT); // ATTENTION: all vars in SP must be set as INT 
		Vars_SP.add(var);
	}

	// Init and set obj of SP
	IloExpr obj_sum(Env_SP);
	for (int k = 0; k < item_types_num; k++) {
		obj_sum += Lists.dual_prices_list[k] * Vars_SP[k];
	}

	IloObjective Obj_SP = IloMaximize(Env_SP, obj_sum);
	Model_SP.add(Obj_SP);

	// Init and set the only one con of SP
	IloExpr con_sum(Env_SP);
	for (int k = 0; k < item_types_num; k++) {
		con_sum += Lists.all_item_types_list[k].item_type_length * Vars_SP[k];
	}
	Model_SP.add(con_sum <= Values.stock_length);

	// solve SP
	printf("\n\n################ SP-%d CPLEX SOLVING START ################\n\n", Values.iter);
	IloCplex Cplex_SP(Env_SP);
	Cplex_SP.extract(Model_SP);
	//Cplex_SP.exportModel("SubProblem.lp");
	bool SP_flag = Cplex_SP.solve(); // solve sub problem
	printf("\n################ SP-%d CPLEX SOLVING OVER ##################\n", Values.iter);

	// print everything
	if (SP_flag == 0) {
		printf("\n	SP-%d is NOT FEASIBLE\n", Values.iter);
	}
	else {
		printf("\n	SP-%d is FEASIBLE\n", Values.iter);

		printf("\n	OBJ of SP-%d is %f\n\n", Values.iter, Cplex_SP.getValue(Obj_SP));

		printf("\n	INT SOLNS of SP: \n\n");
		for (int k = 0; k < item_types_num; k++) {
			IloNum soln_val = Cplex_SP.getValue(Vars_SP[k]);
			printf("	var_y_%d = %f\n", k + 1, soln_val);
		}


		// Case 1:If the reduced cost is larger than 1, the optimal solns are not find, continue CG loop 
		if (Cplex_SP.getValue(Obj_SP) > 1 + RC_EPS) {
			printf("\n	We got a REDUCED COST = %f that is LARGER than 1\n\n	INT SOLNS of SP will be added to the MP as a NEW COLUMN\n", Cplex_SP.getValue(Obj_SP));

			// set the new col for MP
			Lists.new_col.clear();
			for (int k = 0; k < item_types_num; k++) {
				double var_val = Cplex_SP.getValue(Vars_SP[k]);
				Lists.new_col.push_back(var_val);
			}

			optimal_flag = 0;
		}

		// Case 2: If the reduced cost is smaller than 1, then the optimal solns are find, break CG loop
		else {
			printf("\n	We got a REDUCED COST = %f that is NOT LARGER than 1\n\n	COLUMN GENERATION loop stops here!\n\n\n",
				Cplex_SP.getValue(Obj_SP));

			optimal_flag = 1;
		}
	}

	Obj_SP.removeAllProperties();
	Obj_SP.end();
	Vars_SP.clear();
	Vars_SP.end();
	Cplex_SP.removeAllProperties();
	Cplex_SP.end();
	Model_SP.removeAllProperties();
	Model_SP.end();
	Env_SP.removeAllProperties();
	Env_SP.end();

	return optimal_flag; // 1-- break CG loop, 0 -- continue CG loop
}