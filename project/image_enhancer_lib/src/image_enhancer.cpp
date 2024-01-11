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

#define RED_CONV 0.299;
#define GREEN_CONV 0.587;
#define BLUE_CONV 0.114;

//help function
int getLastNonZero(const std::vector<int> &vec) {
  auto it = find_if(vec.rbegin(), vec.rend(), [](int x) { return x != 0; });
  if (it == vec.rend()) {
    return vec[vec.size() - 1];
  }
  return vec.size() - 1 - distance(vec.rbegin(), it);
}

//to convert the RGB Values to gray scaled values
//the *_CONV values are taken from openCV color conversions
float rgb2gray_WP(const int rv, const int gv, const int bv) {
  float red = rv * RED_CONV;
  float green = gv * GREEN_CONV;
  float blue = bv * BLUE_CONV;

  float grey_pixel = red + green + blue;
  return grey_pixel;
}

// to create an array like object, initialized with floats of the value 0
std::vector<std::vector<float>> create_empty_gray(int height, int width) {
  float init = 0.0;
  std::vector<std::vector<float>> empty_array(height,
                                              std::vector<float>(width, init));
  return empty_array;
}

// only for development
template <typename Type> void print_hist(const std::vector<Type> &hist) {
  for (int i = 0; i < (int)hist.size(); i++) {
    std::cout << hist[i] << " ";
  }
}

//to create a histogram out of an 2D array
std::vector<int> create_hist(const std::vector<std::vector<float>> &array) {
  std::vector<int> hist_vec(256, 0);
  for (int y = 0; y < (int)array.size(); y++) {
    for (int x = 0; x < (int)array[0].size(); x++) {
      int index = round(array[y][x]);
      hist_vec[index]++;
    }
  }

  return hist_vec;
}

//to calculate the cumulative histogram of an 2D array
std::vector<float> cumultative_hist(const std::vector<std::vector<float>> &array) {
  std::vector<int> hist = create_hist(array);
  std::vector<int> HKG(256, 0);
  std::vector<float> cumultative_histo(256, 0);

  for (int g = 0; g < 256; g++) {
    int HK = 0;
    for (int i = 0; i < g; i++) {
      HK += hist[i];
    }
    HKG[g] = HK;
  }
  
  //to get the maxium value of the array (also max value in histogram)
  float max_value = getLastNonZero(hist);
  for (int s = 0; s < 256; s++) {
    cumultative_histo[s] = (float(HKG[s]) / float(HKG[255])) * max_value;
  }

  return cumultative_histo;
}

//to enhance the image for a wider range of the contrast, that the full range of possible pixel values is used
std::vector<std::vector<float>>
histo_linearization(int height, int width,
                    const std::vector<std::vector<float>> &array,
                    const std::vector<float> &cum_hist) {
  std::vector<std::vector<float>> linearized_array =
      create_empty_gray(height, width);
#pragma omp parallel
{
#pragma omp for collapse(2) schedule(static)
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      linearized_array[y][x] =
          cum_hist[int(array[y][x])]; 
    }
  }
}
  return linearized_array;
}


//image filter for reducing the gradiant over the full expansion of the image
std::vector<std::vector<float>>
moving_morm(int height, int width, const std::vector<std::vector<float>> &array,
            int size) {
  int h_size = (size - 1) / 2;
  std::vector<std::vector<float>> normed_array =
      create_empty_gray(height, width);
  std::vector<std::vector<float>> big_array(
      2 * h_size + height,
      std::vector<float>(
          2 * h_size + width,
          -1)); // padding 

#pragma omp parallel 
{ 
//fill the inner area of the padded big_array with the current image
#pragma omp for collapse(2) schedule(static)
  for (int i = h_size; i < h_size + height; i++) {
    for (int j = h_size; j < h_size + width; j++) {
      big_array[i][j] = array[i - h_size][j - h_size];
    }
  }
//divide every pixel values through the mean of its surrounding pixels
#pragma omp for collapse(2) schedule(static)
  for (int y = h_size; y < height + h_size; y++) {
    for (int x = h_size; x < width + h_size; x++) {
      float temp_mean = 0.0;
      int temp_count = 0;
      for (int i = y - h_size; i < y + h_size + 1; i++) {
        for (int j = x - h_size; j < x + h_size + 1; j++) {
          bool is_pos = big_array[i][j] > 0;
          temp_mean += big_array[i][j] * is_pos;
          temp_count += is_pos;
        }
      }
      temp_mean /= temp_count;
      normed_array[y - h_size][x - h_size] = big_array[y][x] / temp_mean;
    }
  }
}
  return normed_array;
}


