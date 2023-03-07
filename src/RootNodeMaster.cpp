// 2023-03-01
// 列生成求解根节点

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

	// cplex列建模生成初始主问题模型
	for (int col = 0; col < item_types_num; col++) 
	{
		int obj_coeff_1 = 1;
		IloNumColumn column1 = Obj_MP(obj_coeff_1); 

		for (int row = 0; row < item_types_num; row++) 
		{
			float row_coeff = root_node.model_matrix[row][col];
			column1 += Cons_MP[row](row_coeff); 
		}

		float var_min = 0; // 决策变量下界= 0
		float var_max = IloInfinity; // 决策变量上界 = 正无穷

		string X_name = "X_" + to_string(col + 1);
		IloNumVar var(column1, var_min, var_max, ILOFLOAT, X_name.c_str()); // 生成决策变量对象，var(列对象，决策变量下界，决策变量上界，决策变量数据类型)
		Vars_MP.add(var); // 每一列对应一个决策变量，加入vars_list

		column1.end();
	}

	printf("\n\n####################### MP-1 CPLEX SOLVING START #######################\n");
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("initialMasterProblem.lp"); // 输出主问题lp模型到文件夹
	int feasible_flag = MP_cplex.solve(); // 求解主问题
	printf("####################### MP-1 CPLEX SOLVING END #########################\n");

	if (feasible_flag == 0)
	{
		printf("\n	The MP-1 is NOT FEASIBLE\n");
	}
	else
	{
		feasible_flag = 1;

		printf("\n	The MP-1 is FEASIBLE\n");
		printf("\n	The OBJ of MP-1 is %f\n\n", MP_cplex.getValue(Obj_MP));

		for (int i = 0; i < item_types_num; i++)
		{
			float soln_val = MP_cplex.getValue(Vars_MP[i]); // 主问题各个决策变量的值
			printf("	var_x_%d = %f\n", i + 1, soln_val);
		}

		printf("\n	DUAL PRICES: \n\n");

		for (int k = 0; k < item_types_num; k++)
		{
			float dual_val = MP_cplex.getDual(Cons_MP[k]); // 主问题各个约束的对偶值
			printf("	dual_r_%d = %f\n", k + 1, dual_val);
			root_node.dual_prices_list.push_back(dual_val);
		}
	}

	return feasible_flag;
}

