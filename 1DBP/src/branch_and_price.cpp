// 2023-03-15

#include "CSBP.h"
using namespace std;

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists) {

	Values.node_num = 1; // ���и��ڵ�

	// ��֧����ѭ��
	while (1) {
		if (Values.tree_search_flag == 0) { // ��ǰ�ڵ��ѱ����������������ɵ�δ��֧δ���ڵ������֧

			Node parent_node; // 
			int parent_branch_flag = ChooseNodeToBranch(Values, Lists, parent_node); // �ӽڵ���б����ڵ�

			if (parent_branch_flag == 0) { // ��ǰû���κ������ɽڵ���½�����ȫ�������½�
				printf("\n\t Branch and Bound stop!\n"); // ��֧����ѭ������
				printf("\n\t Final Optimal Lower Bound = %f\n\n\n", Values.tree_optimal_lower_bound); // ȫ�������½�
				break;
			}

			if (parent_branch_flag == 1) {  // �ҵ���һ������ȫ�������½�Ľڵ�

				Node new_left_node;
				Node new_right_node;

				// ��������֧�ӽڵ�
				Values.tree_branch_status = 1;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_left_node, parent_node); // ���ɽڵ�
				NewNodeColumnGeneration(Values, Lists, new_left_node, parent_node); // ���������ڵ�
				int left_search_flag = FinishNode(Values, Lists, new_left_node); // �����ڵ�
				Lists.all_nodes_list.push_back(new_left_node); // �½ڵ����ڵ��

				// ��������֧�ӽڵ�
				Values.tree_branch_status = 2;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_right_node, parent_node);  // ���ɽڵ�
				NewNodeColumnGeneration(Values, Lists, new_right_node, parent_node); // ���������ڵ�
				int right_search_flag = FinishNode(Values, Lists, new_right_node);  //�����ڵ�
				Lists.all_nodes_list.push_back(new_right_node); // �½ڵ����ڵ��

				Values.root_flag = 0; // ѭ���е����нڵ㶼���Ǹ��ڵ���


				double parent_branch_val = parent_node.var_to_branch_soln_val;
				if (parent_branch_val > 1) { // �����ǰ�ڵ��֧������Ӧ���ֵ����1
					if (new_left_node.node_lower_bound < new_right_node.node_lower_bound) {  // �����֧�ӽڵ��½� ���� ��֧�ӽڵ��½�
						Values.tree_search_flag = left_search_flag;// ��ǰ�ڵ��������
						if (Values.tree_search_flag != 1) { // ������֧�ӽڵ㣬����ȥ���������ڵ�
							Values.node_fathom_flag = 1; //  �ӽڵ�ѡ�����
							printf("\n\t Left Node_%d LB %.4f < Right Node_%d LB %.4f\n",new_left_node.index, new_left_node.node_lower_bound,new_right_node.index, new_right_node.node_lower_bound);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
					else {
						Values.tree_search_flag = right_search_flag;  // ��ǰ�ڵ��������
						if (Values.tree_search_flag != 1) {  // ������֧�ӽڵ㣬����ȥ���������ڵ�
							Values.node_fathom_flag = 2; // �ӽڵ��֧����
							printf("\n\t Left Node_%d LB %.4f >= Right Node_%d LB %.4f\n",new_left_node.index, new_left_node.node_lower_bound,new_right_node.index, new_right_node.node_lower_bound);
							printf("\n\t continue to fathom RIGHT Node_%d\n",new_right_node.index);
						}
					}
				}
				if (parent_branch_val <= 1) {  // �����֧������Ӧ���ֵС��1
					{
						Values.tree_search_flag = right_search_flag;  // ��ǰ�ڵ��������
						if (Values.tree_search_flag != 1) {  // ������֧�ӽڵ㣬����ȥ���������ڵ�
							Values.node_fathom_flag = 2; // �ӽڵ��֧����
							printf("\n\t parent branch val = %.4f < 1\n\n\t Have to fathom Right Node_%d",parent_branch_val, new_right_node.index);
						}
					}
					Values.tree_branch_status = 1;  // ��֧�ӽڵ�
				}
			}

			if (Values.tree_search_flag == 1) { //  ������ǰ�ڵ㣬���������ڵ�
				Values.tree_branch_status = 3; // ���������ڵ�
				Values.node_fathom_flag = -1; // ����Ҫ�ӽڵ��֧���ߣ���ʼ��
				Values.tree_search_flag = 0; // ���������ڵ㣬
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