#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*  Read and validate arguements function*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	decInfo -> secret_fname = argv[3];

	decInfo -> stego_image_fname = argv[2];

	return e_success;
}

/* OPen files for stego image file*/
Status open_files(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

	//sanity check
	if (decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");

		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return e_failure;
	}

	return e_success;
}

/*Decoding lsb to byte function*/
Status decode_lsb_tobyte(char *data, char *image_buffer)
{
	int i_index, bits;

	*data = *data & 0;

	for (i_index = 0; i_index < 8; i_index++)
	{
		bits = image_buffer[i_index] & 1;

		*data = *data | (bits << i_index);
	}
	return e_success;
}

/*Decoding the magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
	int byte_read, i_index;

	char image_buffer[50];

	char magic_string[5];

	int size = strlen(MAGIC_STRING);

	//seek to 54th byte
	fseek(decInfo -> fptr_stego_image, 54L, SEEK_SET);

	for (i_index = 0; i_index < size; i_index++)
	{
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[byte_read] = '\0';

		decode_lsb_tobyte(&magic_string[i_index], image_buffer);
	}

	magic_string[i_index] = '\0';

	printf("%s\n", magic_string);

	if (strcmp(magic_string, MAGIC_STRING) == 0)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/*Decoding the password size*/
Status decode_password_size(DecodeInfo *decInfo)
{
	int byte_read;

	int i_index;

	char image_buffer[50];

	unsigned char password_size;

	byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

	image_buffer[byte_read] = '\0';

	//decoding the password size by invoking the decode_lsb_tobyte
	decode_lsb_tobyte(&password_size, image_buffer);

	printf("%d\n", password_size);

	if (password_size == decInfo -> password_size)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/*Decoding the password*/
Status decode_password(DecodeInfo *decInfo)
{
	int i_index;

	int byte_read;

	char image_buffer[50];

	char password[10];

	for (i_index = 0; i_index < decInfo -> password_size; i_index++)
	{
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
	
		image_buffer[byte_read] = '\0';
		
		//decoding the password by invoking the function call
		decode_lsb_tobyte(&password[i_index], image_buffer);
	}

	password[i_index] = '\0';
	
	printf("%s",password);

	if (strcmp(password, decInfo -> password) == 0)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

/*Decoding the secret file extension*/
Status decode_secret_file_extn(char *secret_fname, DecodeInfo *decInfo)
{
	int i_index = 0, j_index;

	int byte_read;

	char ch;

	char image_buffer[30];

	byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

	image_buffer[byte_read] = '\0';

	decode_lsb_tobyte(&decInfo -> secret_extn_size, image_buffer);

	while (decInfo -> secret_fname[i_index] != '\0')
	{
		i_index++;
	}
	

	decInfo -> secret_fname[i_index] = '.';
	
	i_index++;
	
	for (j_index = 0; j_index < decInfo -> secret_extn_size; j_index++)
	{
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[byte_read] = '\0';
		
		decode_lsb_tobyte(&ch, image_buffer);	
		
		decInfo -> secret_fname[i_index] = ch;

		i_index++;
	}

	decInfo -> secret_fname[i_index] = '\0';
	
	//opening the secret file
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");

	if (decInfo->fptr_secret == NULL)
	{
		perror("fopen");

		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
	
		return e_failure;
	}

	printf("%s", decInfo -> secret_fname);

	printf("\n");

	return e_success;
}

/*Decode the secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	int i_index;

	long byte_read;

	char image_buffer[200];

	decInfo -> size_secret_file = 0;

	unsigned char size = 0;

	for (i_index = 0; i_index < sizeof(long); i_index++)
	{
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);

		image_buffer[byte_read] = '\0';

		decode_lsb_tobyte(&size, image_buffer);

		printf("......%d\n", size);

		decInfo -> size_secret_file |= size;
	
	if (sizeof(long) - 1 != i_index)

		decInfo -> size_secret_file <<= (sizeof(char) * 8);
	}

	printf("%ld\n", decInfo -> size_secret_file);
	
	return e_success;
}

/*Decoding the text*/
Status decode_image_to_data(FILE *fptr_stego_image, FILE *fptr_secret, DecodeInfo *decInfo)
{
	int i_index;

	long byte_read;

	char image_buffer[500];

	char ch;

	for (i_index = 0; i_index < decInfo -> size_secret_file; i_index++)
	{
		byte_read = fread(image_buffer, 1, 8, fptr_stego_image);

		image_buffer[byte_read] = '\0';

		decode_lsb_tobyte(&ch, image_buffer);

		fwrite(&ch, 1, 1, fptr_secret);
	}
	
	return e_success;
}

/*The real decoding function*/
Status do_decoding(DecodeInfo *decInfo, int argc, char *argv[])
{
	if (argc == 4)
	{
		//invoking read and validate function call
		if (read_and_validate_decode_args(argv, decInfo) != e_success)
		{
			printf("The passed arguements are invalid!!\n");
		}
	}

	//invoking open files function call
	if (open_files(decInfo) != e_success)
	{
		printf("Error!!!While opening the files\n");
	}
	
	//invoking magic string function call
	if (decode_magic_string(decInfo) != e_success)
	{
		printf("Error!! while decoding the magic string\n");
	}

	//invoking the password_size function call
	if (decode_password_size(decInfo) != e_success)
	{
		printf("Error!! password size is exceeding\n");
	}
	
	if (decode_password(decInfo) != e_success)
	{
		printf("Error!! while decoding the password\n");
	}
	
	if (decode_secret_file_extn(decInfo -> secret_fname, decInfo) != e_success)
	{
		printf("Error!! while decoding the secret file extension\n");
	}

	if (decode_secret_file_size(decInfo) != e_success)
	{
		printf("Error!! While decoding secret file size\n");
	}

	if (decode_image_to_data(decInfo -> fptr_stego_image, decInfo -> fptr_secret, decInfo) != e_success)
	{
		printf("Error!! while decoding the secret data\n");
	}

	return e_success;
}
