#pragma once

#include "image.h"

enum Type_t {
    LUM,
    COL
};

// Compress full image
void compress_image(Image *img);

void block_dct(float **Matrix, int N, int M);
void quantize(float **Matrix, enum Type_t type);
void huffman_encode(float **Matrix, enum Type_t type);

float **calloc_mat(int dimX, int dimY);
void free_mat(float **m);

