// 2023-03-01

#include "CSBP.h"
using namespace std;

// function to init model matrix of root Node
void InitRootNodeMatrix(All_Values& Values, All_Lists& Lists, Node& root_node)
{
	int item_types_num = Values.item_types_num;
	vector<vector<float>> primal_matrix;

	for (int col = 0; col < item_types_num; col++) // cols num == item types num
	{
		vector<float> primal_col;
		for (int row = 0; row < item_types_num; row++) // rows num == item types num
		{
			if (row == col)
			{
				float primal_val = 0;
				primal_val = Values.stock_length / Lists.all_item_types_list[row].length;
				primal_col.push_back(primal_val);
			}
			else
			{
				primal_col.push_back(0);
			}
		}
		primal_matrix.push_back(primal_col);
	}
	root_node.model_matrix = primal_matrix;
}


bool SolveRootNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	Node& root_node)
{
	int item_types_num = Values.item_types_num;

	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	for (int i = 0; i < item_types_num; i++)
	{
		// cons > demand
		con_min.add(IloNum(Lists.all_item_types_list[i].demand)); 
		con_max.add(IloNum(IloInfinity)); 
	}

	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);

	con_min.end();
	con_max.end();

	for (int col = 0; col < item_types_num; col++)
	{
		int obj_coeff_1 = 1;
		IloNumColumn CplexCol = Obj_MP(obj_coeff_1);

		for (int row = 0; row < item_types_num; row++)
		{
			float row_coeff = root_node.model_matrix[row][col];
			CplexCol += Cons_MP[row](row_coeff);
		}

		IloNum var_min = 0;
		IloNum var_max = IloInfinity;

		string X_name = "X_" + to_string(col + 1);
		IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, X_name.c_str());
		Vars_MP.add(Var);

		CplexCol.end();
	}

	printf("\n\n####################### Node_%d MP-1 CPLEX SOLVING START #######################\n",root_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("####################### Node_%d MP-1 CPLEX SOLVING END #########################\n",root_node.index);

	int fsb_num = 0;
	int int_num = 0;
	size_t solns_num = root_node.model_matrix.size();

	if (MP_flag == 0)
	{
		printf("\n	Node_%d MP-1 is NOT FEASIBLE\n", root_node.index);
	}
	else
	{
		printf("\n	Node_%d MP-1 is FEASIBLE\n",  root_node.index);
		printf("\n	OBJ of Node_%d MP-1 is %f\n\n", root_node.index, MP_cplex.getValue(Obj_MP));

		int now_solns_num = item_types_num;
		for (int col = 0; col < now_solns_num; col++)
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
						printf("	var_x_%d = %f int\n", col + 1, soln_val);
					}
				}
				else
				{
					printf("	var_x_%d = %f\n", col + 1, soln_val);
				}
			}
		}

		printf("\n	BRANCHED VARS: \n\n");
		size_t branched_num = root_node.branched_vars_list.size();
		for (int k = 0; k < branched_num; k++)
		{
			printf("	var_x_%d = %d branched \n",
				root_node.branched_idx_list[k]+1, root_node.branched_vars_list[k]);
		}


		printf("\n	DUAL PRICES: \n\n");
		for (int k = 0; k < item_types_num; k++)
		{
			float dual_val = MP_cplex.getDual(Cons_MP[k]);
			printf("	dual_r_%d = %f\n", k + 1, dual_val);
			root_node.dual_prices_list.push_back(dual_val);
		}

		root_node.lower_bound = MP_cplex.getValue(Obj_MP);
		printf("\n	Node_%d MP-%d:\n", root_node.index, root_node.iter);
		printf("\n	Lower Bound:   %f\n", root_node.lower_bound);
		printf("\n	NUM of all solns: %zd\n", solns_num);
		printf("\n	NUM of fsb solns: %d\n", fsb_num);
		printf("\n	NUM of int solns: %d\n", int_num);
		printf("\n	NUM of branched-vars: %zd\n", branched_num);
	}

	MP_cplex.end();
	return MP_flag;
}

