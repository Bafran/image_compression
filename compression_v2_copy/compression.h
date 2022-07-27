#pragma once

#include "image.h"

enum Type_t {
    LUM,
    COL
};

// Compress full image
void compress_image(Image *img);

void compression_init();
void compress_MCU(uint8_t *mcu_array);
void compression_exit();

float **calloc_mat(int dimX, int dimY);
void free_mat(float **m);

