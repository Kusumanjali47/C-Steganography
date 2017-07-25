#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
#define MAX_PASSWORD_BUF_SIZE 10

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
	unsigned char secret_extn_size;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

	/*password info*/
	char password[MAX_PASSWORD_BUF_SIZE];
	unsigned char password_size;


} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo, int argc, char *argv[]);

/* Get File pointers for i/p and o/p files */
Status open_files(DecodeInfo *decInfo);

/* Restore Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *secret_fname, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_image_to_data(FILE *fptr_stego_image, FILE *fptr_secret, DecodeInfo *decInfo);

/* Decode a LSB into byte of image data array */
Status decode_lsb_tobyte(char *data, char *image_buffer);

/* Decode password size*/
Status decode_password_size(DecodeInfo *decInfo);

/* Decode password*/
Status decode_password(DecodeInfo *decInfo);

#endif
