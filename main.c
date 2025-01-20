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

void write_bmp(char *file_name, bmp_data bmp) {
    // Open the output BMP file in binary mode
    FILE *outputFile = fopen(file_name, "wb");
    if (!outputFile) {
        printf("Error opening file %s\n", file_name);
        fclose(outputFile);
    }

    // Write bmp header to output file
    fwrite(&bmp.bmp_header, sizeof(BITMAPFILEHEADER), 1, outputFile);

    // Write dib header to output file
    fwrite(&bmp.dib_header, sizeof(BITMAPV5HEADER), 1, outputFile);

    // Write the modified pixel data to the output file
    for (int i = 0; i < bmp.dib_header.bV5Height; i++) {
        // Write row to output file
        fwrite(bmp.image[i], sizeof(RGBTRIPLE), bmp.dib_header.bV5Width, outputFile);

        // Determine padding for scanlines
        int padding = (4 - (bmp.dib_header.bV5Width * sizeof(RGBTRIPLE)) % 4) % 4;
        // Write padding at end of row
        for (int k = 0; k < padding; k++) {
            fputc(0x00, outputFile);
        }
    }

    // Close the output file
    fclose(outputFile);
}

void free_bmp(bmp_data bmp) {
    // Free memory for image
    for (int i = 0; i < bmp.dib_header.bV5Height; i++) {
        free(bmp.image[i]);
    }
    free(bmp.image);

}

int main() {
    double COLOR[COLOR_NUMBER][RGB_COLOR_VOLUME] = {
            {255, 0,   0}, //RED
            {255, 125, 0}, //ORANGE
            {255, 255, 0}, //YELLOW
            {0,   255, 0}, //GREEN
            {0,   255, 255}, //CYAN
            {0,   125, 255}, //BLUE
            {125, 0,   255} //PURPLE
    };

    bmp_data bmp;
    char fileName[100];
    char filePath[100];
    char processedFilePath[100];
    int userChoice;
    int brightenFactor;
    int saturateFactor;
    double thresholdFactor;
    int colorFilterChoice;

    printf("Please enter the file name:\n");
    scanf("%s", fileName);
    snprintf(filePath, sizeof(filePath), "../image/%s.bmp", fileName);
    bmp = read_bmp(filePath);
    printf("Please choose a prefer way to process the picture (Enter a number):\n");
    printf("1.grayScale 2.reflect 3.sepia 4.brighten 5.blur 6.saturate 7.thresholdFilter 8.colorFilter 9.invert\n");
    scanf("%d", &userChoice);

    switch (userChoice) {
        case 1:
            grayscale(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-grayscale.bmp", fileName);
            break;
        case 2:
            reflect(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-reflect.bmp", fileName);
            break;
        case 3:
            sepia(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-sepia.bmp", fileName);
            break;
        case 4:
            printf("Enter a positive number to brighten or a negative number to darken the picture(-255~255):\n");
            while (scanf("%d", &brightenFactor) != 1) {
                printf("It is not an integer, try again:\n");
                while (getchar() != '\n');
            }
            brighten(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image, brightenFactor);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-brighten.bmp", fileName);
            break;
        case 5:
            blur(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-blur.bmp", fileName);
            break;
        case 6:
            printf("Enter a positive number to change the saturation:\n");
            while (scanf("%d", &saturateFactor) != 1) {
                printf("It is not an integer, try again:\n");
                while (getchar() != '\n');
            }
            saturate(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image, saturateFactor);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-saturate.bmp", fileName);
            break;
        case 7:
            printf("Enter a decimal number from 0.0~1.0 to apply the threshold filter:\n");
            while (scanf("%lf", &thresholdFactor) != 1 || thresholdFactor < 0.0 || thresholdFactor > 1.0) {
                printf("Invalid input, try again:\n");
                while (getchar() != '\n');
            }
            thresholdFilter(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image, thresholdFactor);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-thresholdFilter.bmp", fileName);
            break;
        case 8:
            printf("Choose a color filter:\n");
            printf("1.RED 2.ORANGE 3.YELLOW 4.GREEN 5.CYAN 6.BLUE 7.PURPLE\n");
            while (scanf("%d", &colorFilterChoice) != 1 || colorFilterChoice < 1 || colorFilterChoice > 7) {
                printf("Invalid input, try again:\n");
                while (getchar() != '\n');
            }
            colorFilter(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image, COLOR[colorFilterChoice - 1]);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-colorFilter.bmp", fileName);
            break;
        case 9:
            invert(bmp.dib_header.bV5Height, bmp.dib_header.bV5Width, bmp.image);
            snprintf(processedFilePath, sizeof(filePath), "../image/%s-invert.bmp", fileName);
            break;
    }
    write_bmp(processedFilePath, bmp);
    free_bmp(bmp);
}