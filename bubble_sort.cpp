#include <stdio.h>

int main() {
    int arr[8];
    int i, j, temp;
    printf("Please input 8 numbers:");
    for (i = 0; i < 8; i++) {
        scanf("%d", &arr[i]);
    }
    for (i = 0; i < 8 - 1; i++) {
        // ?????????????
        for (j = 0; j < 8 - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    printf("After sorting: ");
    for (i = 0; i < 8; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
