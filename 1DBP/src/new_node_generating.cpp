// 2022-11-17

#include "CSBP.h"
using namespace std;

// 确定分支节点
int DecideNodeToBranch(All_Values& Values, All_Lists& Lists, Node& parent_node) {

	int parent_branch_flag = -1;
	int pos = -1;
	int nodes_num = Lists.all_nodes_list.size();

	if (Values.tree_branch_status == 3) { // 遍历搜索节点表，寻找新被分支节点
		for (int k = 0; k < nodes_num; k++) {
			if (Lists.all_nodes_list[k].node_branched_flag != 1 && Lists.all_nodes_list[k].node_pruned_flag != 1) { // 必须是未分支且未剪掉的节点
				if (Lists.all_nodes_list[k].LB < Values.optimal_LB) { // 新节点下界必须优于当前全局最优下界
					pos = k; // 确定被分支节点在节点表中的位置
					cout << endl;
				}
				else { // 遍历发现的所有下界差于当前全局最优下界的节点都要剪掉
					int temp_idx = Lists.all_nodes_list[k].index;
					printf("\n	Node_%d has to be pruned\n", temp_idx);
					Lists.all_nodes_list[k].node_pruned_flag = 1; // 节点的被剪标签置为 1
				}
			}
		}
	}

	if (Values.tree_branch_status != 3) {  // 处理被分支节点而不去搜索新节点
		if (Values.root_flag == 1) {  // 被分支节点是的根节点
			if (Values.tree_branch_status == 1) { // 当前节点是被分支节点的左支子节点
				pos = nodes_num - 1; // 左支子节点序号 = 节点总数 - 1
			}
			if (Values.tree_branch_status == 2) { // 当前节点是被分支节点的右支子节点
				pos = nodes_num - 2; // 右支子节点序号 = 节点总数 - 2
			}
		}
		if (Values.root_flag != 1) {  // 被分支节点的不是根节点
			if (Values.node_fathom_flag == 1) {  // 被分支节点是其上节点的左支子节点
				if (Values.tree_branch_status == 1) {  // 当前节点是被分支节点的左支子节点
					pos = nodes_num - 2; // 左支子节点序号 = 节点总数 - 2
				}
				if (Values.tree_branch_status == 2) { // 当前节点是被分支节点的右支子节点
					pos = nodes_num - 3;  // 右支子节点序号 = 节点总数 - 3
				}
			}
			if (Values.node_fathom_flag == 2) { // 被分支节点是其上节点的右支子节点
				if (Values.tree_branch_status == 1) {  // 当前节点是被分支节点的左支子节点
					pos = nodes_num - 1; // 左支子节点序号 = 节点总数 - 1
				}
				if (Values.tree_branch_status == 2) { // 当前节点是被分支节点的右支子节点
					pos = nodes_num - 2; // 右支子节点序号 = 节点总数 - 2
				}
			}
		}
	}

	if (pos == -1) { // 遍历搜索节点表没有找到一个符合要求的被分支节点
		parent_branch_flag = 0;
		printf("\n	No Node to branch! \n");
	}
	else { // 找到了符合要求的被分支节点
		parent_branch_flag = 1;
		parent_node = Lists.all_nodes_list[pos]; // 确定被分支节点
		parent_node.node_branched_flag = 1;
		printf("\n	The Node to branch is Node_%d\n", parent_node.index);
	}

	return parent_branch_flag;
}

// 生成新节点
void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node& new_node, Node& parent_node) {
	int nodes_num = Lists.all_nodes_list.size();

	new_node.index = nodes_num + 1;
	new_node.LB = -1;

	if (Values.tree_branch_status == 1) {
		printf("\n	Node_%d is the LEFT branch of Node_%d	\n", new_node.index, parent_node.index);
	}
	if (Values.tree_branch_status == 2) {
		printf("\n	Node_%d is the RIGHT branch of Node_%d	\n", new_node.index, parent_node.index);
	}

	new_node.parent_index = parent_node.index;
	new_node.parent_branching_flag = Values.tree_branch_status;
	new_node.parent_var_to_branch_val = parent_node.var_to_branch_soln;

	printf("\n############################################\n");
	printf("############################################\n");
	printf("################ NEW NODE_%d ################\n", new_node.index);
	printf("############################################\n");
	printf("############################################\n\n");

	// 这些参数不需要继承被分支节点
	new_node.var_to_branch_idx = -1;
	new_node.var_to_branch_soln = -1;
	new_node.var_to_branch_floor = -1;
	new_node.var_to_branch_ceil = -1;
	new_node.var_to_branch_final = -1;

	int all_cols_num = parent_node.model_matrix.size();
	int all_rows_num = parent_node.model_matrix[0].size();
	int branched_num = parent_node.branched_vars_idx_list.size();

	// 当前节点从被分支节点，继承主问题模型系数矩阵
	for (int col = 0; col < all_cols_num; col++) {
		vector<double> temp_col;
		for (int row = 0; row < all_rows_num; row++) {
			double temp_val = parent_node.model_matrix[col][row];
			temp_col.push_back(temp_val);
		}
		new_node.model_matrix.push_back(temp_col); //6
	}

	// 当前节点继承被分支节点的所有已分支变量对应列序号表
	for (int col = 0; col < branched_num; col++) {
		int temp_idx = parent_node.branched_vars_idx_list[col];
		new_node.branched_vars_idx_list.push_back(temp_idx);
	}

	// 当前节点继承被分支节点的分支变量对应解分支后的整数值
	if (Values.tree_branch_status == 1) { // 当前节点是被分支节点的左支子节点
		new_node.var_to_branch_final = parent_node.var_to_branch_floor; // 被分支节点的分支变量对应解，在当前节点向下取整
	}
	if (Values.tree_branch_status == 2) { // 当前节点是被分支节点的左支子节点
		new_node.var_to_branch_final = parent_node.var_to_branch_ceil; // 被分支节点的分支变量对应解，在当前节点向上取整
	}

	double final_int_val = new_node.var_to_branch_final;
	if (branched_num <= 1)  // 被分支节点是根节点
	{
		new_node.branched_vars_int_list.push_back(final_int_val);
	}
	else // 被分支节点不是根节点
	{
		for (int col = 0; col < branched_num - 1; col++) {
			double temp_val = parent_node.branched_vars_int_list[col];
			new_node.branched_vars_int_list.push_back(temp_val);
		}
		new_node.branched_vars_int_list.push_back(final_int_val);
	}

	// 初始化表，清空
	new_node.all_solns_list.clear();
	new_node.dual_prices_list.clear();
	new_node.new_col.clear();

	cout << endl;
}


