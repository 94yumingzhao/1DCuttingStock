// 2022-11-17

#include "CSBP.h"
using namespace std;


// function to init, set, and solve a new Node with CG loop
void SolveOneNode(All_Values& Values, All_Lists& Lists, Node& new_node)
{
	if (Values.branch_flag == 2) // if root Node
	{	
		ColumnGenerationRootNode(Values, Lists, new_node); // solve root node with CG loop
	}
	else // if other Nodes
	{	
		Node node_to_branch; // Init parent Node

		ChooseNodeToBranch(Lists, node_to_branch); // Set parent Node

		InitNewNode(Values, new_node, node_to_branch); // Set new Node from parent Node
	
		ColumnGenerationNewNode(Values, Lists, new_node, node_to_branch); // solve new Node with CG loop
	}
}

void ChooseNodeToBranch(All_Lists& Lists, Node& node_to_branch)
{
	// This version uses the last Node in Node List as the Node-to-branch
	int pos = Lists.all_nodes_list.size() - 1;

	node_to_branch.index = Lists.all_nodes_list[pos].index; //1

	node_to_branch.branching_col_idx = Lists.all_nodes_list[pos].branching_col_idx; //4
	node_to_branch.branching_final_val = Lists.all_nodes_list[pos].branching_final_val; //5

	size_t cols_num = Lists.all_nodes_list[pos].model_matrix.size();
	size_t rows_num = Lists.all_nodes_list[pos].model_matrix[0].size();
	size_t branched_num = Lists.all_nodes_list[pos].branched_vars_list.size();

	// Init model matrix of the Node-to-branch
	for (int col = 0; col < cols_num; col++)
	{
		vector<float> temp_col;
		for (int row = 0; row < rows_num; row++)
		{
			float temp_val = Lists.all_nodes_list[pos].model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		node_to_branch.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their col-idx list of the Node-to-branch
	for (int col = 0; col < branched_num; col++)
	{
		float temp_val = Lists.all_nodes_list[pos].branched_vars_list[col];
		int temp_idx = Lists.all_nodes_list[pos].branched_idx_list[col];

		node_to_branch.branched_vars_list.push_back(temp_val); //7
		node_to_branch.branched_idx_list.push_back(temp_idx); //8
	}
}

void InitNewNode(All_Values&Values, Node& new_node, Node& parent_node)
{

	if (Values.branch_flag == 0) // 1
	{
		new_node.index = parent_node.index + 1;
		printf("\n	NODE_%d is the LEFT branch of NODE_%d	\n",new_node.index, parent_node.index);
	}
	if (Values.branch_flag == 1)
	{
		new_node.index = parent_node.index + 2;
		printf("\n	NODE_%d is the RIGHT branch of NODE_%d	\n", new_node.index, parent_node.index);
	}

	printf("\n##########################################################\n");
	printf("##########################################################\n");
	printf("####################### NEW NODE_%d #######################\n", new_node.index);
	printf("##########################################################\n");
	printf("##########################################################\n\n");

	new_node.parent_index = parent_node.index;	 //2
	new_node.parent_branch_flag = Values.branch_flag; //3

	new_node.branching_col_idx = parent_node.branching_col_idx; //4
	new_node.branching_final_val = parent_node.branching_final_val; //5

	size_t cols_num = parent_node.model_matrix.size();
	size_t rows_num = parent_node.model_matrix[0].size();
	size_t branched_num = parent_node.branched_vars_list.size();

	// Init model matrix of the new Node
	for (int col = 0; col < cols_num; col++)
	{
		vector<float> temp_col;
		for (int row = 0; row < rows_num; row++)
		{
			float temp_val = parent_node.model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		new_node.model_matrix.push_back(temp_col); //6
	}

	// Init branched-vars list and their col-idx list of the new Node 
	for (int col = 0; col < branched_num; col++)
	{
		float temp_val = parent_node.branched_vars_list[col];
		int temp_idx = parent_node.branched_idx_list[col];

		new_node.branched_vars_list.push_back(temp_val); //7
		new_node.branched_idx_list.push_back(temp_idx); //8
	}

	// Clear to init all otther lists
	new_node.all_solns_list.clear(); 
	new_node.fsb_solns_list.clear();
	new_node.fsb_idx_list.clear();
	new_node.int_idx_list.clear();
	new_node.int_solns_list.clear();;

	new_node.dual_prices_list.clear();
	new_node.new_col.clear();
	new_node.new_cols_list.clear();
}



