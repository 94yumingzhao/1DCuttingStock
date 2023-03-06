// 2022-11-17

#include "CSBP.h"
using namespace std;

int BranchAndPrice(All_Values& Values, All_Lists& Lists)
{
	// Jun
	int integerity_flag = NodeIntergerJudgement(Values, Lists);

	if (integerity_flag == 0) // 如果当前节点的解不全都是整数
	{
		Values.nodes_num = Values.nodes_num + 1;
		Node new_node;
		new_node.branch_val = Values.branch_var_val;
		new_node.branch_floor_val = floor(Values.branch_var_val);
		new_node.branch_value_ceil = ceil(Values.branch_var_val);
		new_node.node_index = Values.node_index;

		printf("\n	The next node is NODE_%d\n", new_node.node_index + 1);
		Lists.all_nodes_list.push_back(new_node);
	}
	else
	{
		printf("\n	There is no more NODE !!!\n");
	}
	return integerity_flag;
}

// judge if current solns are all integers or not
int NodeIntergerJudgement(All_Values& Values, All_Lists& Lists)
{
	float soln_val = 0.0;
	int integerity_flag = 1; // if current solns are all integers or not，0 --> no，1 --> yes
	Lists.node_int_cols_list.clear(); // clear after a CG loop of one Node finished.

	// find int-solns of this Node 
	int solns_num = Lists.node_all_solns_list.size();
	printf("\n	Current NODE has %d columns\n", solns_num);
	for (int col = 0; col < solns_num; col++)
	{
		soln_val = Lists.node_all_solns_list[col]; // soln val

		// Case 1；
		// if this soln is not int
		int soln_int_val = int(soln_val);
		if (soln_int_val != soln_val)
		{
			printf("\n	Of all vars, var_x_%d = %f is NOT AN INTEGER......\n\n",col + 1,soln_val);
			integerity_flag = 0; // set the flag to "non-int-solns still exist"
			Values.branch_var_index = col; // set the var-col index to branch
			Values.branch_var_val = soln_val; // set the var val to branch

			break; // break the loop
		}

		// Case 2：
		// if this soln is int
		else
		{
			Values.int_var_index = col; // 当前整数变量对应的列
			Lists.node_int_cols_list.push_back(col);
			Lists.node_int_solns_list.push_back(soln_val);
		}
	}

	//Case 3：
	// all non-zero-solns are int 
	if (integerity_flag == 1)
	{
		printf("\n	Current Solutions are ALL Integers !!!\n");
	}

	Lists.node_all_solns_list.clear(); // 当前节点求解结果清空，为后面节点求解做准备
	return integerity_flag;
}


