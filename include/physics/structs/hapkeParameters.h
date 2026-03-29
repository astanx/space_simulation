#pragma once

struct HapkeParameters
{
  float w; // single scattering albedo
  float theta; // macroscopic roughness angle (radians)
  float h; // opposition effect width
  float b0; // strength of the opposition effect  
  float h_cb; // width of the coherent backscatter opposition effect
  float b0_cb; // strength of the coherent backscatter opposition effect
  float b; // asymmetry parameter
  float c; // weighting between backward and forward scattering

  HapkeParameters(float w, float theta, float h, float b0, float h_cb, float b0_cb, float b, float c) : w(w), theta(theta), h(h), b0(b0), h_cb(h_cb), b0_cb(b0_cb), b(b), c(c) {}
};
