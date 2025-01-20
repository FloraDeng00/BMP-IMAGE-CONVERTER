#include <malloc.h>
#include "helpers.h"
#include "math.h"
#include "stdio.h"

#define RED ((Color){255, 0, 0})
#define ORANGE {255, 165, 0}
#define YELLOW {255, 255, 0}
#define GREEN {0, 255, 0}
#define CYAN {0, 255, 255}
#define BLUE {0, 0, 255}
#define PURPLE {128, 0, 128}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE **image) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int rgb = round((image[i][j].rgbtBlue + image[i][j].rgbtRed + image[i][j].rgbtGreen) / 3.0);
            image[i][j].rgbtBlue = rgb;
            image[i][j].rgbtRed = rgb;
            image[i][j].rgbtGreen = rgb;
        }
    }
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE **image) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int sR = round(0.393 * image[i][j].rgbtRed + 0.769 * image[i][j].rgbtGreen + 0.189 * image[i][j].rgbtBlue);
            int sG = round(0.349 * image[i][j].rgbtRed + 0.686 * image[i][j].rgbtGreen + 0.168 * image[i][j].rgbtBlue);
            int sB = round(0.272 * image[i][j].rgbtRed + 0.534 * image[i][j].rgbtGreen + 0.131 * image[i][j].rgbtBlue);
            if (sR > 255) {
                sR = 255;
            }
            if (sG > 255) {
                sG = 255;
            }
            if (sB > 255) {
                sB = 255;
            }
            image[i][j].rgbtBlue = sB;
            image[i][j].rgbtRed = sR;
            image[i][j].rgbtGreen = sG;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE **image) {
    // Iterate over each row in the image
    for (int i = 0; i < height; i++) {
        // Iterate over each column in the row, but only up to half the width
        for (int j = 0; j < width / 2; j++) {
            // Swap the pixels across the vertical axis (left and right)
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }
}


// Blur image
void blur(int height, int width, RGBTRIPLE **image) {
    int intensity = 0;
    char input[10]; // Buffer for the input

    while (1) {
        printf("Enter blur intensity (1 to 5): ");
        if (scanf("%d", &intensity) > 1 || intensity < 1 || intensity > 5) {
            printf("invalid, try again\n");
            while (getchar() != '\n');
        } else {
            break;
        }

    }// end of function


    // Map the intensity to a range value. Intensity is scaled by 10.
    int range = intensity * 10;

    // Dynamically allocate memory for the temporary array
    RGBTRIPLE (*temp)[width] = malloc(height * sizeof(RGBTRIPLE[width]));
    if (temp == NULL) {
        printf("Error: Not enough memory to store temporary image.\n");
        return;
    }

    // Loop through each pixel in the original image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            long totalR = 0, totalG = 0, totalB = 0;
            int count = 0;

            // Adjust the loop based on the range
            for (int r = -range; r <= range; r++) {
                for (int c = -range; c <= range; c++) {
                    int currentRow = i + r;
                    int currentCol = j + c;

                    // Check if the current pixel is within bounds and accumulate values
                    if (currentRow >= 0 && currentRow < height && currentCol >= 0 && currentCol < width) {
                        RGBTRIPLE pixel = image[currentRow][currentCol];
                        totalR += pixel.rgbtRed;
                        totalG += pixel.rgbtGreen;
                        totalB += pixel.rgbtBlue;
                        count++;
                    }
                }
            }

            // Calculate the average color values for the surrounding pixels
            temp[i][j].rgbtRed = count > 0 ? round((float) totalR / count) : image[i][j].rgbtRed;
            temp[i][j].rgbtGreen = count > 0 ? round((float) totalG / count) : image[i][j].rgbtGreen;
            temp[i][j].rgbtBlue = count > 0 ? round((float) totalB / count) : image[i][j].rgbtBlue;
        }
    }

    // Copy the blurred image from temp to the original image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = temp[i][j];
        }
    }

    // Free the memory allocated for the temp image
    free(temp);
}

BYTE constraint(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    } else {
        return value;
    }
}

RGBTRIPLE generate_rgb(int red, int green, int blue) {
    RGBTRIPLE rgb;
    rgb.rgbtRed = constraint(red);
    rgb.rgbtGreen = constraint(green);
    rgb.rgbtBlue = constraint(blue);
    return rgb;
}

void brighten(int height, int width, RGBTRIPLE **image, int unit) {
    // Loop through each pixel in the original image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = generate_rgb((int) image[i][j].rgbtRed + unit,
                                       (int) image[i][j].rgbtGreen + unit,
                                       (int) image[i][j].rgbtBlue + unit);
        }
    }
}

void saturate(int height, int width, RGBTRIPLE **image, int unit) {
    // Loop through each pixel in the original image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = generate_rgb((int) image[i][j].rgbtRed * (100 + unit) / 100,
                                       (int) image[i][j].rgbtGreen * (100 + unit) / 100,
                                       (int) image[i][j].rgbtBlue * (100 + unit) / 100);
        }
    }
}

//threshold is a double from 0.0 to 1.0
void thresholdFilter(int height, int width, RGBTRIPLE **image, double threshold) {
    double rbgThreshold = 255 * threshold;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int rgb = round((image[i][j].rgbtBlue + image[i][j].rgbtRed + image[i][j].rgbtGreen) / 3.0);
            if (rgb < rbgThreshold) {
                image[i][j].rgbtBlue = 0;
                image[i][j].rgbtRed = 0;
                image[i][j].rgbtGreen = 0;
            } else {
                image[i][j].rgbtBlue = 255;
                image[i][j].rgbtRed = 255;
                image[i][j].rgbtGreen = 255;
            }

        }
    }
}

//colorRGB is a double array with 3 elements, representing 3 rgb colors
void colorFilter(int height, int width, RGBTRIPLE **image, double const colorRGB[]) {
    double redFactor = 1 + colorRGB[0] / 255;
    double greenFactor = 1 + colorRGB[1] / 255;
    double blueFactor = 1 + colorRGB[2] / 255;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int newRed = (int) (image[i][j].rgbtRed * redFactor);
            int newGreen = (int) (image[i][j].rgbtGreen * greenFactor);
            int newBlue = (int) (image[i][j].rgbtBlue * blueFactor);
            image[i][j].rgbtRed = (BYTE) constraint(newRed);
            image[i][j].rgbtGreen = (BYTE) constraint(newGreen);
            image[i][j].rgbtBlue = (BYTE) constraint(newBlue);
        }
    }
}

//invert colour
void invert(int height, int width, RGBTRIPLE **image) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            image[i][j].rgbtBlue = 255 - image[i][j].rgbtBlue;
            image[i][j].rgbtRed = 255 - image[i][j].rgbtRed;
            image[i][j].rgbtGreen = 255 - image[i][j].rgbtGreen;
        }
    }
}