// 2023-03-16

#include "CSCG.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists)
{
	// Init CPLEX
	IloEnv Env_MP; // Init environment
	IloModel Model_MP(Env_MP); // Init model
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // Init obj
	IloNumVarArray Vars_MP(Env_MP); // Init vars
	IloRangeArray Cons_MP(Env_MP); // Init cons

	Values.iter = 1; 

	// solve the first MP
	int MP_flag = SolveFirstMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP);

	int SP_flag = -1;
	SP_flag = SolveSubProblem(Values, Lists); // solve the SP of the first MP

	// if the LB of the first MP >= 0, then the 1st MP has feasible solns.
	if (MP_flag == 1)
	{
		// Column Generation loop
		while (1)
		{
			Values.iter++; // CG loop iter idx++

			// Case 1: Better reduced cost is get from SP
			if (SP_flag == 0)
			{
				// continue CG loop and update MP with the new col from SP
				SolveUpdateMasterProblem(
					Values,
					Lists,
					Env_MP,
					Model_MP,
					Obj_MP,
					Cons_MP,
					Vars_MP);
			}
			// Case 2: No better reduced cost is get from SP anymore
			if (SP_flag == 1)
			{
				break; // break CG loop
			}
			
			SP_flag = SolveSubProblem(Values, Lists); // solve the SP of the updated MP
		}
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
