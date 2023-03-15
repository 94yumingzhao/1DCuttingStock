// 2023-03-01

#include "CSBP.h"
using namespace std;

// function to init model matrix of Root Node
void InitRootNodeMatrix(All_Values& Values, All_Lists& Lists, Node& root_node)
{
	int item_types_num = Values.item_types_num;
	for (size_t col = 0; col < item_types_num; col++) // cols num == item types num
	{
		vector<int> temp_col;
		for (size_t row = 0; row < item_types_num; row++) // rows num == item types num
		{
			if (row == col)
			{
				int temp_val = 0;
				temp_val = Values.stock_length / Lists.all_item_types_list[row].length;
				temp_col.push_back(temp_val);
			}
			else
			{
				temp_col.push_back(0);
			}
		}
		root_node.model_matrix.push_back(temp_col);
	}
	cout << endl;
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
	// set model cons
	IloNumArray  con_min(Env_MP); // cons LB
	IloNumArray  con_max(Env_MP); // cons UB

	int item_types_num = Values.item_types_num;
	for (size_t i = 0; i < item_types_num; i++)
	{
		// con >= demand
		int item_type_demand = Lists.all_item_types_list[i].demand;
		con_min.add(IloNum(item_type_demand));
		con_max.add(IloNum(IloInfinity)); 
	}

	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);

	con_min.end();
	con_max.end();

	// set model matrix
	int cols_num = item_types_num;
	int rows_num = item_types_num;

	for (size_t col = 0; col < cols_num; col++)
	{
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para);

		for (size_t row = 0; row < rows_num; row++)
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
	printf("\n\n####################### Node_%d MP-1 CPLEX SOLVING START #######################\n",root_node.index);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("####################### Node_%d MP-1 CPLEX SOLVING END #########################\n",root_node.index);

	int fsb_num = 0;
	int int_num = 0;

	// judge model feasibility
	if (MP_flag == 0)
	{
		printf("\n	Node_%d MP-1 is NOT FEASIBLE\n", root_node.index);
	}
	else
	{
		printf("\n	Node_%d MP-1 is FEASIBLE\n",  root_node.index);
		printf("\n	OBJ of Node_%d MP-1 is %f\n\n", root_node.index, MP_cplex.getValue(Obj_MP));

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

		printf("\n	DUAL PRICES: \n\n");
		for (size_t k = 0; k < rows_num; k++)
		{
			double dual_val = MP_cplex.getDual(Cons_MP[k]);
			printf("	dual_r_%zd = %f\n", k + 1, dual_val);
			root_node.dual_prices_list.push_back(dual_val);
		}

		root_node.lower_bound = MP_cplex.getValue(Obj_MP);
		printf("\n	Node_%d MP-%d:\n", root_node.index, root_node.iter);
		printf("\n	Lower Bound = %f", root_node.lower_bound);
		printf("\n	NUM of all solns = %d",  cols_num);
		printf("\n	NUM of fsb solns = %d", fsb_num);
		printf("\n	NUM of int solns = %d", int_num);
	}

	MP_cplex.end();
	return MP_flag;
}

