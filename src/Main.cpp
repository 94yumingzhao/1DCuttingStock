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
	Values.tree_branching_status = 0; 

	InitRootNodeMatrix(Values, Lists, root_node); // generate root Node matrix
	SolveOneNode(Values, Lists, root_node); // solve root Node with CG loop	
	Values.tree_continue_flag = BranchOrSwitch(Values, Lists, root_node); // find the branch var of root Node

	int node_num = 1;
	int level_num = 1;

	// continue to BP
	if (Values.tree_continue_flag == 0)
	{
		printf("\n	//////////// New Level %d //////////// \n", level_num);

		// Branch and Price loop
		BranchTree(Values, Lists);
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

