#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <omp.h>
#include <sstream>
#include <vector>

#include "image_enhancer.h"



int main(int argc, char *argv[])
{
  auto time1 = std::chrono::steady_clock::now();
                      
  float m = std::atof(argv[1]);
  float n = std::atof(argv[2]);

  // Sensible defaults (at least for given image).
  if (m == 0) {m = 0.5;}
  if (n == 0) {n = 0.84;}

  std::cout << "image gets enhanced with parameters: \n"
            << "m = " << m << ", " << "n = " << n << "\n\n";

  std::string input_image_name = argv[3];
  std::string output_image_name = argv[4];

  std::vector<std::vector<float>> array = read_ppm(input_image_name);
  int height = array.size();
  int width = array[0].size();

  auto time2_5 = std::chrono::steady_clock::now();

  std::vector<std::vector<float>> normed_array =
      moving_morm(height, width, array, 15);

  auto time3 = std::chrono::steady_clock::now();

  std::vector<std::vector<bool>> bool_img =
      adaptive_thresh(height, width, normed_array, array, 100, m, n, 0.01);

  auto time4 = std::chrono::steady_clock::now();

  create_ppm(output_image_name, bool_img, height, width, 70);
  auto time5 = std::chrono::steady_clock::now();

  std::cout << "Load ppm in std::vector array: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(time2_5 - time1).count()
            << "ms" << "\n";

  std::cout << "perform movin_norm: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2_5).count()
            << "ms" << "\n";

  std::cout << "apply adaptive threshold: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(time4 - time3).count()
            << "ms" << "\n";

  std::cout << "create ppm file: " << std::chrono::duration_cast<std::chrono::milliseconds>(time5 - time4).count()
            << "ms" << "\n";

  std::cout << "Whole programm: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(time5 - time1).count()
            << "ms" << "\n";

  return 0;
}

