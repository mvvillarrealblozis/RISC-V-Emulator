#include <stdio.h>
#include <stdlib.h>

int swap_c(int *arr, int i, int j);
int swap_s(int *arr, int i, int j);

void print_array(char *prefix, int *arr, int len) {
    printf("%s", prefix);
    for (int i = 0; i < len; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: swap i j a0 a1 [a2 ...]\n");
        exit(-1);
    }

    int i = atoi(argv[1]);
    int j = atoi(argv[2]);
    int len = argc - 3;
    int *arr = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        arr[i] = atoi(argv[i + 3]);

    swap_c(arr, i, j);
    print_array("C:", arr, len);

    for (int i = 0; i < len; i++)
        arr[i] = atoi(argv[i + 3]);

    swap_s(arr, i, j);
    print_array("Asm:", arr, len);

    return 0;
}
