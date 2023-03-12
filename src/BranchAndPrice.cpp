// 2022-11-17
#include "CSBP.h"
using namespace std;

// judge the integerity of the Node, and find the branch var
int BranchOrSwitch(All_Values& Values, All_Lists& Lists, Node& this_node)
{	
	int node_int_flag = -1; // 0 -- some fsb-solns are not int; 1 -- all fsb-solns are int
	int node_continue_flag = -1; // 0 -- some fsb-solns are not int; 1 -- all fsb-solns are int

	// Root Node 
	if (this_node.index == 1)
	{
		node_int_flag = FindNodeBranchVar(Values,Lists,this_node);

		// store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (node_int_flag == 1)
		{
			node_continue_flag = 1;
			printf("\n	Solns of root Node are all INTEGERS!\n");
		}
		else
		{
			node_continue_flag = 0;
		}
		cout << endl;
	}

	// New Node
	if (this_node.index != 1)
	{
		node_int_flag =FindNodeBranchVar(Values, Lists, this_node);

		// Store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (node_int_flag == 1)
		{
			node_continue_flag = 1;
			printf("\n	Solns of this Node are all INTEGERS! \n");
		}
		else
		{
			node_continue_flag = 0;
		}
	}

	return node_continue_flag;
}

int FindNodeBranchVar(All_Values& Values, All_Lists& Lists, Node& this_node)
{
	int node_int_flag = -1; // 0 -- some fsb-solns are not int; 1 -- all fsb-solns are int
	double soln_val;

	// find the branching var of this Node
	size_t all_solns_num = this_node.all_solns_val_list.size();
	for (size_t col = 0; col < all_solns_num; col++)
	{
		soln_val = this_node.all_solns_val_list[col];
		if (soln_val > 0)
		{
			// judge the integerity of the soln
			int soln_int_val = int(soln_val);
			if (soln_int_val != soln_val) // not an integer
			{
				printf("\n	Node_%d var_x_%d = %f is NOT an integer\n", this_node.index, col + 1, soln_val);
				printf("\n	Branching on Node_%d var_x_%d\n", this_node.index, col + 1);

				this_node.branching_var_idx = col; // set the branching var-col index
				this_node.branching_var_val = soln_val; // set the branching var	
				this_node.branching_var_val_floor = floor(soln_val);
				this_node.branching_var_val_ceil = ceil(soln_val);

				/*
				if (soln_int_val >= 1)
				{
					double floor_gap = this_node.branching_var_val - this_node.branching_var_val_floor;
					double ceil_gap = this_node.branching_var_val_ceil - this_node.branching_var_val;

					if (ceil_gap > floor_gap)
					{
						this_node.branching_final_val = this_node.branching_var_val_floor;

						printf("\n	The FLOOR value of %f =  %d\n",
							this_node.branching_var_val, this_node.branching_final_val);

						Values.tree_branching_status = 1; // The next Node is the left branch of this Node
					}
					else
					{
						this_node.branching_final_val = this_node.branching_var_val_ceil;

						printf("\n	The CEIL value of %f = %d\n",
							this_node.branching_var_val, this_node.branching_final_val);

						Values.tree_branching_status = 0; // The next Node is the right branch of this Node

					}
				}
				else
				{
					this_node.branching_final_val = this_node.branching_var_val_ceil;

					printf("\n	The CEIL value of %f = %d\n",
						this_node.branching_var_val, this_node.branching_final_val);

					Values.tree_branching_status = 1; // The next Node is the left branch of this Node
				}
				*/
				node_int_flag = 0; // continue BP algorithm
				break; // break the loop			
			}
		}	
	}

	if (node_int_flag == 0)
	{
		this_node.node_branched_flag = 1;
		int col_idx = this_node.branching_var_idx;
		this_node.branched_vars_idx_list.push_back(col_idx);
	}
	if (node_int_flag == -1)
	{
		Values.tree_optimal_bound = this_node.lower_bound;
		node_int_flag = 1;
	}

	return node_int_flag;
}



