// 2023-03-01

#include "CSBP.h"
using namespace std;

int SolveRootNodeFirstMasterProblem(
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
		con_min.add(IloNum(Lists.all_item_types_list[i].demand)); // cons > demand
		con_max.add(IloNum(IloInfinity)); // 
	}

	Cons_MP = IloRangeArray(Env_MP, con_min, con_max); 
	Model_MP.add(Cons_MP); 


	for (int col = 0; col < item_types_num; col++) 
	{
		int obj_coeff_1 = 1;
		IloNumColumn column1 = Obj_MP(obj_coeff_1); 

		for (int row = 0; row < item_types_num; row++) 
		{
			float row_coeff = root_node.model_matrix[row][col];
			column1 += Cons_MP[row](row_coeff); 
		}

		float var_min = 0; 
		float var_max = IloInfinity; 

		string X_name = "X_" + to_string(col + 1);
		IloNumVar var(column1, var_min, var_max, ILOFLOAT, X_name.c_str()); 
		Vars_MP.add(var); 

		column1.end();
	}

	printf("\n\n####################### MP-1 CPLEX SOLVING START #######################\n");
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp"); 
	IloBool MP_flag = MP_cplex.solve(); 
	printf("####################### MP-1 CPLEX SOLVING END #########################\n");

	if (MP_flag == 0)
	{
		printf("\n	The MP-1 is NOT FEASIBLE\n");
	}
	else
	{
		MP_flag = 1;

		printf("\n	The MP-1 is FEASIBLE\n");
		printf("\n	The OBJ of MP-1 is %f\n\n", MP_cplex.getValue(Obj_MP));

		for (int i = 0; i < item_types_num; i++)
		{
			float soln_val = MP_cplex.getValue(Vars_MP[i]); 
			printf("	var_x_%d = %f\n", i + 1, soln_val);
		}

		printf("\n	DUAL PRICES: \n\n");

		for (int k = 0; k < item_types_num; k++)
		{
			float dual_val = MP_cplex.getDual(Cons_MP[k]); 
			printf("	dual_r_%d = %f\n", k + 1, dual_val);
			root_node.dual_prices_list.push_back(dual_val);
		}
	}

	return MP_flag;
}

