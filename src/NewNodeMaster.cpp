// 2023-03-01
// 列生成求解新生成节点

#include "CSBP.h"
using namespace std;

int SolveNewNodeFirstMasterProblem(
	int branch_flag,
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP_1,
	IloModel& Model_MP_1,
	IloObjective& Obj_MP_1,
	IloRangeArray& Cons_MP_1,
	IloNumVarArray& Vars_MP_1)
{
	int ITEM_TYPES_NUM = Values.item_types_num;
	Node current_node = all_nodes_list.back();

	//int node_final_cols_num = Lists.all_cols_list.size();
	//for (int col = 0; col < node_final_cols_num; col++)
	//{
	//	printf("\n	Column %d\n", col + 1);
	//	for (int row = 0; row < ITEM_TYPES_NUM; row++)
	//	{
	//		printf("	Column %d Row %d coeff = %f \n", col + 1, col + 1, Lists.all_cols_list[col][row]);
	//	}
	//}

	IloNumArray  con_min(Env_MP_1); // cons LB
	IloNumArray  con_max(Env_MP_1); // cons UB

	// set cons bound
	for (int i = 0; i < ITEM_TYPES_NUM; i++)
	{
		con_min.add(IloNum(all_item_types_list[i].demand)); // cons > demand
		con_max.add(IloNum(IloInfinity)); // 
	}

	// set cons
	Cons_MP_1 = IloRangeArray(Env_MP_1, con_min, con_max); 
	Model_MP_1.add(Cons_MP_1); // add cons to the model

	// num of cols
	int all_cols_num = current_node.all_cols_list.size();
	printf("\n	Current number of columns is %d \n", all_cols_num);

	int int_solns_num = current_node.int_cols_list.size();
	for (int k =0; k < int_solns_num; k++)
	{
		// set matrix of master problem
		for (int col = 0; col < all_cols_num; col++)
		{
			int obj_coeff_1 = 1;
			IloNumColumn CplexCol = Obj_MP_1(obj_coeff_1); // Init a col

			for (int row = 0; row < ITEM_TYPES_NUM; row++) // set rows in this col
			{
				float row_coeff = current_node.all_cols_list[col][row];
				CplexCol += Cons_MP_1[row](row_coeff); // set coeff
			}

			float var_min; // var LB
			float var_max; // var UB
			string X_name = "X_" + to_string(col + 1); // var name

			// Case 1: The var of this col has already been solved as an int-soln in PN solns
			// Then set the var to be the val of the corresponding PN int-soln.
			if (col == current_node.int_cols_list[k])
			{
				float int_var_val = current_node.int_solns_list[k];
				 
				//  var = int_var_val
				var_min = int_var_val;
				var_max = int_var_val;

				IloNumVar var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
				Vars_MP_1.add(var);
			}

			// Case 2: 
			// The var of this col has not been solved as an int-soln in PN solns
			if (col != current_node.int_cols_list[k])
			{
				// Case 2.1 : 
				// The var of this col is the non-int branch var of PN
				// Then branch the var to be an integer
				if (col == Values.branch_var_index)
				{
					float branch_var_val = current_node.all_solns_list[col];
					float final_val = 0;

					// Case 2.1.1
					// This node is on the left branch of PN
					// Then set the var to be the floor-val of the corresponding PN soln.
					if (branch_flag == 0)
					{
						final_val = floor(branch_var_val);
						printf("\n	The FLOOR value of var %f  =  %f\n", branch_var_val, final_val);
					}

					// Case 2.1.2
					// This node is on the right branch of PN
					// Them set the var to be the ceil-val of the corresponding PN soln.
					if (branch_flag == 1)
					{
						final_val = ceil(branch_var_val);
						printf("\n	The CEIL value of var  %f = %f\n", branch_var_val, final_val);
					}

					//  var = floor-val or ceil-val
					var_min = final_val;
					var_max = final_val;

					// Init and set the branching var
					// ATTENTION: thought the var is an integer, 
					// it must be set as a FLOAT in code here
					IloNumVar var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
					Vars_MP_1.add(var);
				}

				// Case 2.2:
				// The var of this col is not the branch var of PN
				if (col != Values.branch_var_index)
				{
					// var >= 0
					var_min = 0;
					var_max = IloInfinity;

					// Init and set var
					IloNumVar var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
					Vars_MP_1.add(var);
				}
			}

			CplexCol.end(); // end this col
		}

	}

	printf("\n\n##################CPLEX SOLVING START##################\n");
	IloCplex MP_cplex(Env_MP_1);
	MP_cplex.extract(Model_MP_1);
	MP_cplex.exportModel("NewNodeProblem.lp"); 
	MP_cplex.solve(); // 求解主问题
	printf("##################CPLEX SOLVING END####################\n\n");

	int feasible_flag = MP_cplex.solve();
	if (feasible_flag == 0)
	{
		printf("\n	The MP-1 is NOT FEASIBLE\n");
		return feasible_flag;
	}
	else
	{
		printf("\n	The MP-1 is FEASIBLE\n");
		printf("\n	The OBJ of MP-1 is %f\n\n", MP_cplex.getValue(Obj_MP_1));

		for (int col = 0; col < all_cols_num; col++)
		{
			float soln_value = MP_cplex.getValue(Vars_MP_1[col]); // 主问题各个决策变量的值
			printf("	var_x_%d = %f\n", col + 1, soln_value);
		}

		printf("\n	DUAL PRICES: \n\n");
		current_node.dual_prices_list.clear();

		for (int row = 0; row < ITEM_TYPES_NUM; row++)
		{
			float dual_price = MP_cplex.getDual(Cons_MP_1[row]); // 主问题各个约束的对偶值
			printf("	dual_r_%d = %f\n", row + 1, dual_price);
			current_node.dual_prices_list.push_back(dual_price);
		}
		return feasible_flag;
	}
	return feasible_flag;
}