#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include "stb/stb_image.h"

int main() {
    int width, height, channels;
    unsigned char* data = stbi_load("assets/grises/geometria_gris_2.png", &width, &height, &channels, 0);
    if (data) {
        printf("Successfully loaded image: %dx%d, channels: %d\n", width, height, channels);
        stbi_image_free(data);
    } else {
        printf("Failed to load image: %s\n", stbi_failure_reason());
    }
    
    // Try forcing to grayscale
    data = stbi_load("assets/grises/geometria_gris_2.png", &width, &height, &channels, 1);
    if (data) {
        printf("Successfully loaded as grayscale: %dx%d, channels: %d\n", width, height, channels);
        stbi_image_free(data);
    } else {
        printf("Failed to load as grayscale: %s\n", stbi_failure_reason());
    }
    
    // Try forcing to RGB
    data = stbi_load("assets/grises/geometria_gris_2.png", &width, &height, &channels, 3);
    if (data) {
        printf("Successfully loaded as RGB: %dx%d, channels: %d\n", width, height, channels);
        stbi_image_free(data);
    } else {
        printf("Failed to load as RGB: %s\n", stbi_failure_reason());
    }
    
    return 0;
}