// 2023-03-17

#include "CSBB.h"
using namespace std;

int ChooseNodeToBranch(All_Values& Values, All_Lists& Lists, Node& parent_node)
{
	int parent_branch_flag = -1;
	int pos = -1;
	int nodes_num = Lists.all_nodes_list.size();

	if (Values.tree_branch_status == 3) // search for a previously generated unbranched unpruned Node
	{
		for (int k = 0; k < nodes_num; k++)
		{
			if (Lists.all_nodes_list[k].node_branched_flag != 1 &&
				Lists.all_nodes_list[k].node_pruned_flag != 1) // unbranched unpruned
			{
				if (Lists.all_nodes_list[k].node_lower_bound < Values.tree_optimal_lower_bound)
				{
					pos = k; // branch this previously generated Node_(k+1)
					cout << endl;
				}
				else
				{
					Lists.all_nodes_list[k].node_pruned_flag = 1; // prune this previously generated Node
					printf("\n\t Node_%d has to be pruned\n", Lists.all_nodes_list[k].index);
				}
			}
		}
	}

	if (Values.tree_branch_status != 3) // continue to branch the Parent Node
	{
		if (Values.root_flag == 1) // the Parent Node is Root Node
		{
			if (Values.tree_branch_status == 1) // the the Left Node of Root Node
			{
				pos = nodes_num - 1; // sub left index = parent index + 1
			}
			if (Values.tree_branch_status == 2) // the the Right Node of Root Node
			{
				pos = nodes_num - 2; // sub right index = parent index + 2
			}
		}

		if (Values.root_flag != 1) // the Parent Node is not  Root Node
		{
			if (Values.node_fathom_flag == 1) // the Parent Node is a Left Node
			{
				if (Values.tree_branch_status == 1)
				{
					pos = nodes_num - 2; // sub left index = parent index + 2
				}
				if (Values.tree_branch_status == 2)
				{
					pos = nodes_num - 3;  // sub right index = parent index + 3
				}
			}

			if (Values.node_fathom_flag == 2) // the Parent Node is a Right Node
			{
				if (Values.tree_branch_status == 1)
				{
					pos = nodes_num - 1; // sub left index = parent index + 1
				}
				if (Values.tree_branch_status == 2)
				{
					pos = nodes_num - 2; // sub right index = parent index + 2
				}
			}
		}		
	}

	if (pos == -1)
	{
		parent_branch_flag = 0;

		printf("\n\t No Node to branch! \n");
	}
	else
	{
		parent_branch_flag = 1;

		parent_node = Lists.all_nodes_list[pos];
		parent_node.node_branched_flag = 1;	

		/*
		Lists.all_nodes_list[pos].node_branched_flag = 1;
		
		parent_node.index = Lists.all_nodes_list[pos].index;
		parent_node.node_lower_bound = Lists.all_nodes_list[pos].node_lower_bound;

		parent_node.parent_index = Lists.all_nodes_list[pos].parent_index;
		parent_node.parent_branching_flag = Lists.all_nodes_list[pos].parent_branching_flag;
		parent_node.parent_var_to_branch_val = Lists.all_nodes_list[pos].parent_var_to_branch_val;

		parent_node.var_to_branch_idx = Lists.all_nodes_list[pos].var_to_branch_idx;
		parent_node.var_to_branch_soln_val = Lists.all_nodes_list[pos].var_to_branch_soln_val;
		parent_node.var_to_branch_int_val_floor = Lists.all_nodes_list[pos].var_to_branch_int_val_floor;
		parent_node.var_to_branch_int_val_ceil = Lists.all_nodes_list[pos].var_to_branch_int_val_ceil;

		int all_cols_num = Lists.all_nodes_list[pos].model_matrix.size();
		int all_rows_num = Lists.all_nodes_list[pos].model_matrix[0].size();
		int branched_num = Lists.all_nodes_list[pos].branched_vars_idx_list.size();

		// Init model matrix of the Parent Node
		for (int col = 0; col < all_cols_num; col++)
		{
			vector<double> temp_col;
			for (int row = 0; row < all_rows_num; row++)
			{
				double temp_val = Lists.all_nodes_list[pos].model_matrix[col][row];
				temp_col.push_back(temp_val);
			}
			parent_node.model_matrix.push_back(temp_col);
		}

		for (int k = 0; k < branched_num; k++)
		{
			double temp_val = Lists.all_nodes_list[pos].branched_vars_soln_val_list[k];

			parent_node.branched_vars_soln_val_list.push_back(temp_val);
		}

		// Init branched-vars list of the Parent Node
		for (int k = 0; k < branched_num; k++)
		{
			int temp_idx = Lists.all_nodes_list[pos].branched_vars_idx_list[k];
			parent_node.branched_vars_idx_list.push_back(temp_idx);
		}

		if (branched_num > 1)
		{
			for (int k = 0; k < branched_num - 1; k++)
			{
				double temp_val = Lists.all_nodes_list[pos].branched_vars_int_val_list[k];
				parent_node.branched_vars_int_val_list.push_back(temp_val);
			}
		}
		*/

		printf("\n\t The Node to branch is Node_%d\n", parent_node.index);
	}
	
	return parent_branch_flag;
}


