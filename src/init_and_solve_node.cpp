// 2022-11-17

#include "CSBP.h"
using namespace std;


// function to init, set, and solve a new Node with CG loop
void SolveOneNode(All_Values& Values, All_Lists& Lists, Node& one_node)
{
	if (Values.tree_branching_status == 0) // if root Node
	{
		ColumnGenerationRootNode(Values, Lists, one_node); // solve root node with CG loop
	}
	else
	{
		Node node_to_branch; // Init parent Node
		ChooseNodeToBranch(Values,Lists, node_to_branch); // Set parent Node
		InitNewNode(Values, Lists,one_node, node_to_branch); // Set new Node from parent Nodezzzzzzzzzzzzzzs
		ColumnGenerationNewNode(Values, Lists, one_node, node_to_branch); // solve new Node with CG loop
	}
}

void ChooseNodeToBranch(All_Values&Values, All_Lists& Lists, Node& node_to_branch)
{
	int pos = -1;

	if (Values.tree_branching_status == 1)
	{
		pos = Lists.all_nodes_list.size() - 1;
	}
	if (Values.tree_branching_status == 2)
	{
		pos = Lists.all_nodes_list.size() - 2;
	}
	if (Values.tree_branching_status == 3)
	{
		size_t nodes_num = Lists.all_nodes_list.size();
		for (size_t k = 0; k < nodes_num; k++)
		{
			if (Lists.all_nodes_list[k].node_branched_flag != 1 &&
				Lists.all_nodes_list[k].lower_bound <= Values.tree_optimal_bound)
			{
				pos = k;
				cout << endl;
			}
		}
	}
	node_to_branch.parent_branching_flag = Lists.all_nodes_list[pos].parent_branching_flag;
	node_to_branch.parent_index = Lists.all_nodes_list[pos].parent_index;

	node_to_branch.index = Lists.all_nodes_list[pos].index; //1
	node_to_branch.lower_bound = Lists.all_nodes_list[pos].lower_bound;

	node_to_branch.branching_var_idx = Lists.all_nodes_list[pos].branching_var_idx; //4
	node_to_branch.branching_var_val = Lists.all_nodes_list[pos].branching_var_val;
	node_to_branch.branching_var_val_floor = Lists.all_nodes_list[pos].branching_var_val_floor;
	node_to_branch.branching_var_val_ceil = Lists.all_nodes_list[pos].branching_var_val_ceil;

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
		node_to_branch.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their col-idx list of the Node-to-branch
	for (size_t col = 0; col < branched_num; col++)
	{
		double temp_val = Lists.all_nodes_list[pos].branched_vars_val_list[col];
		int temp_idx = Lists.all_nodes_list[pos].branched_vars_idx_list[col];

		node_to_branch.branched_vars_val_list.push_back(temp_val); //7
		node_to_branch.branched_vars_idx_list.push_back(temp_idx); //8
	}
}

void InitNewNode(All_Values&Values, All_Lists&Lists,Node& new_node, Node& parent_node)
{
	size_t nodes_num = Lists.all_nodes_list.size();
	new_node.index = nodes_num + 1;

	if (Values.tree_branching_status == 1) // 1
	{
		printf("\n	NODE_%d is the LEFT branch of NODE_%d	\n",new_node.index, parent_node.index);
	}
	if (Values.tree_branching_status == 2)
	{
		printf("\n	NODE_%d is the RIGHT branch of NODE_%d	\n", new_node.index, parent_node.index);
	}

	printf("\n##########################################################\n");
	printf("##########################################################\n");
	printf("####################### NEW NODE_%d #######################\n", new_node.index);
	printf("##########################################################\n");
	printf("##########################################################\n\n");

	new_node.lower_bound = parent_node.lower_bound;

	new_node.parent_index = parent_node.index;	 //2
	new_node.parent_branching_flag = Values.tree_branching_status; //3

	new_node.branching_var_idx = parent_node.branching_var_idx; //4
	new_node.branching_var_val = parent_node.branching_var_val;

	if (Values.tree_branching_status == 1)
	{
		new_node.branching_var_val_final = parent_node.branching_var_val_floor;
	}
	if (Values.tree_branching_status == 2)
	{
		new_node.branching_var_val_final = parent_node.branching_var_val_ceil;
	}

	size_t cols_num = parent_node.model_matrix.size();
	size_t rows_num = parent_node.model_matrix[0].size();
	size_t branched_num = parent_node.branched_vars_val_list.size();

	// Init model matrix of the new Node
	for (size_t col = 0; col < cols_num; col++)
	{
		vector<int> temp_col;
		for (size_t row = 0; row < rows_num; row++)
		{
			int temp_val = parent_node.model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		new_node.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their idx list of the new Node 
	for (size_t col = 0; col < branched_num; col++)
	{
		double temp_val = parent_node.branched_vars_val_list[col];
		int temp_idx = parent_node.branched_vars_idx_list[col];

		new_node.branched_vars_val_list.push_back(temp_val); //7
		new_node.branched_vars_idx_list.push_back(temp_idx); //8
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
}



