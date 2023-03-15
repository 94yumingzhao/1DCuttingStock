// 2022-11-17

#include "CSBP.h"
using namespace std;

void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node& new_node)
{
	int pos = -1;
	size_t nodes_num = Lists.all_nodes_list.size();

	if (Values.branch_status !=3) // continue to branch on the parent Node
	{
		if (Values.fathom_flag == 0) // the parent Node is Root Node
		{
			if (Values.branch_status == 1) // the left Node of Root Node
			{
				pos = nodes_num - 1;
			}
			if (Values.branch_status == 2) // the right Node of Root Node
			{
				pos = nodes_num - 2;
			}	
		}
		if (Values.fathom_flag == 1) // the parent Node is a left Node
		{
			pos = nodes_num - 2;
		}
		if (Values.fathom_flag == 2) // the parent Node is a right Node
		{
			pos = nodes_num - 1;
		}
	}

	if (Values.branch_status == 3) // search for a previously generated unbranched unpruned Node
	{	
		for (size_t k = 0; k < nodes_num; k++)
		{
			if (Lists.all_nodes_list[k].node_branched_flag != 1 &&
				Lists.all_nodes_list[k].node_pruned_flag != 1)
			{
				if (Lists.all_nodes_list[k].lower_bound < Values.tree_optimal_bound)
				{
					pos = k; // branch this previously generated Node
					new_node.index = Lists.all_nodes_list[pos].index;
					cout << endl;
				}
				//else
				//{
				//	Lists.all_nodes_list[k].node_pruned_flag = 1; // prune this previously generated Node
				//}
			}
		}
	}

	printf("\n	The next Node to branch is Node_%d\n", Lists.all_nodes_list[pos].index);
	new_node.parent_index = Lists.all_nodes_list[pos].index;
	new_node.parent_branching_flag = Values.branch_status;

	new_node.index = nodes_num+1;

	if (Values.branch_status == 1)
	{
		
		printf("\n	Node_%d is the LEFT branch of Node_%d	\n", new_node.index, Lists.all_nodes_list[pos].index);
	}
	if (Values.branch_status == 2)
	{
		new_node.index = Lists.all_nodes_list[pos].index + 2;
		printf("\n	Node_%d is the RIGHT branch of Node_%d	\n", new_node.index, Lists.all_nodes_list[pos].index);
	}

	new_node.lower_bound = Lists.all_nodes_list[pos].lower_bound;

	printf("\n##########################################################\n");
	printf("##########################################################\n");
	printf("####################### NEW NODE_%d #######################\n", new_node.index);
	printf("##########################################################\n");
	printf("##########################################################\n\n");

	new_node.var_to_branch_idx = Lists.all_nodes_list[pos].var_to_branch_idx;
	new_node.var_to_branch_val = Lists.all_nodes_list[pos].var_to_branch_val;
	new_node.var_to_branch_val_floor = Lists.all_nodes_list[pos].var_to_branch_val_floor;
	new_node.var_to_branch_val_ceil = Lists.all_nodes_list[pos].var_to_branch_val_ceil;

	if (Values.branch_status == 1)
	{
		new_node.var_to_branch_val_final = Lists.all_nodes_list[pos].var_to_branch_val_floor;

		/*if (new_node.var_to_branch_val > 1)
		{
			new_node.var_to_branch_val_final = Lists.all_nodes_list[pos].var_to_branch_val_floor;
		}
		else
		{
			new_node.var_to_branch_val_final = Lists.all_nodes_list[pos].var_to_branch_val_ceil;
		}*/
	}
	if (Values.branch_status == 2)
	{
		new_node.var_to_branch_val_final = Lists.all_nodes_list[pos].var_to_branch_val_ceil;
	}

	size_t cols_num = Lists.all_nodes_list[pos].model_matrix.size();
	size_t rows_num = Lists.all_nodes_list[pos].model_matrix[0].size();
	size_t branched_num = Lists.all_nodes_list[pos].branched_vars_val_list.size();

	// Init model matrix of the Node-to-branch
	for (size_t col = 0; col < cols_num; col++)
	{
		vector<int> temp_col;
		for (size_t row = 0; row < rows_num; row++)
		{
			int temp_val = Lists.all_nodes_list[pos].model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		new_node.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their col-idx list of the Node-to-branch
	for (size_t col = 0; col < branched_num; col++)
	{
		double temp_val = Lists.all_nodes_list[pos].branched_vars_val_list[col];
		int temp_idx = Lists.all_nodes_list[pos].branched_vars_idx_list[col];

		new_node.branched_vars_val_list.push_back(temp_val);
		new_node.branched_vars_idx_list.push_back(temp_idx);
	}

	// Clear all other lists to init them
	new_node.all_solns_val_list.clear();
	new_node.fsb_solns_val_list.clear();
	new_node.fsb_solns_idx_list.clear();
	new_node.int_solns_idx_list.clear();
	new_node.int_solns_val_list.clear();

	new_node.dual_prices_list.clear();
	new_node.new_col.clear();
	new_node.new_cols_list.clear();

	cout << endl;
}


