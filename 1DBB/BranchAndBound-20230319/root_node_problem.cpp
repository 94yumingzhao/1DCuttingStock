// 2023-03-17

#include "CSBB.h"
using namespace std;

bool SolveRootNodeProblem(All_Values& Values,All_Lists& Lists, Node& root_node)
{
	IloEnv Env_MP; // int environment
	IloModel Model_MP(Env_MP); // int model 
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init and set obj
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons

	// set model cons
	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	int item_types_num = Values.item_types_num;
	int rows_num = item_types_num;
	int cols_num = item_types_num;

	for (int row = 0; row < rows_num; row++)
	{
		// con >= item_type_demand
		int item_type_demand = Lists.all_item_types_list[row].item_type_demand;

		con_min.add(IloNum(item_type_demand)); // con LB
		con_max.add(IloNum(IloInfinity));  // con UB
	}

	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);

	con_min.end();
	con_max.end();

	// Cplex Modeling
	for (int col = 0; col < cols_num; col++)
	{
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para);

		for (int row = 0; row < rows_num; row++)
		{
			IloNum row_para = root_node.model_matrix[row][col];
			CplexCol += Cons_MP[row](row_para);
		}

		// var >= 0
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		string X_name = "X_" + to_string(col + 1); // var name

		IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
		Vars_MP.add(Var);

		CplexCol.end(); // end this IloNumColumn object
	}

	// solve model
	printf("\n\n####################### Node_%d MP-1 CPLEX SOLVING START #######################\n", root_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("####################### Node_%d MP-1 CPLEX SOLVING END #########################\n", root_node.index);

	if (MP_flag == 0)
	{
		root_node.node_pruned_flag = 1;
		printf("\n MP-1 NOT FEASIBLE\n");
	}
	else
	{
		root_node.node_lower_bound = MP_cplex.getValue(Obj_MP); // set Node LB in the last MP
		printf("\n	OBJ of Node_%d MP-final is %f \n\n", root_node.index, MP_cplex.getValue(Obj_MP));

		for (int col = 0; col < cols_num; col++)
		{
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			root_node.all_solns_val_list.push_back(soln_val); // Node all solns (including zero-solns)

			if (soln_val > 0)
			{
				int soln_int_val = int(soln_val); // TODO
				if (soln_int_val == soln_val) // if this soln is not int
				{
					if (soln_int_val >= 1) // and it is an int-soln that larger than 1
					{
						root_node.int_solns_val_list.push_back(soln_val); //  Node int-solns
						root_node.int_solns_idx_list.push_back(col); // Node int-solns' index

						printf("	var_x_%d = %f int\n", col + 1, soln_val);
					}
				}
				else // if this soln is not int
				{
					printf("	var_x_%d = %f\n", col + 1, soln_val);
				}

				root_node.fsb_solns_val_list.push_back(soln_val); // Node feasible (i.e. non-zero-solns) solns 
				root_node.fsb_solns_idx_list.push_back(col); 	// Node fsb-solns' index
			}
		}

		printf("\n	BRANCHED VARS: \n\n	None\n");

		int fsb_num = root_node.fsb_solns_val_list.size();
		int int_num = root_node.int_solns_idx_list.size();
		printf("\n	Node_%d MP-final:\n", root_node.index);
		printf("\n	Lower Bound = %f", root_node.node_lower_bound);
		printf("\n	NUM of all solns = %d", cols_num);
		printf("\n	NUM of fsb-solns = %d", fsb_num);
		printf("\n	NUM of int-solns = %d", int_num);
	}
	
	MP_cplex.removeAllProperties();
	MP_cplex.end();
	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Vars_MP.clear();
	Vars_MP.end();
	Cons_MP.clear();
	Cons_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();

	return MP_flag;
}

