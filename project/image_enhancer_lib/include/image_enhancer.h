#ifndef IMAGE_ENHANCER
#define IMAGE_ENHANCER

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <vector>

#define RED_CONV 0.299;
#define GREEN_CONV 0.587;
#define BLUE_CONV 0.114;

void create_ppm_float(const std::string &output_name,
                      const std::vector<std::vector<float>> &array,
                      const int height, const int width, int buff_size);

void create_ppm(const std::string &output_name,
                const std::vector<std::vector<bool>> &array, const int height,
                const int width,const int buff_size);

template <typename Type> void print_hist(const std::vector<Type> &hist);

void print_array(const std::vector<std::vector<float>> array);

int getLastNonZero(const std::vector<int> &vec);

float rgb2gray_WP(const int rv, const int gv, const int bv);

std::vector<std::vector<float>> read_ppm(const std::string &input_name);

std::vector<std::vector<float>> create_empty_gray(int height, int width);

// Actual algorithms for image enhancement.
std::vector<int> create_hist(const std::vector<std::vector<float>> &array);

std::vector<float>
cumultative_hist(const std::vector<std::vector<float>> &array);

std::vector<std::vector<float>>
histo_linearization(int height, int width,
                   const std::vector<std::vector<float>> &array,
                   const std::vector<float> &cum_hist);

std::vector<std::vector<float>>
moving_morm(int height, int width, const std::vector<std::vector<float>> &array,
            int size);

std::vector<std::vector<bool>>
adaptive_thresh(int height, int width,
                const std::vector<std::vector<float>> &array,
                const std::vector<std::vector<float>> &original_array,
                int sections, float m, float n, float f);

#endif
