#include "maths/factorial.h"

unsigned long long factorial(unsigned n)
{
  if (n == 0 || n == 1)
    return 1;

  unsigned long long result = 1;
  
  return result * n * factorial(n - 1);
}

unsigned long long doubleFactorial(unsigned n)
{
  if (n == 0 || n == -1)
    return 1;

  unsigned long long result = 1;
  
  return result * n * doubleFactorial(n - 2);
}