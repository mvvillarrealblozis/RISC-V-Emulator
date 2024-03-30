#include <stdio.h>
#include <string.h>

int eval_c(char *expr);
int eval_s(char *expr);

int main(int argc, char **argv) {
    int value;
    
    value = eval_c(argv[1]);
    printf("C: %d\n", value);

    value = eval_s(argv[1]);
    printf("Asm: %d\n", value);

    return 0;
}
