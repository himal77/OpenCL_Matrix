__kernel void gpuMatAdd(__global float *A, __global float *B, int N, int NUM_ITERATION, __local float *localA, __local float *localB) {

for(int k = 0; k < NUM_ITERATION; k++) {
  /* global Work item */
  int i = get_global_id(0);
  int j = get_global_id(1);

  /* local work item */
  int a = get_local_id(0);
  int b = get_local_id(1);

  /* local and global dimension, i.e no of element in matrix */
  int localDim = get_work_dim();
  int globalDim = get_global_size(0);

  /* size of local workgroup */
  int localSize = get_local_size(0);

  /* local group index x, and y */
  int groupIdXSide = get_group_id(0);  // 2D Thread IDx
  int groupIdYSide = get_group_id(1);   // 2D Thread IDy

  /* Return if thread out of boundary */
  if (i>=N && j>=N) return;

  /* Index of item in global and local */
  int globalIndex = i * N + j;
  int localIndex = a * localSize + b;

  /* Copying element from global address to local address */
   localA[localIndex] = A[globalIndex];

  barrier(CLK_LOCAL_MEM_FENCE);

   /**
     * Copying from the local memory and average the border element.
     * if the workgroup matrix is on outer border from four side.
     */

  /* four other element for averaging */
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    /* If the group is from the top line */
    if (groupIdXSide == 0 ) {  // if the work group is from top line
        if(groupIdYSide == 0) {  // if item is top left corner
            left =    (localIndex % localSize) == 0                    ? 0.0 : localA[localIndex - 1];
            top  =    (localIndex - localSize) < 0                     ? 0.0 : localA[localIndex - localSize];

            if(localSize == N) { // if it has only one work group
                printf("LocalSize: %d, globalDim: %d\n", localSize, localDim);
                 right =   (localIndex % localSize) == localSize-1           ? 0.0 : localA[localIndex + 1];
                 bottom =  (localIndex + localSize) >= localSize*localSize    ? 0.0 : localA[localIndex + localSize];
            }else{
                right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
                bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];
            }
            /* Copying in localB to store */
            localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);

        } else if(groupIdYSide == N / localSize - 1) { // if item is top right corner
            left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
            top  =    (localIndex - localSize) < 0                     ? 0 : localA[localIndex - localSize];
            right =   (localIndex % localSize) == localSize-1           ? 0 : localA[localIndex + 1];
            bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];

            /* Copying in localB to store */
            localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);

        } else { // if it lies in the middle

            left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
            top  =    (localIndex - localSize) < 0                     ? 0 : localA[localIndex - localSize];
            right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
            bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];

            /* Copying in localB to store */
            localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
        }
    } else if(groupIdYSide == 0) {
        if(groupIdXSide == N / localSize - 1) {
             left =    (localIndex % localSize) == 0                    ? 0.0 : localA[localIndex - 1];
             top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
             right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
             bottom =  (localIndex + localSize) >= localSize*localSize    ? 0.0 : localA[localIndex + localSize];

             /* Copying in localB to store */
             localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
        } else {
            left =    (localIndex % localSize) == 0                    ? 0.0 : localA[localIndex - 1];
            top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
            right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
            bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];

            /* Copying in localB to store */
            localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
        }
    } else if(groupIdXSide == N / localSize - 1) {
        if(groupIdYSide == N / localSize - 1) {
             left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
             top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
             right =   (localIndex % localSize) == localSize-1           ? 0.0 : localA[localIndex + 1];
             bottom =  (localIndex + localSize) >= localSize*localSize    ? 0.0 : localA[localIndex + localSize];

             /* Copying in localB to store */
             localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
        } else {
             left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
             top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
             right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
             bottom =  (localIndex + localSize) >= localSize*localSize    ? 0.0 : localA[localIndex + localSize];

             /* Copying in localB to store */
             localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
        }
    } else if( groupIdYSide == N / localSize - 1) {
            left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
            top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
            right =   (localIndex % localSize) == localSize-1           ? 0.0 : localA[localIndex + 1];
            bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];

            /* Copying in localB to store */
            localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);

    } else {

        left =    (localIndex % localSize) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
        top  =    (localIndex - localSize) < 0                     ? A[globalIndex - N] : localA[localIndex - localSize];
        right =   (localIndex % localSize) == localSize-1           ? A[globalIndex + 1] : localA[localIndex + 1];
        bottom =  (localIndex + localSize) >= localSize*localSize    ? A[globalIndex + N] : localA[localIndex + localSize];

        /* Copying in localB to store */
        localB[localIndex] = 0.2 * (left + right + top + bottom + localA[localIndex]);
   }
  /* iteration of element ended */


  /* waiting for local and global elements to finish */
  barrier(CLK_LOCAL_MEM_FENCE);
  barrier(CLK_GLOBAL_MEM_FENCE);

  /**
   * if the iteration is last, copy in global and return
   * else copy in global A and do next iteration
   */
  if(k == NUM_ITERATION-1) {
    B[globalIndex] =  localB[localIndex];
  } else {
    localA[localIndex] = localB[localIndex];
    A[globalIndex] =  localB[localIndex];
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  barrier(CLK_GLOBAL_MEM_FENCE);

 }
 return;
}
