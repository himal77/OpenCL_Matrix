__kernel void gpuMatAdd(__global float *A, __global float *B, int N) {

  /* Work item */
  int i = get_global_id(0);
  int j = get_global_id(1);

  /* Return if thread out of boundary */
  if (i>=N && j>=N) return;

  /* Index of next item */
  int index = i * N + j;

  /* four other element for averaging */
  int left = (index % N) == 0       ? 0: A[index - 1];
  int top  = (index - N) < 0        ? 0: A[index - N];
  int right = (index % N) == N-1    ? 0: A[index + 1];
  int bottom  = (index + N) >= N*N  ? 0: A[index + N];

  /* keeping element in the new matrix */
  B[index] = 0.2 * (A[index] + left + top + right + bottom);

}
