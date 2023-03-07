// 2022-11-17

#include "CSBP.h"
using namespace std;

// judge the integerity of the Node, and find the branch var
int BranchAndPrice(All_Values& Values, All_Lists& Lists,Node& this_node)
{
	int continue_flag = 1;

	// If this Node provide a better bound than current optimal bound
	if (this_node.lower_bound <= Values.current_optimal_bound)
	{
		// update current optimal bound val
		Values.current_optimal_bound = this_node.lower_bound;

		// if current solns are all integers or not，0 -- no，1 -- yes
		

		// find the branch var of this Node
		float soln_val;
		size_t solns_num = this_node.fsb_solns_list.size();
		printf("\n	Current Node has %zd columns\n", solns_num);
		for (int col = 0; col < solns_num; col++)
		{
			soln_val = this_node.fsb_solns_list[col];

			// judge the integerity of the soln
			int soln_int_val = int(soln_val);
			if (soln_int_val != soln_val) // not an integer
			{			
				printf("\n	Of all vars, var_x_%d = %f is NOT an integer......\n\n", col + 1, soln_val);
				this_node.branch_var_index = col; // set the var-col index to branch
				this_node.branch_var_val = soln_val; // set the var val to branch

				continue_flag = 0; // continue BP algorithm
				break; // break the loop
			}
		}

		// all non-zero-solns are int, stop BP algorithm
		if (continue_flag == 1)
		{
			printf("\n	Current Solutions are ALL Integers !!!\n");
		}
	}
	// If this Node provide NO better bound than current optimal bound
	if (this_node.lower_bound > Values.current_optimal_bound)
	{
		// no need to continue branch this Node
		continue_flag = 2;
	}

	return continue_flag;
}


