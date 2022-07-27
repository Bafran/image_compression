#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image_resize.h"

#include "stdint.h"
#include "stdio.h"

static FILE *fptr;

int main() {
    int height;
    int width;
    int channels;

    uint8_t *data = stbi_load("C:\\Users\\adelc\\CLionProjects\\new_test\\camera.jpg", &height, &width, &channels, 0);

    stbi_write_jpg("C:\\Users\\adelc\\CLionProjects\\new_test\\output.jpg", height, width, channels, data, 15);

    return 0;
}
