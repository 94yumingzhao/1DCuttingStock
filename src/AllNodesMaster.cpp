// 2022-11-17
#include "CSBP.h"
using namespace std;

int SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP)
{
	int item_types_num = Values.item_types_num;

	// set the obj coeff of the new col
	int obj_coeff = 1;
	IloNumColumn CplexCol = Obj_MP(obj_coeff);

	// add the new col from SP to the MP of it to create a new updated MP
	for (int row = 0; row < item_types_num; row++)
	{
		CplexCol += Cons_MP[row](Lists.new_col[row]);
	}

	// var >= 0
	float var_min = 0; 
	float var_max = IloInfinity; 

	IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT); // float, not int
	Vars_MP.add(Var);
	CplexCol.end();
	
	// solve the new updated MP
	printf("\n	Continue to solve the new MP-%d", Values.iter + 1);
	printf("\n\n####################### MP-%d CPLEX SOLVING START #######################\n", Values.iter+1);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("updateMasterProblem.lp");
	MP_cplex.solve(); // 求解当前主问题
	printf("####################### MP-%d CPLEX SOLVING END #########################\n", Values.iter+1);
	printf("\n	The OBJ of update MP-%d is %f\n\n", Values.iter+1,MP_cplex.getValue(Obj_MP));

	// print solns of the updated MP
	int cols_num = Lists.all_cols_list.size();
	for (int k = 0; k < cols_num; k++)
	{
		float soln_val = MP_cplex.getValue(Vars_MP[k]);
		printf("	var_x_%d = %f\n", k + 1, soln_val);
	}

	// print and store dual price vals of the updated MP
	Lists.dual_prices_list.clear(); // 清空约束对偶值list
	printf("\n	DUAL PRICES: \n\n");
	for (int k = 0; k < item_types_num; k++)
	{
		float dual_val = MP_cplex.getDual(Cons_MP[k]); // 对一行约束getDual()求对偶解
		printf("	dual_r_%d = %f\n", k + 1, dual_val);
		Lists.dual_prices_list.push_back(dual_val);
	}

	return 0;
}

float SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP)
{
	int item_types_num = Values.item_types_num;

	// solve th final MP of the CG loop of this Node
	printf("\n	Continue to solve Final MP");
	printf("\n\n####################### MP-%d CPLEX SOLVING START #######################\n", Values.iter);
	IloCplex MP_cplex(Model_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("FinalMasterProblem.lp");
	MP_cplex.solve(); // 求解当前主问题
	printf("####################### MP-%d CPLEX SOLVING END #########################\n", Values.iter);

	printf("\n	The OBJ of Final-MP is %f\n\n" ,  MP_cplex.getValue(Obj_MP));

	// print and store all solns of this Node, including the 0-solns
	int cols_num = Lists.all_cols_list.size();
	for (int col = 0; col < cols_num; col++)
	{
		float soln_val = MP_cplex.getValue(Vars_MP[col]);
		printf("	var_x_%d = %f\n", col + 1, soln_val);
		Lists.all_solns_list.push_back(soln_val);
	}

	float node_bound = MP_cplex.getValue(Obj_MP);

	/*for (int col = 0; col < node_final_cols_num; col++)
	{
		printf("\n	Column %d\n",col+1);
		for (int row = 0; row < ITEM_TYPES_NUM; row++)
		{
			printf("	Column %d Row %d coeff = %f \n",col+1,col+1, Lists.all_cols_list[col][row]);
		}
	}*/

	return node_bound;
}






