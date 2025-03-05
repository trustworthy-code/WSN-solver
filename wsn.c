#include <stdlib.h>
#include <stdio.h>
#include "wsn_solver.h"

void wsn(int argc, char* argv[])
{
    FILE *fin;
    int is_input_file = 0;
    if (argc > 1)
    {
        fin = fopen(argv[1], "r");
        is_input_file = 1;
    }

    int sensor_count = 0;
    int sensor_connection_count = 0;
    if (is_input_file == 1)
    {
        fscanf(fin, " %d %d", &sensor_count, &sensor_connection_count);
    }
    else
    {
        scanf(" %d %d", &sensor_count, &sensor_connection_count);
    }

    int* communication_graph =(int*)malloc(2 * sensor_connection_count * sizeof(int));

    for (int i = 0; i < sensor_connection_count; i++)
    {
        int predecessor;
        int successor;

        if (is_input_file == 1)
        {
            fscanf(fin, " %d %d", &predecessor, &successor);
        }
        else
        {
            scanf(" %d %d", &predecessor, &successor);
        }

        int index = 2 * i;
        communication_graph[index] = predecessor;
        communication_graph[index + 1] = successor;
    }

    if (is_input_file == 1)
    {
        fclose(fin);
    }
    
    int result = wsn_solver(sensor_count, sensor_connection_count, communication_graph);

    free(communication_graph);

    FILE *fout;
    int is_output_file = 0;
    if (argc > 2)
    {
        fout = fopen(argv[2], "w");
        is_output_file = 1;
    }

    if (is_output_file == 1)
    {
        if (result == 0)
        {
            fprintf(fout, "Communication graph is strongly connected");
        }
        else if (result == 1)
        {
            fprintf(fout, "Communication graph is not strongly connected");
        }
        else if (result == -1)
        {
            fprintf(fout, "Solver error");
        }
    }
    else
    {
        if (result == 0)
        {
            printf("Communication graph is strongly connected");
        }
        else if (result == 1)
        {
            printf("Communication graph is not strongly connected");
        }
        else if (result == -1)
        {
            printf("Solver error");
        }
    }

    if (is_output_file == 1)
    {
        fclose(fout);
    }
}