//decide for every pixel of the image if it is white background or black letters
std::vector<std::vector<bool>>
adaptive_thresh(int height, int width,
                const std::vector<std::vector<float>> &array,
                const std::vector<std::vector<float>> &original_array,
                int sections, float m, float n, float f) {
  std::vector<float> cum_hist = cumultative_hist(original_array);

  std::vector<std::vector<float>> linearized_img =
      histo_linearization(height, width, original_array, cum_hist);

//divide the array in sections 
  std::vector<int> width_sect(sections + 1);
  iota(width_sect.begin(), width_sect.end(), 0);
  transform(width_sect.begin(), width_sect.end(), width_sect.begin(),
            [width, sections](int x) { return x * width / sections; });

  std::vector<int> height_sect(sections + 1);
  iota(height_sect.begin(), height_sect.end(), 0);
  transform(height_sect.begin(), height_sect.end(), height_sect.begin(),
            [height, sections](int x) { return x * height / sections; });

  std::vector<std::vector<bool>> thresh_array(height,
                                              std::vector<bool>(width, false));

//apply the adaptive threshold filter on every section
  for (int ro = 0; ro < sections; ro++) {
    for (int co = 0; co < sections; co++) {
      float var = 0.0;
      float sum = 0.0;
      int count = 0;

      //calculate the mean value of the section
      for (int i = height_sect[ro]; i < height_sect[ro + 1]; i++) {
        for (int j = width_sect[co]; j < width_sect[co + 1]; j++) {
          sum += array[i][j];
          count++;
        }
      }
      float mean = sum / count;

      //calculate the variance of the section
      for (int i = height_sect[ro]; i < height_sect[ro + 1]; i++) {
        for (int j = width_sect[co]; j < width_sect[co + 1]; j++) {
          var += pow(array[i][j] - mean, 2);
        }
      }
      var /= (count - 1);

      //calculate the maximum value of the section
      float b_max = 0;
      for (int i = height_sect[ro]; i < height_sect[ro + 1]; i++) {
        for (int j = width_sect[co]; j < width_sect[co + 1]; j++) {
          b_max = std::max(b_max, linearized_img[i][j]);
        }
      }

      //calculate the threshold and apply on the image
      float thresh = m * var + n + pow(b_max, 0.5) * f;
      for (int i = height_sect[ro]; i < height_sect[ro + 1]; i++) {
        for (int j = width_sect[co]; j < width_sect[co + 1]; j++) {
          bool smaller = array[i][j] > thresh;
          thresh_array[i][j] = smaller;
        }
      }
    }
  }
  return thresh_array;
}

// only for development 
void print_array(const std::vector<std::vector<float>> array) {
  for (int i = 0; i < (int)array.size(); i++) {
    for (int j = 0; j < (int)array[0].size(); j++) {
      std::cout << array[i][j] << ",";
    }
    std::cout << "\n";
  }
}


