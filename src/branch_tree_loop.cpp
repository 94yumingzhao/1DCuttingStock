// 2023-03-15

#include "CSBP.h"
using namespace std;

int BranchTree(All_Values& Values, All_Lists& Lists)
{
	Values.level_num = 1;
	Values.node_num = 1;

	while (1)
	{
		Values.level_num++;

		if (Values.tree_branching_status == 1) // left Node		
		{
			Values.node_num++;
			Node new_node; // generate the left Node
			SolveOneNode(Values, Lists, new_node); // solve the Node with CG	loop (Pricing)
			Values.tree_continue_flag = BranchOrSwitch(Values, Lists, new_node); // branch this Node or switch to another Node
			
			if (Values.tree_continue_flag == 1) // if all solns are integer in this Node
			{
				Values.tree_branching_status = 3; // no need to branch this Node 

				// 1. switch to another unbranched Node in tree
			}
			else
			{
				Values.tree_branching_status = 2;

				// 2. continue to sibling right Node
			}
		}

		if (Values.tree_branching_status == 2) 	// sibling right Node
		{
			Values.node_num++;
			Node new_node; // generate the right Node
			SolveOneNode(Values, Lists, new_node); // solve the Node with CG loop  (Pricing)
			Values.tree_continue_flag = BranchOrSwitch(Values, Lists, new_node); // branch this Node or switch to another Node
			
			if (Values.tree_continue_flag == 1) // if all solns are integer in this Node
			{
				Values.tree_branching_status = 3; // no need to branch

				// 3. switch to an unbranched Node in tree
			}
			else
			{
				Values.tree_branching_status = 1;

				// 4. branch and continue to sub left Node
			}
		}

		if (Values.tree_branching_status == 3) // a previous unbranched Node in BP Tree
		{
			Node avail_node; // switch to a previous unbranched Node
			SolveOneNode(Values, Lists, avail_node); // solve the Node with CG loop  (Pricing)
			Values.tree_continue_flag = BranchOrSwitch(Values, Lists, avail_node); // branch this Node or switch to another Node
			
			if (Values.tree_continue_flag == 1) // if all solns are integer in this Node
			{
				Values.tree_branching_status = 3; // no need to branch

				// 5. switch to an unbranched Node in tree
			}
			else
			{
				Values.tree_branching_status = 1;

				// 6. continue to sub left Node
			}
		}

		if (Values.node_num > 30)
		{
			printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
			break;
		}
	}
	return 0;
}