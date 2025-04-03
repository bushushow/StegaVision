# StegaVision

## Overview
**StegaVision** is a C++-based image enhancement and encryption system developed for defense-grade image analysis and covert communication. It utilizes grayscale image filtering and LSB (Least Significant Bit) steganography to embed secret messages inside images while enhancing visual clarity through noise reduction and sharpening techniques.

## Features
- 🧠 **Image Filters**:
  - Mean Filter (Noise smoothing)
  - Gaussian Filter (Edge-preserving smoothing)
  - Unsharp Masking (Image sharpening)
- 🕵️‍♂️ **Steganography**:
  - LSB-based message embedding and extraction
  - Secure `.dat` format for disguised image storage
- 🧮 **Grayscale Image Matrix Manipulation**:
  - Dynamic memory management
  - Upper and Lower Triangular Matrix storage for secure encoding

## Installation

```bash
git clone https://github.com/bushushow/StegaVision.git
cd StegaVision
g++ -std=c++11 -o clearvision main.cpp SecretImage.cpp GrayscaleImage.cpp Filter.cpp Crypto.cpp
```

## Usage

After compilation, run the program using one of the following commands:
```bash
./clearvision mean input.png 3
./clearvision gauss input.png 5 1.0
./clearvision unsharp input.png 7 2.5
./clearvision enc input.png "Secret message"
./clearvision dec input.png 20
./clearvision disguise input.png
./clearvision reveal secret.dat
```
