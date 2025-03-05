int sat_solution_checker(int variable_count,
                         int disjunction_count,
                         int* twocnf_formula,
                         int* variable_values)
{
    int result = 1;
    for (int i = 0; (i < disjunction_count) && (result == 1);
         i++)
    {
        int index_i = 2*i;
        int first_literal = twocnf_formula[index_i];
        int second_literal = twocnf_formula[index_i + 1];

        int first_literal_variable;
        if (first_literal > 0)
        {
            first_literal_variable = first_literal;
        }
        else
        {
            first_literal_variable = -first_literal;
        }

        int first_variable_value;
        int first_variable_find = 0;

        for (int i = 0; (i < variable_count) && (first_variable_find == 0); i++)
        {
            int variable;
            if (variable_values[i] > 0)
            {
                variable = variable_values[i];
            }
            else
            {
                variable = -variable_values[i];
            }
            if (variable == first_literal_variable)
            {
                if (variable_values[i] > 0)
                {
                    first_variable_value = 1;
                }
                else
                {
                    first_variable_value = 0;
                }
                first_variable_find = 1;
            }
        }

        int second_literal_variable;
        if (second_literal > 0)
        {
            second_literal_variable = second_literal;
        }
        else
        {
            second_literal_variable = -second_literal;
        }

        int second_variable_value;
        int second_variable_find = 0;

        for (int i = 0; (i < variable_count) && (second_variable_find == 0); i++)
        {
            int variable;
            if (variable_values[i] > 0)
            {
                variable = variable_values[i];
            }
            else
            {
                variable = -variable_values[i];
            }
            if (variable == second_literal_variable)
            {
                if (variable_values[i] > 0)
                {
                    second_variable_value = 1;
                }
                else
                {
                    second_variable_value = 0;
                }
                second_variable_find = 1;
            }
        }
        
        if ((first_literal > 0) && (second_literal > 0))
        {            
            if ((first_variable_value == 0) && (second_variable_value == 0))
            {
                result = 0;
            }
        }
        else if ((first_literal > 0) && (second_literal < 0))
        {
            if ((first_variable_value == 0) && (second_variable_value == 1))
            {
                result = 0;
            }
        }
        else if ((first_literal < 0) && (second_literal > 0))
        {            
            if ((first_variable_value == 1) && (second_variable_value == 0))
            {
                result = 0;
            }
        }
        else
        {            
            if ((first_variable_value == 1) && (second_variable_value == 1))
            {
                result = 0;
            }
        }
    }
    return result;
}