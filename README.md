# LSB Image Steganography (C | Systems Programming)

A C-based implementation of image steganography using the Least Significant Bit (LSB) technique to securely embed and extract data within BMP images. The system modifies pixel-level data while preserving the visual integrity of the image.

This project demonstrates low-level data manipulation, file handling, and modular design practices commonly used in system programming, embedded systems, and security-oriented applications.

---

## Overview

The project provides a complete workflow for hiding and retrieving secret data inside bitmap images. It operates directly on pixel data by modifying the least significant bits, ensuring minimal perceptible change to the original image.

The implementation is structured into encoding and decoding modules, enabling clear separation of responsibilities and maintainable code design.

---

## Core Capabilities

- Embedding secret data within BMP images using LSB technique  
- Extraction of hidden data from stego images  
- Bit-level manipulation of image data  
- Efficient file I/O handling for binary files  
- Modular and maintainable C implementation  
- Preservation of image quality after data embedding  

---

## Functional Description

### Encoding Module (`encode`)

Responsible for embedding secret data into a cover image.

- Reads and validates the input BMP image  
- Skips header to access pixel data  
- Converts secret data into binary representation  
- Embeds data into least significant bits of pixel bytes  
- Generates a stego image with hidden content  

---

### Decoding Module (`decode`)

Responsible for extracting hidden data from a stego image.

- Reads the stego image  
- Skips header to access encoded pixel data  
- Extracts least significant bits from pixel bytes  
- Reconstructs the original hidden data  
- Writes the recovered content to an output file  

---

### Common Utilities (`common`, `types`)

Provides shared functionality and definitions.

- Data type definitions for portability  
- Helper functions for bit manipulation  
- Common utilities used across encoding and decoding  

---

## Encoding and Decoding Process

### Encoding Process

- Load cover image  
- Skip BMP header  
- Convert secret data into binary  
- Embed bits into pixel LSBs  
- Generate stego image  

### Decoding Process

- Load stego image  
- Skip BMP header  
- Extract LSB bits  
- Reconstruct original data  
- Save decoded output  

---

## Technical Concepts Applied

- Bitwise operations and masking  
- Binary data encoding and decoding  
- File handling in C (binary mode)  
- BMP image format processing  
- Memory-efficient data handling  
- Modular software design  

---

## Build Instructions

Compile the project using GCC:

```bash
gcc main.c src/*.c -Iheader -o steganography
```

---

## Usage

### Encode Data

```bash
./steganography -e cover.bmp secret.txt stego.bmp
```

### Decode Data

```bash
./steganography -d stego.bmp output.txt
```

---

## Design Approach

The project is implemented using principles aligned with production-level system software:

- Clear separation between encoding and decoding logic  
- Modular design for maintainability and scalability  
- Efficient handling of binary data  
- Minimal dependency design for portability  
- Focus on low-level control and performance  

---

## Industry Relevance

This project demonstrates key skills relevant to system programming and embedded domains:

- Low-level data manipulation using bitwise operations  
- Working with binary file formats  
- Secure data embedding techniques  
- Understanding of memory-efficient processing  
- Design of modular and reusable software components  

---

## Applications

- Secure communication systems  
- Digital watermarking  
- Multimedia content protection  
- Data hiding and steganographic systems  

---

## Future Enhancements

- Support for additional image formats (PNG, JPEG)  
- Encryption of secret data before embedding  
- Improved capacity and compression techniques  
- GUI-based interface for usability  
- Integration with embedded imaging systems  

---

## Author

Shreyas Pawangadkar  
