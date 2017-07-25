#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */


/* Check operation type */
OperationType check_operation_type(char **argv)
{
	//if the passed arguemnt is -e then perform encoding
	if (strncmp(*argv, "-e", 2) == 0)
	{
		return e_encode;
	}
	//if the passed arguement is -d then perform decoding
	else if (strncmp(*argv, "-d", 2) == 0)
	{
		return e_decode;
	}
	//if the arguent 2 is not passed either -e or -d then,print the Warning message
	else
	{
		return e_unsupported;
	}
}


/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	encInfo -> src_image_fname = argv[2];

	encInfo -> secret_fname = argv[3];

	encInfo -> stego_image_fname = argv[4];

	return e_success;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files_encode(EncodeInfo *encInfo)
{
	//Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

	//Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");

		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	//Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

	//Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");

		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	//Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

	//Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");

		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;

	//Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	//Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);

	printf("width = %u\n", width);

	//Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);

	printf("height = %u\n", height);

	fseek(fptr_image, 0L, SEEK_SET);

	//Return image capacity
	return width * height * 3;
}

/* Get file size
 * Input: secret file
 * Output: secret file size
 * Description: In message file,we calculate the size of that file
 */
uint get_file_size(FILE *fptr_secret)
{
	long file_size;

	//Seek to EOF of the secret file data
	fseek(fptr_secret, 0L, SEEK_END);

	//computing the file size
	file_size = ftell(fptr_secret);

	//printing the file size
	printf("The secret file size is %ld\n", file_size);

	//Seek to the 1st byte of the secret file data
	fseek(fptr_secret, 0L, SEEK_SET);

	return file_size;
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
	// Storing the size of the bmp image in the image_capacity by invoking the call get_image_size_for_bmp
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

	// Storing the size of the secret file in the size_secret_file by invoking the call get_file_size
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

	//comparing the image_capacity and size_secret_file
	if (encInfo -> image_capacity > encInfo -> size_secret_file)
	{
		return e_success;
	}
	else
	{	
		//if the size exceeds the image file size, prints the error image
		printf("Error!!! The size of the secret file exceeds the image file size\n");

		return e_failure;
	}
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[200];

	int byte_read;

	//copying first 54 bytes of the bmp image to the stegno image
	byte_read = fread(buffer, 1, 54, fptr_src_image);

	buffer[byte_read] = '\0';

	fwrite(buffer, 1, byte_read, fptr_stego_image);

	return e_success;
}


/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	int byte_read, i_index;

	char image_buffer[50];

	//For loop to count the magic string
	for (i_index = 0; magic_string[i_index] != '\0'; i_index++)
	{
		//reading the 8bytes from the source and storing in image buffer
		byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		//encoding the bytes of the source image with magic string
		encode_byte_tolsb(magic_string[i_index], image_buffer);

		//writing encoded bytes into the destination image
		fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}

	return e_success;
}

/* Encode password size*/
Status encode_password_size(EncodeInfo *encInfo)
{
	int byte_read;

	char image_buffer[50];

	//reading the 8 bytes from the source image and store into the image_buffer
	byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

	//storing the null character
	image_buffer[byte_read] = '\0';

	//encoding the password size with the source image
	encode_byte_tolsb(encInfo -> password_size, image_buffer);

	//writing the encoded bytes into the destination image
	fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);

	return e_success;
}

/* Encode password*/
Status encode_password(EncodeInfo *encInfo)
{
	int i_index;

	int byte_read;

	char image_buffer[50];

	//for loop to count the length of the password 
	for (i_index = 0; encInfo -> password[i_index] != '\0'; i_index++)
	{
		//reading the 8 bytes from the source image and store into the image_buffer
		byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		//storing the null character
		image_buffer[byte_read] = '\0';

		//encoding the password size with the source image
		encode_byte_tolsb(encInfo -> password[i_index], image_buffer);

		//writing the encoded bytes into the destination image
		fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);

	}

	return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	int i_index;

	int byte_read;

	char image_buffer[30];

	//pointer is pointing  to the '.' from file extension
	char *cptr = strrchr(file_extn , '.');

	//copy the the contents of the cptr to the extn secret file
	strcpy(encInfo -> extn_secret_file, ++cptr);

	//computing the strlen of the extn of the secret file
	encInfo -> secret_extn_size = strlen(encInfo -> extn_secret_file);

	//reading the bytes from the source image and storing into the buffer
	byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

	//storing the null character to the buffer
	image_buffer[byte_read] = '\0';

	//encoding the secret_extn file size with the source_image
	encode_byte_tolsb(encInfo -> secret_extn_size, image_buffer);

	//writing the encoded bytes to the destination file
	fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);

	//for loop to count the each character in the the extn_secret_file
	for (i_index = 0; encInfo -> extn_secret_file[i_index] != '\0'; i_index++)
	{
		//reading the bytes from the source image and storing into the buffer
		byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		//storing the null character to the buffer
		image_buffer[byte_read] = '\0';

		//encoding the extn_of the secret file one by one character with the image_buffer
		encode_byte_tolsb(encInfo -> extn_secret_file[i_index], image_buffer);

		//writing the encoded bytes to the destination file
		fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}

	return e_success;
}


