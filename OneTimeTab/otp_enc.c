/***********************************************************************
*  Author: Deirdre Moran
*  Program: otp_enc.c
*  Date: 3/17/2018
*  Description: Sends key and file contents for server side encoding
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	// variables for sockets and server address structure
	int socketFD, portNumber, charsWritten, charsRead;
	int i;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	// buffers
	char buffer[25600], dataBuffer[25600];
	// File descriptors
	int fd[2];
	// final int version of size
	int finalSize;
	// If less than 2 arguments, error message

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	// Get the port number, convert to an integer from a string
	portNumber = atoi(argv[3]);
	// Create a network-capable socket
	serverAddress.sin_family = AF_INET;
	// Store the port number
	serverAddress.sin_port = htons(portNumber);
	// Convert the machine name into a special form of address
	serverHostInfo = gethostbyname("localhost");
	if (serverHostInfo == NULL) {
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(1);
	}
	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		fprintf(stderr, "ERROR: Cannot open socket");
		exit(1);
	}

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
		fprintf(stderr, "ERROR: Cannot connect");
	}
	// open plaintext file specified in command line argument
	fd[1] = open(argv[1], O_RDONLY, 0664);
	// If error opening file
	if(fd[1] == -1){
		fflush(stdout);
		fprintf(stderr, "Cannot open for input");
		exit(1);
	}
	// Else file is open
	else{
		// Get the size of plaintext file
		struct stat st;
		stat(argv[1], &st);
		int mysize = st.st_size;
		mysize--;
		char str[20];
		// Store file size in finalSize
		finalSize = mysize;
		sprintf(str, "%d ", mysize);
        // send size to server
        send(socketFD, str, sizeof(str), 0);
        // Wait for a little bit
        sleep(1);

		// Clear the memory for size string
		memset(str, '\0', sizeof(str));
        // Get the size of the key
        stat(argv[2], &st);
		mysize = st.st_size;
		mysize--;
		int keyS = mysize;
		// If key is less than plaintext file, error message
		if(keyS < finalSize){
			fprintf(stderr, "ERROR: key is too short\n");
			exit(1);
		}
	    sprintf(str, "%d ", mysize);
	  	// Send key file size to server
	  	send(socketFD, str, sizeof(str), 0);
		sleep(1);

		// Clear the data buffer for retrieving contents of plaintext file
		memset(dataBuffer, '\0', sizeof(dataBuffer));
		// Read contents into buffer
		int k = read(fd[1], dataBuffer, sizeof(dataBuffer));
		// if error reading, error message
		if(k < 0){
			fprintf(stderr, "ERROR: reading");
			exit(1);
		}
		// void pointer to data buffer
		void *p = dataBuffer;
		// while there are still bytes to be read
		// Send file contents stored in databuffer to server
		while(k > 0){
			int bytes_written = send(socketFD, p, k, 0);
			if(bytes_written <= 0){
				fprintf(stderr, "ERROR writing bytes");
				exit(1);
			}
			k -= bytes_written;
			p += bytes_written;
		}
	}
	// Close the file
	close(fd[1]);
	sleep(1);

	// Open key file
	fd[1] = open(argv[2], O_RDONLY, 0664);
	// If error opening file
	if(fd[1] == -1){
		fflush(stdout);
		fprintf(stderr, "ERROR: Cannot open for input");
		exit(1);
	}
	// Else process key file
	else{
		memset(dataBuffer, '\0', sizeof(dataBuffer));
		// read from file into data buffer
		int k = read(fd[1], dataBuffer, sizeof(dataBuffer));
		// if error reading
		if(k < 0){
			fprintf(stderr, "ERROR: reading");
			exit(1);
		}
		// void pointer to data buffer
		void *p = dataBuffer;
		// while there are still bytes to be read,
		// Send contents of key file to server
		while(k > 0){
			int bytes_written = send(socketFD, p, k, 0);
			if(bytes_written <= 0){
				fprintf(stderr, "error writing bytes");
				exit(1);
			}
			k -= bytes_written;
			p += bytes_written;
		}
	}
	sleep(1);


	memset(dataBuffer, '\0', sizeof(dataBuffer));
	// Print decoded message
	for (i = 0; i < finalSize; i++){
		int k = recv(socketFD, dataBuffer, sizeof(dataBuffer), 0);

		printf("%c", dataBuffer[i]);
	}
	printf("\n");
	// Close the socket
	close(socketFD);
	return 0;
}
