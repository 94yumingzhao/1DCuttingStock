// 2023-03-01

#include "CSBP.h"
using namespace std;

// CG to solve the root node
float ColumnGenerationRootNode(All_Values& Values, All_Lists& Lists,Node root_node)
{
	IloEnv Env_MP; // Init environment
	IloModel Model_MP(Env_MP); // Init model
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init obj

	root_node.iter = 0; // root node index == 0
	root_node.lower_bound = Values.current_optimal_bound; // Init root node bound 

	int feasible_flag;

	// solve the first MP of the root node 
	feasible_flag = SolveRootNodeFirstMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP,
		root_node);

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

		root_node.dual_prices_list.clear();
		root_node.new_col.clear();

		//Lists.all_cols_list.clear();

		return node_bound;
	}
	else
	{
		// Column Generation loop
		while (1)
		{
			root_node.iter++; // update CG loop iteration index

			int SP_solve_flag = SolveSubProblem(Values, Lists); // solve the SP of MP

			// Case 1:
			// No better reduced cost is get from SP anymore
			if (SP_solve_flag == 1) 
			{
				break; // break CG loop and here the Node get final solns
			}
			// Case 2:
			// Better reduced cost is get from SP
			if (SP_solve_flag == 0)
			{
				// continue CG loop and update MP with the new col from SP
				// solve the new updated MP
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP,
					root_node);
			}
		}

		// solve the last MP to get optimal int-solns and optimal lower bound of the Node
		root_node.lower_bound = SolveFinalMasterProblem(
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

		root_node.dual_prices_list.clear();
		root_node.new_col.clear();

		//Lists.all_cols_list.clear();
		return node_bound;
	}
}
