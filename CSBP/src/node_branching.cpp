﻿// 2022-11-17
#include "CSBP.h"
using namespace std;

// judge the integerity of the Node, and find the branch var
int BranchOrSearch(All_Values& Values, All_Lists& Lists, Node& this_node)
{	
	int node_int_flag = -1; // 0 -- some fsb-solns are not int; 1 -- all fsb-solns are int
	int search_flag = -1; // 0 -- continue to branch this Node; 1 -- search another generated Node

	node_int_flag = NodeBranchAndStore(Values, Lists, this_node); // branch this Node

	if (node_int_flag == 1)  // ALL non-zero-solns are int in this Node
	{
		search_flag = 1; 
	}
	else  // NOT ALL non-zero-solns are int in this Node
	{
		search_flag = 0; 
	}

	return search_flag; // 0 -- continue to branch this Node; 1 -- search another generated Node
}



int NodeBranchAndStore(All_Values& Values, All_Lists& Lists, Node& this_node)
{
	bool node_int_flag = 1; // 0 -- some fsb-solns are not int; 1 -- all fsb-solns are int
	double soln_val;

	// find the var-to-branch of this Node
	int all_solns_num = this_node.all_solns_val_list.size();
	for (int col = 0; col < all_solns_num; col++)
	{
		soln_val = this_node.all_solns_val_list[col];
		if (soln_val > 0)
		{		
			int soln_int_val = int(soln_val); // judge the integerity
			if (soln_int_val != soln_val) // not an integer
			{
				printf("\n	Node_%d var_x_%d = %f is NOT an integer\n", this_node.index, col + 1, soln_val);

				this_node.var_to_branch_idx = col; // set the var-to-branch-col index
				this_node.var_to_branch_val = soln_val; // set the var-to-branch	
				this_node.var_to_branch_val_floor = floor(soln_val);
				this_node.var_to_branch_val_ceil = ceil(soln_val);
					
				node_int_flag = 0; // continue BP algorithm

				break; // break the loop			
			}
		}	
	}

	if (node_int_flag == 0)
	{
		int var_idx = this_node.var_to_branch_idx;
		this_node.branched_vars_idx_list.push_back(var_idx);

		double var_val = this_node.var_to_branch_val;
		this_node.branched_vars_soln_val_list.push_back(var_val);

		cout << endl;
	}

	if (node_int_flag == 1)
	{
		this_node.node_branched_flag = 0;
		if (this_node.lower_bound < Values.tree_optimal_bound)
		{
			Values.tree_optimal_bound = this_node.lower_bound;
		}
		cout<<endl;
	}

	Lists.all_nodes_list.push_back(this_node); 	// store this Node

	return node_int_flag;
}


