__kernel void gpuMatAdd(__global float *A, __global float *B, int N, int NUM_ITERATION) {

for(int k = 0; k < NUM_ITERATION; k++) {
  /* Work item */
  int i = get_global_id(0);
  int j = get_global_id(1);

  int a = get_local_id(0);
  int b = get_local_id(1);

  int localDim = get_work_dim(); // local dimension
  int globalDim = get_global_size(0); // global dimension

  int localSize = get_local_size(0);

  int groupIdXSide = get_group_id(0);  // 2D Thread IDx
  int groupIdYSide = get_group_id(1);   // 2D Thread IDy

  __local float *localA;
  __local float *localB;

  // TODO initialize the localA......


  /* Return if thread out of boundary */
  if (i>=N && j>=N) return;

  /* Index of next item */
  int globalIndex = i * N + j;
  int localIndex = a * localDim + b;


  /* four other element for averaging */
  float left    = 0;
  float top     = 0;
  float right   = 0;
  float bottom  = 0;


  /**
   * Copying from the local memory and average the border element.
   * if the workgroup matrix is on outer border from four side.
   */
  barrier(CLK_LOCAL_MEM_FENCE);
  if (groupIdXSide == 0 || groupIdYSide == 0 || groupIdXSide == globalDim / localDim - 1 || groupIdYSide == globalDim / localDim - 1) {


  } else {
    left =    (localIndex % localDim) == 0                    ? A[globalIndex - 1] : localA[localIndex - 1];
    top  =    (localIndex - localDim) < 0                     ? A[globalIndex - N] : localA[localIndex - localDim];
    right =   (localIndex % localDim) == N-1                  ? A[globalIndex + 1] : localA[localIndex - 1];
    bottom =  (localIndex + localDim) >= localDim*localDim    ? A[globalIndex + N] : localA[localIndex + localDim];

    // Copying in localB to store
    localB[localIndex] = 0.2 * (A[localIndex] + left + top + right + bottom);
  }

  // waiting for local and global elements to finish
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
  barrier(CLK_GLOBAL_MEM_FENCE);

 }
 return;
}
