// 2022-11-17
// 2023-02-23 �ָ�

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

#define RC_EPS 1.0e-6 // һ���ӽ�0�ĺ�С���������ڸ�������Ŀ�꺯����ֵ�����������õ�ֵ���Ա�


// һ���ӹ�
struct ItemTypeProperties
{
	int type = -1;
	int length = -1;
	int demand = -1;
};

// һ��ĸ��
struct StockTypeProperties
{
	int type = -1;
	int count = 0;
};

// �����ӹ�
struct ItemProperties
{
	int type = -1;
	int demand = -1;
	int length = -1;
	int index = -1;
	int stock_index = -1;
	int occupied = 0;

	int cutting_distance = -1;
	int material_cutting_loss = -1;
};

// ����ĸ��
struct StockProperties
{
	int type = 0;
	int pattern = -1;
	int length = -1;
	int index = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;

	int wasted_area = -1;
	int material_area_loss = -1;
};

struct Node
{
	// parent node info
	int parent_index;
	int parent_branch_flag;
	int parent_branch_val;

	// this node info
	int index;
	float lower_bound;
	float branch_val;
	float branch_floor_val;
	float branch_value_ceil;

	vector<float> node_final_soln_values_list;
};

struct All_Values
{
	int Root = 0;
	int stocks_num; // �ܼƿ���ĸ������
	int item_types_num; // �ӹ���������
	int stock_length; // ĸ�ܳ���

	int node_index;
	float branch_var_val;
	int branch_var_index;
	int nodes_num = 1;
	int current_branch_flag;

	float current_optimal_bound;

	int int_var_index;

	int current_iter;
};

struct All_Lists
{
	vector<ItemProperties> all_items_list; // all items
	vector<ItemTypeProperties> all_item_types_list; // all item types
	vector<Node> all_nodes_list; // all Nodes

	// Lists used for one Node
	vector<float> node_all_solns_list; // all solns of this Node
	vector<float> node_int_solns_list; // all int-solns of this Node
	vector<float> node_int_cols_list;  // col index of all int-solns of this Node

	// Lists used for a CG iter of one Node
	vector<vector<float>> node_all_cols_list; // model matrix in current CG iter
	vector<float> dual_prices_list; // dual prices of the cons of MP in current CG iter
	vector<float> new_col; // one new col from SP in current CG iter
	vector<vector<float>> new_cols_list; // new cols from SP in current CG iter
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

vector<vector<float>> Heuristic(All_Values& Values, All_Lists& Lists);

int NodeIntergerJudgement(All_Values& Values, All_Lists& Lists);

int BranchAndPrice(All_Values& Values, All_Lists& Lists);

int SolveNode(int branch_flag, All_Values& Values, All_Lists& Lists);

float ColumnGenerationRootNode(All_Values& Values, All_Lists& Lists);

float ColumnGenerationNewNode(int branch_flag, All_Values& Values, All_Lists& Lists);

int SolveRootNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP);

int SolveNewNodeFirstMasterProblem(
	int branch_flag,
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP);

int SolveSubProblem(All_Values& Values, All_Lists& Lists);

int SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP);

float SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP);





