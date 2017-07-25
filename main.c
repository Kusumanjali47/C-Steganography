#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "encode.h"
#include "types.h"
#include "common.h"

/* Read the source file name, secret file name, and destination file name*/
Status read_info_encode(EncodeInfo *encInfo)
{
	int size;

	char buffer[100];

	printf("Enter the source file name : ");

	scanf("%s", buffer);

	//computing the strlen of the buffer
	size = strlen(buffer);

	//Allocating the dynamic memory 
	char *ptr = (char*)malloc(sizeof(char*) * size);

	//copying the contents into the ptr
	strcpy(ptr, buffer);

	//assigning the  address of the ptr to the src_image_fname 
	encInfo -> src_image_fname = ptr;

	//printing the source file name
	printf("%s\n", encInfo -> src_image_fname);



	printf("Enter the secret file name : ");

	scanf("%s", buffer);

	//computing the strlen of the buffer
	size = strlen(buffer);

	//Allocating the dynamic memory
	ptr = (char*)malloc(sizeof(char*) * size);

	//copying the contents into the ptr
	strcpy(ptr, buffer);

	//assigning the address of the ptr to the secret_fname
	encInfo -> secret_fname = ptr;

	//printing the secret file name
	printf("%s\n", encInfo -> secret_fname);


	printf("Enter the Destination file name : ");

	scanf("%s", buffer);

	//computing the strlen of the buffer
	size = strlen(buffer);

	//Allocating the Dynamic memory
	ptr = (char*)malloc(sizeof(char*) * size);

	//copying the contents into the ptr
	strcpy(ptr, buffer);

	//assigning the address of the ptr to the stego image file name
	encInfo -> stego_image_fname = ptr;

	//printing the stego image file name
	printf("%s\n", encInfo -> stego_image_fname);

	return e_success;
}

/* Reading the stego image file name and and secret file name*/
Status read_info_decode(DecodeInfo *decInfo)
{
	int size;

	char buffer[100];

	printf("Enter the source file name : ");

	scanf("%s", buffer);

	//computing the strlen of the buffer
	size = strlen(buffer);

	//Allocating the Dyanamic memory
	char *ptr = (char*)malloc(sizeof(char*) * size);

	//copying the contents into the ptr
	strcpy(ptr, buffer);

	//assigning the address of the ptr to the stego image file name
	decInfo -> stego_image_fname = ptr;

	//printing the stego image file name
	printf("%s\n", decInfo -> stego_image_fname);


	printf("Enter the secret file name : ");

	scanf("%s", buffer);

	//computing the strlen of the buffer
	size = strlen(buffer);

	//Allocating the Dynamic memory
	ptr = (char*)malloc(sizeof(char*) * size);

	//copying the contents into the ptr
	strcpy(ptr, buffer);

	//assigning the address of the ptr to the secret file name
	decInfo -> secret_fname = ptr;

	//printing the secret file name
	printf("%s\n", decInfo -> secret_fname);

	return e_success;

}

/* Read password for encoding and decoding*/
void read_password(EncodeInfo *encInfo, DecodeInfo *decInfo, int option)
{
	int size;

	char buffer[20];

	if (option == 1)
	{

		//read the password
		printf("Enter the password : ");

		scanf("%s", buffer);
		
		//computing the strlen of the buffer
		size = strlen(buffer);

		//checking whether the size is less than 8 or not
		if (size < 8)
		{
			//copying the contents of the buffer to the password
			strcpy(encInfo -> password, buffer);

			//assigning the size to the password_size
			encInfo -> password_size = size;
		}
		else
		{
			printf("Check the password\n");
		}

	}

	if (option == 2)
	{
		//read the password
		printf("Enter the password : ");

		scanf("%s", buffer);

		//computing the strlen of the buffer
		size = strlen(buffer);

		//copying the contents of the buffer to the password
		strcpy(decInfo -> password, buffer);

		//assigning the size to the password_size
		decInfo -> password_size = size;
	}
}

/* MAIN PROGRAM*/
int main(int argc, char *argv[])
{
	int option;

	char decision;

	EncodeInfo encInfo;

	DecodeInfo decInfo;

	do
	{
		/*checking whether the arguement count is "one" or not*/
		if (argc == 1)
		{
			//read the option
			printf("Choose the Option : \n\t1.Encode\n\t2.Decode\nChoose : ");

			scanf("%d", &option);
			
			//reading the information for encoding
			if (option == 1)
			{
				//invoking the function call
				read_info_encode(&encInfo);
			}
			//reading the information for decoding
			else if (option == 2)
			{
				//invoking the function call
				read_info_decode(&decInfo);
			}
		}

		else
		{
			//Using command line arguements
			option = check_operation_type(&argv[1]);
		}
		
		//Switch case statement for the decoding and encoding
		switch (option)
		{
			//case statement for encoding
			case e_encode:
				
				//invoking the function call for reading the password
				read_password(&encInfo, &decInfo, option);
				
				//invoking the function call to do encoding
				do_encoding(&encInfo, argc, argv);

				break;
			
			//case statement for decoding
			case e_decode:
				
				//invoking the function call to read the password
				read_password(&encInfo, &decInfo, option);
				
				//invoking the function call to do decoding
				do_decoding(&decInfo, argc, argv);

				break;

			//case statement for default option
			default:

				printf("unsupported\n");

				break;
		}

		printf("Do You Want to Continue : (Y/N)");

		scanf("\n%c", &decision);

	} while (decision == 'y');

	return 0;

}
