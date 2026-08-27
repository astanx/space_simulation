__kernel void scanGroup(__global uint* groupSums, __global uint* groupOffsets, __local uint* scratch, uint groupCount)
{
  uint id = get_global_id(0);
  uint lid = get_local_id(0);
  uint groupSize = get_local_size(0);

  if (id < groupCount)
    scratch[lid] = groupSums[id];
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

  if (id < groupCount)
    {if (lid == 0)
      groupOffsets[id] = 0;
    else
      groupOffsets[id] = scratch[lid - 1];
    }
}