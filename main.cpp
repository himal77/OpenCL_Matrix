#include <iostream>
#include <stdlib.h>

size_t SIZE = 5;
size_t n = SIZE - 1;
size_t noOfIteration = 4;


int main() {
    double **A = new double *[SIZE];

    for (int i = 0; i < SIZE; i++) {
        A[i] = new double[SIZE];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 100;
        }
    }

    // inserting into matrix
    // serial version
    for (int iteration = 0; iteration < noOfIteration; iteration++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {

            if (i <= 0 || i >= n - 1 || j <= 0 || j >= n - 1) {
                continue;
            }
            A[i][j] = 0.2 * (
                    A[i][j] +
                    A[i - 1][j] +
                    A[i + 1][j] +
                    A[i][j - 1] +
                    A[i][j + 1]
            );
            }
        }
    }
    // parallel version

    // displaying matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i][j] << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
