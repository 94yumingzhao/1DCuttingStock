// 2022-11-17

#include "CSBP.h"
using namespace std;

// judge the integerity of the Node, and find the branch var
int BranchAndPrice(All_Values& Values, All_Lists& Lists, Node& this_node)
{
	
	int continue_flag = -1;

	// Root Node 
	if (this_node.index == 1)
	{
		continue_flag = FindNodeBranchVar(this_node);

		// store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (continue_flag == 1)
		{
			printf("\n	Solns of this Node are all integers!\n");
		}
		cout << endl;
	}

	// New Node
	if (this_node.index != 1)
	{
		continue_flag =FindNodeBranchVar(this_node);

		// Store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (continue_flag == 1)
		{
			printf("\n	Solns of this Node are all integers!\n");
		} 
	}

	return continue_flag;
}

int FindNodeBranchVar(Node& this_node)
{
	// find the branch var of this Node
	int continue_flag = 1;
	float soln_val;
	size_t all_solns_num = this_node.all_solns_list.size();
	for (int col = 0; col < all_solns_num; col++)
	{
		soln_val = this_node.all_solns_list[col];
		// judge the integerity of the soln
		int soln_int_val = int(soln_val);
		if (soln_int_val != soln_val) // not an integer
		{
			printf("\n	Node_%d: var_x_%d = %f is NOT an integer\n",  this_node.index, col + 1, soln_val);
			printf("\n	So branch on Node_%d var_x_%d\n", this_node.index, col + 1);

			this_node.branch_var_index = col; // set the var-col index to branch
			this_node.branch_var_val = soln_val; // set the var val to branch	
			this_node.branch_floor_val = floor(soln_val);
			this_node.branch_ceil_val = ceil(soln_val);

			continue_flag = 0; // continue BP algorithm
			break; // break the loop
		}
	}

	return continue_flag;
}


