// 2023-03-15

#include "CSBP.h"
using namespace std;

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists) {

	Values.node_num = 1; // 已有根节点

	// 分支定价循环
	while (1) {
		if (Values.tree_search_flag == 0) { // 当前节点已被剪掉，搜索已生成的未分支未剪节点继续分支

			Node parent_node; // 
			int parent_branch_flag = ChooseNodeToBranch(Values, Lists, parent_node); // 从节点表中遍历节点

			if (parent_branch_flag == 0) { // 当前没有任何已生成节点的下界优于全局最优下界
				printf("\n\t Branch and Bound stop!\n"); // 分支定价循环结束
				printf("\n\t Final Optimal Lower Bound = %f\n\n\n", Values.tree_optimal_lower_bound); // 全局最优下界
				break;
			}

			if (parent_branch_flag == 1) {  // 找到了一个优于全局最优下界的节点

				Node new_left_node;
				Node new_right_node;

				// 先生成左支子节点
				Values.tree_branch_status = 1;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_left_node, parent_node); // 生成节点
				NewNodeColumnGeneration(Values, Lists, new_left_node, parent_node); // 列生成求解节点
				int left_search_flag = FinishNode(Values, Lists, new_left_node); // 最后处理节点
				Lists.all_nodes_list.push_back(new_left_node); // 新节点加入节点表

				// 再生成右支子节点
				Values.tree_branch_status = 2;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_right_node, parent_node);  // 生成节点
				NewNodeColumnGeneration(Values, Lists, new_right_node, parent_node); // 列生成求解节点
				int right_search_flag = FinishNode(Values, Lists, new_right_node);  //最后处理节点
				Lists.all_nodes_list.push_back(new_right_node); // 新节点加入节点表

				Values.root_flag = 0; // 循环中的所有节点都不是根节点了


				double parent_branch_val = parent_node.var_to_branch_soln_val;
				if (parent_branch_val > 1) { // 如果当前节点分支变量对应解的值大于1
					if (new_left_node.node_lower_bound < new_right_node.node_lower_bound) {  // 如果左支子节点下界 优于 右支子节点下界
						Values.tree_search_flag = left_search_flag;// 当前节点操作决策
						if (Values.tree_search_flag != 1) { // 继续分支子节点，而不去搜索其他节点
							Values.node_fathom_flag = 1; //  子节点选择决策
							printf("\n\t Left Node_%d LB %.4f < Right Node_%d LB %.4f\n",new_left_node.index, new_left_node.node_lower_bound,new_right_node.index, new_right_node.node_lower_bound);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
					else {
						Values.tree_search_flag = right_search_flag;  // 当前节点操作决策
						if (Values.tree_search_flag != 1) {  // 继续分支子节点，而不去搜索其他节点
							Values.node_fathom_flag = 2; // 子节点分支决策
							printf("\n\t Left Node_%d LB %.4f >= Right Node_%d LB %.4f\n",new_left_node.index, new_left_node.node_lower_bound,new_right_node.index, new_right_node.node_lower_bound);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
				}
				if (parent_branch_val <= 1) {  // 如果分支变量对应解的值小于1
					{
						Values.tree_search_flag = right_search_flag;  // 当前节点操作决策
						if (Values.tree_search_flag != 1) {  // 继续分支子节点，而不去搜索其他节点
							Values.node_fathom_flag = 2; // 子节点分支决策
							printf("\n\t parent branch val = %.4f < 1\n\n\t Have to fathom Right Node_%d",parent_branch_val, new_right_node.index);
						}
					}
					Values.tree_branch_status = 1;  // 左支子节点
				}
			}

			if (Values.tree_search_flag == 1) { //  剪掉当前节点，搜索其他节点
				Values.tree_branch_status = 3; // 搜索其他节点
				Values.node_fathom_flag = -1; // 不需要子节点分支决策，初始化
				Values.tree_search_flag = 0; // 搜索其他节点，
				printf("\n\t Solns of this Node are all INTEGERS!\n");
				printf("\n\t Current Optimal Lower Bound = %f\n", Values.tree_optimal_lower_bound);
			}

			if (Values.node_num > 30) {
				printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
				break;
			}
		}

		return 0;
	}