#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern void     stbi_image_free(void *retval_from_stbi_load);
extern void     stbi_set_flip_vertically_on_load(int flag_true_if_flipped);

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>

void stbi_set_flip_vertically_on_load(int flag_true_if_flipped) {
   (void)flag_true_if_flipped;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
    (void)filename;
    *x = 16;
    *y = 16;
    *channels_in_file = 4;
    int channels = desired_channels ? desired_channels : 4;
    stbi_uc *data = (stbi_uc*)malloc(16 * 16 * channels);
    if (!data) return NULL;
    for (int i = 0; i < 16 * 16; ++i) {
        data[i * channels + 0] = 34;   // R (Dirt/Grass tint)
        data[i * channels + 1] = 139;  // G
        data[i * channels + 2] = 34;   // B
        if (channels == 4) data[i * channels + 3] = 255;
    }
    return data;
}

void stbi_image_free(void *retval_from_stbi_load) {
    if (retval_from_stbi_load) free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // STBI_INCLUDE_STB_IMAGE_H
