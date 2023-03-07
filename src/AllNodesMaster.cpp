// 2022-11-17
#include "CSBP.h"
using namespace std;

int SolveUpdateMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	Node& this_node)
{
	int item_types_num = Values.item_types_num;

	// add new col to the matrix of MP
	this_node.model_matrix.push_back(this_node.new_col);

	// set the obj coeff of the new col
	int obj_coeff = 1;
	IloNumColumn CplexCol = Obj_MP(obj_coeff);

	// add the new col ro the model of MP 
	for (int row = 0; row < item_types_num; row++)
	{
		CplexCol += Cons_MP[row](this_node.new_col[row]);
	}

	// var >= 0
	float var_min = 0; 
	float var_max = IloInfinity; 

	IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT); // float, not int
	Vars_MP.add(Var);
	CplexCol.end();
	
	// solve the new updated MP
	printf("\n	Continue to solve the new MP-%d", this_node.iter + 1);
	printf("\n\n####################### MP-%d CPLEX SOLVING START #######################\n", this_node.iter+1);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("updateMasterProblem.lp");
	int feasible_flag = MP_cplex.solve(); // solve MP
	printf("####################### MP-%d CPLEX SOLVING END #########################\n", this_node.iter+1);
	printf("\n	The OBJ of update MP-%d is %f\n\n", this_node.iter+1,MP_cplex.getValue(Obj_MP));

	// print fsb-solns of the updated MP
	int solns_num = this_node.model_matrix.size();
	for (int col = 0; col < solns_num; col++)
	{
		float soln_val = MP_cplex.getValue(Vars_MP[col]);
		if (soln_val != 0)
		{
			printf("	var_x_%d = %f\n", col + 1, soln_val);
		}
	}

	// print and store dual-prices of MP cons
	this_node.dual_prices_list.clear(); 
	printf("\n	DUAL PRICES: \n\n");

	for (int row = 0; row < item_types_num; row++)
	{
		float dual_val = MP_cplex.getDual(Cons_MP[row]); // get dual-prices of all cons
		printf("	dual_r_%d = %f\n", row + 1, dual_val);
		this_node.dual_prices_list.push_back(dual_val);
	}
	return feasible_flag;
}

int SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP,
	Node& this_node)
{
	int item_types_num = Values.item_types_num;

	// solve the final MP of this Node
	printf("\n	Continue to solve Final MP");
	printf("\n\n####################### MP-%d CPLEX SOLVING START #######################\n", this_node.iter);
	IloCplex MP_cplex(Model_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("FinalMasterProblem.lp");
	int feasible_flag = MP_cplex.solve(); // 求解当前主问题
	printf("####################### MP-%d CPLEX SOLVING END #########################\n", this_node.iter);

	printf("\n	The OBJ of Final-MP is %f\n\n" ,  MP_cplex.getValue(Obj_MP));

	// store Node fsb-solns (i.e. non-zero solns)  and their col-index 
	int solns_num = this_node.model_matrix.size();
	for (int col = 0; col < solns_num; col++)
	{
		float soln_val = MP_cplex.getValue(Vars_MP[col]);
		if (soln_val != 0)
		{
			printf("	var_x_%d = %f\n", col + 1, soln_val);
			this_node.fsb_solns_list.push_back(soln_val);
			this_node.fsb_cols_list.push_back(col);

			// And store int-solns and their col-index among these fsb-solns
			int soln_int_val = int(soln_val);
			if (soln_int_val == soln_val)
			{
				this_node.int_solns_list.push_back(soln_val);
				this_node.int_cols_list.push_back(col);
			}
		}	
	}

	this_node.lower_bound = MP_cplex.getValue(Obj_MP);

	/*for (int col = 0; col < node_final_cols_num; col++)
	{
		printf("\n	Column %d\n",col+1);
		for (int row = 0; row < item_types_num; row++)
		{
			printf("	Column %d Row %d coeff = %f \n",col+1,col+1, Lists.model_matrix[col][row]);
		}
	}*/

	return feasible_flag;
}






