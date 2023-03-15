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

			Node new_left_node;
			Node new_right_node;

			int left_search_flag = 0;
			int right_search_flag = 0;;

			Values.branch_status = 1;
			GenerateNewNode(Values, Lists, new_left_node);
			NewNodeColumnGeneration(Values, Lists, new_left_node); // solve left Node with CG loop
			left_search_flag = BranchOrSearch(Values, Lists, new_left_node);

			Values.branch_status = 2;
			GenerateNewNode(Values, Lists, new_right_node);
			NewNodeColumnGeneration(Values, Lists, new_right_node); // solve right Node with CG loop
			right_search_flag = BranchOrSearch(Values, Lists, new_right_node);

			if (new_left_node.lower_bound < new_right_node.lower_bound)
			{
				Values.search_flag = left_search_flag; // continue to branch and fathom on left Node
				Values.fathom_flag = 1;

				printf("\n	LLB %.4f < RLB %.4f, continue to fathom LEFT Node_%d\n", 
					new_left_node.lower_bound, new_right_node.lower_bound,new_left_node.index);
			}
			else
			{
				Values.search_flag = right_search_flag; // continue to branch and fathom on right Node
				Values.fathom_flag = 2;

				printf("\n	LLB %.4f >= RLB %.4f, continue to fathom RIGHT Node_%d\n",
					new_left_node.lower_bound, new_right_node.lower_bound, new_left_node.index);
			}
			Values.branch_status = 0;
		}

		if (Values.search_flag == 1)
		{
			Values.branch_status = 3;
			printf("\n	Solns of this Node are all INTEGERS! \n");

			Node avail_node;
			GenerateNewNode(Values, Lists, avail_node);
			NewNodeColumnGeneration(Values, Lists, avail_node); // solve avail Node with CG loop
			Values.search_flag = BranchOrSearch(Values, Lists, avail_node);
		}

		if (Values.node_num > 30)
		{
			printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
			break;
		}
	}
	return 0;
}