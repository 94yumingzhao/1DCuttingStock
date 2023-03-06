// 2022-11-17 branch and price
// 2022-11-23 实现基本的Branch and Price
// 2023-02-23 恢复

// CG -- column generation
// MP -- master  problem
// SP  -- sub problem
// LB  -- lower bound
// UB -- upper bound
// PN -- parent node
// CN -- children node

#include "CSBP.h"
using namespace std;

int main()
{
	clock_t start, finish;
	start = clock();

	All_Lists Lists;
	All_Values Values;

	tuple<int, int, int> fileTxt;
	fileTxt = ReadData(Values, Lists);

	Values.stocks_num = get<0>(fileTxt);// number of all stocks
	Values.item_types_num = Lists.all_item_types_list.size(); // number of item types
	Values.stock_length = get<2>(fileTxt); // length of a stock

	Node root_node; // Init root node
	root_node.all_cols_list = Heuristic(Values, Lists); // primal heuristic to build the first model
	root_node.index = 1; // node index

	int branch_flag = 2; // flag of branching, 0 -- left , 1 -- right, 2 -- root 
	int node_solve_flag; // if there is feasible solns in a node, 0 -- yes , 1 -- no 
	int integerity_flag; //  if there is non-int-solns in a node, 0 -- yes, 1 -- no

	SolveNode(branch_flag, Values, Lists,root_node); // solve the root node with CG loop
	integerity_flag = BranchAndPrice(Values, Lists,root_node); // find the non-int branch var

	printf("\n	//////////// BRANCHING //////////////\n");

	// Case 0:
	// If non-int solns exist in the root node after CG loop
	if (integerity_flag == 0) 
	{
		printf("\n	//////////// BRANCHING PROCEDURE START //////////////\n");

		int cnt = 0;

		// Branch and Price loop
		while (1)
		{
			// Case 1:
			// First calcu left branch node
			branch_flag = 0; // LEFT			
			if (branch_flag == 0)
			{
				Node this_node;
				SolveNode(branch_flag, Values, Lists,this_node); // solve the left branch node with CG	loop
				integerity_flag = BranchAndPrice(Values, Lists,this_node); // judge integerity and find the branch var

				// Case 1.1:
				// all solns are integer in this new node
				if (integerity_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP //////////////\n");
					break;
				}
			}

			// Case 2:
			// Then calcu right branch node
			branch_flag = 1; // RIGHT
			if (branch_flag == 1)
			{
				Node this_node;
				node_solve_flag = SolveNode(branch_flag, Values, Lists,this_node); // solve the right branch node with CG loop
				integerity_flag = BranchAndPrice(Values, Lists,this_node); // judge integerity and find the branch var

				if (integerity_flag == 1)
				{
					printf("\n	//////////// PROCEDURE STOP //////////////\n");
					break;
				}
			}

			cnt++;
			if (cnt == 4)
			{
				printf("\n	//////////// PROCEDURE STOP at 4 Branched Nodes//////////////\n");		
				break;
			}				
		}
	}

	finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\n	Process Time = %f seconds\n", duration);

	return 0;
}

