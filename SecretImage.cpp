#include "SecretImage.h"


// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
    width = image.get_width();
    height = image.get_height();

    int row = image.get_height();
    int col = image.get_width();
    int upper_size = col * (col+1) /2;
    int lower_size = col * (col-1) /2;

    // 1. Dynamically allocate the memory for the upper and lower triangular matrices.
    upper_triangular = new int[upper_size];
    lower_triangular = new int[lower_size];

    int upper_index = 0;
    int low_index = 0;

    // 2. Fill both matrices with the pixels from the GrayscaleImage.
    for (int m=0; m< row; m++) {
        for (int n=0; n< col; n++) {
            if (m<= n) {
                upper_triangular[upper_index] = image.get_pixel(m, n);
                upper_index++;
            }
            else {
                lower_triangular[low_index] = image.get_pixel(m, n);
                low_index++;
            }

        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int w, int h, int * upper, int * lower) {

    // You should simply copy the parameters to instance variables.
    width = w;
    height = h;
    upper_triangular =  upper;
    lower_triangular =lower;
}

// Destructor: free the arrays
SecretImage::~SecretImage() {

    // Simply free the dynamically allocated memory
    // for the upper and lower triangular matrices.

    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices.
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height);

    // Initialize indices to track positions in the upper and lower triangular arrays.
    int upper_index = 0;
    int lower_index = 0;

    // Calculate the sizes of the upper and lower triangular arrays.
    int upper_size = width * (width + 1) / 2;
    int lower_size = width * (width - 1) / 2;

    int size = image.get_height();

    int row = 0;
    int col = 0;

    // Initialize current_n to keep track of how many pixels to set from the upper triangular array.
    int current_n = size;

    // Used to track how many pixels to set from the lower triangular array.
    int lower_increment= 1;

    // Loop until all pixels from the upper and lower triangular arrays have been placed in the image.
    while (current_n >0 && upper_index < upper_size && row <size) {

        // Fill pixels from the upper triangular matrix.
        for (int i =0; i < current_n && upper_index < upper_size && row < size; ++i) {
            // If the current column reaches the image width, move to the next row.
            if (col == size ) {
                row++;
                col = 0;
            }
            // Set pixels from the upper triangular array to the image if still within bounds.
            if (row < size) {
                int upper_set = upper_triangular[upper_index++];
                image.set_pixel(row, col++, upper_set);
            }
        }

        // Fill pixels from the lower triangular matrix.
        for (int j = 0; j < lower_increment && lower_index < lower_size && row < size; ++j) {
            // If the current column reaches the image width, move to the next row.
            if (col == size ) {
                row++;
                col = 0;
            }
            // Set pixels from the lower triangular array to the image if still within bounds.
            if (row < size) {
                int lower_set = lower_triangular[lower_index++];
                image.set_pixel(row, col++, lower_set);
            }
        }

        // Decrease the number of pixels to set from the upper triangular matrix for the next row.
        current_n--;

        // Increase the number of pixels to set from the lower triangular matrix.
        lower_increment++;
    }
    return image;
}

// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {

    // Update the lower and upper triangular matrices
    // based on the GrayscaleImage given as the parameter.

    // Get the height and width of the input image
    int row = image.get_height();
    int col = image.get_width();

    // Calculate the sizes of the upper and lower triangular matrices
    int upper_size = col * (col + 1) / 2;
    int lower_size = col * (col - 1) / 2;

    // Initialize indices for tracking positions in the triangular matrices
    int upper_index = 0;
    int low_index = 0;

    // Iterate through each pixel of the input image
    for (int m = 0; m < row; m++) {
        for (int n = 0; n < col; n++) {
            // Check if the current pixel belongs to the upper triangular part
            if (m <= n && upper_index < upper_size) {
                // Update upper triangular matrix
                upper_triangular[upper_index] = image.get_pixel(m, n);
                upper_index++;
            }
            // Check if the current pixel belongs to the lower triangular part
            else if (low_index < lower_size) {
                // Update lower triangular matrix
                lower_triangular[low_index] = image.get_pixel(m, n);
                low_index++;
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {

    // Open the output file stream
    std::ofstream outfile(filename);

    // Check if the file opened successfully
    if (outfile.is_open()) {
        // 1. Write width and height on the first line, separated by a single space.
        outfile << get_width() << " " << get_height() << std::endl;

        int upper_size = width * (width + 1) / 2;

        // 2. Write the upper_triangular array to the second line.
        for (int i = 0; i < upper_size; i++) {
            outfile << upper_triangular[i]; // Write each element of the upper triangular array
            if (i < upper_size - 1) { // If it's not the last element, add a space
                outfile << " ";
            }
        }
        outfile << std::endl;

        int lower_size = width * (width - 1) / 2;

        // 3. Write the lower_triangular array to the third line in a similar manner as the second line.
        for (int i = 0; i < lower_size; i++) {
            outfile << lower_triangular[i];
            if (i < lower_size - 1) {
                outfile << " ";
            }
        }
        // Close the output file stream
        outfile.close();
    }
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {

    // 1. Open the file and read width and height from the first line, separated by a space.
    std::ifstream infile(filename);

    int width = 0, height = 0;
    infile >> width >> height;

    // 2. Calculate the sizes of the upper and lower triangular arrays.
    int upper_size = width * (width + 1) / 2;
    int lower_size = width * (width - 1) / 2;

    // 3. Allocate memory for both arrays.
    int* upper_triangular = new int[upper_size];
    int* lower_triangular = new int[lower_size];

    // 4. Read the upper_triangular array from the second line, space-separated.
    for (int i = 0; i < upper_size; ++i) {
        infile >> upper_triangular[i];
    }

    // 5. Read the lower_triangular array from the third line, space-separated.
    for (int i = 0; i < lower_size; ++i) {
        infile >> lower_triangular[i];
    }

    // 6. Close the file and return a SecretImage object initialized with the
    //    width, height, and triangular arrays.
    infile.close();
    SecretImage secret_image(width, height, upper_triangular, lower_triangular);

    return secret_image;
}


// Returns a pointer to the upper triangular part of the secret image.
int * SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int * SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}