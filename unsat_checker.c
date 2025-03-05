#include <stdlib.h>
#include "twosat_solver.h"

int unsat_checker(int variable_count, int disjunction_count, int* disjunctions)
{
    int double_variable_count = variable_count + variable_count; 
    int implication_graph_size = double_variable_count*double_variable_count;
    int* implication_graph = (int*)malloc(implication_graph_size * sizeof(int));

    for(int i = 0; i < double_variable_count; i++)
    {
        for(int j = 0; j < double_variable_count; j++)
        {
            implication_graph[i * double_variable_count + j] = 0;
        }
    }

    for (int i = 0; i < disjunction_count; i++)
    {
        int first_disjunct = disjunctions[2*i];
        int second_disjunct = disjunctions[2*i + 1];

        int i1 = variable_count + first_disjunct - 1;
        int j1 = second_disjunct - 1;
        int i2 = variable_count + second_disjunct - 1;
        int j2 = first_disjunct - 1;

        if (first_disjunct > 0)
        {
            if (second_disjunct < 0)
            {
                j1 = variable_count + (-1)*second_disjunct - 1;
                i2 = (-1)*second_disjunct - 1;
            }
        }
        else
        {
            if (second_disjunct > 0)
            {
                i1 = (-1)*first_disjunct - 1;
                j2 = variable_count + (-1)*first_disjunct - 1;
            }
            else
            {
                i1 = (-1)*first_disjunct - 1;
                j1 = variable_count + (-1)*second_disjunct - 1;
                i2 = (-1)*second_disjunct - 1;
                j2 = variable_count + (-1)*first_disjunct - 1;
            }
        }

        implication_graph[i1 * double_variable_count + j1] = 1;
        implication_graph[i2 * double_variable_count + j2] = 1;
    }

    for(int k = 0; k < double_variable_count; k++)
    {
        for(int i = 0; i < double_variable_count; i++)
        {
            for(int j = 0; j < double_variable_count; j++)
            {
                if ((implication_graph[i * double_variable_count + j] == 0) &&
                    (implication_graph[i * double_variable_count + k] == 1) &&
                    (implication_graph[k * double_variable_count + j] == 1))
                {
                    implication_graph[i * double_variable_count + j] = 1;
                }
            }
        }
    }

    int twosat_solver_result = twosat_solver(variable_count, implication_graph);

    free(implication_graph);

    int result;
    if (twosat_solver_result == 0)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    return result;
}