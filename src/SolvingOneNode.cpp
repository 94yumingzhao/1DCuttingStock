// 2022-11-17

#include "CSBP.h"
using namespace std;

// function to 
void Heuristic(All_Values& Values, All_Lists& Lists,Node& root_node)
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

// function to solve a node with CG loop
void SolveNode(int branch_flag, All_Values& Values, All_Lists& Lists, Node& this_node)
{
	if (this_node.index == 1) // root node
	{	
		// solve root node with CG loop
		ColumnGenerationRootNode(Values, Lists,this_node); 
	}
	else // other nodes
	{
		if (branch_flag == 0) // left branch node
		{
			printf("\n	======NEW NODE======\n");
			printf("\n	NODE_%d is the LEFT branch of NODE_%d	\n",
				this_node.index,
				this_node.index - 1);
		}

		if (branch_flag == 1) // right branch node
		{
			printf("\n	======NEW NODE======\n");
			printf("\n	NODE_%d is the RIGHT branch of NODE_%d	\n",
				this_node.index,
				this_node.index - 2);
		}
		// solve new node with CG loop
		ColumnGenerationNewNode(branch_flag, Values, Lists, this_node);
	}

}



