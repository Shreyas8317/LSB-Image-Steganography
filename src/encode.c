#include <stdio.h>
#include <string.h>
#include "../header/encode.h"
#include "../header/common.h"

/*===========================================================
 *  Function: get_image_size_for_bmp()
 *===========================================================*/
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width = 0, height = 0;

    fseek(fptr_image, 18, SEEK_SET);
    if (fread(&width, sizeof(int), 1, fptr_image) != 1)
    {
        fprintf(stderr, "ERROR: Unable to read width from BMP.\n");
        return 0;
    }
    if (fread(&height, sizeof(int), 1, fptr_image) != 1)
    {
        fprintf(stderr, "ERROR: Unable to read height from BMP.\n");
        return 0;
    }

    printf("Width = %u, Height = %u\n", width, height);

    /* Return total bytes in pixel data (3 bytes per pixel) */
    return width * height * 3u;
}

/*===========================================================
 *  Function: get_file_size()
 *===========================================================*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    if (size < 0) return 0;
    return (uint)size;
}

/*===========================================================
 *  Function: read_and_validate_encode_args()
 *===========================================================*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
    {
        fprintf(stderr, "ERROR: Missing arguments for encoding operation.\n");
        fprintf(stderr, "Usage: ./stego -e <source.bmp> <secret.txt> <output.bmp>\n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];
    encInfo->secret_fname = argv[3];
    encInfo->stego_image_fname = argv[4];

    /* Validate source BMP */
    const char *src_ext = strrchr(encInfo->src_image_fname, '.');
    if (src_ext == NULL || strcmp(src_ext, ".bmp") != 0)
    {
        fprintf(stderr, "ERROR: Source image must be .bmp format.\n");
        return e_failure;
    }

    /* Validate secret file extension */
    const char *secret_ext = strrchr(encInfo->secret_fname, '.');
    if (secret_ext == NULL ||
        (strcmp(secret_ext, ".txt") != 0 &&
         strcmp(secret_ext, ".c") != 0 &&
         strcmp(secret_ext, ".h") != 0 &&
         strcmp(secret_ext, ".sh") != 0))
    {
        fprintf(stderr, "ERROR: Secret file must be .txt/.c/.h/.sh.\n");
        return e_failure;
    }

    strncpy(encInfo->extn_secret_file, secret_ext, sizeof(encInfo->extn_secret_file) - 1);
    encInfo->extn_secret_file[sizeof(encInfo->extn_secret_file) - 1] = '\0';
    encInfo->extn_size = (unsigned int)strlen(encInfo->extn_secret_file);

    /* Validate stego file extension */
    const char *stego_ext = strrchr(encInfo->stego_image_fname, '.');
    if (stego_ext == NULL || strcmp(stego_ext, ".bmp") != 0)
    {
        fprintf(stderr, "ERROR: Output (stego) image must be .bmp format.\n");
        return e_failure;
    }

    if (strcmp(encInfo->src_image_fname, encInfo->stego_image_fname) == 0)
    {
        fprintf(stderr, "ERROR: Source and output image filenames must differ.\n");
        return e_failure;
    }

    return e_success;
}

/*===========================================================
 *  Function: open_files()
 *===========================================================*/
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open source image '%s'.\n", encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open secret file '%s'.\n", encInfo->secret_fname);
        fclose(encInfo->fptr_src_image);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open output stego image '%s'.\n", encInfo->stego_image_fname);
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        return e_failure;
    }

    return e_success;
}

/*===========================================================
 *  Function: check_capacity()
 *===========================================================*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("Image capacity = %u bytes\n", encInfo->image_capacity);

    encInfo->size_secret_file = (long)get_file_size(encInfo->fptr_secret);
    printf("Secret file size = %ld bytes\n", encInfo->size_secret_file);

    int total_bytes_required =
          54   /* BMP header (copied unchanged) */
        + (int)(strlen(MAGIC_STRING) * 8)        /* magic string bits */
        + 32                                    /* extn size (32 bits) */
        + ((int)encInfo->extn_size * 8)         /* extension string bits */
        + 32                                    /* secret file size (32 bits) */
        + (int)(encInfo->size_secret_file * 8); /* payload bits */

    printf("Total bytes required for encoding = %d bytes\n", total_bytes_required);

    if (encInfo->image_capacity < (uint)total_bytes_required)
    {
        fprintf(stderr, "ERROR: Image capacity insufficient.\n");
        return e_failure;
    }

    printf("Sufficient image capacity ✅\n");
    return e_success;
}