//read the ppm file into a 2D gray scaled array
std::vector<std::vector<float>> read_ppm(const std::string &input_name) {
  std::ifstream file(input_name, std::ios::binary | std::ios::in);
  // get the file size
  file.seekg(0, file.end);
  int file_size = file.tellg();
  file.seekg(0, file.beg);

  char *buffer = new char[file_size];
  file.read(buffer, file_size);
  int buffer_index = 0;

  // read the magic number
  int magic_number;
  while (buffer[buffer_index] != '\n') {
        if (buffer[buffer_index] - '0' >= 0){
            magic_number = buffer[buffer_index] - '0';
            buffer_index++;
        }
        else{
            buffer_index++;
        }
  }
  buffer_index++;
  if (magic_number != 3) {
    throw std::runtime_error("Invalid file format, expected P3");
  }
  // read the width and height
  int width = 0, height = 0;
  while (buffer[buffer_index] != ' ') {
    width = width * 10 + buffer[buffer_index] - '0';
    buffer_index++;
  }
  buffer_index++;
  while (buffer[buffer_index] != '\n') {
        if (buffer[buffer_index] - '0' >= 0){
            height = height * 10 + buffer[buffer_index] - '0';
            buffer_index++;
        }
        else{
            buffer_index++;
        }
  }
  buffer_index++;

  // read the max color value
  int max_color_value = 0;
  while (buffer[buffer_index] != '\n') {
        if (buffer[buffer_index] - '0' >= 0){
            max_color_value = max_color_value * 10 + buffer[buffer_index] - '0';
            buffer_index++;
        }
        else{
            buffer_index++;
        }
  }
  buffer_index++;

  std::vector<std::vector<float>> image = create_empty_gray(height, width);

  // read the image data
  int x = 0, y = 0;
  while (y < height) {
    int r = 0, g = 0, b = 0;
    while (buffer[buffer_index] != ' ') {
      r = r * 10 + buffer[buffer_index] - '0';
      buffer_index++;
    }
    while (buffer[buffer_index] == ' '){
        buffer_index++;
    }
    while (buffer[buffer_index] != ' ') {
      g = g * 10 + buffer[buffer_index] - '0';
      buffer_index++;
    }
    while (buffer[buffer_index] == ' '){
    buffer_index++;
    }
    while (buffer[buffer_index] != ' ') {
      b = b * 10 + buffer[buffer_index] - '0';
      buffer_index++;
    }
    while (buffer[buffer_index] == ' '){
        buffer_index++;
    }

    if (buffer[buffer_index] == '\n') {
      buffer_index++;
    }

    image[y][x] = rgb2gray_WP(r, g, b);

    x++;
    if (x == width) {
      x = 0;
      y++;
    }
  }

  delete[] buffer;
  file.close();

  return image;
}


//create a ppm P3 file and save the calculated bool array in it
void create_ppm(const std::string &output_name,
                const std::vector<std::vector<bool>> &array, const int height,
                const int width,const int buff_size) {
  std::ofstream file(output_name, std::ios::binary | std::ios::out);

  file << "P3\n" << width << " " << height << "\n" << 1 << "\n";

  const int block_size = buff_size * 1024; // arbitrary chosen
  char buffer[block_size];
  int buffer_offset = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int color = array[y][x] ? 1 : 0;
      buffer_offset +=
          sprintf(buffer + buffer_offset, "%d %d %d ", color, color, color);

      if (buffer_offset >= block_size - 32) {
        file.write(buffer, buffer_offset);
        buffer_offset = 0;
      }
    }
    buffer_offset += sprintf(buffer + buffer_offset, "\n");

    if (buffer_offset >= block_size - 32) {
      file.write(buffer, buffer_offset);
      buffer_offset = 0;
    }
  }

  if (buffer_offset > 0) {
    file.write(buffer, buffer_offset);
  }

  file.close();
}

//only for development
void create_ppm_float(const std::string &output_name,
                      const std::vector<std::vector<float>> &array,
                      const int height, const int width, int buff_size) {
  std::ofstream file(output_name, std::ios::binary | std::ios::out);

  file << "P3\n" << width << " " << height << "\n" << 255 << "\n";

  const int block_size = buff_size * 1024; // arbitrary chosen
  char buffer[block_size];
  int buffer_offset = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int color = array[y][x];
      buffer_offset +=
          sprintf(buffer + buffer_offset, "%d %d %d ", color, color, color);

      if (buffer_offset >= block_size - 32) {
        file.write(buffer, buffer_offset);
        buffer_offset = 0;
      }
    }
    buffer_offset += sprintf(buffer + buffer_offset, "\n");

    if (buffer_offset >= block_size - 32) {
      file.write(buffer, buffer_offset);
      buffer_offset = 0;
    }
  }

  if (buffer_offset > 0) {
    file.write(buffer, buffer_offset);
  }

  file.close();
}
