// 2023-03-01
// 列生成求解新生成节点

#include "CSBP.h"
using namespace std;

// 生成并求解新节点的初始主问题
bool SolveNewNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	Node& this_node,
	Node& parent_node) {

	int all_cols_num = this_node.model_matrix.size();
	int all_rows_num = Values.item_types_num;

	// set model cons
	IloNumArray  con_min(Env_MP); 
	IloNumArray  con_max(Env_MP); 
	for (int k = 0; k < all_rows_num; k++) {
		int demand = Lists.all_item_types_list[k].demand; // cons > demand	
		con_min.add(IloNum(demand)); // con LB
		con_max.add(IloNum(IloInfinity)); // con UB
	}
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP); // add cons to the model

	// Cplex modeling
	for (int col = 0; col < all_cols_num; col++) {
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para); // Init a col
		for (int row = 0; row < all_rows_num; row++) {
			IloNum row_para = this_node.model_matrix[col][row];
			CplexCol += Cons_MP[row](row_para); // set para
		}

		// Case 1 :  
		if (col == parent_node.var_to_branch_idx) { // 当前列对应的决策变量，正是上节点的分支变量
			string X_name = "X_" + to_string(col + 1); // var name
			IloNum var_min = this_node.var_to_branch_final; // 根据当前节点左支或右支，取分支变量分支向上或向下取整后的值
			IloNum var_max = this_node.var_to_branch_final;
			IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // 
			Vars_MP.add(Var);
			printf("\n	x_var_%d is set as %f, to be branched", col + 1, var_min);
		}

		// Case 2
		else { // 当前列对应的决策变量，不是上节点的分支变量

			int branched_num = parent_node.branched_vars_int_list.size();
			bool find_flag = 0;
			for (int index = 0; index < branched_num; index++) // 遍历搜索从根节点到上节点的过程中产生的所有已分支变量
			{
				int branched_col = parent_node.branched_vars_idx_list[index];

				// Case 2.1
				if (col == branched_col) {    // 当前列对应的决策变量，是一个已分支变量
					string X_name = "X_" + to_string(col + 1); // var name
					IloNum var_min = parent_node.branched_vars_int_list[index]; // 继承已分支变量对应的最终已分支整数值
					IloNum var_max = parent_node.branched_vars_int_list[index];
					IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // Init and set var
					Vars_MP.add(Var);
					printf("\n	x_var_%d is set as %f, branched", col + 1, var_min);

					find_flag = 1; // 停止搜索
					break;
				}
			}

			// Case 2.2: 
			if (find_flag == 0) { // 当前列对应的决策变量，既不是上节点的分支变量，也不是已分支变量
				string X_name = "X_" + to_string(col + 1); // var name
				IloNum var_min = 0;
				IloNum var_max = IloInfinity;
				IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // Init and set var
				Vars_MP.add(Var);
			}
		}

		CplexCol.end(); // 
	}

	printf("\n\n################## Node_%d MP-1 CPLEX SOLVING START ##################\n\n", this_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	//MP_cplex.exportModel("NewNodeProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("\n################## Node_%d MP-1 CPLEX SOLVING OVER #################\n\n", this_node.index);

	int fsb_num = 0;
	int int_num = 0;
	if (MP_flag == 0) {
		this_node.node_pruned_flag = 1;
		printf("\n\t Node_%d MP-1 is NOT FEASIBLE\n", this_node.index);
		printf("\n\t Node_%d MP-1 has to be pruned\n", this_node.index);
	}
	else {
		printf("\n\t Node_%d MP-1 is FEASIBLE\n", this_node.index);
		printf("\n\t OBJ of Node_%d MP-1 is %f\n\n", this_node.index, MP_cplex.getValue(Obj_MP));

		for (int col = 0; col < all_cols_num; col++) {
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			if (soln_val > 0) {   // feasible soln > 0
				fsb_num++;
				int soln_int_val = int(soln_val);
				if (soln_int_val == soln_val) {  // ATTTENTION:  
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

		printf("\n\t BRANCHED VARS: \n\n");
		int branched_num = this_node.branched_vars_int_list.size();
		for (int k = 0; k < branched_num; k++) {
			printf("\t var_x_%d = %f branched \n", this_node.branched_vars_idx_list[k] + 1, this_node.branched_vars_int_list[k]);
		}

		printf("\n\t DUAL PRICES: \n\n");
		this_node.dual_prices_list.clear();
		for (int row = 0; row < all_rows_num; row++) {
			double dual_price = MP_cplex.getDual(Cons_MP[row]); // 从模型约束获得对偶值
			this_node.dual_prices_list.push_back(dual_price);
			printf("	dual_r_%d = %f\n", row + 1, dual_price);
		}

		printf("\n\t Node_%d MP-1:\n", this_node.index);
		printf("\n\t Lower Bound = %f", MP_cplex.getValue(Obj_MP));
		printf("\n\t NUM of all solns = %d", all_cols_num);
		printf("\n\t NUM of fsb solns = %d", fsb_num);
		printf("\n\t NUM of int solns = %d", int_num);
		printf("\n\t NUM of branched-vars = %d\n", branched_num);
	}

	MP_cplex.end();
	return MP_flag;
}