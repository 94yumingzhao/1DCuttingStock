// 2023-03-06 
// 
// CG -- col generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound
//
// var -- variable
// con -- constraint
// para -- parameter
// col -- col
//
// fsb -- feasible
// int -- int-
// soln -- solution
// val -- val

#include<vector>
#include<queue>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <array>
#include <algorithm>
#include <stdio.h>
#include <ilcplex/ilocplex.h>

using namespace std;

#define RC_EPS 1.0e-6 // a num that is very close to 0

// item_type
struct Item_Type_Stc {
	int item_type = -1;
	int item_type_length = -1;
	int item_type_demand = -1;
};

// stock type
struct Stock_Type_Stc {
	int item_type = -1;
	int count = -1;
};

// item
struct Item_Stc {
	int item_type = -1;
	int item_type_demand = -1;
	int length = -1;
	int index = -1;
	int stock_index = -1;
	int occupied = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;
};

// stock
struct Stock_Stc {
	int item_type = -1;
	int pattern = -1;
	int item_type_length = -1;
	int index = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;

	int wasted_area = -1;
	int material_area_loss = -1;
};

// Node
struct Node {

	int index = -1;
	int parent_index = -1;
	int parent_branching_flag = -1;
	int node_branched_flag = -1; // flag: 判断当前节点在上节点的左支还是右支, 1 -- left, 2 -- right
	int node_pruned_flag = -1; // flag: 判断当前节点是否已被剪掉. 1 -- pruned, 0 -- not pruned

	double node_lower_bound = -1; // 节点最终下界
	double parent_var_to_branch_val = -1; // 当前节点的上节点的分支变量值
	int var_to_branch_idx = -1; // 当前节点的待分支变量，对应的列序号
	double var_to_branch_soln_val = -1; // 当前节点的待分支变量，对应解的值
	double var_to_branch_int_val_floor = -1; // 当前节点的待分支变量，对应解的四舍分支后的值
	double var_to_branch_int_val_ceil = -1; // 当前节点的待分支变量，对应解的五入分支后的值
	double var_to_branch_int_val_final = -1; // var-to-branch's final int-val (floored or ceiled)

	// Lists of final branching of one Node
	vector<double> all_solns_val_list; //  当前节点所有解的表
	vector<int> branched_vars_idx_list; // 当前节点所有已分支变量，对应列序号的表
	vector<double> branched_vars_soln_val_list; // 当前节点所有已分支变量，对应解的值的表
	vector<double> branched_vars_int_val_list; // 当前节点所有已分支变量，对应解分支后的整数值的表


	// Lists of one Column Generation iter of one Node
	int iter = -1;
	vector<vector<double>> model_matrix; // 当前节点每一次列生成循环，系数矩阵
	vector<double> dual_prices_list; // 当前节点每一次列生成循环，约束的对偶变量的表
	vector<double> new_col; //  当前节点每一次列生成循环，生成的新列

};

struct All_Values {

	int stocks_num = -1; // 所有可用 stock 的数量
	int item_types_num = -1; // 所有 item type 的数量
	int stock_length = -1; // stock 的长度
	//int level_num; // number of all node-branch structurew
	int node_num; // number of all Nodes

	double tree_optimal_lower_bound = -1; // 当前整个分支定价树上的最优下界

	/*操作决定
	0 -- 分支：从当前节点继续分支
	1 --  搜索：剪掉当前节点，并从之前生成过但未分支也未剪掉的某个节点，重新继续分支定价*/
	int tree_search_flag = -1;

	/*求解决定：
	1 -- 左支：从当前节点生成并求解左支子节点
	2 -- 右支：从当前节点生成并求解右支子节点
	3 -- 放弃：剪掉当前节点，并从之前生成过但未分支也未剪掉的某个节点，重新继续分支定价*/
	int tree_branch_status = -1;

	/*选支决定：
	1 -- 左支：将左支节点继续分支
	2 -- 右支：将右支节点继续分支*/
	int node_fathom_flag = -1;

	// int integerity_nodes_num = -1;
	int root_flag = -1;
};

struct All_Lists {
	vector<Item_Stc> all_items_list; // list of all items 
	vector<Item_Type_Stc> all_item_types_list; // list of all item_types
	vector<Node> all_nodes_list; // list of all Nodes generated
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

void PrimalHeuristic(All_Values& Values, All_Lists& Lists, Node& root_node);

void RootNodeColumnGeneration(All_Values& Values, All_Lists& Lists, Node& root_node);

bool SolveRootNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& root_node);

bool SolveSubProblem(All_Values& Values, All_Lists& Lists, Node& this_node);

bool SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node);

bool SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node);

int FinishNode(All_Values& Values, All_Lists& Lists, Node& this_node);

int ChooseVarToBranch(All_Values& Values, All_Lists& Lists, Node& this_node);

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists);

int ChooseNodeToBranch(All_Values& Values, All_Lists& Lists, Node& parent_node);

void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node& new_node, Node& parent_node);

void NewNodeColumnGeneration(All_Values& Values, All_Lists& Lists, Node& this_node, Node& parent_node);

bool SolveNewNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node,
	Node& parent_node);

















