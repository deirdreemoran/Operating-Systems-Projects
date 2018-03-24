/***********************************************************************
*  Author: Deirdre Moran
*  Program: otp_enc_d.c
*  Date: 3/17/2018
*  Description: Server end decryption
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

#define MAX_BUFFER 80000
int main(int argc, char *argv[]) {

    // variables for sockets and buffers
    int sockFD, sockFD2;
    char decodeBuffer[MAX_BUFFER];
    // port number
    int sPortNum;
    // pid for forking child process
    pid_t pid;
    // child exit status variables
    int status, childExitStatus;

	// change the port number argument to integer
    sPortNum = atoi(argv[1]);


   	// Set up the socket
   	sockFD = socket(AF_INET, SOCK_STREAM, 0);
   	// If error creating socket
   	if(sockFD < 0){
        fprintf(stderr, "ERROR: Socket error");
        exit(1);
    }
    // socket address struct
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(sPortNum);
    sock.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to the client address
    if(bind(sockFD, (struct sockaddr *) &sock, sizeof(sock)) < 0){
        fprintf(stderr, "ERROR: Socket binding");
        exit(1);
    }

    // listen for connections
    if(listen(sockFD, 10) < 0){
        fprintf(stderr, "ERROR: Socket listening");
    }

    // Loop for server
    while(1) {
		// Close socket
		close(sockFD2);
		// accept any incoming connections
        sockFD2 = accept(sockFD, NULL, NULL);
        // error if socket does not accept
        if(sockFD2 < 0) {
            fprintf(stderr, "ERROR: Socket Accept\n");
       		exit(1);
       }

        // fork a new process
        pid = fork();
        // If there was an error forking
        if (pid < 0) {
            fprintf(stderr, "ERROR: Forking\n");
       		exit(1);
       }
		// If it is the child process
        if (pid == 0)
        {
			// variables to store file and key sizes
			char fSize[20];
			char keySize[20];
            // Buffers to hold file contents
			char buffer[MAX_BUFFER];
			char buffer2[MAX_BUFFER];
			memset(buffer, '\0', sizeof(buffer));
			memset(buffer2, '\0', sizeof(buffer2));

			// Get the plaintext file size
			recv(sockFD2, fSize, sizeof(fSize), 0);
			sleep(1);
			// Get the key file size
			recv(sockFD2, keySize, sizeof(keySize), 0);
			sleep(1);


			// Receive plaintext data into buffer
			int k = recv(sockFD2, buffer, sizeof(buffer), 0);

			// if error reading
			if(k < 0){
				fprintf(stderr, "ERROR: reading");
				exit(1);
			}
			//Receive key file data into buffer2
			int kk = recv(sockFD2, buffer2, sizeof(buffer2), 0);
			// if error reading
			if(kk < 0){
				fprintf(stderr, "ERROR: reading");
				exit(1);
			}

			// Convert file and key sizes to integers
			int fileSize = atoi(fSize);
			int kSize = atoi(keySize);

			// For each character in file data buffer
			int i = 0;
			for(i = 0; i < fileSize; i++){
                // Turn space into at symbol for both file and key buffers
			 	if (buffer[i] == ' ')
			 	{
					buffer[i] = '@';
				}
				if (buffer2[i] == ' ')
				{
					buffer2[i] = '@';
				}
				// Change the chars to integers
			    int plaintext = (int) buffer[i];
				int myKey = (int) buffer2[i];
	            // Subtract 64 for ASCII
			    plaintext = plaintext - 64;
			    myKey = myKey - 64;
			    int modCrypt = plaintext - myKey;
			    if(modCrypt < 0){
					modCrypt = modCrypt + 27;
				}
				modCrypt = modCrypt + 64;
                // Change integers to chars again
			    decodeBuffer[i] = (char) modCrypt + 0;
                // Turn at symbols to spaces again
			    if (decodeBuffer[i] == '@')
			    {
			        decodeBuffer[i] = ' ';
			    }
			}

			k = fileSize;
			// void pointer to encoded buffer
			void *ePtr = decodeBuffer;
			// while there are still bytes to be read
			while(k > 0){
			int bytes_sent = send(sockFD2, ePtr, k, 0);
				if(bytes_sent <= 0){
					printf("error writing bytes");
				}
				k -= bytes_sent;
				ePtr += bytes_sent;
			}
		}
		if(pid > 0){
			close(sockFD2);
			exit(0);
		}	// end of parent inner fork
	//end of while loop
	close(sockFD2);
	}
	close(sockFD);
}