/* Encode a byte into LSB of image data array */
Status encode_byte_tolsb(char data, char *image_buffer)
{
	int i_index;

	for (i_index = 0; i_index < 8; i_index++)
	{
		//check whether the masking is true or false
		if (data & (1 << i_index))
		{
			image_buffer[i_index] = image_buffer[i_index] | 1;
		}
		else
		{
			image_buffer[i_index] = image_buffer[i_index] & (~1);
		}
	}
	return e_success;
}


/*Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	int i_index, j_index;

	int byte_read, size;

	char ch;

	int bit;

	char image_buffer[200];

	unsigned int num = 0;

	//storing the size of the secret file to the total_size
	long total_size = encInfo -> size_secret_file;

	//computing the size
	size = sizeof(long) * 8;

	//for loop to count upto the sizeof(long)
	for (i_index = 0; i_index < sizeof(long); i_index++)
	{
		ch = '\0';

		num = 0;

		//for loop to count the 8bits
		for (j_index = 0; j_index < 8; j_index++)
		{
			//decrementing the size
			size--;

			//masking
			bit = total_size & (1 << size);

			//num logical OR with bit
			num = num | bit;
		}

		//performing the right shift
		num >>= size;

		//ch logical OR with num
		ch = ch | num;

		//reading the bytes from the source image and storing into the image buffer 
		byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);

		//storing the null character
		image_buffer[byte_read] = '\0';

		//encoding the character and image buffer
		encode_byte_tolsb(ch, image_buffer);

		//writing the encoded byte to the destination file
		fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}
	printf("\n");

	return e_success;
} 

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	int byte_read;

	char image_buffer[1000001];

	//feof returns 0 if there are bytes to read and returns EOF if there are no bytes to read
	while (feof(fptr_src_image) == 0)
	{
		//reading the bytes from the source file and storing into the image_buffer
		byte_read = fread(image_buffer, 1, 1000000, fptr_src_image);

		//storing the null character
		image_buffer[byte_read] = '\0';

		//writing the read byte to the destination file
		fwrite(image_buffer, 1 , byte_read, fptr_stego_image);
	}
	return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(FILE *fptr_secret, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	int i_index;

	char image_buffer[50];

	int byte_read_secret, byte_read_image;

	//feof returns the 0 if there are bytes to read, and returns EOF if there are no bytes read
	while (feof(fptr_secret) == 0)
	{
		//reading the bytes form the secret file and storing into the secret data
		byte_read_secret = fread(encInfo -> secret_data, 1, 8, fptr_secret);

		//storing the null cahracter
		encInfo -> secret_data[byte_read_secret] = '\0';

		//for loop to count the secret data upto null character to read the secret data
		for (i_index = 0; encInfo -> secret_data[i_index] != '\0'; i_index++)
		{
			//reading the bytes from the source image and storing into the image_buffer
			byte_read_image = fread(image_buffer, 1, 8, fptr_src_image);

			//storing the null character
			image_buffer[byte_read_image] = '\0';

			//encoding the secrret data with the source image file
			encode_byte_tolsb(encInfo -> secret_data[i_index], image_buffer);

			//writing the encoded bytes to the destination file
			fwrite(image_buffer, 1, byte_read_image, fptr_stego_image);
		}
	}
	return e_success;
}

/*Perform the encoding */
Status do_encoding(EncodeInfo *encInfo, int argc, char *argv[])
{
	//check whether the arguemnet count is 5 or not
	if (argc == 5)
	{
		//invoking the function call for read_and_validate_encode_args
		if (read_and_validate_encode_args(argv, encInfo) != e_success)
		{
			//printing the error message
			printf("The passed arguements are invalid!!\n");
		}
	}

	//invoking the function call to open the files
	if (open_files_encode(encInfo) != e_success)
	{
		//printing the error message
		printf("Error!!!While opening the files\n");
	}

	//invoking the function call to check the capacity
	if (check_capacity(encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! While checking the capacity\n");
	}

	//invoking the function call to copy the bmp header
	if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
	{
		//printing the error message
		printf("Error!! while copying the bmp header\n");
	}

	//invoking the function call to encode the magic string
	if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! while encoding the magic string\n");
	}

	//invoking the function call to encode the password_size
	if (encode_password_size(encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! password size is exceeding\n");
	}

	//invoking the function call to encode the password
	if (encode_password(encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! while encoding the password\n");
	}

	//invoking the function call to encode the secret_file_extn
	if (encode_secret_file_extn(encInfo -> secret_fname, encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! while encoding the secret file extension\n");
	}

	//invoking the function call to encode the secret file size
	if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! While encoding secret file size\n");
	}

	//invoking the function call to encode the secret data
	if (encode_data_to_image(encInfo -> fptr_secret, encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo) != e_success)
	{
		//printing the error message
		printf("Error!! while encoding the data\n");
	}

	//invoking the function call to encode the remaining image data
	if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
	{
		//printing the error message
		printf("Error!!! while copying the image data\n");
	}

	//closing the fptr_stego_image file
	fclose(encInfo -> fptr_stego_image);

	//closing the fptr_secret file
	fclose(encInfo -> fptr_secret);

	//closing the fptr_src_image file
	fclose(encInfo -> fptr_src_image);

	return e_success;
}
