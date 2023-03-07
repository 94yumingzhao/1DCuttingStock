// 2023-03-01
// 求解子问题

#include "CSBP.h"
using namespace std;

int SolveSubProblem(All_Values& Values, All_Lists& Lists,Node& this_node)
{
	int item_types_num = Values.item_types_num;

	IloEnv Env_SP;
	IloModel Model_SP(Env_SP);
	IloNumVarArray Vars_SP(Env_SP);

	// var >= 0
	float var_min = 0; 
	float var_max = IloInfinity; 

	// Init and set vars of SP
	for (int k = 0; k < item_types_num; k++) 
	{
		IloNumVar SP_var(Env_SP, var_min, var_max, ILOINT); 
		Vars_SP.add(SP_var); 
	}

	// Init and set obj of SP
	IloExpr obj_sum(Env_SP); 
	for (int k = 0; k < item_types_num; k++) 
	{
		obj_sum += this_node.dual_prices_list[k] * Vars_SP[k];
	}
	IloObjective Obj_SP = IloMaximize(Env_SP, obj_sum);
	Model_SP.add(Obj_SP); 

	// Init and set the only on con of SP
	IloExpr con_sum(Env_SP);
	for (int k = 0; k < item_types_num; k++) 
	{
		con_sum += Lists.all_item_types_list[k].length * Vars_SP[k];
	}
	Model_SP.add(con_sum <= Values.stock_length);

	// solve SP
	printf("\n	Continue to solve the SP-MP-%d\n", this_node.iter);
	printf("\n\n####################### SP-MP-%d CPLEX SOLVING START #######################\n", this_node.iter);
	IloCplex Cplex_SP(Env_SP);
	Cplex_SP.extract(Model_SP);
	Cplex_SP.exportModel("SubProblem.lp");
	int Solve_SP = Cplex_SP.solve(); // 求解子问题
	printf("####################### SP-MP-%d CPLEX SOLVING END #########################\n", this_node.iter);
	
	// print everything
	if (Solve_SP == 0)
	{
		printf("\n	The SP-MP-%d is NOT FEASIBLE\n", this_node.iter);
	}
	else
	{
		printf("\n	The SP-MP-%d is FEASIBLE\n", this_node.iter);
	}

	printf("\n	The OBJ of SP-MP-%d is %f\n\n", this_node.iter, Cplex_SP.getValue(Obj_SP));

	for (int k = 0; k < item_types_num; k++)
	{
		float soln_val = Cplex_SP.getValue(Vars_SP[k]);
		printf("	var_y_%d = %f\n", k + 1, soln_val);
	}

	// Init the new col for MP
	this_node.new_col.clear(); // 
	vector<float> New_Column; 
	int solve_flag = 0;

	// Case 1:
	// If the obj of SP, which is called reduce cost,, is larger than 1
	// Then the optimal solns of this Node is not find, continue CG loop 
	if (Cplex_SP.getValue(Obj_SP) > 1 + RC_EPS) 
	{
		printf("\n	We got a REDUCED COST = %f that LARGER than 1\n\n	A NEW COLUMN will be added to the MP\n", Cplex_SP.getValue(Obj_SP));

		// set the new col for MP
		for (int k = 0; k < item_types_num; k++)
		{
			float var_value = Cplex_SP.getValue(Vars_SP[k]);
			New_Column.push_back(var_value); 
		}

		// print the new col
		for (int k = 0; k < item_types_num; k++)
		{
			printf("	New Column Row %d = %f\n", k + 1, New_Column[k]);
		}

		this_node.new_col = New_Column;
		solve_flag = 0;
	}

	// Case 2
	// If the reduced cost is smaller than 1
	// Then the optimal solns of this Node is find, break CG loop
	else
	{
		printf("\n	We got a REDUCED COST = %f that NOT LARGER than 1\n\n	COLUMN GENERATION procedure stops here!\n", Cplex_SP.getValue(Obj_SP));
		printf("\n	Return to the last MP and get the FINAL INTEGER solutions\n");
		solve_flag = 1; 
	}

	return solve_flag; // 1-- break CG loop, 0 -- continue CG loop
}