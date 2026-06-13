#pragma once

struct Radii
{
  double equatorian;
  double polar;
  double mean;

  Radii(
      double equatorian = 0.0,
      double polar = 0.0,
      double mean = 0.0) : equatorian(equatorian), polar(polar), mean(mean) {};

  Radii scaled(double scale) const
  {
    return Radii(
        equatorian * scale,
        polar * scale,
        mean * scale);
  }
};
