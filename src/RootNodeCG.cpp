// 2023-03-01


#include "CSBP.h"
using namespace std;

// CG to solve the root node
float ColumnGenerationRootNode(All_Values& Values, All_Lists& Lists)
{
	IloEnv Env_MP; // Init environment
	IloModel Model_MP(Env_MP); // Init model
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init obj
	
	Values.current_iter = 0; // root node index == 0

	float node_bound = Values.current_optimal_bound; // Init root node bound 

	int feasible_flag;

	// solve the first MP of the root node 
	feasible_flag = SolveRootNodeFirstMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP);

	// 初始主问题不可行，则返回负数的下界，在主流程中进行判断，
	// 如果得到的下界<0,则该问题没有可行解
	// if the lb of the first MP <0, then this MP has no feasible solns.
	if (feasible_flag == 0)
	{
		// 只能结束节点求解
		Vars_MP.clear();
		Vars_MP.end();
		Cons_MP.clear();
		Cons_MP.end();
		Model_MP.removeAllProperties();
		Model_MP.end();
		Env_MP.removeAllProperties();
		Env_MP.end();

		Lists.dual_prices_list.clear();
		Lists.new_col.clear();

		//Lists.node_all_cols_list.clear();

		return node_bound;
	}
	else
	{
		// column generation loop
		while (1)
		{
			Values.current_iter++; // update loop iteration index

			int SP_solve_flag = SolveSubProblem(Values, Lists); // solve the SP of MP

			if (SP_solve_flag == 1) // SP have no better reduced cost
			{
				break; // get optimal solns, break CG loop 
			}
			if (SP_solve_flag == 0) // SP have better reduced cost
			{
				// add new col to MP, and solve this new updated MP.
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP);
			}
		}

		// solve the last MP to get optimal integer solns and optimal lower bound
		node_bound = SolveFinalMasterProblem(
			Values,
			Lists,
			Env_MP,
			Model_MP,
			Obj_MP,
			Cons_MP,
			Vars_MP); 

		// clear all CPLEX objects to release memory. 
		Vars_MP.clear();
		Vars_MP.end();
		Cons_MP.clear();
		Cons_MP.end();
		Model_MP.removeAllProperties();
		Model_MP.end();
		Env_MP.removeAllProperties();
		Env_MP.end();

		Lists.dual_prices_list.clear();
		Lists.new_col.clear();

		//Lists.node_all_cols_list.clear();

		return node_bound;
	}
}
