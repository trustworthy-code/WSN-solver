#include "twosat_satisfiability_solver.h"
#include "unsat_checker.h"
#include "sat_solution_checker.h"

int twosat(int variable_count, int disjunction_count, int* disjunctions, int* model)
{
    int result = -1;
    int twosat_oracle_result = twosat_satisfiability_solver(variable_count, disjunction_count,
                                                            disjunctions, model);
    if (twosat_oracle_result == 0)
    {
        int unsat_checker_result = unsat_checker(variable_count, disjunction_count,
                                                 disjunctions);
        if (unsat_checker_result == 1)
        {
            result = 0;
        }
        else
        {
            result = -1;
        }
    }
    else if (twosat_oracle_result == 1)
    {
        int sat_solution_checker_result = sat_solution_checker(variable_count, disjunction_count,
                                                               disjunctions, model);
        if (sat_solution_checker_result == 1)
        {
            result = 1;
        }
        else
        {
            result = -1;
        }
    }
    else
    {
        result = -1;
    }
    return result;
}