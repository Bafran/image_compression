#include <stdio.h>
#include "stdlib.h"
#include "compression.h"
#include "image.h"

int main() {
    Image img;
    Image_load(&img, "C:\\Users\\adelc\\Desktop\\compression_v2\\sky.jpg");
    compress_image(&img);

    Image_free(&img);

    return 0;
}
