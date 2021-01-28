__kernel void gpuMatAdd(__global float *A, __global float *B, int N, int NUM_ITERATION) {

for(int k = 0; k < NUM_ITERATION; k++) {
  /* Work item */
  int i = get_global_id(0);
  int j = get_global_id(1);

  /* Return if thread out of boundary */
  if (i>=N && j>=N) return;

  /* Index of next item */
  int index = i * N + j;

  /* four other element for averaging */
  float left    = (index % N) == 0      ? 0: A[index - 1];
  float top     = (index - N) < 0       ? 0: A[index - N];
  float right   = (index % N) == N-1    ? 0: A[index + 1];
  float bottom  = (index + N) >= N*N    ? 0: A[index + N];

  /* keeping element in the new matrix */
  B[index] = 0.2 * (A[index] + left + top + right + bottom);
  barrier(CLK_GLOBAL_MEM_FENCE);

  A[index] = B[index];
  barrier(CLK_GLOBAL_MEM_FENCE);
 }
 return;
}
