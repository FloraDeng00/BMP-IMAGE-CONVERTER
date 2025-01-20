#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helpers.h"

#define COLOR_NUMBER 7
#define RGB_COLOR_VOLUME 13

typedef struct {
    BITMAPFILEHEADER bmp_header;
    BITMAPV5HEADER dib_header;
    RGBTRIPLE **image;
} bmp_data;

bmp_data read_bmp(char *file_name) {
    FILE *inputFile;
    bmp_data bmp = {0};

    // Open the BMP inputFile in binary mode
    inputFile = fopen(file_name, "rb");
    if (!inputFile) {
        printf("Error opening inputFile %s\n", file_name);
        fclose(inputFile);
        return bmp;
    }

    // Obtain the Bitmap File header and DIB header(V5)
    if (fread(&bmp.bmp_header, sizeof(BITMAPFILEHEADER), 1, inputFile) != 1) {
        printf("Error reading BMP header\n");
        fclose(inputFile);
        return bmp;
    }

    // Check if the file is BMP
    if (bmp.bmp_header.bfType != 0x4D42) {
        printf("Unexpected signature: not BMP\n");
        fclose(inputFile);
        return bmp;
    }

    // Read the BITMAPV5HEADER structure
    if (fread(&bmp.dib_header, sizeof(BITMAPV5HEADER), 1, inputFile) != 1) {
        printf("Error reading DIB header\n");
        fclose(inputFile);
        return bmp;
    }

    int width = bmp.dib_header.bV5Width;
    int height = bmp.dib_header.bV5Height;

    printf("width: %d, height: %d \n", width, height);

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Allocate memory for image
    bmp.image = malloc(height * sizeof(RGBTRIPLE *));
    if (bmp.image == NULL) {
        printf("Not enough memory to store image.\n");
        fclose(inputFile);
        return bmp;
    }

    for (int i = 0; i < height; i++) {
        bmp.image[i] = malloc(width * sizeof(RGBTRIPLE));
        if (bmp.image[i] == NULL) {
            printf("Not enough memory to store image.\n");
            fclose(inputFile);
            while (--i >= 0) free(bmp.image[i]);
            free(bmp.image);
            return bmp;
        }
    }

    // Read image data
    for (int i = 0; i < height; i++) {
        // Read row into pixel array
        fread(bmp.image[i], sizeof(RGBTRIPLE), width, inputFile);

        // Skip over padding
        fseek(inputFile, padding, SEEK_CUR);
    }

    // Close the input file
    fclose(inputFile);

    return bmp;
}

