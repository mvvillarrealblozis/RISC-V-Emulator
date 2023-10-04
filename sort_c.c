int swap_c(int arr1[], int idx1, int idx2);

int sort_c(int *arr, int len) {
     int i, j;
     for (i = 1; i < len; i++) {
         j = i;
         while (j > 0 && arr[j-1] > arr[j]) {
             swap_c(arr, j, j-1);
             j--;
         }
     }

}
