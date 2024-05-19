#include "include/sort.h"

void BubbleSort(int *arr, int len)
{
    for (int i = 1; i <= len; i++) {
        for (int j = 1; j < i; j++) {
            if (arr[j] < arr[j - 1]) {
                int temp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = temp;
            }
        }
    }
}