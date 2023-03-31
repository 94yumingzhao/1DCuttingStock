// 2022-11-17
#include "CSBP.h"
using namespace std;

// 求解完节点后，确定节点的分支变量等信息，并确定接下来在分支树上的的操作
int FinishNode(All_Values& Values, All_Lists& Lists, Node& this_node) {

	int node_int_flag = -1; // 判断是否当前节点所有可行解都是整数，0 -- 否，1- - 是
	int tree_search_flag = -1; // 返回操作指令到函数BranchAndPriceTree()，0 -- 继续分支当前节点，1 -- 搜索新节点

	node_int_flag = DecideVarToBranch(Values, Lists, this_node); // 确定当前节点的分支变量

	if (this_node.node_pruned_flag == 1) { // 当前节点已经被剪掉
		tree_search_flag = 1; 
	}
	if (this_node.node_pruned_flag != 1) { // 当前节点未被剪掉

		if (node_int_flag == 0) { // 当前节点的可行解不都是整数
			int var_idx = this_node.var_to_branch_idx; 
			double var_val = this_node.var_to_branch_soln;
			this_node.branched_vars_idx_list.push_back(var_idx); // 分支变量对应列序号加入表
			this_node.branched_vars_soln_list.push_back(var_val); // 分支变量对应解值加入表
			tree_search_flag = 0; // 返回操作指令：继续分支当前节点
		}

		if (node_int_flag == 1) {  // 当前节点的所有可行解都是整数
			if (this_node.index == 1) { // 如果当前节点是根节点
				Values.optimal_LB = this_node.LB; // 更新全局最优下界 = 当前节点下界
				printf("\n	Current Optimal Lower Bound = %f\n", Values.optimal_LB);
			}
			if (this_node.index != 1) { // 当前节点不是根节点
				if (Values.optimal_LB == -1) { // 如果当前节点是第一个所有可行解都是整数的节点
					Values.optimal_LB = this_node.LB; // 更新全局最优下界 = 当前节点下界
					printf("\n	Current Optimal Lower Bound = %f\n", Values.optimal_LB);
				}
				if (Values.optimal_LB != -1) { // 当前节点不是第一个所有可行解都是整数的节点
					if (this_node.LB < Values.optimal_LB) { // 如果当前节点下界优于全局最优下界
						Values.optimal_LB = this_node.LB; // 更新全局最优下界 = 当前节点下界
						printf("\n	Current Optimal Lower Bound = %f\n", Values.optimal_LB);
					}
					if (this_node.LB >= Values.optimal_LB) {  // 如果当前节点下界差于全局最优下界
						this_node.node_pruned_flag = 1; // 将当前节点剪掉
						printf("\n	Node_%d has to be pruned\n", this_node.index);
					}
				}
			}
			tree_search_flag = 1;  // 返回操作指令：当前节点已经剪掉，搜索新节点用于分支
		}
	}
	return tree_search_flag; 
}

// 确定节点的分支变量
int DecideVarToBranch(All_Values& Values, All_Lists& Lists, Node& this_node) {

	int node_int_flag = 1; // 返回判断结果到函数FinishNode()，是否当前节点所有可行解都是整数，0 -- 否，1- - 是

    // 遍历搜索当前节点的所有解，寻找用来继续分支的变量
	int all_solns_num = this_node.all_solns_list.size();
	for (int col = 0; col < all_solns_num; col++) {
		double soln_val = this_node.all_solns_list[col];

		if (soln_val > 0) { // 解值大于0即为可行解
			int soln_int_val = int(soln_val); // 判断当前可行解是否为整数

			if (soln_int_val != soln_val) { //如果不是整数，则这个解对应的决策变量即为当前节点的分支变量
				node_int_flag = 0; // 当前节点可行解不都是整数
				this_node.var_to_branch_idx = col; // 分支变量对应列序号
				this_node.var_to_branch_soln = soln_val; //  分支变量对应解值
				this_node.var_to_branch_floor = floor(soln_val); // 分支变量对应解值向下取整的值
				this_node.var_to_branch_ceil = ceil(soln_val); // 分支变量对应解值向上取整的值		
				printf("\n	Node_%d var_x_%d = %f is NOT an integer\n", this_node.index, col + 1, soln_val);

				break; // 停止遍历
			}
		}
	}
	return node_int_flag;
}



