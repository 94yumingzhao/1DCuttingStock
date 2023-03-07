// 2023-03-01
// 求解子问题

#include "CSBP.h"
using namespace std;

int SolveSubProblem(All_Values& Values, All_Lists& Lists,Node this_node)
{
	int ITEM_TYPES_NUM = Values.item_types_num;

	IloEnv SP_env;
	IloModel SP_model(SP_env);
	IloNumVarArray SP_vars_list(SP_env);

	// var >= 0
	float var_min = 0; 
	float var_max = IloInfinity; 

	// Init and set vars of SP
	for (int k = 0; k < ITEM_TYPES_NUM; k++) 
	{
		IloNumVar SP_var(SP_env, var_min, var_max, ILOINT); 
		SP_vars_list.add(SP_var); 
	}

	// Init and set obj of SP
	IloExpr SP_obj_sum1(SP_env); 
	for (int k = 0; k < ITEM_TYPES_NUM; k++) 
	{
		SP_obj_sum1 += this_node.dual_prices_list[k] * SP_vars_list[k];
	}
	IloObjective SP_obj = IloMaximize(SP_env, SP_obj_sum1); 
	SP_model.add(SP_obj); 

	// Init and set the only on con of SP
	IloExpr SP_constriant1_sum1(SP_env);
	for (int k = 0; k < ITEM_TYPES_NUM; k++) 
	{
		SP_constriant1_sum1 += Lists.all_item_types_list[k].length * SP_vars_list[k];  
	}
	SP_model.add(SP_constriant1_sum1 <= Values.stock_length); 

	// solve SP
	printf("\n	Continue to solve the SP-MP-%d\n", this_node.iter);
	printf("\n\n####################### SP-MP-%d CPLEX SOLVING START #######################\n", this_node.iter);
	IloCplex SP_cplex(SP_env);
	SP_cplex.extract(SP_model);
	SP_cplex.exportModel("SubProblem.lp");
	int SP_solve = SP_cplex.solve(); // 求解子问题
	printf("####################### SP-MP-%d CPLEX SOLVING END #########################\n", this_node.iter);
	
	// print everything
	if (SP_solve == 0)
	{
		printf("\n	The SP-MP-%d is NOT FEASIBLE\n", this_node.iter);
	}
	else
	{
		printf("\n	The SP-MP-%d is FEASIBLE\n", this_node.iter);
	}

	printf("\n	The OBJ of SP-MP-%d is %f\n\n", this_node.iter, SP_cplex.getValue(SP_obj));

	for (int k = 0; k < ITEM_TYPES_NUM; k++)
	{
		float soln_val = SP_cplex.getValue(SP_vars_list[k]);
		printf("	var_y_%d = %f\n", k + 1, soln_val);
	}

	// Init the new col for MP
	this_node.new_col.clear(); // 
	vector<float> New_Column; 
	int SP_solve_flag = 0;

	// Case 1:
	// If the obj of SP, which is called reduce cost,, is larger than 1
	// Then the optimal solns of this Node is not find, continue CG loop 
	if (SP_cplex.getValue(SP_obj) > 1 + RC_EPS) 
	{
		printf("\n	We got a REDUCED COST = %f that LARGER than 1\n\n	A NEW COLUMN will be added to the MP\n", SP_cplex.getValue(SP_obj));

		// set the new col for MP
		for (int k = 0; k < ITEM_TYPES_NUM; k++)
		{
			float var_value = SP_cplex.getValue(SP_vars_list[k]);
			New_Column.push_back(var_value); 
		}

		// print the new col
		for (int k = 0; k < ITEM_TYPES_NUM; k++)
		{
			printf("	New Column Row %d = %f\n", k + 1, New_Column[k]);
		}

		// add new col to the matrix of MP
		this_node.all_cols_list.push_back(New_Column);
		this_node.new_col = New_Column;

		SP_solve_flag = 0;
	}

	// Case 2
	// If the reduced cost is smaller than 1
	// Then the optimal solns of this Node is find, break CG loop
	else
	{
		printf("\n	We got a REDUCED COST = %f that NOT LARGER than 1\n\n	COLUMN GENERATION procedure stops here!\n", SP_cplex.getValue(SP_obj));
		printf("\n	Return to the last MP and get the FINAL INTEGER solutions\n");
		SP_solve_flag = 1; 
	}

	return SP_solve_flag; // 1-- break CG loop, 0 -- continue CG loop
}