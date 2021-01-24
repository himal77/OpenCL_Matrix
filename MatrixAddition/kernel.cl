__kernel void gpuMatAdd(
  __global float *A,
  __global float *B,
  __global float *C,
  int N,
  int M)
{
  // define work-item indeces
  //----- insert OpenCL code -----
  int i = get_global_id(0);
  int j = get_global_id(1);

  //---------------------------
  // check that the thread is not out of the vector boundary
  if (i>=N || j>=M) return;

  int index = j*M + i;
  // write the operation for the sum of vectors
  //----- insert OpenCL code -----
  C[index] = A[index] + B[index];
  //----------------------------
}
