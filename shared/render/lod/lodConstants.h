#ifndef LOD_CONSTANTS_H
#define LOD_CONSTANTS_H

#ifdef __OPENCL_VERSION__
constant uint LOD_FULL = 1;
constant uint LOD_IMPOSTOR = 2;
constant uint LOD_POINT = 3;
#else
#include <cstdint>
using uint = uint32_t;
const uint LOD_FULL = 1;
const uint LOD_IMPOSTOR = 2;
const uint LOD_POINT = 3;
#endif

#endif