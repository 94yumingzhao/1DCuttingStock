// 2023-03-16

#include "CSCG.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists) {
	// cplex 环境初始化
	IloEnv Env_MP; // 环境
	IloModel Model_MP(Env_MP); // 模型
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // 目标函数
	IloNumVarArray Vars_MP(Env_MP); // 决策变量表
	IloRangeArray Cons_MP(Env_MP); // 约束表

	Values.iter = 1;

	// 求解初始主问题
	int MP_flag = SolveFirstMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP);

	if (MP_flag == 1) { // 如果初始主问题有可行解

		while (1) { // 列生成循环
			Values.iter++; // 循环次数

			int SP_flag = -1;
			SP_flag = SolveSubProblem(Values, Lists); // 求解主问题的子问题

			// Case 1:
			if (SP_flag == 0) { //  子问题尚未求得最优削减费用
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP);  // 子问题生成的新列加入主问题，求解更新主问题
			}
			// Case 2:
			if (SP_flag == 1) { //  子问题求得最优削减费用
				break;  // 列生成循环结束
			}
		}
	}
	Model_MP.removeAllProperties();
	Vars_MP.clear();
	Cons_MP.clear();

	SolveFinalMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP);

	// clear all CPLEX objects to release memory. 
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
}
