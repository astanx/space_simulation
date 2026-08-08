__kernel void scanLocal(__global uint* input, __global uint* output, __global uint* groupSums, __local uint* scratch, uint count)
{
  uint id = get_global_id(0);
  uint lid = get_local_id(0);
  uint group = get_group_id(0);
  uint groupSize = get_local_size(0);

  if (id < count)
    scratch[lid] = input[id];
  else
    scratch[lid] = 0;

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

  if (id < count)
  {
    if (lid == 0)
      output[id] = 0;
    else
      output[id] = scratch[lid - 1];
  }

  if (lid == groupSize - 1)
    if (group < (count + groupSize - 1) / groupSize)
        groupSums[group] = scratch[lid];
}