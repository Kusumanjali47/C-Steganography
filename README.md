# Steganography

Introduction:

“Steganography is the art and science of writing hidden messages in such a way that no one, apart from the sender and intended recipient, suspects the existence of the message, a form of security through obscurity.”


ALGORITHM FOR ENCODING:

Step1 	: Start
Step2 	: Encoding:
Step3 	: Read files : 
			Text file
 			Source image file
 			Destination image file
Step 4 	: Create password
Step 5 	: Open all files
Step 6 	: Check capacity of source file, Source file size is not more than secret file then  goto step 14
Step 7	: Copy header file from source image to destination image
Step 8	: Encode magic string
Step 9	: Encode password and password size
Step 10	: Encode secret file extension
Step 11	: Encode secret file size
Step 12	: Encode secret file data
Step 13	: copy remaing data to the destination file
Step 14	: Stop


ALGORITHM FOR DECODING

Step 1	: Start
Step 2	: Decoding
Step 3	: Read files, Text file and stego image file
Step 4	: Read the password
Step 5	: Decode the magic strin
Step 6	: Decode the password
Step 7	: Decode the password size
Step 8	: compare entered password and decoded password size is equal to or not. Else goto step 12
Step 9	: Decode secret file size
Step 10	: Decode secret file extension
Step 11	: Decode the data
Step 12	: Stop

ADVANTAGES

1.Text file can be encoded  and hidden in given Image.
2.There will be no clue that a message is hidden in the source image.
3.There is negligible difference between source image and image with hidden file.

DISADVANTAGES

1.But, the size of the file should be less than or equal to capacity of image.
2.Hidden data will be destroyed if image is compressed or manipulated in any way.
3.Without encryption anyone with proper knowledge can decode the message.

APPLICATIONS
1.Hiding important information in plain sight.
2.Communication without attracting any attention.
3.Used in modern laser printers.
