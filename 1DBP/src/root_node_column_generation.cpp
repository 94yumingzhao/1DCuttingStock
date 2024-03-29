﻿// 2023-03-01

#include "CSBP.h"
using namespace std;

// 根节点的列生成循环
void RootNodeColumnGeneration(All_Values& Values, All_Lists& Lists, Node& root_node) {
	printf("\n############################################\n");
	printf("############################################\n");
	printf("################ ROOT Node ###################\n");
	printf("############################################\n");
	printf("############################################\n\n");

	// Init CPLEX
	IloEnv Env_MP; // Init environment
	IloModel Model_MP(Env_MP); // Init model
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init obj
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons

	root_node.iter = 0; // Root node index == 0

	// solve the first MP of the Root node 
	int MP_flag = SolveRootNodeFirstMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP,
		root_node);

	// if the LB of the first MP >= 0, then the 1st MP has feasible solns.
	if (MP_flag == 1) {
		// Column Generation loop
		while (1) {
			root_node.iter++; // CG loop iter index++

			if (root_node.iter == 100) {
				cout << endl;
			}

			int SP_flag = SolveSubProblem(Values, Lists, root_node); // solve the SP of MP

			// Case 1:
			// No better reduced cost is get from SP anymore
			if (SP_flag == 1) {
				break; // break CG loop
			}
			// Case 2:
			// Better reduced cost is get from SP
			if (SP_flag == 0) {
				// continue CG loop and update MP with the new col from SP
				// solve the new updated MP
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP,
					root_node);
			}
		}

		// solve the last MP to get optimal int-solns and optimal lower bound of the Node
		SolveFinalMasterProblem(
			Values,
			Lists,
			Env_MP,
			Model_MP,
			Obj_MP,
			Cons_MP,
			Vars_MP,
			root_node);
	}

	// clear all CPLEX objects to release memory. 
	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Vars_MP.clear();
	Vars_MP.end();
	Cons_MP.clear();
	Cons_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();
}
