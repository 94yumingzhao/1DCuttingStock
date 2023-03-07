// 2022-11-17

#include "CSBP.h"
using namespace std;

// judge the integerity of the Node, and find the branch var
int BranchAndPrice(All_Values& Values, All_Lists& Lists,Node& this_node)
{
	int integerity_flag = 1; // if current solns are all integers or not，0 -- no，1 -- yes

	// find the branch var
	float soln_val;
	int solns_num = this_node.fsb_solns_list.size();
	printf("\n	Current Node has %d columns\n", solns_num);
	for (int col = 0; col < solns_num; col++)
	{
		soln_val = this_node.fsb_solns_list[col]; 

		// judge the integerity of the soln
		int soln_int_val = int(soln_val);
		if (soln_int_val != soln_val)
		{
			printf("\n	Of all vars, var_x_%d = %f is NOT an integer......\n\n",col + 1,soln_val);
			integerity_flag = 0; // set the flag to "non-int-solns still exist"

			this_node.branch_var_index = col; // set the var-col index to branch
			this_node.branch_var_val = soln_val; // set the var val to branch

			break; // break the loop
		}
	}

	// all non-zero-solns are int 
	if (integerity_flag == 1)
	{
		printf("\n	Current Solutions are ALL Integers !!!\n");
	}

	return integerity_flag;
}


