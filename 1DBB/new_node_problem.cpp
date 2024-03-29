// 2023-03-17

#include "CSBB.h"
using namespace std;

bool SolveNewNodeProblem(All_Values& Values, All_Lists& Lists, Node& this_node, Node& parent_node) {
	IloEnv Env_MP; // int environment
	IloModel Model_MP(Env_MP); // int model 
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init and set obj as Min
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons

	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	int item_types_num = Values.item_types_num;
	int all_cols_num = this_node.model_matrix.size();
	int all_rows_num = item_types_num;

	for (int k = 0; k < item_types_num; k++) {
		int demand = Lists.all_item_types_list[k].demand; // cons > demand	
		con_min.add(IloNum(demand)); // con LB
		con_max.add(IloNum(IloInfinity)); // con UB
	}
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP); // add cons to the model


	for (int col = 0; col < all_cols_num; col++) {
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para); // Init a col
		for (int row = 0; row < all_rows_num; row++) {
			IloNum row_para = this_node.model_matrix[col][row];
			CplexCol += Cons_MP[row](row_para); // set para
		}



		// Case 1 :  var of this col is the to be branched-var of Parent Node
		if (col == parent_node.var_to_branch_idx) {
			string X_name = "X_" + to_string(col + 1); // var name
			IloNum to_branch_val = this_node.var_to_branch_final;
			IloNumVar Var(CplexCol, to_branch_val, to_branch_val, ILOFLOAT, X_name.c_str()); // Init and set var
			Vars_MP.add(Var);
			printf("\n\t x_var_%d is set as %f, to be branched", col + 1, to_branch_val);
		}

		// Case 2:	var of this col is not the to be branched-var of Parent Node
		else {
			// Case 2.1: var of this col is NOT a branched - var in previous Nodes	
			int branched_num = parent_node.branched_vars_int_list.size();
			bool find_flag = 0;
			for (int index = 0; index < branched_num; index++) {   // loop of all branched-vars in previous Nodes
				int branched_col = parent_node.branched_vars_idx_list[index];
				if (col == branched_col) {   // var of this col is a branched-var in Parent Node
					find_flag = 1;
					string X_name = "X_" + to_string(col + 1); // var name
					IloNum branched_val = parent_node.branched_vars_int_list[index];
					IloNumVar Var(CplexCol, branched_val, branched_val, ILOFLOAT, X_name.c_str()); // Init and set var
					Vars_MP.add(Var);
					printf("\n\t x_var_%d is set as %f, branched", col + 1, branched_val);

					break;
				}
			}

			// Case 2.2: var of this col is NOT a branched-var in previous Nodes
			if (find_flag == 0) {
				string X_name = "X_" + to_string(col + 1); // var name
				IloNum var_min = 0;
				IloNum var_max = IloInfinity;
				IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // Init and set var
				Vars_MP.add(Var);
			}
		}

		CplexCol.end(); // must end this IloNumColumn object
	}

	printf("\n\n################## Node_%d MP CPLEX SOLVING START ##################\n\n", this_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("NewNodeProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("\n################## Node_%d MP CPLEX SOLVING END #################\n\n", this_node.index);

	if (MP_flag == 0) {
		this_node.node_pruned_flag = 1;
		printf("\n\t Node_%d MP IS NOT FEASIBLE!\n", this_node.index);
		printf("\n\t Node_%d has to be pruned\n", this_node.index);
	}
	else {
		this_node.LB = MP_cplex.getValue(Obj_MP); // set Node LB in the last MP
		printf("\n\t Obj of Node_%d MP-final is %f \n\n", this_node.index, MP_cplex.getValue(Obj_MP));
		printf("\n\t MP VARS: \n\n");
		for (int col = 0; col < all_cols_num; col++) {
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			this_node.all_solns_list.push_back(soln_val); // Node all solns (including zero-solns)
			printf("\t var_x_%d = %f\n", col + 1, soln_val);
		}

		printf("\n\t BRANCHED VARS: \n\n");
		int branched_num = this_node.branched_vars_int_list.size();
		int var_idx = -1;
		double var_int_val = -1;
		for (int k = 0; k < branched_num; k++) {
			var_idx = this_node.branched_vars_idx_list[k] + 1;
			var_int_val = this_node.branched_vars_int_list[k];
			printf("\t var_x_%d = %f branched \n", var_idx, var_int_val);
		}

		printf("\n\t Node_%d MP-final:\n", this_node.index);
		printf("\n\t Lower Bound = %f", this_node.LB);
		printf("\n\t NUM of all solns = %d", all_cols_num);
		printf("\n\t NUM of branched-vars = %d\n", branched_num);
	}

	MP_cplex.removeAllProperties();
	MP_cplex.end();
	Vars_MP.clear();
	Vars_MP.end();
	Cons_MP.clear();
	Cons_MP.end();
	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();

	return MP_flag;
}