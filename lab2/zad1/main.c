#include <stdio.h>

#ifndef LIB_DYNAMIC
#include "collatz.h"
#endif

#ifdef LIB_DYNAMIC
#include <dlfcn.h>
void *handle;
int (*collatz_conjecture)(unsigned int input);
int (*test_collatz_convergence)(unsigned int input, int max_iter);
#endif

void handlePrint(int number, int steps)
{
    switch (steps)
    {
    case -1:
        printf("Przekroczono MAX_ITER dla liczby %d\n", number);
        break;

    case -2:
        printf("Liczba (%d) musi być większa od 1\n", number);
        break;

    default:
        printf("Dla %d\tliczba krokow: %d\n", number, steps);
        break;
    }
}

int main()
{
#ifdef LIB_DYNAMIC
    handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handle)
    {
        printf("ERROR WITH DYNAMIC LIBRARY LOADING");
        return 1;
    }

    collatz_conjecture = dlsym(handle, "collatz_conjecture");
    test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
#endif

    const int MAX_ITER = 1000;

    // trzeba zmienic n w zaleznosci od ilosci liczb!
    unsigned int arr[] = {5, 8734, 100, 101, 77, 2, 8, 1999, 2005, 1};
    int n = 10;

    for (int i = 0; i < n; i++)
    {
        handlePrint(arr[i], test_collatz_convergence(arr[i], MAX_ITER));
    }

#ifdef LIB_DYNAMIC
    dlclose(handle);
#endif

    return 0;
}