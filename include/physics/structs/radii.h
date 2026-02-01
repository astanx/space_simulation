#pragma once

struct Radii
{
  double equatorian;
  double polar;
  double mean;

  Radii(
      double equatorian,
      double polar,
      double mean) : equatorian(equatorian), polar(polar), mean(mean) {};

  Radii scaled(double scale) const
  {
    return Radii(
        equatorian * scale,
        polar * scale,
        mean * scale);
  }
};
