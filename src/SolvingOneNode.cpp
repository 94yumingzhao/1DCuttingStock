// 2022-11-17

#include "CSBP.h"
using namespace std;

vector<vector<float>> Heuristic(All_Values& Values, All_Lists& Lists,Node root_node)
{
	// 启发式生成初始主问题的系数矩阵
	// 假设一种子管只在一根母管上切

	int ITEM_TYPES_NUM = Values.item_types_num;
	vector<vector<float>> initMatrix;
	for (int col = 0; col < ITEM_TYPES_NUM; col++) // 列的数量是子管种类的数量
	{
		vector<float> primal_column;
		for (int row = 0; row < ITEM_TYPES_NUM; row++) // 行的数量也是子管种类的数量
		{
			if (row == col)
			{
				int primal_value = 0;
				primal_value = Values.stock_length / all_item_types_list[row].length; // 一类子管放在一根母管上切最多能切的数量
				primal_column.push_back(primal_value);
				// printf("col-%d row-%d value-%d\n", col + 1, row + 1, primal_value);
			}
			else
			{
				primal_column.push_back(0);
				// printf("col-%d row-%d value-%d\n", col + 1, row + 1, 0);
			}
		}
		//printf("---------------------");
		initMatrix.push_back(primal_column); // 列加矩阵
	}
	printf("\n");
	return initMatrix;
}

// 求解一个节点的函数
int SolveNode(int branch_flag, All_Values& Values, All_Lists& Lists, Node this_node)
{
	if (this_node.index == 1) // 根节点
	{
		clock_t start, finish;
		start = clock();
		
		printf("\n	Solving root node NODE_%d\n", this_node.index);
		// 列生成求解根节点
		
		ColumnGenerationRootNode(Values, Lists); 

		finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		printf("\n	ColumnGenerationRootNode() = %f seconds\n", duration);

		cout << endl;
	}
	else // 求解新节点
	{

		if (branch_flag == 0)
		{
			printf("\n	======NEW NODE======\n");
			printf("\n	NODE_%d is the LEFT branch of NODE_%d	\n",
				Values.node_index,
				Values.node_index - 1);
		}

		if (branch_flag == 1)
		{
			printf("\n	======NEW NODE======\n");
			printf("\n	NODE_%d is the RIGHT branch of NODE_%d	\n",
				Values.node_index,
				Values.node_index - 2);
		}

		// 列生成求解新节点
		//ColumnGenerationNewNode(branch_flag,Values, Lists); 
	}
	ColumnGenerationNewNode(branch_flag, Values, Lists);

	
	return 0;
}



