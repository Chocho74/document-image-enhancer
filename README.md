# document-image-enhancer
Conception, development and optimization of an image quality enhancement program

# Purpose
This is an university related project. The aim of this software is to enhance images of dokuments.
After the initial programm in python, the task was to rewrite it in c++ and optimize it with several techniques like:
- parallelisation
- vector instructions
- ideal compiler flags

# Results
The results are written in a paper, which is also part of the repository, but in german language.
I am sure with your favourite AI tool it is translateable.

# Cloning

This repository can be cloned using

    git clone https://github.com/Chocho74/document-image-enhancer.git

This pulls down the `paper`, `project` directories. The last directory
contains the program-.
# Building

The build files for this program can be created via *CMake*, the specific command is

    cmake -DCMAKE_BUILD_TYPE=Release .
    
After that, the compilation can be started with `make -k`.
This compiles the `main.cpp` file, that then compiles the source code, contained
in the `image_enhancer_lib` directory. 

# Usage

The build binary can then be used to enhance images and bring them into a good, printable form.
These images must be in form of a `ppm` file of version *P3* for this program to work.

It hat takes 4 command line arguments:
1. the parameter m
2. the parameter n
3. the path to the input image
4. the path of the generated output image

The first two parameters control how the algorithms deal with the brightness gradient 
of the input image, thus affecting the outcome of the resulting image.
These are highly sensitive and need tuning for ideal ad-hoc image enhancing.

If these are both set to 0, they default to sensible values, printed by the execution call of
the program. They then need to be adjusted to achieve the most pristine quality of the resulting
image.

A basic execution of the command with it's command line arguments looks like this:

    ./enhance 0 0 path/to/input_image.ppm path/to/output_image.ppm

Some further guidance of how to tune those parameters can be found in the corresponding paper,
also found in this repository.
