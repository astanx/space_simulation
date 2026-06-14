#pragma once

struct TidalParameters
{
  double k2; // Love numbers
  double Q;  // Tidal quality factor

  TidalParameters(double k2 = -1, double Q = -1) : k2(k2), Q(Q) {};
};