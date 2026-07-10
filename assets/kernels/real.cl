#ifdef USE_DOUBLE
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef double real;
typedef double2 real2;
typedef double3 real3;
typedef double4 real4;

#else

typedef float real;
typedef float2 real2;
typedef float3 real3;
typedef float4 real4;

#endif