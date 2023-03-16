// 2023-03-06 
// 
// CG -- column generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound
//
// var -- variable
// con -- constraint
// para -- parameter
// col -- column
//
// fsb -- feasible
// int -- integer
// soln -- solution
// val -- value

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
	int count = -1;
};

// item
struct ItemProperties
{
	int type = -1;
	int demand = -1;
	int length = -1;
	int index = -1;
	int stock_index = -1;
	int occupied = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;
};

// stock
struct StockProperties
{
	int type = -1;
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
	int index = -1; 

	// Values of the Parent Node of one Node
	int parent_index = -1;
	int parent_branching_flag = -1;

	double lower_bound = -1;
	double parent_var_to_branch_val = -1;

	// Values of Node status
	int node_branched_flag=-1;
	int node_pruned_flag=-1;

	// Values of final branching of one Node
	int var_to_branch_idx = -1; // column index of the var-to-branch in Parent Node
	double var_to_branch_val = -1; // soln-val of the var-to-branch in Parent Node
	double var_to_branch_val_floor = -1; // floor integer value of the var-to-branch in Parent Node
	double var_to_branch_val_ceil = -1; // ceil interger value of the var-to-branch in Parent Node
	double var_to_branch_int_val_final =-1; // the fixed val of the var-to-branch

	// Lists of final branching of one Node
	vector<int> branched_vars_idx_list; // column indexes of all branched-vars of previous Nodes on BP Tree
	vector<double> branched_vars_int_val_list; // all branched-vars of previous Nodes on BP Tree 
	vector<double> branched_vars_soln_val_list; // all branched-vars of previous Nodes on BP Tree 

	vector<vector<int>>branched_cols_list;

	vector<double> all_solns_val_list; // final all (include 0) solutions of this Node
	vector<double> fsb_solns_val_list; // final feasible (i.e. non-0) solutions of this Node
	vector<int> fsb_solns_idx_list; // final column indexes of feasible solutions of this Node
	vector<double> int_solns_val_list; // final all integer solutions of this Node
	vector<int> int_solns_idx_list;  // final column indexes of integer solutions of this Node

	// Lists of one Column Generation iter of one Node
	int iter = -1;
	vector<vector<double>> model_matrix; // model matrix in this CG iter
	vector<double> dual_prices_list; // dual prices of Master Problem cons in this CG iter
	vector<double> new_col; // one new col from Sub Problem in this CG iter
	vector<vector<double>> new_cols_list; // new cols from Sub Problem in this CG iter

};

struct All_Values
{
	int stocks_num = -1; // number of all available stocks
	int item_types_num = -1; // number of all item_types demanded
	int stock_length = -1; // length of a stock

	double tree_optimal_bound = -1; // current optimal lower bound of BP Tree

	// flag of the next Node
	// 1 -- new left Node,
	// 2 -- new right Node
	// 3 -- previously generated Node
	int branch_status = -1;

	// flag of branching or searching
	// 0 -- contiinue to branch current Node
	// 1 -- stop at current Node and search for a previously generated Node
	int search_flag = -1; 

	// 
	int fathom_flag = -1;

	int level_num;
	int node_num;

	int integerity_nodes_num = -1;
};

struct All_Lists
{
	vector<ItemProperties> all_items_list; // all items 
	vector<ItemTypeProperties> all_item_types_list; // all item_types
	vector<Node> all_nodes_list; // all Nodes generated on BP Tree
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

void InitRootNodeMatrix(All_Values& Values, All_Lists& Lists, Node& root_node);

void RootNodeColumnGeneration(All_Values& Values, All_Lists& Lists, Node& root_node);

void NewNodeColumnGeneration(All_Values& Values, All_Lists& Lists, Node& this_node, Node& parent_node);

bool SolveRootNodeFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_List_MP,
	IloNumVarArray& Vars_List_MP,
	Node& root_node);

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

//int NodeIntergerityJudgement(All_Values& Values, All_Lists& Lists, Node& this_node);

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists);

int BranchOrSearch(All_Values& Values, All_Lists& Lists, Node& this_node);

int NodeBranchAndStore(All_Values& Values, All_Lists& Lists,Node& this_node);

void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node&new_node,Node& parent_node);

void TerminalDisplay(int display_idx, int para_1,int para_2);

void FindNodeToBranch(All_Values& Values, All_Lists& Lists, Node& parent_node);






