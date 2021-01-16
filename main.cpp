#include <iostream>
#include <iomanip>
#include <cstdio>

size_t SIZE = 4;
size_t n = SIZE;
size_t noOfIteration = 2;
size_t precision = 3;


int main() {
    // Declaring two matrix of same size
    double **A = new double *[SIZE];
    double **B = new double *[SIZE];


    for (int i = 0; i < SIZE; i++) {
        A[i] = new double[SIZE];
        B[i] = new double[SIZE];
    }


    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 1 && j == 2) {
                A[i][j] = 10.0;
                B[i][j] = A[i][j];
                continue;
            }
            A[i][j] = 0.0; //rand() % 100;
            B[i][j] = A[i][j];
        }
    }


    // inserting into matrix

    // serial version
    for (int iteration = 0; iteration < noOfIteration; iteration++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {

                // Adding new value to new B
                B[i][j] = 0.2 * (
                        A[i][j] +
                        (i - 1 > -1 ? A[i - 1][j] : 0) + // if out of bound on left
                        (i + 1 < n ? A[i + 1][j] : 0) + // if out of bound on right
                        (j - 1 > -1 ? A[i][j - 1] : 0) + // if out of bound on top
                        (j + 1 < n ? A[i][j + 1] : 0) // if out of bound on bottom
                );
            }
        }


        // Assigning all value of iterated B to original A, for next iteration
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = B[i][j];
            }
        }
    }


    // displaying matrix in serial version
    std::cout<<"Serial version: \n";
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i][j] << "  ";
        }
        std::cout << "\n";
    }


    // parallel version



    // displaying matrix in parallel version
    std::cout<<"Parallel version: \n";
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i][j] << "  ";
        }
        std::cout << "\n";
    }


    return 0;
}
