// 2022-11-17

#include "CSBP.h"
using namespace std;

// function to 
void InitPrimalMatrix(All_Values& Values, All_Lists& Lists, Node& root_node)
{
	int item_types_num = Values.item_types_num;
	vector<vector<float>> primal_matrix;

	for (int col = 0; col < item_types_num; col++) // cols num == item types num
	{
		vector<float> primal_col;
		for (int row = 0; row < item_types_num; row++) // rows num == item types num
		{
			if (row == col)
			{
				float primal_val = 0;
				primal_val = Values.stock_length / Lists.all_item_types_list[row].length;
				primal_col.push_back(primal_val);
			}
			else
			{
				primal_col.push_back(0);
			}
		}
		primal_matrix.push_back(primal_col);
	}
	root_node.model_matrix = primal_matrix;
}

void InitNewNode(int branch_flag, Node& this_node,Node& parent_node)
{
	if (branch_flag == 0)
	{
		this_node.index = parent_node.index + 1;
	}
	if (branch_flag == 1)
	{
		this_node.index = parent_node.index + 2;
	}

	this_node.parent_index = parent_node.index;
	this_node.branching_col_idx = parent_node.branching_col_idx;
	this_node.parent_branch_flag = branch_flag;

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
		this_node.model_matrix.push_back(temp_col);
	}

	// Init branched-vars list and their col-idx list of the new Node 
	for (int col = 0; col < branched_num; col++)
	{
		float temp_val = parent_node.branched_vars_list[col];
		int temp_idx = parent_node.branched_idx_list[col];

		this_node.branched_vars_list.push_back(temp_val);
		this_node.branched_idx_list.push_back(temp_idx);
	}

	// Clear to init all otther lists
	this_node.all_solns_list.clear();
	this_node.fsb_solns_list.clear();
	this_node.fsb_idx_list.clear();
	this_node.int_idx_list.clear();
	this_node.int_solns_list.clear();;

	this_node.dual_prices_list.clear();
	this_node.new_col.clear();
	this_node.new_cols_list.clear();
}

// function to solve a node with CG loop
void SolveNode(int branch_flag, All_Values& Values, All_Lists& Lists, Node& this_node)
{
	if (this_node.index == 1) // root node
	{
		printf("\n##########################################################\n");
		printf("##########################################################\n");
		printf("####################### NODE_%d ###########################\n", this_node.index);
		printf("##########################################################\n");
		printf("##########################################################\n\n");

		// solve root node with CG loop
		ColumnGenerationRootNode(Values, Lists, this_node);
	}
	if (this_node.index != 1)// other nodes
	{
		Node parent_node = Lists.all_nodes_list.back();

		InitNewNode(branch_flag,this_node, parent_node);

		printf("\n##########################################################\n");
		printf("##########################################################\n");
		printf("####################### NODE_%d ###########################\n", this_node.index);
		printf("##########################################################\n");
		printf("##########################################################\n\n");

		if (branch_flag == 0) // left branch node
		{
			printf("\n	NODE_%d is the LEFT branch of NODE_%d	\n",
				this_node.index,
				this_node.index - 1);
		}

		if (branch_flag == 1) // right branch node
		{
			printf("\n	NODE_%d is the RIGHT branch of NODE_%d	\n",
				this_node.index,
				this_node.index - 2);
		}

		// solve new node with CG loop
		ColumnGenerationNewNode(branch_flag, Values, Lists, this_node,parent_node);
	}
}



