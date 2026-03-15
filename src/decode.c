#include <stdio.h>
#include <string.h>
#include "../header/decode.h"
#include "../header/common.h"

/*===========================================================
 * Function: read_and_validate_decode_args()
 *===========================================================*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL || argv[3] == NULL)
    {
        fprintf(stderr, "ERROR: Missing arguments for decoding.\n");
        fprintf(stderr, "Usage: ./stego -d <stego.bmp> <output_file>\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];
    decInfo->secret_fname = argv[3];
    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_secret = NULL;
    decInfo->extn_secret_file[0] = '\0';
    decInfo->output_fname[0] = '\0';

    const char *ext = strrchr(decInfo->stego_image_fname, '.');
    if (ext == NULL || strcmp(ext, ".bmp") != 0)
    {
        fprintf(stderr, "ERROR: Stego image must be a .bmp file.\n");
        return e_failure;
    }

    return e_success;
}

/*===========================================================
 * Function: open_decode_files()
 *===========================================================*/
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open stego image '%s'.\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 * Function: byte_from_lsb()
 *===========================================================*/
unsigned char byte_from_lsb(FILE *fptr_image)
{
    unsigned char data = 0;
    for (int i = 0; i < 8; i++)
    {
        unsigned char byte = 0;
        fread(&byte, 1, 1, fptr_image);
        data |= ((byte & 1) << (7 - i));
    }
    return data;
}

/*===========================================================
 * Function: size_from_lsb()
 *===========================================================*/
unsigned int size_from_lsb(FILE *fptr_image)
{
    unsigned int size = 0;
    for (int i = 0; i < 32; i++)
    {
        unsigned char byte = 0;
        fread(&byte, 1, 1, fptr_image);
        size |= ((byte & 1) << (31 - i));
    }
    return size;
}

/*===========================================================
 * Function: decode_magic_string()
 *===========================================================*/
Status decode_magic_string(DecodeInfo *decInfo)
{
    const char magic_string[] = MAGIC_STRING;
    for (int i = 0; i < strlen(magic_string); i++)
    {
        unsigned char ch = byte_from_lsb(decInfo->fptr_stego_image);
        if (ch != (unsigned char)magic_string[i])
        {
            fprintf(stderr, "ERROR: Invalid magic string in stego image.\n");
            return e_failure;
        }
    }
    return e_success;
}

/*===========================================================
 * Function: decode_secret_file_extn_size()
 *===========================================================*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decInfo->extn_size = size_from_lsb(decInfo->fptr_stego_image);
    if (decInfo->extn_size == 0 || decInfo->extn_size >= sizeof(decInfo->extn_secret_file))
    {
        fprintf(stderr, "ERROR: Invalid extension size decoded.\n");
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 * Function: decode_secret_file_extn()
 *===========================================================*/
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    for (unsigned int i = 0; i < decInfo->extn_size; i++)
    {
        decInfo->extn_secret_file[i] = byte_from_lsb(decInfo->fptr_stego_image);
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    return e_success;
}

/*===========================================================
 * Function: decode_secret_file_size()
 *===========================================================*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->size_secret_file = size_from_lsb(decInfo->fptr_stego_image);
    if (decInfo->size_secret_file == 0)
    {
        fprintf(stderr, "ERROR: Secret file size decoded as zero.\n");
        return e_failure;
    }
    return e_success;
}

/*===========================================================
 * Function: decode_secret_file_data()
 *===========================================================*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->output_fname, "wb");
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Cannot open '%s' for writing.\n", decInfo->output_fname);
        return e_failure;
    }

    for (unsigned int i = 0; i < decInfo->size_secret_file; i++)
    {
        unsigned char ch = byte_from_lsb(decInfo->fptr_stego_image);
        fwrite(&ch, 1, 1, decInfo->fptr_secret);
    }

    fclose(decInfo->fptr_secret);
    return e_success;
}

/*===========================================================
 * Function: do_decoding()
 *===========================================================*/
Status do_decoding(DecodeInfo *decInfo)
{
    printf("\n--- Starting Decoding Process ---\n");

    if (open_decode_files(decInfo) == e_failure)
        return e_failure;

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); // skip BMP header

    if (decode_magic_string(decInfo) == e_failure)
        return e_failure;

    if (decode_secret_file_extn_size(decInfo) == e_failure)
        return e_failure;

    if (decode_secret_file_extn(decInfo) == e_failure)
        return e_failure;

    /* --- Always ignore user extension and use decoded one --- */
    char base_name[256];
    strncpy(base_name, decInfo->secret_fname, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';

    /* Remove user extension if present */
    char *dot = strrchr(base_name, '.');
    if (dot != NULL)
        *dot = '\0'; // cut off at the dot

    /* Now append decoded extension */
    const char *ext = decInfo->extn_secret_file;
    if (ext[0] == '.')
        snprintf(decInfo->output_fname, sizeof(decInfo->output_fname), "%s%s", base_name, ext);
    else
        snprintf(decInfo->output_fname, sizeof(decInfo->output_fname), "%s.%s", base_name, ext);

    if (decode_secret_file_size(decInfo) == e_failure)
        return e_failure;

    if (decode_secret_file_data(decInfo) == e_failure)
        return e_failure;

    fclose(decInfo->fptr_stego_image);

    printf("\n✅ Decoding Completed Successfully!\n");
    printf("Decoded extension: %s\n", decInfo->extn_secret_file);
    printf("Secret data extracted to '%s'\n\n", decInfo->output_fname);

    return e_success;
}
