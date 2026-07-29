__kernel void scan(__global uint* input, __global uint* output, __local uint* scratch)
{
  uint id = get_global_id(0);
  uint lid = get_local_id(0);
  uint groupSize = get_local_size(0);

  scratch[lid] = input[id];
  barrier(CLK_LOCAL_MEM_FENCE);

  for (uint offset = 1; offset < groupSize; offset <<= 1)
  {
    uint value = 0;

    if (lid >= offset)
      value = scratch[lid - offset];

    barrier(CLK_LOCAL_MEM_FENCE);

    scratch[lid] += value;

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  output[id] = scratch[lid];
}