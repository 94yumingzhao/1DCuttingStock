// 2022-11-17 branch and price
// 2022-11-23 Branch and Price
// 2023-02-23 

// CG -- column generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound

#include "CSBP.h"
using namespace std;

int main()
{
	printf("\n	//////////// START //////////////\n\n");

	clock_t start, finish;
	start = clock();

	All_Lists Lists;
	All_Values Values;

	tuple<int, int, int> fileTxt;
	fileTxt = ReadData(Values, Lists);

	Values.stocks_num = get<0>(fileTxt);// number of all stocks
	Values.item_types_num = Lists.all_item_types_list.size(); // number of item types
	Values.stock_length = get<2>(fileTxt); // length of a stock

	Node root_node; // Init root Node
	root_node.index = 1; // Node index

	InitRootNodeMatrix(Values, Lists, root_node); // generate root Node matrix
	SolveOneNode(Values, Lists, root_node); // solve root Node with CG loop	
	Values.continue_flag = BranchAndPrice(Values, Lists, root_node); // find the branch var of root Node

	int node_num = 1;
	int level_num = 1;

	// continue to BP
	if (Values.continue_flag == 0)
	{
		level_num++;
		printf("\n	//////////// New Level %d //////////// \n", level_num);

		// Branch and Price loop
		while (1)
		{
			// First solve the left branch Node		
			if (Values.branch_flag == 0)
			{
				node_num++;
				Node new_node; // generate the left branch Node
				SolveOneNode(Values, Lists, new_node); // solve the Node with CG	loop
				Values.continue_flag = BranchAndPrice(Values, Lists, new_node); // judge Node integerity and find the branch var

				// Case 1.1:
				// all solns are integer in this new Node
				if (Values.continue_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP 1 //////////////\n");
					break;
				}
			}

			// Then solve the right branch Node
			if (Values.branch_flag == 1)
			{
				node_num++;
				Node new_node; // generate the right branch Node
				SolveOneNode(Values, Lists, new_node); // solve the Node with CG loop
				Values.continue_flag = BranchAndPrice(Values, Lists, new_node); // judge Node integerity and find the branch var

				if (Values.continue_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP 2//////////////\n");
					break;
				}
			}

			if (level_num == 6)
			{
				printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
				break;
			}
		}
	}

	else
	{
		printf("\n	//////////// PROCEDURE STOP 4 //////////////\n");
	}

	finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\n	Process Time = %f seconds\n", duration);
	printf("\n	There are %d Levels and %d Nodes in the TREE\n", level_num, node_num);

	cout << endl;
	return 0;
}

