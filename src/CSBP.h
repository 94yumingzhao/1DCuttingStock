// 2022-03-06 

// CG -- column generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound

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
	int parent_branch_flag = -1;
	//int parent_branch_val = -1;

	// Values of one Node
	float lower_bound = -1; 
	float branching_var_val = -1; // soln-val of the to-branching var in Parent Node
	int branching_col_idx = -1; // column index of the to-branching var in Parent Node
	int branching_floor_val = -1; // floor integer value of the to-branching var in Parent Node
	int branching_ceil_val = -1; // ceil interger value of the to-branching var in Parent Node
	int branching_final_val =-1; // the fixed val of the to-branching var

	// Lists of one Node
	vector<int> branched_vars_list; // all branched-vars of previous Nodes on the TREE 
	vector<int> branched_idx_list; // column indexes of all branched-vars of previous Nodes on the TREE
	vector<vector<float>>branched_cols_list;

	vector<float> all_solns_list; // final all (include 0) solutions of this Node
	vector<float> fsb_solns_list; // final feasible (i.e. non-0) solutions of this Node
	vector<int> fsb_idx_list; // final column indexes of feasible solutions of this Node
	vector<int> int_solns_list; // final all integer solutions of this Node
	vector<int> int_idx_list;  // final column indexes of integer solutions of this Node

	// Lists of one Column Generation iter of one Node
	int iter = -1;
	vector<vector<float>> model_matrix; // model matrix in this CG iter
	vector<float> dual_prices_list; // dual prices of Master Problem cons in this CG iter
	vector<float> new_col; // one new col from Sub Problem in this CG iter
	vector<vector<float>> new_cols_list; // new cols from Sub Problem in this CG iter

};

struct All_Values
{
	int stocks_num = -1; // number of all available stocks
	int item_types_num = -1; // number of all item_types demanded
	int stock_length = -1; // length of a stock
	float current_optimal_bound = -1; // current optimal lower bound of the TREE

	float Branch_Val = -1;

	int branch_flag = 2; // flag of branching, 0 -- left , 1 -- right, 2 -- root 
	int continue_flag = -1; //  if there is non-int-solns in a Node, 0 -- yes, 1 -- no
};

struct All_Lists
{
	vector<ItemProperties> all_items_list; // all items 
	vector<ItemTypeProperties> all_item_types_list; // all item_types
	vector<Node> all_nodes_list; // all Nodes generated on the TREE
	vector<float> all_branched_vars_list;
	vector<int> all_branched_idx_list;
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

void InitRootNodeMatrix(All_Values& Values, All_Lists& Lists, Node& root_node);

void SolveOneNode(All_Values& Values, All_Lists& Lists, Node& this_node);

void ColumnGenerationRootNode(All_Values& Values, All_Lists& Lists, Node& root_node);

void ColumnGenerationNewNode(All_Values& Values, All_Lists& Lists, Node& this_node, Node& parent_node);

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

int BranchAndPrice(All_Values& Values, All_Lists& Lists, Node& this_node);

int FindNodeBranchVar(All_Values& Values,  Node& this_node);

void ChooseNodeToBranch(All_Lists& Lists, Node&node_to_branch);

void InitNewNode(All_Values& Values, Node& new_node, Node& parent_node);






