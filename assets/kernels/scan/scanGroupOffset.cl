__kernel void scanGroupOffset(__global uint* output, __global uint* groupOffsets, uint count)
{
  uint id = get_global_id(0);
  
  if (id >= count) return;

  uint group = get_group_id(0);

  output[id] += groupOffsets[group];
}