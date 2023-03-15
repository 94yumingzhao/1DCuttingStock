﻿// 2023-03-01
// 列生成求解新生成节点

#include "CSBP.h"
using namespace std;

bool SolveNewNodeFirstMasterProblem(
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



	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	// set cons bound
	int item_types_num = Values.item_types_num;
	for (size_t k = 0; k < item_types_num; k++)
	{
		int item_type_demand = Lists.all_item_types_list[k].demand;
		con_min.add(IloNum(item_type_demand)); // cons > demand
		con_max.add(IloNum(IloInfinity)); // 
	}

	// set cons
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP); // add cons to the model

	// set model matrix 
	size_t cols_num = this_node.model_matrix.size();
	size_t rows_num = item_types_num;

	for (size_t col = 0; col < cols_num; col++)
	{
		IloNum obj_coeff_1 = 1;
		IloNumColumn CplexCol = Obj_MP(obj_coeff_1); // Init a col

		for (size_t row = 0; row < rows_num; row++) // set rows in this col
		{
			IloNum row_coeff = this_node.model_matrix[col][row];
			CplexCol += Cons_MP[row](row_coeff); // set coeff
		}

		string X_name = "X_" + to_string(col + 1); // var name

		// Case 1 : 
		// This var is the to-branching-var of Parent Node
		// Branch this var to be an integer
		if (col == this_node.branching_var_idx)
		{
			if (Values.tree_branching_status == 1)
			{
				IloNum branching_val = this_node.branching_var_val_floor;
				printf("\n	x_var_%zd is set as %f, branching", col + 1, branching_val);

				IloNumVar Var(CplexCol, branching_val, branching_val, ILOFLOAT, X_name.c_str()); // Init and set the to-branching-var
				Vars_MP.add(Var);
			}

			if (Values.tree_branching_status == 2)
			{
				IloNum branching_val = this_node.branching_var_val_ceil;
				printf("\n	x_var_%zd is set as %f, branching", col + 1, branching_val);

				IloNumVar Var(CplexCol, branching_val, branching_val, ILOFLOAT, X_name.c_str()); // Init and set the to-branching-var
				Vars_MP.add(Var);
			}		
		}

		// Case 2:
		// var of this col is not the to-branching-var of Parent Node
		else
		{
			// Case 2.1
			size_t branched_num = this_node.branched_vars_val_list.size();
			int find_flag = -1;

			for (size_t idx = 0; idx < branched_num; idx++) // loop of all branched-vars in previous Nodes
			{
				int branched_col = this_node.branched_vars_idx_list[idx];
				if (col == branched_col) // var of this col is a branched-var in Parent Node
				{
					IloNum branched_val = this_node.branched_vars_val_list[idx];
					printf("\n	x_var_%zd is set as %f, branched", col + 1, branched_val);

					IloNumVar Var(CplexCol, branched_val, branched_val, ILOFLOAT, X_name.c_str()); // Init and set var
					Vars_MP.add(Var);

					find_flag = 1;
					break;
				}
			}

			// Case 2.2
			// The var of this col is NOT a branched-var in previous Nodes
			if (find_flag == -1)
			{
				IloNum var_min = 0;
				IloNum var_max = IloInfinity;
				IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // Init and set var
				Vars_MP.add(Var);
			}
		}
		CplexCol.end(); // end this col
	}

	printf("\n\n################## Node_%d MP-1 CPLEX SOLVING START ##################\n\n", this_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("NewNodeProblem.lp");
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

		for (size_t col = 0; col < cols_num; col++)
		{
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			if (soln_val > 0) // feasible soln > 0
			{
				fsb_num++;
				int soln_int_val = int(soln_val);
				if (soln_int_val == soln_val)
				{
					// ATTTENTION:  
					if (soln_int_val >= 1)
					{
						int_num++;
						printf("	var_x_%zd = %f int\n", col + 1, soln_val);
					}
				}
				else
				{
					printf("	var_x_%zd = %f\n", col + 1, soln_val);
				}
			}
		}


		printf("\n	BRANCHED VARS: \n\n");
		size_t branched_num = this_node.branched_vars_val_list.size();
		for (size_t k = 0; k < branched_num; k++)
		{
			printf("	var_x_%d = %f branched \n",
				this_node.branched_vars_idx_list[k] + 1, this_node.branched_vars_val_list[k]);
		}

		printf("\n	DUAL PRICES: \n\n");
		this_node.dual_prices_list.clear();

		for (size_t row = 0; row < rows_num; row++)
		{
			double dual_price = MP_cplex.getDual(Cons_MP[row]);
			printf("	dual_r_%zd = %f\n", row + 1, dual_price);
			this_node.dual_prices_list.push_back(dual_price);
		}

		this_node.lower_bound = MP_cplex.getValue(Obj_MP);
		printf("\n	Node_%d MP-1:\n", this_node.index);
		printf("\n	Lower Bound = %f", this_node.lower_bound);
		printf("\n	NUM of all solns = %zd", cols_num);
		printf("\n	NUM of fsb solns = %d", fsb_num);
		printf("\n	NUM of int solns = %d", int_num);
		printf("\n	NUM of branched-vars = %zd\n", branched_num);
	}

	MP_cplex.end();
	return MP_flag;
}