// 2023-03-17

#include "CSBB.h"
using namespace std;

int BranchAndBoundTree(All_Values& Values, All_Lists& Lists) {
	Values.node_num = 1; // Root Node already generated

	while (1) {

		if (Values.tree_search_flag == 0) {// tree_search_flag set to branch current Parent Node

			Node parent_node;
			int parent_branch_flag = DecideNodeToBranch(Values, Lists, parent_node); // decide the Node to branch

			if (parent_branch_flag == 0) {
				printf("\n\t Branch and Bound stop!\n");
				printf("\n\t Final Optimal Lower Bound = %f\n\n\n", Values.optimal_LB);
				break;
			}

			if (parent_branch_flag == 1) {
				Node sub_left_node;
				Node sub_right_node;

				// always start from the Left Node
				Values.tree_branch_status = 1;
				Values.node_num++;
				GenerateNewNode(Values, Lists, sub_left_node, parent_node); // set the Left Node
				SolveNewNodeProblem(Values, Lists, sub_left_node, parent_node); // solve the Left Node with CG loop
				int left_search_flag = FinishNode(Values, Lists, sub_left_node); // finish the Left Node
				Lists.all_nodes_list.push_back(sub_left_node);

				// Then the Right Node
				Values.tree_branch_status = 2;
				Values.node_num++;
				GenerateNewNode(Values, Lists, sub_right_node, parent_node);  // set the Right Node
				SolveNewNodeProblem(Values, Lists, sub_right_node, parent_node); // solve the Right Node with CG loop
				int right_search_flag = FinishNode(Values, Lists, sub_right_node);  // finish the RightNode
				Lists.all_nodes_list.push_back(sub_right_node);

				Values.root_flag = 0;

				// the var-to-branch val of the Parent Node decide which Node to fathom in next while-iter
				double parent_branch_val = parent_node.var_to_branch_soln;
				if (parent_branch_val > 1) {
					if (sub_left_node.LB < sub_right_node.LB) {  // choose the Node with better LB to fathom
						Values.tree_search_flag = left_search_flag;
						if (Values.tree_search_flag != 1) {
							Values.node_fathom_flag = 1;  // node_fathom_flag set to fathom the Left Node and branch it in next while-iter
							printf("\n\t Left Node_%d LB %.4f < Right Node_%d LB %.4f \n\n	continue to fathom RIGHT Node_%d\n",
								sub_left_node.index, sub_left_node.LB,
								sub_right_node.index, sub_right_node.LB,
								sub_right_node.index);
						}
					}
					else {
						Values.tree_search_flag = right_search_flag;
						if (Values.tree_search_flag != 1) {
							Values.node_fathom_flag = 2; // node_fathom_flag set to fathom the Right Node and branch it in next while-iter
							printf("\n\t Left Node_%d LB %.4f >= Right Node_%d LB %.4f \n\n	continue to fathom RIGHT Node_%d\n",
								sub_left_node.index, sub_left_node.LB,
								sub_right_node.index, sub_right_node.LB,
								sub_right_node.index);
						}
					}
				}
				else {
					Values.tree_search_flag = right_search_flag;
					if (Values.tree_search_flag != 1) {
						Values.node_fathom_flag = 2; // node_fathom_flag set to fathom the Right Nodeand branch it in next while - iter
						printf("\n\t parent branch val = %.4f < 1, \n\n\t Have to fathom Right Node_%d\n",
							parent_branch_val, sub_right_node.index);
					}
				}
				Values.tree_branch_status = 1;  // tree_branch_status set to the Left Node in next while-iter
			}
		}

		if (Values.tree_search_flag == 1) {// tree_search_flag set to find a previously generated Node
			Values.tree_branch_status = 3; // tree_branch_status set to find a previously generated unbranched unpruned Node in Tree
			Values.node_fathom_flag = -1; // better deactivate node_fathom_flag
			Values.tree_search_flag = 0; // tree_search_flag set to continue to the next while-iter
			printf("\n\t Solns of this Node are all INTEGERS! \n");
			printf("\n\t Current Optimal Lower Bound = %f\n", Values.optimal_LB);
		}

		if (Values.node_num > 100) {
			printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
			break;
		}

		// continue while-iter
	}

	return 0;
}