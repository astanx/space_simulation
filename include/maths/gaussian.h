#pragma once

#include <vector>

float gaussian(float x, float mean, float stddev);
std::vector<float> createGaussianBlurWeights(size_t kernel_size, float stddev);