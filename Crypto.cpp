#include "Crypto.h"
#include "GrayscaleImage.h"


// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array;

    // 1. Reconstruct the SecretImage to a GrayscaleImage.
    GrayscaleImage reconstructed_image = secret_image.reconstruct();

    // 2. Calculate the image dimensions.
    int width = reconstructed_image.get_width();
    int height = reconstructed_image.get_height();

    // Calculate the total number of pixels in the image.
    int total_pixel = width * height;

    // 3. Determine the total bits required based on message length.
    int total_bits = message_length * 7;

    // 4. Ensure the image has enough pixels; if not, throw an error.
    if (total_bits > total_pixel) {
        std::cerr << "ERROR: Message has too many pixels." << std::endl;
    }

    // 5. Calculate the starting pixel from the message_length knowing that
    //    the last LSB to extract is in the last pixel of the image.
    int start_index = total_pixel - total_bits;

    // 6. Extract LSBs from the image pixels and return the result.
    if (start_index > 0) {
        for (int index = start_index; index < total_pixel; ++index) {
            int i = index / width; // Calculate row index
            int j = index % width; // Calculate column index

            // Get the pixel value and extract the least significant bit.
            int pixel = reconstructed_image.get_pixel(i, j);
            int pixel_LSB = pixel & 1;

            LSB_array.push_back(pixel_LSB);
        }
    } else {
        // If start_index is less than or equal to 0, extract all pixels from the image.
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Get the pixel value and extract the least significant bit.
                int pixel = reconstructed_image.get_pixel(i, j);
                int pixel_LSB = pixel & 1; // Extract the LSB using bitwise AND operation.

                LSB_array.push_back(pixel_LSB);
            }
        }
    }
    return LSB_array;
}

// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message;

    // 1. Verify that the LSB array size is a multiple of 7, else throw an error.
    if (LSB_array.size() % 7 != 0) {
        std::cerr << ("ERROR: Message cannot be divided by 7.");
    }

    // 2. Convert each group of 7 bits into an ASCII character.
    for (int i = 0; i < LSB_array.size(); i += 7) {
        std::bitset<7> bits;

        // Populate the bitset with bits from LSB array.
        // Bits are inserted in reverse order to ensure correct endianness.
        for (int j = 0; j < 7; ++j) {
            bits[6 - j] = LSB_array[i + j]; // Fill bits from MSB to LSB.
        }

        // Convert the bitset to an ASCII character.
        char character = static_cast<char>(bits.to_ulong());

        // 3. Collect the characters to form the decrypted message.
        message += character;
    }

    // 4. Return the resulting message.
    return message;
}


// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array;

    // 1. Convert each character of the message into a 7-bit binary representation
    for (char c : message) {
        std::bitset<7> bits(c);

        // 2. Collect the bits into the LSB array.
        for (int i = 6; i >= 0; --i) {
            LSB_array.push_back(bits[i]);
        }
    }
    // 3. Return the array of bits.
    return LSB_array;
}


// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {

    int width = image.get_width();
    int height = image.get_height();
    int total_pixel = width * height;

    // 1. Ensure the image has enough pixels to store the LSB array, else throw an error.
    if (total_pixel < static_cast<int>(LSB_array.size())) {
        std::cerr << "ERROR: Can't hold LSB array." << std::endl;
    }

    // 2. Find the starting pixel based on the message length knowing that
    //    the last LSB to embed should end up in the last pixel of the image.
    int start_index = total_pixel - static_cast<int>(LSB_array.size());

    int LSB_index = 0;

    // 3. Iterate over the image pixels, embedding LSBs from the array.
    if (start_index > 0) {
        for (int index = start_index; index < total_pixel; ++index) {
            int i = index / width; // Calculate the row index.
            int j = index % width; // Calculate the column index.
            int pixel = image.get_pixel(i, j);

            // Convert pixel value to an 8-bit bitset to modify the LSB.
            std::bitset<8> pixel_bit_value(pixel);

            // Get the next bit from the LSB array to embed.
            int bit = LSB_array[LSB_index++];

            pixel_bit_value.set(0, bit);
            // Convert the modified bitset back to an integer pixel value.
            pixel = static_cast<int>(pixel_bit_value.to_ulong());
            image.set_pixel(i, j, pixel);
        }
    }
    else {
        // If starting index is 0 or less, iterate over the entire image and embed LSBs.
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int pixel = image.get_pixel(i, j);

                // Convert pixel value to an 8-bit bitset to modify the LSB.
                std::bitset<8> pixel_bit_value(pixel);
                int bit = LSB_array[LSB_index++];
                pixel_bit_value.set(0, bit);

                // Convert the modified bitset back to an integer pixel value.
                pixel = static_cast<int>(pixel_bit_value.to_ulong());
                image.set_pixel(i, j, pixel);
            }
        }
    }

    // 4. Return a SecretImage object constructed from the given GrayscaleImage
    //    with the embedded message.
    SecretImage secret_image(image);
    return secret_image;
}