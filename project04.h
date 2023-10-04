#include <stdint.h>
#include <stdbool.h>

#define MAX_ARRAY 1024

int quadratic_c(int x, int a, int b, int c);
int quadratic_s(int x, int a, int b, int c);

int midpoint_c(int start, int end);
int midpoint_s(int start, int end);

int max3_c(int a, int b, int c);
int max3_s(int a, int b, int c);

int get_bitseq_c(int n, int start, int end);
int get_bitseq_s(int n, int start, int end);

int get_bitseq_signed_c(int n, int start, int end);
int get_bitseq_signed_s(int n, int start, int end);

void to_upper_c(char *src, char *dst);
void to_upper_s(char *src, char *dst);

uint64_t fib_rec_c(uint32_t);
uint64_t fib_rec_s(uint32_t);

void swap_c(int arr[], int i, int j);
void swap_s(int arr[], int i, int j);

void sort_c(int arr[], int len);
void sort_s(int arr[], int len);

int eval_c(char *expr_str);
int eval_s(char *expr_str);
