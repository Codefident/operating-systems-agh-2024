#include "collatz.h"

int collatz_conjecture(unsigned int input)
{
    if (input % 2 == 0)
        return input / 2;
    return 3 * input + 1;
}

int test_collatz_convergence(unsigned int input, int max_iter)
{
    if (input < 2)
    {
        return -2;
    }

    unsigned int result = input;
    for (int i = 0; i < max_iter; i++)
    {
        result = collatz_conjecture(result);
        if (result == 1)
            return i + 1;
    }
    return -1;
}
