// 2023-03-01

#include "CSBP.h"
using namespace std;

float ColumnGenerationNewNode(int branch_flag, All_Values& Values, All_Lists& Lists,Node& this_node)
{
	IloEnv Env_MP; // int environment
	IloModel Model_MP(Env_MP); // int model 
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init and set obj

	Node parent_node = Lists.all_nodes_list.back(); // need branch var -val from parent node
	float node_lower_bound = Values.current_optimal_bound;
	this_node.iter = 0; // The firsth MP index ==0

	// 生成并求解初始主问题
	int feasible_flag = SolveNewNodeFirstMasterProblem(
		branch_flag,
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP,
		this_node,
		parent_node);

	cout << endl;

	// 初始主问题不可行，则返回负数的下界，在主流程中进行判断，
	// 如果得到的下界<0,则该问题没有可行解
	if (feasible_flag == 1)
	{
		while (1) // 列生成循环求解
		{
			this_node.iter++;

			int solve_flag = SolveSubProblem(Values, Lists, this_node); // 求解当前主问题对应的子问题

			if (solve_flag == 1) // 求解子问题未得到非负削减费用，子问题所属主问题是最优
			{
				break; // 跳出循环
			}
			if (solve_flag == 0) // 求解子问题得到非负削减费用，子问题所属主问题可以继续加列
			{
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP,
					this_node); // 继续求解加入新列的更新主问题
			}
		}

		// 最后确定当前节点的解，结束节点求解
		node_lower_bound = SolveFinalMasterProblem(
			Values,
			Lists,
			Env_MP,
			Model_MP,
			Obj_MP,
			Cons_MP,
			Vars_MP,
			this_node); // 确定当前下界

		Vars_MP.clear();
		Vars_MP.end();
		Cons_MP.clear();
		Cons_MP.end();
		Model_MP.removeAllProperties();
		Model_MP.end();
		Env_MP.removeAllProperties();
		Env_MP.end();

		this_node.dual_prices_list.clear();
		this_node.new_col.clear();

		//Lists.model_matrix.clear();

		return node_lower_bound;
	}

	if (feasible_flag == 0)
	{
		Vars_MP.clear();
		Vars_MP.end();
		Cons_MP.clear();
		Cons_MP.end();
		Model_MP.removeAllProperties();
		Model_MP.end();
		Env_MP.removeAllProperties();
		Env_MP.end();

		this_node.dual_prices_list.clear();
		this_node.new_col.clear();

		//Lists.model_matrix.clear();

		return node_lower_bound;
	}
	
	

	//记录本次列生成的结果和目标函数值
	return node_lower_bound;

}
