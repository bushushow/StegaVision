#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>

// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {

    // 1. Copy the original image for reference.
    GrayscaleImage copyImage = image;

    // 2. For each pixel, calculate the mean value of its neighbors using a kernel.
    int row = image.get_height();
    int col = image.get_width();

    // Calculate the edge size of the kernel.
    int edge = (kernelSize-1) / 2;

    // Iterate through each pixel in the image.
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            int sum = 0;
            for (int i = -edge; i <= edge; i++) {
                for (int j = -edge; j <= edge; j++) {
                    int neighborRow = r + i;
                    int neighborCol = c + j;

                    // Check if the neighboring pixel is within the bounds of the image.
                    if (neighborRow >= 0 && neighborRow < row &&
                        neighborCol >= 0 && neighborCol < col) {

                        // Add the value of the neighboring pixel to the sum.
                        sum += copyImage.get_pixel(neighborRow, neighborCol);
                        }
                }
            }
            // Calculate the mean value by dividing the sum by the total number of pixels in the kernel.
            int mean = sum / (kernelSize * kernelSize);

            // 3. Update each pixel with the computed mean.
            image.set_pixel(r, c, mean);
        }
    }
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {

    int row = image.get_height();
    int col = image.get_width();
    int edge = (kernelSize - 1) / 2;
    double** kernel;

    // 1. Create a Gaussian kernel based on the given sigma value.
    kernel = new double*[kernelSize];
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = new double[kernelSize];
    }

    double sum = 0.0;

    // Calculate the values for the Gaussian kernel and keep track of their sum.
    for (int x = -edge; x <= edge; ++x) {
        for (int y = -edge; y <= edge; ++y) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma);
            kernel[x + edge][y + edge] = exp(exponent) / (2 * M_PI * sigma * sigma);
            sum += kernel[x + edge][y + edge];
        }
    }

    // Make a copy of the original image to avoid modifying pixel values during calculation.
    GrayscaleImage copyImage = image;

    // 3. For each pixel, compute the weighted sum using the kernel.
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            double weightedSum = 0.0;

            // Iterate through the kernel and calculate the weighted sum of neighbors.
            for (int i = -edge; i <= edge; ++i) {
                for (int j = -edge; j <= edge; ++j) {
                    int neighborRow = r + i;
                    int neighborCol = c + j;

                    // Ensure the neighboring pixel is within bounds.
                    if (neighborRow >= 0 && neighborRow < row && neighborCol >= 0 && neighborCol < col) {
                        weightedSum += copyImage.get_pixel(neighborRow, neighborCol) * kernel[i + edge][j + edge] / sum; // 2. Normalize the kernel to ensure it sums to 1.
                    }
                }
            }
            // 4. Update the pixel values with the smoothed results.
            image.set_pixel(r, c, static_cast<int>(weightedSum));
        }
    }

    // Free the allocated memory for the Gaussian kernel.
    for (int i = 0; i < kernelSize; i++) {
        delete[] kernel[i]; // Free each row of the kernel.
    }
    delete[] kernel; // Free the kernel pointer array itself.
}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {

    // Create a copy of the original image to apply Gaussian smoothing.
    GrayscaleImage gaussianImage = image;

    // 1. Blur the image using Gaussian smoothing, use the default sigma given in the header.
    apply_gaussian_smoothing(gaussianImage, kernelSize, 1);

    int row = image.get_height();
    int col = image.get_width();

    // 2. For each pixel, apply the unsharp mask formula: original + amount * (original - blurred).
    for (int r = 0; r < row; ++r) {
        for (int c = 0; c < col; ++c) {
            int originalPixel = image.get_pixel(r, c);
            int gaussianPixel = gaussianImage.get_pixel(r, c);

            // 2. For each pixel, apply the unsharp mask formula: original + amount * (original - blurred).
            double maskedPixel = originalPixel + amount * (originalPixel - gaussianPixel);

            // 3. Clip values to ensure they are within a valid range [0-255].
            if (maskedPixel > 255) {
                maskedPixel = 255;
            }
            if (maskedPixel < 0) {
                maskedPixel = 0;
            }
            image.set_pixel(r, c, static_cast<int>(maskedPixel));
        }
    }
}