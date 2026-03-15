LSB Image Steganography
Overview

This project implements Image Steganography using the Least Significant Bit (LSB) technique in C. It enables secure embedding and extraction of secret data within BMP images by modifying the least significant bits of pixel values while preserving the visual quality of the image.

The implementation demonstrates bit-level data manipulation, file I/O operations, and modular software design, which are commonly used in system programming, embedded systems, and security-focused applications.

Key Features

Secure embedding of secret data inside BMP images

Extraction of hidden data from stego images

Implementation based on the LSB steganography technique

Minimal visual distortion in the cover image

Modular and maintainable C code structure

Project Structure
lsb-image-steganography
│
├── header/
│   ├── types.h
│   ├── encode.h
│   ├── decode.h
│   └── common.h
│
├── src/
│   ├── encode.c
│   └── decode.c
│
├── main.c
└── README.md
System Workflow
Encoding

The encoding module embeds secret data into a cover image.

Steps:

Read the cover BMP image

Skip the BMP header information

Convert secret data into binary format

Embed bits into the Least Significant Bits of image pixels

Generate the stego image

Decoding

The decoding module retrieves hidden data from the stego image.

Steps:

Read the stego image

Skip the BMP header information

Extract LSB bits from pixel data

Reconstruct the hidden message

Write the recovered data to the output file

Technologies and Concepts

C Programming

Bit Manipulation

File I/O Operations

BMP Image Format Processing

Modular Software Design

Build Instructions

Compile the project using GCC:

gcc main.c src/*.c -Iheader -o steganography
Usage
Encode secret data into an image
./steganography -e cover.bmp secret.txt stego.bmp
Decode hidden data from an image
./steganography -d stego.bmp output.txt
Applications

Secure communication systems

Digital watermarking

Multimedia security

Data hiding and information protection

Learning Outcomes

This project demonstrates the practical implementation of data hiding techniques using low-level programming. It highlights skills relevant to embedded systems development, system-level programming, and security-focused software engineering.