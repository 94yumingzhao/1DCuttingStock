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
		continue_flag = FindNodeBranchVar(Values,this_node);

		// store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (continue_flag == 1)
		{
			printf("\n	Solns of root Node are all INTEGERS!\n");
		}
		cout << endl;
	}

	// New Node
	if (this_node.index != 1)
	{
		continue_flag =FindNodeBranchVar(Values,this_node);

		// Store this Node
		Lists.all_nodes_list.push_back(this_node);

		// all non-zero-solns are int, stop BP algorithm
		if (continue_flag == 1)
		{
			printf("\n	Solns of this Node are all INTEGERS! \n");
		} 
	}

	return continue_flag;
}

int FindNodeBranchVar(All_Values& Values, Node& this_node)
{
	// find the branch var of this Node
	int continue_flag = 1;
	float soln_val;

	size_t all_solns_num = this_node.all_solns_list.size();
	for (int col = 0; col < all_solns_num; col++)
	{
		soln_val = this_node.all_solns_list[col];
		if (soln_val > 0)
		{
			// judge the integerity of the soln
			int soln_int_val = int(soln_val);
			if (soln_int_val != soln_val) // not an integer
			{
				printf("\n	Node_%d var_x_%d = %f is NOT an integer\n", this_node.index, col + 1, soln_val);
				printf("\n	Branching on Node_%d var_x_%d\n", this_node.index, col + 1);

				this_node.branching_col_idx = col; // set the var-col index to branch
				this_node.branching_var_val = soln_val; // set the var val to branch	
				this_node.branching_floor_val = floor(soln_val);
				this_node.branching_ceil_val = ceil(soln_val);

				if (soln_int_val >= 1)
				{
					float floor_gap = this_node.branching_var_val - this_node.branching_floor_val;
					float ceil_gap = this_node.branching_ceil_val - this_node.branching_var_val;

					if (ceil_gap > floor_gap)
					{
						this_node.branching_final_val = this_node.branching_floor_val;

						printf("\n	The FLOOR value of %f =  %d\n",
							this_node.branching_var_val, this_node.branching_final_val);

						Values.branch_flag = 1; // The next Node is the left branch of this Node
					}
					else
					{
						this_node.branching_final_val = this_node.branching_ceil_val;

						printf("\n	The CEIL value of %f = %d\n",
							this_node.branching_var_val, this_node.branching_final_val);

						Values.branch_flag = 0; // The next Node is the right branch of this Node

					}
				}
				else
				{
					this_node.branching_final_val = this_node.branching_ceil_val;

					printf("\n	The CEIL value of %f = %d\n",
						this_node.branching_var_val, this_node.branching_final_val);

					Values.branch_flag = 1; // The next Node is the left branch of this Node
				}
				
				continue_flag = 0; // continue BP algorithm
				break; // break the loop			
			}
		}	
	}

	this_node.branched_vars_list.push_back(this_node.branching_final_val);
	this_node.branched_idx_list.push_back(this_node.branching_col_idx);

	return continue_flag;
}


