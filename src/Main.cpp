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

	int branch_flag = 2; // flag of branching, 0 -- left , 1 -- right, 2 -- root 
	//int node_solve_flag; // if there is feasible solns in a Node, 0 -- yes , 1 -- no 
	int continue_flag; //  if there is non-int-solns in a Node, 0 -- yes, 1 -- no

	// generate root Node matrix
	InitPrimalMatrix(Values, Lists, root_node);

	// solve root Node with CG loop
	SolveNode(branch_flag, Values, Lists, root_node);

	// find the branch var of root Node
	continue_flag = BranchAndPrice(branch_flag,Values, Lists, root_node);

	printf("\n////////////////////////// BRANCHING ////////////////////////////\n");

	// continue to BP
	if (continue_flag == 0)
	{
		printf("\n//////////////BRANCHING PROCEDURE START //////////////\n");

		int cnt = 0;

		// Branch and Price loop
		while (1)
		{
			// Then solve the right branch Node
			branch_flag = 1; // RIGHT
			if (branch_flag == 1)
			{
				Node new_node; // generate the right branch Node
				SolveNode(branch_flag, Values, Lists, new_node); // solve the Node with CG loop
				continue_flag = BranchAndPrice(branch_flag,Values, Lists, new_node); // judge Node integerity and find the branch var

				if (continue_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP 2//////////////\n");
					break;
				}
			}

			// First solve the left branch Node
			branch_flag = 0; // LEFT			
			if (branch_flag == 0)
			{
				Node new_node; // generate the left branch Node
				SolveNode(branch_flag, Values, Lists, new_node); // solve the Node with CG	loop
				continue_flag = BranchAndPrice(branch_flag,Values, Lists, new_node); // judge Node integerity and find the branch var

				// Case 1.1:
				// all solns are integer in this new Node
				if (continue_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP 1 //////////////\n");
					break;
				}
			}

			cnt++;
			if (cnt == 6)
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

	cout << endl;
	return 0;
}

