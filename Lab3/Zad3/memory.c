#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
    int *arr = malloc(1000000 * sizeof(int));
    for (int j = 0; j < 1000000; j ++)
    {
	    arr[j] = j;
        if(j == 100000) j = 0;
    }
    free(arr);
    return 0;
}
