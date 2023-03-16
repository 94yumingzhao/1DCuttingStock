// 2023-03-15

#include "CSBP.h"
using namespace std;

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists)
{
	Values.level_num = 1;
	Values.node_num = 1;

	while (1)
	{
		if (Values.search_flag == 0)
		{
			Values.level_num++;

			Node parent_node;
			Node new_left_node;
			Node new_right_node;

			int left_search_flag = 0;
			int right_search_flag = 0;;
			
			FindNodeToBranch(Values, Lists, parent_node);

			Values.branch_status = 1;
			Values.node_num++;
			GenerateNewNode(Values, Lists, new_left_node,parent_node);
			NewNodeColumnGeneration(Values, Lists, new_left_node, parent_node); // solve left Node with CG loop
			left_search_flag = BranchOrSearch(Values, Lists, new_left_node);

			Values.branch_status = 2;
			Values.node_num++;
			GenerateNewNode(Values, Lists, new_right_node,parent_node);
			NewNodeColumnGeneration(Values, Lists, new_right_node, parent_node); // solve right Node with CG loop
			right_search_flag = BranchOrSearch(Values, Lists, new_right_node);

			double parent_branch_val = new_left_node.parent_var_to_branch_val;

			if(parent_branch_val > 1)
			{
				if (new_left_node.lower_bound < new_right_node.lower_bound)
				{
					Values.search_flag = left_search_flag; // continue to branch and fathom on left Node

					if (Values.search_flag != 1)
					{
						Values.fathom_flag = 1;
						printf("\n	LLB %.4f < RLB %.4f, continue to fathom LEFT Node_%d\n",
							new_left_node.lower_bound, new_right_node.lower_bound, new_left_node.index);
					}
				}
				else
				{
					Values.search_flag = right_search_flag; // continue to branch and fathom on right Node

					if (Values.search_flag != 1)
					{
						Values.fathom_flag = 2;
						printf("\n	Left Node_%d LB %.4f >= Right Node_%d LB %.4f \n\n	continue to fathom RIGHT Node_%d\n",
							new_left_node.index, new_right_node.index,
							new_left_node.lower_bound, new_right_node.lower_bound,
							new_right_node.index);
					}					
				}
			}
			else
			{
				Values.search_flag = right_search_flag; // continue to branch and fathom on right Node

				if (Values.search_flag != 1)
				{
					Values.fathom_flag = 2;
					printf("\n	parent branch val = %.4f < 1, \n\n	Have to fathom Right Node_%d",
						parent_branch_val, new_right_node.index);
				}
			}

			Values.branch_status = 1;
		}

		if (Values.search_flag == 1)
		{
			Values.branch_status = 3;

			Values.fathom_flag = 0;
			Values.search_flag = 0;

			printf("\n	Solns of this Node are all INTEGERS! \n");
		}

		if (Values.node_num > 30)
		{
			printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
			break;
		}
	}

	return 0;
}