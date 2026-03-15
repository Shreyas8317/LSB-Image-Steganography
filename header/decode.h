#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

/*----------------------------------------------------------
 * Structure for decoding information
 *----------------------------------------------------------*/
typedef struct _DecodeInfo
{
    char *stego_image_fname;      /* Input stego image (argv[2]) */
    char *secret_fname;           /* Output base name (argv[3]) */
    char output_fname[512];       /* Final filename after adding decoded extension */
    char extn_secret_file[64];    /* Decoded extension (.txt, .c, etc.) */

    FILE *fptr_stego_image;
    FILE *fptr_secret;

    unsigned int extn_size;
    unsigned int size_secret_file;

} DecodeInfo;

/* Function prototypes */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

unsigned char byte_from_lsb(FILE *fptr_image);
unsigned int size_from_lsb(FILE *fptr_image);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif /* DECODE_H */
