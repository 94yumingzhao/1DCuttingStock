﻿// 2023-03-01
// Solve Sub Problem

#include "CSBP.h"
using namespace std;

// 生成并求解子问题
bool SolveSubProblem(All_Values& Values, All_Lists& Lists, Node& this_node) {

	IloEnv Env_SP;
	IloModel Model_SP(Env_SP);
	IloNumVarArray Vars_SP(Env_SP);
	bool optimal_flag = 0;

	// Init and set vars of SP
	int item_types_num = Values.item_types_num;
	for (int k = 0; k < item_types_num; k++) {
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		IloNumVar var(Env_SP, var_min, var_max, ILOINT); // ATTENTION: all vars in SP must be set as INT 
		Vars_SP.add(var);
	}

	// Init and set obj of SP
	IloExpr obj_sum(Env_SP);
	for (int k = 0; k < item_types_num; k++) {
		obj_sum += this_node.dual_prices_list[k] * Vars_SP[k];
	}
	IloObjective Obj_SP = IloMaximize(Env_SP, obj_sum);
	Model_SP.add(Obj_SP);

	// Init and set the only one con of SP
	IloExpr con_sum(Env_SP); 
	for (int k = 0; k < item_types_num; k++) {
		con_sum += Lists.all_item_types_list[k].length * Vars_SP[k];
	}
	Model_SP.add(con_sum <= Values.stock_length);

	printf("\n\n################ Node_%d SP-%d CPLEX SOLVING START ################\n\n", this_node.index, this_node.iter);
	IloCplex Cplex_SP(Env_SP);
	Cplex_SP.extract(Model_SP);
	//Cplex_SP.exportModel("SubProblem.lp");
	bool SP_flag = Cplex_SP.solve(); // solve sub problem
	printf("\n################ Node_%d SP-%d CPLEX SOLVING OVER ##################\n", this_node.index, this_node.iter);

	// print everything
	if (SP_flag == 0) {
		printf("\n\t Node_%d MP-%d is NOT FEASIBLE\n", this_node.index, this_node.iter);
	}
	else {
		printf("\n\t Node_%d SP-%d is FEASIBLE\n", this_node.index, this_node.iter);
		printf("\n\t OBJ of Node_%d MP-%d is %f\n\n", this_node.index, this_node.iter, Cplex_SP.getValue(Obj_SP));
		for (int k = 0; k < item_types_num; k++) {
			IloNum soln_val = Cplex_SP.getValue(Vars_SP[k]);
			printf("\t var_y_%d = %f\n", k + 1, soln_val);
		}

		// Case 1:
		if (Cplex_SP.getValue(Obj_SP) > 1 + RC_EPS) { // If the reduced cost is larger than 1
			optimal_flag = 0; // the optimal solns of this Node is not find, continue CG loop
			this_node.new_col.clear(); 	// set the new col for MP
			for (int k = 0; k < item_types_num; k++) {
				double var_val = Cplex_SP.getValue(Vars_SP[k]);
				this_node.new_col.push_back(var_val);
			}			
			printf("\n\t We got a REDUCED COST = %f that LARGER than 1\n", Cplex_SP.getValue(Obj_SP));
			printf("\n\t A NEW COLUMN will be added to the MP\n");
		}

		// Case 2: 
		else { // If the reduced cost is smaller than 1	
			optimal_flag = 1; // then the optimal solns of this Node is find, break CG loop
			printf("\n\t We got a REDUCED COST = %f that NOT LARGER than 1\n", Cplex_SP.getValue(Obj_SP));
			printf("\n\t COLUMN GENERATION procedure stops here!\n");
			printf("\n\t Return to the last MP and get the FINAL INTEGER solutions\n");
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