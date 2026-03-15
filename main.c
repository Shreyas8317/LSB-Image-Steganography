/*
 ============================================================================
 Name        : Shreyas Santosh Pawangadkar
 Date        : 03/11/2025
 Batch Id    : 21C_308
 Roll No.    : 308
 Project     : Image Steganography using LSB (Least Significant Bit) Technique
 Language    : C
 ============================================================================
 Problem Statement :
 -------------------
 Implement a C program to perform image steganography using the Least Significant
 Bit (LSB) technique. The program should be capable of both encoding (hiding)
 and decoding (extracting) secret data within a BMP image file.

 Encoding:
    - Hide a secret text file (.txt, .c, .h, etc.) inside a 24-bit BMP image.
    - Store necessary metadata such as magic string, file extension, and
      secret file size within the image pixels using LSB modification.
    - Generate a new output BMP file (stego image) containing the hidden data.

 Decoding:
    - Read the stego BMP image.
    - Validate the presence of the magic string to confirm hidden data.
    - Extract the file extension, size, and secret file data bit-by-bit.
    - Recreate the original secret file (e.g., message.txt).

 Functional Requirements:
 -------------------------
 1. The project must handle command-line arguments for both operations:
      Encoding  : ./a.out -e <source.bmp> <secret.txt> <output.bmp>
      Decoding  : ./a.out -d <stego_image.bmp> <decoded_secret.txt>
 2. Support only 24-bit uncompressed BMP images.
 3. Preserve BMP header (first 54 bytes) without modification.
 4. Ensure data integrity — decoded file must match the original secret file.
 5. Handle file I/O errors and capacity checks gracefully.

 Expected Output :
 -----------------
 During Encoding:
   - Display width, height, and image capacity.
   - Show total bytes required for encoding.
   - Confirm sufficient capacity.
   - Print progress messages:
        > Encoding Magic String...
        > Encoding Secret File Extension Size...
        > Encoding Secret File Extension...
        > Encoding Secret File Size...
        > Encoding Secret File Data...
        > Copying Remaining Image Data...
   - Print success message: "Encoding Completed Successfully ✅"

 During Decoding:
   - Confirm decoding start.
   - Validate magic string.
   - Extract secret file extension and size.
   - Write decoded data to output file.
   - Print success message:
        "Decoding completed successfully!"
        "Secret file 'message.txt' extracted from 'output.bmp'"

 ============================================================================
*/


#include <stdio.h>
#include <string.h>
#include "./header/decode.h"
#include "./header/encode.h"
#include "./header/types.h"

/* Function Declaration */
OperationType check_operation_type(char *symbol);

/*===========================================================
 * Function: main()
 * Purpose : Entry point for Steganography Program
 *===========================================================*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Operation type missing.\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  Encode: %s -e <source.bmp> <secret.txt> <output.bmp>\n", argv[0]);
        fprintf(stderr, "  Decode: %s -d <stego.bmp> <output.txt>\n", argv[0]);
        return e_failure;
    }

    /* Step 1: Identify operation type */
    OperationType op_type = check_operation_type(argv[1]);

    if (op_type == e_encode)
    {
        /* ===================== ENCODING SECTION ===================== */
        if (argc < 5)
        {
            fprintf(stderr, "ERROR: Invalid number of arguments for encoding.\n");
            fprintf(stderr, "Usage: %s -e <source.bmp> <secret.txt> <output.bmp>\n", argv[0]);
            return e_failure;
        }

        printf("\nSelected Operation: Encoding 🔒\n");

        EncodeInfo encInfo;

        /* Step 2: Validate and Read Encoding Arguments */
        if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            fprintf(stderr, "ERROR: Argument validation failed for encoding.\n");
            return e_failure;
        }

        /* Step 3: Perform Encoding */
        if (do_encoding(&encInfo) == e_success)
        {
            printf("\n✅ Encoding completed successfully!\n");
            printf("Secret file '%s' is hidden inside '%s'\n\n",
                   encInfo.secret_fname, encInfo.stego_image_fname);
        }
        else
        {
            fprintf(stderr, "❌ Encoding failed.\n");
            return e_failure;
        }
    }
    else if (op_type == e_decode)
    {
        /* ===================== DECODING SECTION ===================== */
        if (argc < 4)
        {
            fprintf(stderr, "ERROR: Invalid number of arguments for decoding.\n");
            fprintf(stderr, "Usage: %s -d <stego.bmp> <output.txt>\n", argv[0]);
            return e_failure;
        }

        printf("\nSelected Operation: Decoding 🔓\n");

        DecodeInfo decInfo;

        /* Step 2: Validate and Read Decoding Arguments */
        if (read_and_validate_decode_args(argv, &decInfo) == e_failure)
        {
            fprintf(stderr, "ERROR: Argument validation failed for decoding.\n");
            return e_failure;
        }

        /* Step 3: Perform Decoding */
        if (do_decoding(&decInfo) == e_success)
        {
            printf("\n✅ Decoding completed successfully!\n");
            printf("Secret data extracted to '%s'\n\n", decInfo.secret_fname);
        }
        else
        {
            fprintf(stderr, "❌ Decoding failed.\n");
            return e_failure;
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Unsupported operation '%s'. Use -e for encoding or -d for decoding.\n", argv[1]);
        return e_failure;
    }

    return e_success;
}

/*===========================================================
 * Function: check_operation_type()
 * Purpose : Determine whether user chose encoding or decoding
 *===========================================================*/
OperationType check_operation_type(char *symbol)
{
    if (symbol == NULL)
        return e_unsupported;

    if (strcmp(symbol, "-e") == 0)
        return e_encode;
    else if (strcmp(symbol, "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
