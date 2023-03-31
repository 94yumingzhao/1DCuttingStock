// 2023-03-15

#include "CSBP.h"
using namespace std;

// 分支定价循环
int BranchAndPriceTree(All_Values& Values, All_Lists& Lists) {

	Values.node_num = 1; // 已有根节点

	// 分支定价循环
	while (1) {
		if (Values.tree_search_flag == 0) { // 获得操作指令：当前节点已被剪掉，搜索新的分支节点

			Node parent_node; // 
			int parent_branch_flag = DecideNodeToBranch(Values, Lists, parent_node); // 从节点表中遍历搜索新节点

			if (parent_branch_flag == 0) { // 没有找到优于全局最优下界的节点
				printf("\n\t Branch and Bound stop!\n"); // 分支定价循环结束
				printf("\n\t Final Optimal Lower Bound = %f\n\n\n", Values.optimal_LB); // 最终的全局最优下界
				break;
			}

			if (parent_branch_flag == 1) {  // 找到了一个优于全局最优下界的分支节点

				Node new_left_node;
				Node new_right_node;

				// 先生成分支节点的左支节点
				Values.tree_branch_status = 1;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_left_node, parent_node); // 生成节点
				NewNodeColumnGeneration(Values, Lists, new_left_node, parent_node); // 列生成求解节点
				int left_search_flag = FinishNode(Values, Lists, new_left_node); // 最后处理节点，获得操作指令
				Lists.all_nodes_list.push_back(new_left_node); // 新节点加入节点表

				// 再生成分支节点的右支节点
				Values.tree_branch_status = 2;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_right_node, parent_node);  // 生成节点
				NewNodeColumnGeneration(Values, Lists, new_right_node, parent_node); // 列生成求解节点
				int right_search_flag = FinishNode(Values, Lists, new_right_node);  //最后处理节点，获得操作指令
				Lists.all_nodes_list.push_back(new_right_node); // 新节点加入节点表

				Values.root_flag = 0; // 循环中的所有节点都不是根节点了

				double parent_branch_val = parent_node.var_to_branch_soln;
				if (parent_branch_val > 1) { // 如果分支节点的分支变量对应解值大于1
					if (new_left_node.LB < new_right_node.LB) {  // 如果左支节点下界优于右支节点下界
						Values.tree_search_flag = left_search_flag; // 设置节点操作指令
						if (Values.tree_search_flag != 1) { // 如果指令为继续分支
							Values.node_fathom_flag = 1; // 则继续分支左支节点
							printf("\n\t Left Node_%d LB %.4f < Right Node_%d LB %.4f\n",
								new_left_node.index, new_left_node.LB,new_right_node.index, new_right_node.LB);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
					else {
						Values.tree_search_flag = right_search_flag;  // 设置节点操作指令
						if (Values.tree_search_flag != 1) {  // 如果指令为继续分支
							Values.node_fathom_flag = 2; // 则继续分支右支节点
							printf("\n\t Left Node_%d LB %.4f >= Right Node_%d LB %.4f\n",
								new_left_node.index, new_left_node.LB,new_right_node.index, new_right_node.LB);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
				}
				if (parent_branch_val <= 1) {  // 如果分支节点的分支变量对应解的值小于1
					{
						Values.tree_search_flag = right_search_flag;   // 设置节点操作指令
						if (Values.tree_search_flag != 1) {  // 如果指令为继续分支
							Values.node_fathom_flag = 2; // 则继续分支右支节点
							printf("\n\t parent branch val = %.4f < 1\n\n\t Have to fathom Right Node_%d",parent_branch_val, new_right_node.index);
						}
					}
					Values.tree_branch_status = 1;  // 左支子节点
				}
			}

			if (Values.tree_search_flag == 1) { // 如果指令为放弃分支，重新搜索
				Values.tree_branch_status = 3; // 搜索其他节点
				Values.node_fathom_flag = -1; // 初始化
				Values.tree_search_flag = 0; // 生成操作指令：搜索
				printf("\n\t Solns of this Node are all INTEGERS!\n");
				printf("\n\t Current Optimal Lower Bound = %f\n", Values.optimal_LB);
			}

			if (Values.node_num > 30) {
				printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
				break;
			}
		}

		return 0;
	}