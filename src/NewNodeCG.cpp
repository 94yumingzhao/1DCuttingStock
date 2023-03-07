// 2023-03-01

#include "CSBP.h"
using namespace std;

float ColumnGenerationNewNode(int branch_flag, All_Values& Values, All_Lists& Lists,Node this_node)
{
	IloEnv Env_MP_1; // int environment
	IloModel Model_MP_1(Env_MP_1); // int model 
	IloNumVarArray Vars_MP_1(Env_MP_1); // Init vars
	IloRangeArray Cons_MP_1(Env_MP_1); // Init cons
	IloObjective Obj_MP_1 = IloAdd(Model_MP_1, IloMinimize(Env_MP_1)); // Init and set obj

	this_node.iter = 0; // The firsth MP index ==0

	float node_bound = Values.current_optimal_bound;
	int feasible_flag;

	// 生成并求解初始主问题
	feasible_flag = SolveNewNodeFirstMasterProblem(
		branch_flag,
		Values,
		Lists,
		Env_MP_1,
		Model_MP_1,
		Obj_MP_1,
		Cons_MP_1,
		Vars_MP_1,
		this_node);

	cout << endl;

	// 初始主问题不可行，则返回负数的下界，在主流程中进行判断，
	// 如果得到的下界<0,则该问题没有可行解

	if (feasible_flag == 0)
	{
		Vars_MP_1.clear();
		Vars_MP_1.end();
		Cons_MP_1.clear();
		Cons_MP_1.end();
		Model_MP_1.removeAllProperties();
		Model_MP_1.end();
		Env_MP_1.removeAllProperties();
		Env_MP_1.end();

		this_node.dual_prices_list.clear();
		this_node.new_col.clear();

		//Lists.all_cols_list.clear();

		return node_bound;
	}
	else
	{
		while (1) // 列生成循环求解
		{
			this_node.iter++;

			int SP_solve_flag = SolveSubProblem(Values, Lists,this_node); // 求解当前主问题对应的子问题

			if (SP_solve_flag == 1) // 求解子问题未得到非负削减费用，子问题所属主问题是最优
			{
				break; // 跳出循环
			}
			if (SP_solve_flag == 0) // 求解子问题得到非负削减费用，子问题所属主问题可以继续加列
			{
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP_1,
					Model_MP_1,
					Obj_MP_1,
					Cons_MP_1,
					Vars_MP_1,
					this_node); // 继续求解加入新列的更新主问题
			}
		}

		// 最后确定当前节点的解，结束节点求解
		node_bound = SolveFinalMasterProblem(
			Values,
			Lists,
			Env_MP_1,
			Model_MP_1,
			Obj_MP_1,
			Cons_MP_1,
			Vars_MP_1,
			this_node); // 确定当前下界

		Vars_MP_1.clear();
		Vars_MP_1.end();
		Cons_MP_1.clear();
		Cons_MP_1.end();
		Model_MP_1.removeAllProperties();
		Model_MP_1.end();
		Env_MP_1.removeAllProperties();
		Env_MP_1.end();

		this_node.dual_prices_list.clear();
		this_node.new_col.clear();

		//Lists.all_cols_list.clear();

		return node_bound;
	}
	

	//记录本次列生成的结果和目标函数值
	return node_bound;

}
