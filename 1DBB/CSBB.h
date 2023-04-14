// 2023-03-17
// 
// CG -- col generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound
//
// var -- variable
// con -- constraint
// para -- parameter
// col -- col
//
// fsb -- feasible
// int -- int-
// soln -- solution
// val -- val

#include<vector>
#include<queue>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <array>
#include <algorithm>
#include <stdio.h>
#include <ilcplex/ilocplex.h>

using namespace std;

#define RC_EPS 1.0e-6 // a num that is very close to 0

// item type�ṹ��
struct Item_Type_Stc {
	int item_type = -1;
	int length = -1;
	int demand = -1;
};

// stock type�ṹ��
struct Stock_Type_Stc {
	int item_type = -1;
	int count = -1;
};

// item
struct Item_Stc {
	int item_type = -1;
	int demand = -1;
	int length = -1;
	int index = -1;
	int stock_index = -1;
	int occupied = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;
};

// stock
struct Stock_Stc {

	int item_type = -1;
	int pattern = -1;
	int length = -1;
	int index = -1;

	int cutting_distance = -1;
	int material_cutting_loss = -1;

	int wasted_area = -1;
	int material_area_loss = -1;
};

// Node �ڵ�ṹ��
struct Node {

	int index = -1;
	int parent_index = -1;
	int parent_branching_flag = -1;
	int node_branched_flag = -1; // flag: �жϵ�ǰ�ڵ����Ͻڵ����֧������֧, 1 -- left, 2 -- right
	int node_pruned_flag = -1; // flag: �жϵ�ǰ�ڵ��Ƿ��ѱ�����. 1 -- pruned, 0 -- not pruned

	double LB = -1; // �ڵ������½�
	double parent_var_to_branch_val = -1; // ��ǰ�ڵ���Ͻڵ�ķ�֧����ֵ
	int var_to_branch_idx = -1; // ��ǰ�ڵ�Ĵ���֧��������Ӧ�������
	double var_to_branch_soln = -1; // ��ǰ�ڵ�Ĵ���֧��������Ӧ���ֵ
	double var_to_branch_floor = -1; // ��ǰ�ڵ�Ĵ���֧��������Ӧ��������֧���ֵ
	double var_to_branch_ceil = -1; // ��ǰ�ڵ�Ĵ���֧��������Ӧ��������֧���ֵ
	double var_to_branch_final = -1; // var-to-branch's final int-val (floored or ceiled)

	// Lists of final branching of one Node
	vector<double> all_solns_list; //  ��ǰ�ڵ����н�ı�
	vector<int> branched_vars_idx_list; // ��ǰ�ڵ������ѷ�֧��������Ӧ����ŵı�
	vector<double> branched_vars_soln_list; // ��ǰ�ڵ������ѷ�֧��������Ӧ���ֵ�ı�
	vector<double> branched_vars_int_list; // ��ǰ�ڵ������ѷ�֧��������Ӧ���֧�������ֵ�ı�


	// Lists of one Column Generation iter of one Node
	int iter = -1;
	vector<vector<double>> model_matrix; // ��ǰ�ڵ�ÿһ��������ѭ����ϵ������
	vector<double> dual_prices_list; // ��ǰ�ڵ�ÿһ��������ѭ����Լ���Ķ�ż�����ı�
	vector<double> new_col; //  ��ǰ�ڵ�ÿһ��������ѭ�������ɵ�����
};

struct All_Values {

	int stocks_num = -1; // ���п��� stock ������
	int item_types_num = -1; // ���� item type ������
	int stock_length = -1; // stock �ĳ���
	//int level_num; // number of all node-branch structurew
	//int node_num; // number of all Nodes

	double optimal_LB = -1; // ��ǰ������֧�������ϵ������½�

	/*��������
    0 -- ��֧���ӵ�ǰ�ڵ������֧
    1 --  ������������ǰ�ڵ㣬����֮ǰ���ɹ���δ��֧Ҳδ������ĳ���ڵ㣬���¼�����֧����*/
	int tree_search_flag = -1;

	 /*��������
	 1 -- ��֧���ӵ�ǰ�ڵ����ɲ������֧�ӽڵ�
	 2 -- ��֧���ӵ�ǰ�ڵ����ɲ������֧�ӽڵ�
	 3 -- ������������ǰ�ڵ㣬����֮ǰ���ɹ���δ��֧Ҳδ������ĳ���ڵ㣬���¼�����֧����*/
	int tree_branch_status = -1;

	 /*ѡ֧������
	 1 -- ��֧������֧�ڵ������֧
	 2 -- ��֧������֧�ڵ������֧*/
	int node_fathom_flag = -1;

	// int integerity_nodes_num = -1;
	int root_flag = -1;
};

struct All_Lists {
	vector<Item_Stc> all_items_list; // ���� item �ı�
	vector<Item_Type_Stc> all_item_types_list; // ���� item type �ı�
	vector<Node> all_nodes_list; // ��ǰ���ɵ����нڵ�ı�
};

// ��ȡtxt����ַ���
void SplitString(const string& s, vector<string>& v, const string& c); 
// ��ȡԭʼ����
tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists);
// ����ʽ���ɸ��ڵ������ϵ������
void InitModelMatrix(All_Values& Values, All_Lists& Lists, Node& root_node);
// �����ڵ�
bool SolveRootNodeProblem(All_Values& Values, All_Lists& Lists, Node& root_node);
// ���սڵ�����в���
int FinishNode(All_Values& Values, All_Lists& Lists, Node& this_node);
// ѡ��ǰ�ڵ�ķ�֧����
int DecideVarToBranch(All_Values& Values, All_Lists& Lists, Node& this_node);
// ��֧������
int BranchAndBoundTree(All_Values& Values, All_Lists& Lists);
// ѡ��
int DecideNodeToBranch(All_Values& Values, All_Lists& Lists, Node& parent_node);
// 
void GenerateNewNode(All_Values& Values, All_Lists& Lists, Node& new_node, Node& parent_node);
// 
bool SolveNewNodeProblem(All_Values& Values, All_Lists& Lists, Node& this_node, Node& parent_node);


















