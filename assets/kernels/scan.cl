__kernel scan(__global uint* input, __global uint* output, __local uint* scratch)
{
  int id = get_global_id(0);
  int lid = get_local_id(0);
  int groupSize = get_local_size(0);

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

  output[gid] = scratch[lid];
}