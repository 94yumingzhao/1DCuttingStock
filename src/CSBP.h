// 2022-03-06 

// CG -- column generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound
// PN -- parent node
// CN -- children node

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

// item type
struct ItemTypeProperties
{
	int type = -1;
	int length = -1;
	int demand = -1;
};

// stock type
struct StockTypeProperties
{
	int type = -1;
	int count = 0;
};

// item
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

// stock
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
	float branch_var_val;
	int branch_var_index;
	float branch_floor_val;
	float branch_ceil_val;

	// Lists used for this node
	vector<float> fsb_solns_list; // feasible (i.e. non-zero) solns of this Node
	vector<int> fsb_cols_list; // col-index of fsb-solns of this Node
	vector<float> int_solns_list; // all int-solns of this Node
	vector<int> int_cols_list;  // col-index of int-solns of this Node

	// Lists used for a CG iter of this node
	int iter;
	vector<vector<float>> model_matrix; // model matrix in current CG iter
	vector<float> dual_prices_list; // dual prices of MP cons in current CG iter
	vector<float> new_col; // one new col from SP in current CG iter
	vector<vector<float>> new_cols_list; // new cols from SP in current CG iter

	// this node flag
	int this_node_continue;
};

struct All_Values
{
	int stocks_num; // 
	int item_types_num; // 
	int stock_length; // 

	float current_optimal_bound = 0;	
};

struct All_Lists
{
	vector<ItemProperties> all_items_list; // all items
	vector<ItemTypeProperties> all_item_types_list; // all item types
	vector<Node> all_nodes_list; // all Nodes
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

void Heuristic(All_Values& Values, All_Lists& Lists, Node& root_node);

void SolveNode(int branch_flag, All_Values& Values, All_Lists& Lists, Node& this_node);

void ColumnGenerationRootNode(All_Values& Values, All_Lists& Lists, Node& root_node);

void ColumnGenerationNewNode(int branch_flag, All_Values& Values, All_Lists& Lists, Node& this_node);

int SolveRootNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& root_node);

int SolveNewNodeFirstMasterProblem(
	int branch_flag,
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node,
	Node& parent_node);

int SolveSubProblem(All_Values& Values, All_Lists& Lists,Node& this_node);

int SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node);

int SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& this_node);

//int NodeIntergerityJudgement(All_Values& Values, All_Lists& Lists, Node& this_node);

int BranchAndPrice(All_Values& Values, All_Lists& Lists, Node& this_node);






