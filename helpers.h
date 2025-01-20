#include "bmp.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE **image);

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE **image);

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE **image);

// Blur image
void blur(int height, int width, RGBTRIPLE **image);

// Increase or decrease brightness
void brighten(int height, int width, RGBTRIPLE **image, int unit);

// Increase or decrease saturation
void saturate(int height, int width, RGBTRIPLE **image, int unit);

void thresholdFilter(int height, int width, RGBTRIPLE **image, double threshold);

// combine 2 same size image together, image blend
void colorFilter(int height, int width, RGBTRIPLE **image, double const color[]);

void invert(int height, int width, RGBTRIPLE **image);