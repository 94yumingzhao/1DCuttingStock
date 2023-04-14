// 2023-03-16

#include "CSCG.h"
using namespace std;

// 求解第一个主问题
bool SolveFirstMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP) {

	OutputMasterProblem(Values, Lists);
	OutputDualMasterProblem(Values, Lists);

	int N_num = Values.item_types_num;;
	int J_num = N_num;

	IloNumArray  con_min(Env_MP); 
	IloNumArray  con_max(Env_MP);
	for (int row = 0; row < N_num; row++) {	
		int demand_val = Lists.all_item_types_list[row].demand; // con >= demand
		con_min.add(IloNum(demand_val)); // con LB
		con_max.add(IloNum(IloInfinity));  // con UB
	}
	Cons_MP = IloRangeArray(Env_MP, con_min, con_max);
	Model_MP.add(Cons_MP);
	con_min.end();
	con_max.end();

	// 列建模
	for (int col = 0; col < J_num; col++) {
		IloNum obj_para = 1;
		IloNumColumn CplexCol = Obj_MP(obj_para);
		for (int row = 0; row < N_num; row++) {
			IloNum row_para = Lists.model_matrix[row][col];
			CplexCol += Cons_MP[row](row_para);
		}
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		string var_name = "X_" + to_string(col + 1); // var name
		IloNumVar Var(CplexCol, var_min, var_max, ILOFLOAT, var_name.c_str()); 		// var >= 0
		Vars_MP.add(Var);

		CplexCol.end(); // end this IloNumColumn object
	}

	// solve model
	printf("\n\n################  MP-1 CPLEX SOLVING START ################\n");
	IloCplex MP_cplex(Env_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel("First Master Problem.lp");
	bool MP_flag = MP_cplex.solve();
	printf("################  MP-1 CPLEX SOLVING OVER ##################\n");

	int fsb_num = 0;
	int int_num = 0;

	// judge model feasibility
	if (MP_flag == 0) {
		printf("\n\t MP-1 is NOT FEASIBLE\n");
	}
	else {
		printf("\n\t MP-1 is FEASIBLE\n");
		printf("\n\t Obj = %f\n\n", MP_cplex.getValue(Obj_MP));
		printf("\n\t X Vars: \n\n");

		for (int col = 0; col < J_num; col++) {
			IloNum soln_val = MP_cplex.getValue(Vars_MP[col]);
			if (soln_val > 0) // feasible soln > 0
			{
				fsb_num++;
				int soln_int_val = int(soln_val);
				if (soln_int_val == soln_val) {
					if (soln_int_val >= 1) {
						int_num++;
						printf("\t var_x_%d = %f int\n", col + 1, soln_val);
					}
				}
				else {
					printf("\t var_x_%d = %f\n", col + 1, soln_val);
				}
			}
		}

		printf("\n\t DUAL PRICES of MP cons: \n\n");
		for (int k = 0; k < N_num; k++) {
			double dual_val = MP_cplex.getDual(Cons_MP[k]);
			printf("\t dual_r_%d = %f\n", k + 1, dual_val);
			Lists.dual_prices_list.push_back(dual_val);
		}

		printf("\n\t MP-%d:\n", Values.iter);
		printf("\n\t Lower Bound = %f", MP_cplex.getValue(Obj_MP));
		printf("\n\t NUM of all solns = %d", J_num);
		printf("\n\t NUM of fsb solns = %d", fsb_num);
		printf("\n\t NUM of int solns = %d", int_num);
	}

	MP_cplex.end();
	return MP_flag;
}
