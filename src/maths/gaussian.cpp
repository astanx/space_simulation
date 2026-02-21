#include "maths/gaussian.h"

float gaussian(float x, float mean, float stddev)
{
  const float a = (x - mean) / stddev;
  return std::exp(-0.5 * a * a);
}

std::vector<float> createGaussianBlurWeights(size_t kernel_size, float std_dev)
{
  std::vector<float> gaussian_blur_weights(kernel_size, 0.0f);
  for (size_t i = 0; i < (kernel_size + 1) / 2; ++i)
    gaussian_blur_weights[i] = gaussian_blur_weights[kernel_size - 1 - i] = gaussian(i, (kernel_size - 1) / 2.0, std_dev);

  float weight_sum = 0.0;
  for (auto weight : gaussian_blur_weights)
    weight_sum += weight;

  for (size_t i = 0; i < (kernel_size + 1) / 2; ++i)
    gaussian_blur_weights[i] = gaussian_blur_weights[kernel_size - 1 - i] = gaussian_blur_weights[i] / weight_sum;

  return gaussian_blur_weights;
}