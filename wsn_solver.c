#include <stdlib.h>
#include "twosat.h"

int wsn_solver(int sensor_count, int sensor_connection_count, int* communication_graph)
{
    int* formula = (int*)malloc((2 * (sensor_connection_count + 2)) * sizeof(int));

    for (int i = 0; i < sensor_connection_count; i++)
    {
        int index = 2 * i;
        formula[index] = -communication_graph[index];
        formula[index + 1] = communication_graph[index + 1];
    }

    int sat_solution_found = 0;
    int solver_error = 0;

    int* model = (int*)malloc(sensor_count * sizeof(int));

    for (int i = 1; (i <= sensor_count) && (sat_solution_found == 0) && (solver_error == 0);
         i++)
    {
        for (int j = i+1; (j <= sensor_count) && (sat_solution_found == 0) && (solver_error == 0);
             j++)
        {
            formula[2 * sensor_connection_count] = i;
            formula[2 * sensor_connection_count + 1] = j;
            formula[2 * sensor_connection_count + 2] = -i;
            formula[2 * sensor_connection_count + 3] = -j;

            int result_tmp = twosat(sensor_count, sensor_connection_count + 2, formula, model);
            if (result_tmp == 1)
            {
                sat_solution_found = 1;
            }
            else if (result_tmp == -1)
            {
                solver_error = 1;
            }
        }
    }

    int result = 0;
    if (sat_solution_found == 1)
    {
        result = 1;
    }
    else if (solver_error == 1)
    {
        result = -1;
    }

    free(formula);
    free(model);

    return result;
}