void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node& new_node, Node& parent_node)
{
	int nodes_num = Lists.all_nodes_list.size();

	new_node.index = nodes_num + 1;
	new_node.node_lower_bound = -1;

	if (Values.tree_branch_status == 1)
	{
		printf("\n\t Newly Generated Node_%d is the LEFT branch of Node_%d	\n", new_node.index, parent_node.index);
	}
	if (Values.tree_branch_status == 2)
	{
		printf("\n\t Newly Generated Node_%d is the RIGHT branch of Node_%d	\n", new_node.index, parent_node.index);
	}

	new_node.parent_index = parent_node.index;
	new_node.parent_branching_flag = Values.tree_branch_status;
	new_node.parent_var_to_branch_val = parent_node.var_to_branch_soln_val;

	printf("\n##########################################################\n");
	printf("##########################################################\n");
	printf("####################### NEW NODE_%d #######################\n", new_node.index);
	printf("##########################################################\n");
	printf("##########################################################\n\n");

	new_node.var_to_branch_idx = -1;
	new_node.var_to_branch_soln_val = -1;
	new_node.var_to_branch_int_val_floor = -1;
	new_node.var_to_branch_int_val_ceil = -1;
	new_node.var_to_branch_int_val_final = -1;

	int all_cols_num = parent_node.model_matrix.size();
	int all_rows_num = parent_node.model_matrix[0].size();
	int branched_num = parent_node.branched_vars_idx_list.size();

	// Init model matrix of the Node-to-branch
	for (int col = 0; col < all_cols_num; col++)
	{
		vector<double> temp_col;
		for (int row = 0; row < all_rows_num; row++)
		{
			double temp_val = parent_node.model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		new_node.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their col-index list of the Node-to-branch
	for (int col = 0; col < branched_num; col++)
	{
		int temp_idx = parent_node.branched_vars_idx_list[col];
		new_node.branched_vars_idx_list.push_back(temp_idx);
	}

	if (Values.tree_branch_status == 1)
	{
		new_node.var_to_branch_int_val_final = parent_node.var_to_branch_int_val_floor;
	}
	if (Values.tree_branch_status == 2)
	{
		new_node.var_to_branch_int_val_final = parent_node.var_to_branch_int_val_ceil;
	}

	double final_int_val = new_node.var_to_branch_int_val_final;

	if (branched_num <= 1) // if new_node is the left or the Right Node of Root Node
	{
		new_node.branched_vars_int_val_list.push_back(final_int_val);
	}
	if (branched_num > 1) // other Nodes
	{
		for (int col = 0; col < branched_num - 1; col++)
		{
			double temp_val = parent_node.branched_vars_int_val_list[col];
			new_node.branched_vars_int_val_list.push_back(temp_val);
		}
		new_node.branched_vars_int_val_list.push_back(final_int_val);
	}

	// Clear all other lists to init them
	new_node.all_solns_val_list.clear();
	//new_node.fsb_solns_val_list.clear();
	//new_node.fsb_solns_idx_list.clear();
	//new_node.int_solns_idx_list.clear();
	//new_node.int_solns_val_list.clear();

	new_node.dual_prices_list.clear();
	new_node.new_col.clear();
	//new_node.new_cols_list.clear();

}


