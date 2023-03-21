// 2023-03-16
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
// col -- column
//
// fsb -- feasible
// int -- integer
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
struct ItemTypeProperties
{
	int item_type_idx = -1;
	int item_type_length = -1;
	int item_type_demand = -1;
};

// item
struct ItemProperties
{
	int length = -1;
	int idx = -1;
};

struct All_Values
{
	int stocks_num = -1; // number of all available stocks
	int item_types_num = -1; // number of all item_types demanded
	int stock_length = -1; // item_type_length of a stock

	int iter = -1;
	double lower_bound = -1;
};

struct All_Lists
{
	vector<ItemProperties> all_items_list; // list of all items 
	vector<ItemTypeProperties> all_item_types_list; // list of all item_types

	vector<vector<double>> model_matrix;
	vector<double> dual_prices_list;
	vector<double>new_col;
};

void SplitString(const string& s, vector<string>& v, const string& c);

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);

void InitModelMatrix(All_Values& Values, All_Lists& Lists);

void ColumnGeneration(All_Values& Values, All_Lists& Lists);

bool SolveFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP);

bool SolveSubProblem(All_Values& Values, All_Lists& Lists);

bool SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP);
















