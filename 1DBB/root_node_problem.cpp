// 2023-03-17

#include "CSBB.h"
using namespace std;

bool SolveRootNodeProblem(All_Values& Values, All_Lists& Lists, Node& root_node) {

	IloEnv Env_MP; // Init environment
	IloModel Model_MP(Env_MP); // Init model 
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init and set obj
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons

	int item_types_num = Values.item_types_num;
	int all_rows_num = item_types_num;
	int all_cols_num = item_types_num;

	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB
	for (int row = 0; row < all_rows_num; row++) {
		int item_type_demand = Lists.all_item_types_list[row].item_type_demand; // con >= item_type_demand
		con_min.add(IloNum(item_type_demand)); // con LB
		con_max.add(IloNum(IloInfinity));  // con UB
	}
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);
	con_min.end();
	con_max.end();

	// Cplex Modeling
	for (int col = 0; col < all_cols_num; col++) {
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para);
		for (int row = 0; row < all_rows_num; row++) {
			IloNum row_para = root_node.model_matrix[row][col];
			CplexCol += Cons_MP[row](row_para);
		}

		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		string X_name = "X_" + to_string(col + 1); // var name
		IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str()); // var >= 0
		Vars_MP.add(Var);

		CplexCol.end(); // end this IloNumColumn object
	}

	printf("\n\n################ Node_%d MP-1 CPLEX SOLVING START ################\n", root_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("################ Node_%d MP-1 CPLEX SOLVING END ##################\n", root_node.index);

	if (MP_flag == 0) {
		root_node.node_pruned_flag = 1;
		printf("\n\t MP-1 NOT FEASIBLE\n");
	}
	else {
		root_node.node_lower_bound = MP_cplex.getValue(Obj_MP); // set Node LB in the last MP
		printf("\n\t Obj of Node_%d MP-final is %f \n\n", root_node.index, MP_cplex.getValue(Obj_MP));
		for (int col = 0; col < all_cols_num; col++) {
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			root_node.all_solns_val_list.push_back(soln_val); // Node all solns (including zero-solns)
			printf("\t var_x_%d = %f\n", col + 1, soln_val);
		}

		printf("\n\t BRANCHED VARS: \n\n	None\n");
		printf("\n\t Node_%d MP-final:\n", root_node.index);
		printf("\n\t Lower Bound = %f", root_node.node_lower_bound);
		printf("\n\t NUM of all solns = %d", all_cols_num);
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

