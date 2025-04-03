#include "GrayscaleImage.h"
#include <iostream>
#include <cstring>  // For memcpy
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdexcept>


// Constructor: load from a file
GrayscaleImage::GrayscaleImage(const char* filename) {

    // Image loading code using stbi
    int channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_grey);

    if (image == nullptr) {
        std::cerr << "Error: Could not load image " << filename << std::endl;
        exit(1);
    }

    // Dynamically allocate memory for a 2D matrix based on the given dimensions.
    data = new int*[height];
    for (int i = 0; i < height; i++) {
        data[i] = new int[width];
    }

    // Fill the matrix with pixel values from the image
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int index = (width * row) + col;
            unsigned char pixel_value = image[index];
            data[row][col] = static_cast<int>(pixel_value);
        }
    }

    // Free the dynamically allocated memory of stbi image
    stbi_image_free(image);
}

// Constructor: initialize from a pre-existing data matrix
GrayscaleImage::GrayscaleImage(int** inputData, int h, int w) {

    // Initialize the image with a pre-existing data matrix by copying the values.
    // Don't forget to dynamically allocate memory for the matrix.

    // Set height and width of the image.
    height = h;
    width = w;

    // Allocate dynamic memory for a 2D matrix.
    data = new int*[height];
    for (int i = 0; i < height; i++) {
        data[i] = new int[width];
    }

    // Copy the values from the input data matrix to the new matrix.
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            data[row][col] = inputData[row][col];
        }
    }
}

// Constructor to create a blank image of given width and height
GrayscaleImage::GrayscaleImage(int w, int h) : width(w), height(h) {

    // Just dynamically allocate the memory for the new matrix.
    data = new int*[height];
    for (int i = 0; i < height; i++) {
        data[i] = new int[width];
    }

    // Initialize all pixels to 0 to create a blank image
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            data[row][col] = 0;
        }
    }
}

// Copy constructor
GrayscaleImage::GrayscaleImage(const GrayscaleImage& other) {

    // Copy constructor: dynamically allocate memory and
    // copy pixel values from another image.

    width = other.width;
    height = other.height;

    data = new int*[height];
    for (int i = 0; i < height; i++) {
        data[i] = new int[width];
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            data[row][col] = other.data[row][col];
        }
    }
}

// Destructor: Free dynamically allocated memory for matrix
GrayscaleImage::~GrayscaleImage() {

    // Destructor: deallocate memory for the matrix.

    for (int i = 0; i < height; i++) {
        delete[] data[i];
    }
    delete[] data;
}

// Equality operator
bool GrayscaleImage::operator==(const GrayscaleImage& other) const {

    // Check if two images have the same dimensions and pixel values.
    // If they do, return true.

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (data[row][col] != other.data[row][col]) {
                return false;
            }
        }
    }
    return true;
}

// Addition operator
GrayscaleImage GrayscaleImage::operator+(const GrayscaleImage& other) const {

    // Create a new image for the result
    GrayscaleImage result(width, height);

    // Add two images' pixel values and return a new image, clamping the results.

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int sum = data[row][col] + other.data[row][col];
            if (sum > 255) {
                sum = 255;
            } else if (sum < 0) {
                sum = 0;
            }
            result.set_pixel(row, col, sum);
        }
    }
    return result;
}

// Subtraction operator
GrayscaleImage GrayscaleImage::operator-(const GrayscaleImage& other) const {

    // Create a new image for the result
    GrayscaleImage result(width, height);

    // Subtract pixel values of two images and return a new image, clamping the results.
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int diff = data[row][col] - other.data[row][col] ;
            if (diff > 255) {
                diff = 255;
            } else if (diff < 0) {
                diff = 0;
            }
            result.set_pixel(row, col, diff);
        }
    }
    return result;
}

// Get a specific pixel value
int GrayscaleImage::get_pixel(int row, int col) const {
    return data[row][col];
}

// Set a specific pixel value
void GrayscaleImage::set_pixel(int row, int col, int value) {
    data[row][col] = value;
}

// Function to save the image to a PNG file
void GrayscaleImage::save_to_file(const char* filename) const {
    // Create a buffer to hold the image data in the format stb_image_write expects
    unsigned char* imageBuffer = new unsigned char[width * height];

    // Fill the buffer with pixel data (convert int to unsigned char)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            imageBuffer[i * width + j] = static_cast<unsigned char>(data[i][j]);
        }
    }

    // Write the buffer to a PNG file
    if (!stbi_write_png(filename, width, height, 1, imageBuffer, width)) {
        std::cerr << "Error: Could not save image to file " << filename << std::endl;
    }

    // Clean up the allocated buffer
    delete[] imageBuffer;
}