/*===========================================================
 *  Function: copy_bmp_header()
 *===========================================================*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char header[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    if (fread(header, 1, 54, fptr_src_image) != 54)
    {
        fprintf(stderr, "ERROR: Unable to read BMP header.\n");
        return e_failure;
    }
    if (fwrite(header, 1, 54, fptr_dest_image) != 54)
    {
        fprintf(stderr, "ERROR: Unable to write BMP header.\n");
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_byte_to_lsb()
 *===========================================================*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_size_to_lsb()
 *===========================================================*/
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_magic_string()
 *===========================================================*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    unsigned char image_buffer[8];
    for (size_t i = 0; magic_string[i] != '\0'; i++)
    {
        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            fprintf(stderr, "ERROR: Reading image while encoding magic string.\n");
            return e_failure;
        }
        encode_byte_to_lsb((char)magic_string[i], (char *)image_buffer);
        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Writing stego while encoding magic string.\n");
            return e_failure;
        }
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_secret_file_extn_size()
 *===========================================================*/
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    unsigned char image_buffer[32];
    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        fprintf(stderr, "ERROR: Reading image while encoding extn size.\n");
        return e_failure;
    }
    encode_size_to_lsb(size, (char *)image_buffer);
    if (fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Writing stego while encoding extn size.\n");
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_secret_file_extn()
 *===========================================================*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    unsigned char image_buffer[8];
    for (size_t i = 0; file_extn[i] != '\0'; i++)
    {
        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            fprintf(stderr, "ERROR: Reading image while encoding extn.\n");
            return e_failure;
        }
        encode_byte_to_lsb((char)file_extn[i], (char *)image_buffer);
        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Writing stego while encoding extn.\n");
            return e_failure;
        }
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_secret_file_size()
 *===========================================================*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    unsigned char image_buffer[32];
    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        fprintf(stderr, "ERROR: Reading image while encoding file size.\n");
        return e_failure;
    }
    encode_size_to_lsb((int)file_size, (char *)image_buffer);
    if (fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Writing stego while encoding file size.\n");
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 *  Function: encode_secret_file_data()
 *===========================================================*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int ch;
    unsigned char image_buffer[8];
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            fprintf(stderr, "ERROR: Reading image while encoding data.\n");
            return e_failure;
        }
        encode_byte_to_lsb((char)ch, (char *)image_buffer);
        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            fprintf(stderr, "ERROR: Writing stego while encoding data.\n");
            return e_failure;
        }
    }
    return e_success;
}

/*===========================================================
 *  Function: copy_remaining_img_data()
 *===========================================================*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    unsigned char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, fptr_dest) != bytes)
        {
            fprintf(stderr, "ERROR: Writing remaining image data.\n");
            return e_failure;
        }
    }
    return e_success;
}

/*===========================================================
 *  Function: do_encoding()
 *===========================================================*/
Status do_encoding(EncodeInfo *encInfo)
{
    printf("\n--- Starting Encoding Process ---\n");

    if (open_files(encInfo) == e_failure)
        return e_failure;

    if (check_capacity(encInfo) == e_failure)
        return e_failure;

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
        return e_failure;

    printf("Encoding Magic String...\n");
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure) return e_failure;

    printf("Encoding Secret File Extension Size...\n");
    if (encode_secret_file_extn_size((int)encInfo->extn_size, encInfo) == e_failure) return e_failure;

    printf("Encoding Secret File Extension...\n");
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure) return e_failure;

    printf("Encoding Secret File Size...\n");
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure) return e_failure;

    printf("Encoding Secret File Data...\n");
    if (encode_secret_file_data(encInfo) == e_failure) return e_failure;

    printf("Copying Remaining Image Data...\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) return e_failure;

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    printf("--- Encoding Completed Successfully ✅ ---\n");
    return e_success;
}
