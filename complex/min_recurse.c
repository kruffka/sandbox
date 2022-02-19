#include <stdio.h>

/*          //  0  1  2
    int A[n] = {0, 2, 1};

    0 != 3
    0 > max(i+1)
            |
            V
            1 != 3
            2 > max(i+1)
                    |
                    V
                    2 != 3
                    1 > max(i+1) [0]
                            |
                            V
                            3 == 3
a > b
*/
int max(int *a, int i, int n) {
    if(i == n) {
        return 0;
    } else if (a[i] > max(a, i+1, n)) {
        return ;
    } else {
        return ;
    }
}
#define n 3

void main() {
            //  0  1  2
    int A[n] = {0, 2, 1};

    printf("min = %d\n", max(A, 0, 3));

    
}