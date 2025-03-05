#include <stdlib.h>

int twosat_satisfiability_solver(int variable_count, int disjunction_count,
                                 int* disjunctions, int* model)
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

    int* strongly_connected_components = (int*)malloc(double_variable_count * sizeof(int));

    for (int i = 0; i < double_variable_count; i++)
    {
        strongly_connected_components[i] = i;
    }

    for(int i = 0; i < double_variable_count; i++)
    {
        for(int j = 0; j < double_variable_count; j++)
        {
            if (i != j)
            {
                if (strongly_connected_components[i] != strongly_connected_components[j])
                {
                    if ((implication_graph[i * double_variable_count + j] == 1) &&
                        (implication_graph[j * double_variable_count + i] == 1))
                    {
                        if (strongly_connected_components[j] > strongly_connected_components[i])
                        {
                            strongly_connected_components[j] = strongly_connected_components[i];
                        }
                        else
                        {
                            strongly_connected_components[i] = strongly_connected_components[j];
                        }
                    }
                }
            }
        }
    }

    int* components = (int*)malloc(double_variable_count * sizeof(int));
    for (int i = 0; i < double_variable_count; i++)
    {
        components[i] = -1;
    }

    for (int i = 0; i < double_variable_count; i++)
    {
        if (components[strongly_connected_components[i]] == -1)
        {
            components[strongly_connected_components[i]] = i;
        }
    }

    int component_count = 0;
    int* component_map = (int*)malloc(double_variable_count*sizeof(int));
    for (int i = 0; i < double_variable_count; i++)
    {
        component_map[i] = -1;
    } 

    for (int i = 0; i < double_variable_count; i++)
    {
        if (components[i] != -1)
        {
            component_map[i] = component_count;
            component_count++;
        }
    }

    free(components);

    for (int i = 0; i < double_variable_count; i++)
    {
        strongly_connected_components[i] = component_map[strongly_connected_components[i]];
    }

    free(component_map);

    int* component_vertex = (int*)malloc(component_count * sizeof(int));
    for (int i = 0; i < component_count; i++)
    {
        component_vertex[i] = -1;
    }

    for (int i = 0; i < double_variable_count; i++)
    {
        if (component_vertex[strongly_connected_components[i]] == -1)
        {
            component_vertex[strongly_connected_components[i]] = i;
        }
    }

    int* component_topological_order = (int*)malloc(component_count * sizeof(int));

    for (int i = 0; i < component_count; i++)
    {
        int component_found = 0;
        int component_number = component_count - 1;
        for (int j = 0; (j < component_count) && (component_found == 0); j++)
        {
            if (component_vertex[j] != -1)
            {
                int path_found = 0;
                for (int k = 0; (k < component_count) && (path_found == 0); k++)
                {
                    if (component_vertex[k] != -1)
                    {
                        if (k != j)
                        {
                            if (implication_graph[
                                component_vertex[k] * double_variable_count +
                                component_vertex[j]] == 1)
                            {
                                path_found = 1;
                            }
                        }
                    }
                }
                if (path_found == 0)
                {
                    component_found = 1;
                    component_vertex[j] = -1;
                    component_number = j;
                }
            }
        }
            
        component_topological_order[component_number] = i;
    }

    free(implication_graph);
    free(component_vertex);            

    for (int i = 0; i < double_variable_count; i++)
    {
        strongly_connected_components[i] = component_topological_order[strongly_connected_components[i]];
    }

    free(component_topological_order);

    int result = 1;

    for (int i = 0; (i < variable_count) && (result == 1); i++)
    {
        if (strongly_connected_components[i] == strongly_connected_components[i + variable_count])
        {
            result = 0;
        }
    }

    if (result == 1)
    {
        for (int i = 0; i < variable_count; i++)
        {
            if (strongly_connected_components[i] > strongly_connected_components[i + variable_count])
            {
                model[i] = i + 1;
            }
            else
            {
                model[i] = -(i + 1);
            }
        }
    }

    free(strongly_connected_components);

    return result;
}