// 2022-11-17
#include "CSBP.h"
using namespace std;


bool SolveUpdateMasterProblem(
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
	IloNum var_min = 0;
	IloNum var_max = IloInfinity;

	IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT); // float, not int
	Vars_MP.add(Var);
	CplexCol.end();
	

	// solve the new updated MP
	printf("\n\n####################### Node_%d MP-%d CPLEX SOLVING START #######################\n\n",this_node.index, this_node.iter + 1);
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	//MP_cplex.exportModel("updateMasterProblem.lp");
	bool MP_flag = MP_cplex.solve(); // solve MP
	printf("\n####################### Node_%d MP-%d CPLEX SOLVING END #########################\n", this_node.index, this_node.iter + 1);
	printf("\n	OBJ of Node_%d MP-%d is %f\n\n", this_node.index, this_node.iter + 1, MP_cplex.getValue(Obj_MP));

	// print fsb-solns of the updated MP
	int fsb_num = 0;
	int int_num = 0;
	size_t now_solns_num = this_node.model_matrix.size();
	for (int col = 0; col < now_solns_num; col++)
	{
		IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
		if (soln_val > 0) // feasible soln > 0
		{
			fsb_num++;
			int soln_int_val = int(soln_val);
			if (soln_int_val == soln_val)
			{
				// ATTTENTION:  
				if (soln_int_val >= 1)
				{
					int_num++;
					printf("	var_x_%d = %f int\n", col + 1, soln_val);
				}
			}
			else
			{
				printf("	var_x_%d = %f\n", col + 1, soln_val);
			}					
		}
	}

	printf("\n	BRANCHED VARS: \n\n");
	size_t branched_num = this_node.branched_vars_list.size();
	for (int k = 0; k < branched_num; k++)
	{
		printf("	var_x_%d = %d branched \n", 
			this_node.branched_idx_list[k]+1, this_node.branched_vars_list[k]);
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


	this_node.lower_bound = MP_cplex.getValue(Obj_MP);
	printf("\n	Node_%d MP-%d: \n", this_node.index,this_node.iter);
	printf("\n	Lower Bound:   %f\n", this_node.lower_bound);
	printf("\n	NUM of now solns: %zd\n", now_solns_num);
	printf("\n	NUM of fsb-solns: %d\n", fsb_num);
	printf("\n	NUM of int-solns: %d\n", int_num);
	printf("\n	NUM of branched-vars: %zd\n", branched_num);

	MP_cplex.end();
	return MP_flag;
}

bool SolveFinalMasterProblem(
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
	printf("\n\n####################### Node_%d MP-final CPLEX SOLVING START #######################\n\n",this_node.index);
	IloCplex MP_cplex(Model_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("FinalMasterProblem.lp");
	bool MP_flag = MP_cplex.solve(); // 求解当前主问题
	printf("\n####################### Node_%d MP-final CPLEX SOLVING END #########################\n", this_node.index);

	printf("\n	OBJ of Node_%d MP-final is %f \n\n", this_node.index, MP_cplex.getValue(Obj_MP));

	size_t all_solns_num = this_node.model_matrix.size();
	for (int col = 0; col < all_solns_num; col++)
	{
		IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);

		this_node.all_solns_list.push_back(soln_val); // 1. Node all solns (including zero-solns)

		if (soln_val > 0)
		{
			int soln_int_val = int(soln_val);
			if (soln_int_val == soln_val)
			{
				if (soln_int_val >= 1) // int-soln that larger than 1
				{
					this_node.int_solns_list.push_back(soln_val); // 4. Node int-solns
					this_node.int_idx_list.push_back(col); // 5. Node int-solns' index

					printf("	var_x_%d = %f int\n", col + 1, soln_val);
				}
			}
			else // non-int-solns
			{
				printf("	var_x_%d = %f\n", col + 1, soln_val);
			}

			this_node.fsb_solns_list.push_back(soln_val); // 2. Node feasible (i.e. non-zero-solns) solns 
			this_node.fsb_idx_list.push_back(col); 	// 3. Node fsb-solns' index
		}
	}


	printf("\n	BRANCHED VARS: \n\n");
	size_t branched_num = this_node.branched_vars_list.size();
	for (int k = 0; k < branched_num; k++)
	{
		printf("	var_x_%d = %d branched \n",
			this_node.branched_idx_list[k]+1, this_node.branched_vars_list[k]);
	}

	size_t fsb_num= this_node.fsb_solns_list.size();
	size_t int_num = this_node.int_idx_list.size();
	this_node.lower_bound = MP_cplex.getValue(Obj_MP);
	printf("\n	Node_%d MP-final: \n", this_node.index);
	printf("\n	Lower Bound:  %f\n", this_node.lower_bound);
	printf("\n	NUM of all solns: %zd\n", all_solns_num);
	printf("\n	NUM of fsb-solns: %zd\n", fsb_num);
	printf("\n	NUM of int-solns: %zd\n", int_num);
	printf("\n	NUM of branched-vars: %zd\n", branched_num);

	MP_cplex.end();
	return MP_flag;
}






