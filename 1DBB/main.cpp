// 2023-03-17

#include "CSBB.h"
using namespace std;

int main()
{
	All_Lists Lists;
	All_Values Values;

	tuple<int, int, int> fileTxt;
	fileTxt = ReadData(Values, Lists);

	Values.stocks_num = get<0>(fileTxt);// number of all stocks
	Values.item_types_num = Lists.all_item_types_list.size(); // number of item types
	Values.stock_length = get<2>(fileTxt); // item_type_length of a stock

	Node root_node; // Init Root Node
	root_node.index = 1; // Node index
	Values.tree_branch_status = 0;

	InitModelMatrix(Values, Lists, root_node); // generate Root Node matrix
	SolveRootNodeProblem(Values, Lists, root_node);
	Values.tree_search_flag = FinishNode(Values, Lists, root_node); // find the branch var of Root Node
	Lists.all_nodes_list.push_back(root_node);
	Values.root_flag = 1;

	// continue to BP
	if (Values.tree_search_flag == 0)
	{
		Values.tree_branch_status = 1;
		BranchAndBoundTree(Values, Lists); // Branch and Price loop
	}

	Lists.all_nodes_list.clear();

	cout << endl;


}