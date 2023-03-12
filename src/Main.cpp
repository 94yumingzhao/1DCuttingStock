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
	Values.tree_branching_flag = 0; 

	InitRootNodeMatrix(Values, Lists, root_node); // generate root Node matrix
	SolveOneNode(Values, Lists, root_node); // solve root Node with CG loop	
	Values.tree_continue_flag = BranchandPrice(Values, Lists, root_node); // find the branch var of root Node

	int node_num = 1;
	int level_num = 1;

	// continue to BP
	if (Values.tree_continue_flag == 0)
	{
		printf("\n	//////////// New Level %d //////////// \n", level_num);

		// Branch and Price loop
		while (1)
		{
			level_num++;
			// First solve the left branch Node		
			if (Values.tree_branching_flag == 1)
			{
				node_num++;
				Node new_node; // generate the left branch Node
				SolveOneNode(Values, Lists, new_node); // solve the Node with CG	loop
				Values.tree_continue_flag = BranchandPrice(Values, Lists, new_node); // judge Node integerity and find the branch var			
				if (Values.tree_continue_flag == 1) // all solns are integer in this new Node
				{
					Values.tree_branching_flag = 3; // no need to  branch this Node and switch to another un-branched Node
				}
			}

			// Then solve the right branch Node
			if (Values.tree_branching_flag == 2)
			{
				node_num++;
				Node new_node; // generate the right branch Node
				SolveOneNode(Values, Lists, new_node); // solve the Node with CG loop
				Values.tree_continue_flag = BranchandPrice(Values, Lists, new_node); // judge Node integerity and find the branch var
				if (Values.tree_continue_flag == 1) // all solns are integer in this new Node
				{
					Values.tree_branching_flag = 3; // no need to  branch this Node and switch to another un-branched Node
				} 
			}

			if (Values.tree_branching_flag == 3) // switch to another un-branched Node in BP Tree
			{
				Node avail_node; // generate the right branch Node
				SolveOneNode(Values, Lists, avail_node); // solve the Node with CG loop
				Values.tree_continue_flag = BranchandPrice(Values, Lists, avail_node); // judge Node integerity and find the branch var
				if (Values.tree_continue_flag == 1) // all solns are integer in this new Node
				{
					Values.tree_branching_flag = 3; // no need to  branch this Node and switch to another un-branched Node
				}
			}

			if (node_num > 30)
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
	printf("\n	There are %d Levels and %d Nodes in BP Tree\n", level_num, node_num);

	Lists.all_nodes_list.clear();
	cout << endl;
	return 0;
}

