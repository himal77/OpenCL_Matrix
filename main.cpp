#include <iostream>
#include <iomanip>
#include <cstdio>
#include <chrono>

size_t SIZE = 4;
size_t n = SIZE;
size_t noOfIteration = 3;
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

    /////////////////////////////////////////////////
    ///////////////Serial Version////////////////////
    /////////////////////////////////////////////////

    /**
     *  inserting into matrix for serial version
     *  @iteration: no of times the matrix will be iterated
     *
     *  @A: matrix which will store the data
     *  @B: helper matrix for smoothing out the iteration
     */
    auto start = std::chrono::high_resolution_clock::now();
    for (int iteration = 0; iteration < noOfIteration; iteration++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {

                // Adding new value to new A using B
                A[i][j] = 0.2 * (
                        B[i][j] +
                        (i - 1 > -1 ? B[i - 1][j] : 0) +   // if out of bound on left
                        (i + 1 < n ? B[i + 1][j] : 0) +   // if out of bound on right
                        (j - 1 > -1 ? B[i][j - 1] : 0) +   // if out of bound on top
                        (j + 1 < n ? B[i][j + 1] : 0)     // if out of bound on bottom
                );
            }
        }

        /**
         * Assigning value of A to B for next iteration
         * if its last iteration, no need to assign
         */
        if (iteration != noOfIteration - 1) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    B[i][j] = A[i][j];
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    /**
     * displaying matrix in serial version
     * As well as the time it takes to compute
     */
    std::cout << "Serial version: \n";
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i][j] << "  ";
        }
        std::cout << "\n";
    }
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              <<" microseconds"<< std::endl;


    /////////////////////////////////////////////////
    ///////////////Parallel Version//////////////////
    /////////////////////////////////////////////////
    start = std::chrono::high_resolution_clock::now();
    /**
     * Parallel version using OpenCL
     *
     * TODO write the code for OpenCL
     */
    end = std::chrono::high_resolution_clock::now();

    /**
     * Displaying matrix in parallel version.
     * The time it takes to compute.
     */
/**
    std::cout<<"Parallel version: \n";
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i][j] << "  ";
        }
        std::cout << "\n";
    }
    std::cout<<"Time taken: "<<std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()<<<<" microseconds"<<std::endl;
*/

    return 0;
}
