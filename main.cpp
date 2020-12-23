#include <iostream>
#include <stdlib.h>

size_t SIZE = 4;
size_t n = SIZE;
size_t noOfIteration = 1;


int main() {
    double **A = new double *[SIZE];

    for (int i = 0; i < SIZE; i++) {
        A[i] = new double[SIZE];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = 0;//rand() % 100;
        }
    }

    A[1][2] = 10;

    double **B = A;
    // inserting into matrix
    // serial version
    for (int iteration = 0; iteration < noOfIteration; iteration++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {

                int second = 0;
                if (i - 1 >= 0) second = B[i - 1][j];
                int third = 0;
                if (i + 1 < n) third = B[i + 1][j];
                int fourth = 0;
                if (j - 1 >= 0) fourth = B[i][j - 1];
                int fifth = 0;
                if (j + 1 < n) fifth = B[i][j + 1];

                A[i][j] = 0.2 * (
                        A[i][j] +
                        second +
                        third +
                        fourth +
                        fifth

                );
            }
        }
        B = A;
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
