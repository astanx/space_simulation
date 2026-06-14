#pragma once

struct GravityField
{
  // normalized factors
  double C20;
  double C22;
  double C;

  GravityField(double C20 = -1, double C22 = -1, double C = -1) : C20(C20), C22(C22), C(C)
  {
  }
};