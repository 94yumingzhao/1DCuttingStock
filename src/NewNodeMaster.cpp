﻿// 2023-03-01
// 列生成求解新生成节点

#include "CSBP.h"
using namespace std;

bool SolveNewNodeFirstMasterProblem(
	int branch_flag,
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	Node& this_node,
	Node& parent_node)
{

	int item_types_num = Values.item_types_num;

	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	// set cons bound
	for (int i = 0; i < item_types_num; i++)
	{
		con_min.add(IloNum(Lists.all_item_types_list[i].demand)); // cons > demand
		con_max.add(IloNum(IloInfinity)); // 
	}

	// set cons
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP); // add cons to the model

	// num of cols
	size_t all_cols_num = parent_node.model_matrix.size();
	printf("\n	Current number of columns is %zd \n", all_cols_num);

	// set model_matrix of master problem
	for (int col = 0; col < all_cols_num; col++)
	{
		int obj_coeff_1 = 1;
		IloNumColumn CplexCol = Obj_MP(obj_coeff_1); // Init a col

		for (int row = 0; row < item_types_num; row++) // set rows in this col
		{
			float row_coeff = parent_node.model_matrix[col][row];
			CplexCol += Cons_MP[row](row_coeff); // set coeff
		}

		float var_min; // var LB
		float var_max; // var UB
		string X_name = "X_" + to_string(col + 1); // var name

		// Case 1 : 
		// This var is the branch var of PN, and branch the var to be an integer
		if (col == parent_node.branch_var_index)
		{
			float branch_var_val = parent_node.fsb_solns_list[col];
			float final_val = 0;

			// Case 1.1
			// left branch of PN, set the var to be its floor-val.
			if (branch_flag == 0)
			{
				final_val = parent_node.branch_floor_val;
				printf("\n	The FLOOR value of var %f  =  %f\n", branch_var_val, final_val);
			}

			// Case 1.2
			// right branch of PN, set the var to be its ceil-val
			if (branch_flag == 1)
			{
				final_val = parent_node.branch_ceil_val;
				printf("\n	The CEIL value of var %f = %f\n", branch_var_val, final_val);
			}
			//  var = floor-val or ceil-val
			var_min = final_val;
			var_max = final_val;

			// Init and set the branching var
			// ATTENTION: thought the var is an integer, it must be set as a FLOAT in code here
			IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
			Vars_MP.add(Var);
		}

		// Case 2:
		// The var of this col is not the branch var of PN
		if (col != parent_node.branch_var_index)
		{
			// Case 2.1
			if (col == parent_node.int_cols_list[col])
			{
				float int_soln_val = parent_node.int_solns_list[col];

				//  var = int-soln
				var_min = int_soln_val;
				var_max = int_soln_val;

				// Init and set var
				IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
				Vars_MP.add(Var);
			}
			// Case 2.2
			if (col != parent_node.int_cols_list[col])
			{
				// var >= 0
				var_min = 0;
				var_max = IloInfinity;

				// Init and set var
				IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
				Vars_MP.add(Var);
			}		
		}	
		CplexCol.end(); // end this col
	}

	printf("\n\n################## Node_%d MP-1 CPLEX SOLVING START ##################\n\n",this_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	//MP_cplex.exportModel("NewNodeProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("\n################## Node_%d MP-1 CPLEX SOLVING END ####################\n\n", this_node.index);

	int fsb_num = 0;
	int int_num = 0;
	if (MP_flag == 0)
	{
		printf("\n	Node_%d MP-1 is NOT FEASIBLE\n", this_node.index);
	}
	else
	{
		printf("\n	Node_%d MP-1 is FEASIBLE\n", this_node.index);
		printf("\n	OBJ of Node_%d MP-1 is %f\n\n", this_node.index, MP_cplex.getValue(Obj_MP));

		for (int col = 0; col < all_cols_num; col++)
		{
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			if (soln_val > 0)
			{
				fsb_num++;
				printf("	var_x_%d = %f\n", col + 1, soln_val);

				int soln_int_val = int(soln_val);
				if (soln_int_val == soln_val)
				{
					int_num++;
				}
			}
		}

		printf("\n	DUAL PRICES: \n\n");
		this_node.dual_prices_list.clear();

		for (int row = 0; row < item_types_num; row++)
		{
			float dual_price = MP_cplex.getDual(Cons_MP[row]);
			printf("	dual_r_%d = %f\n", row + 1, dual_price);
			this_node.dual_prices_list.push_back(dual_price);
		}
	}

	this_node.lower_bound = MP_cplex.getValue(Obj_MP);
	printf("\n	Node_%d MP-1:\n", this_node.index);
	printf("\n	Lower Bound:   %f\n", this_node.lower_bound);
	printf("\n	NUM of all solns: %zd\n",all_cols_num);
	printf("\n	NUM of fsb solns: %d\n", fsb_num);
	printf("\n	NUM of int solns: %d\n", int_num);

	MP_cplex.end();
	return MP_flag;
}