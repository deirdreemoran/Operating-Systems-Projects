/***********************************************************************
*
*  Author: Deirdre Moran
*  Program: morand.adventure.c
*  Date: 2/13/2018
*  Description: Adventure game incorporating file reading
*		and writing, threading, and mutexes.
*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

// Maximum filepath size
#define MAX_FILEPATH 100
// Maximum number of room connections
#define MAX_CONNECTIONS 6
// Maximum number of total rooms
#define MAX_ROOMS 7
// Minimum number of room connections
#define MIN_ROOM_CONNECTIONS 3
// Maximum number of rooms visited
#define MAX_ROOMS_VISITED 100
// Maximum characters
#define MAX_CHARS 100

// Global variables for POSIX thread, mutex, and room file names
pthread_t tid[1];
pthread_mutex_t mutex;
char myFileArray[MAX_ROOMS][MAX_CHARS];


/***********************************************************************
*  Structure:		room
*  Description:  	Information for graph of rooms
*  name:  		name of room
*  numConnections: 	number of rooms connected to this room
*  roomType:  		type of room (START_ROOM, MID_ROOM, END_ROOM)
*  connectedRooms:	array of pointers to connected room structures
************************************************************************/
struct room
{
	char name[MAX_CHARS];
	int numConnections;
	char roomType[MAX_CHARS];
	struct room * connectedRooms[MAX_CONNECTIONS];
} myRooms[MAX_ROOMS];  // array of accessible room structures

/***********************************************************************
*  Structure: 		visited
*  Description:  	Structure to hold current game state
*  currentRoom:  	index to reference array of room structures
*  numConnections: 	number of rooms connected to this room
*  stepCount:  		counter for number of steps/rooms visited
*  stepRooms:		array of pointers to visited room structures
************************************************************************/
struct visited
{
	int currentRoom;
	int stepCount;
	struct room * stepRooms[MAX_CHARS];
} visitedRooms[1];

/***********************************************************************
*  Function: 		whereToPrompt()
*  Description:  	Prints prompt for user entry, gets next command.
*  Parameters:   	Buffer for userEntry (room name or "time")
*  Pre-conditions:	Buffer is empty
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void whereToPrompt(char * userEntry)
{
		printf("WHERE TO? >");
		//fflush(stdout);
		fflush(stdin);
		//fgets(possibleConnections, sizeof(possibleConnections), stdin);
		scanf("%s", userEntry);
}


/***********************************************************************
*  Function: 		printPossibleConnections()
*  Description:  	Prints rooms connected to current room
*  Parameters:   	Buffer to print connecting rooms, userEntry buffer
*  Pre-conditions:	Buffers are empty, rooms are connected
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void printPossibleConnections(char * possibleConnections, char * userEntry)
{
		int i;
		// Set current room variable and print current room name
		int currentRoom = visitedRooms[0].currentRoom;
		printf("\nCURRENT LOCATION: %s \n", myRooms[currentRoom].name);

		// Format and print all possible connecting rooms
		memset(possibleConnections, '\0', sizeof(possibleConnections));
		for(i = 0; i < myRooms[currentRoom].numConnections - 1; i++){
			strcat(possibleConnections, myRooms[currentRoom].connectedRooms[i]->name);
			strcat(possibleConnections, ", ");
		}
		strcat(possibleConnections, myRooms[currentRoom].connectedRooms[i]->name);
		strcat(possibleConnections, ".");
		printf("POSSIBLE CONNECTIONS: %s\n", possibleConnections);

		// Clear the buffer for next time
		memset(possibleConnections, '\0', sizeof(possibleConnections));

		// Pass buffer to print Where-to prompt function
		whereToPrompt(userEntry);
}


/***********************************************************************
*  Function: 		compareEntry()
*  Description:  	Invokes game action based on userEntry
*  Parameters:   	User input, empty userEntry2 buffer
*  Pre-conditions:	Buffers are empty, rooms are connected
*  Post-conditions:	None
*  Return:		Returns True (0) if game has ended,
*			otherwise False(1)
************************************************************************/
int compareEntry(char * userEntry, char * userEntry2)
{
		char buffer[256];
		int currentRoom = visitedRooms[0].currentRoom;

		int notConnected = 0;
		int newCurrentRoom;
		int errorMarker = currentRoom;
		int i;
		int stepCounter = visitedRooms[0].stepCount;

		// If user enters time, return to main for time command
		if(strcmp(userEntry, "time") == 0){
			return 1;
		}

		// Check that userEntry is a room connected to current room
		for (i = 0; i < myRooms[currentRoom].numConnections; i++){
			if(strcmp(userEntry, myRooms[currentRoom].connectedRooms[i]->name) == 0){
				notConnected++;
			}
		}

		// If user Entry is not a connected room, print error message and return to main
		if(notConnected == 0){
			memset(userEntry, '\0', sizeof(userEntry));
			memset(userEntry2, '\0', sizeof(userEntry2));
			memset(buffer, '\0', sizeof(buffer));
			printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			return 1;
		}

		// Get room index of userEntry, store in newCurrentRoom
		for (i = 0; i < MAX_ROOMS; i++){
			if (strcmp(userEntry, myRooms[i].name) == 0){
				newCurrentRoom = i;
			}
		}

		// If user newCurrentRoom is NOT the end room, make this currentRoom
		if (strcmp(myRooms[newCurrentRoom].roomType, "END_ROOM") != 0){
			// Add room to visitedRooms array, increase stepCount
			visitedRooms[0].stepRooms[visitedRooms[0].stepCount] = &myRooms[newCurrentRoom];
			visitedRooms[0].stepCount++;
			// Set newCurrentRoom and currentRoom
			visitedRooms[0].currentRoom = newCurrentRoom;
			// Clear userEntry buffers
			memset(userEntry, '\0', sizeof(userEntry));
			memset(userEntry2, '\0', sizeof(userEntry));
			return 1;
		}

		// If user newCurrentRoom IS the end room, display congratulation message and print
		// number of steps and rooms visited, return to main with gameOver set to True
		if (strcmp(myRooms[newCurrentRoom].roomType, "END_ROOM") == 0){
			visitedRooms[0].stepRooms[visitedRooms[0].stepCount] = &myRooms[newCurrentRoom];
			visitedRooms[0].stepCount++;
			printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", visitedRooms[0].stepCount );
			for (i = 0; i < visitedRooms[0].stepCount; i++){
				printf("%s\n", visitedRooms[0].stepRooms[i]->name);
			}
			return 0;
		}
}


/***********************************************************************
*  Function: 		getTime()
*  Description:  	Writes current time to file using separate thread
*  Parameters:   	void pointer
*  Pre-conditions:	Thread created, mutex locked
*  Post-conditions:	None
*  Return:		void pointer set to NULL
************************************************************************/
void * getTime(void * passed_in_value)
{
		// lock the mutex in current thread
		pthread_mutex_lock(&mutex);

		// empty buffer and create filepath for writing time
		char buffer[MAX_CHARS];
		memset(buffer,'\0', sizeof(buffer));
		FILE * timeFP;
		char fileName[MAX_FILEPATH];
		memset(fileName, '\0', sizeof(fileName));
		strcpy(fileName, "currentTime.txt");

		// get the current time and store in structure
		time_t currentTime;
		struct tm * info;
		time(&currentTime);
		info = localtime(&currentTime);
		// write formatted time to buffer
		strftime(buffer, MAX_CHARS, "%l:%M%P, %A, %B %d, %Y", info);

		// open file to write to
		timeFP = fopen(fileName, "w");
		// print time to file
		fprintf(timeFP,"%s\n", buffer);
		// close file
		fclose(timeFP);
		// Unlock the mutex
		pthread_mutex_unlock(&mutex);
		return(NULL);
}


/***********************************************************************
*  Function: 		initializeMutex()
*  Description:		locks the main and secondary threads
*  Parameters:   	None
*  Pre-conditions:	global mutex and pthreads created
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void initializeMutex()
{
		// initialize mutex
		pthread_mutex_init(&mutex, NULL);
		// lock mutex in main thread
		pthread_mutex_lock(&mutex);
		// create POSIX thread, will execute when mutex is unlocked
		pthread_create(&(tid[0]), NULL, &getTime, (void*)NULL);
}


/***********************************************************************
*  Function: 		initializeRooms()
*  Description:		Sets myRoom and visitedRoom structs to 0/NULL
*  Parameters:   	None
*  Pre-conditions:	myRoom and visitedRoom struct memory set aside
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void initializeRooms()
{
	int i, j, k;
	// set all structure variables to null
	for (i = 0; i < MAX_ROOMS_VISITED; i++){
		visitedRooms[0].stepRooms[i] = NULL;
	}
	for (i = 0; i < MAX_ROOMS; i++)
	{
		myRooms[i].numConnections = 0;
		memset(myRooms[i].name, '\0', sizeof(myRooms[i].name));
		memset(myRooms[i].roomType, '\0', sizeof(myRooms[i].roomType));
		for(j = 0; j < MAX_CONNECTIONS; j++){
			myRooms[i].connectedRooms[j] = NULL;
		}
	}
	visitedRooms[0].stepCount = 0;
}


/***********************************************************************
*  Function: 		getMostRecentDir()
*  Description:		Gets the most recently created directory of rooms
*  Parameters:   	String to hold room directory name,
*			struct to read directory info into
*  Pre-conditions:	morand.buildrooms.c compilation and execution
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void getMostRecentDir(char * myRoomDir, struct dirent * ent)
{
	int mostRecentDir;
	mostRecentDir = 0;
	struct stat dirStats;
	// directory pointer
	DIR * dir;
	// in current directory
	if (dir = opendir("./")){
		// read contents of directory
		while ((ent = readdir(dir)) != NULL){
			// if directory name begins morand.rooms
			if(strstr(ent->d_name, "morand.rooms") != NULL){
				// get stat on time of creation
				stat(ent->d_name, &dirStats);
				// if time of creation is newer than most recent dir
				// make that the most recent directory by copying in name
				if(((int)dirStats.st_mtime) > mostRecentDir){
					mostRecentDir = (int) dirStats.st_mtime;
					strcpy(myRoomDir, ent->d_name);
				}
			}
		}
	}
}


/***********************************************************************
*  Function: 		createRooms()
*  Description:		Creates rooms for game play
*  Parameters:   	C string containing current directory
*  Pre-conditions:	Current directory is open
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void createRooms(char * myRoomDir){
	int i;
	// file pointer
	FILE * myFile;
	// string to hold line of file
	char line[100];
	int index;
	// string to hold file name
	char result[MAX_FILEPATH];
	// parsing variables
	char firstArg[20];
	char secondArg[20];
	char thirdArg[20];
	int roomCounter = 0;
	// for each file in the directory
	for(index = 0; index < MAX_ROOMS; index++){
		// get name of files
		memset(result, '\0', sizeof(result));
		strcat(result, "./");
		strcat(result, myRoomDir);
		strcat(result, "/");
		strcat(result, myFileArray[index]);
		// open file
		if(	myFile = fopen(result, "r"))
		{
			// while reading file, get line
			while(fgets(line, sizeof(line), myFile) != NULL)
			{
				memset(thirdArg, '\0', sizeof(thirdArg));
				memset(secondArg, '\0', sizeof(secondArg));
				memset(firstArg, '\0', sizeof(firstArg));
				// while scanning line, parse contents into variables
				sscanf(line, "%s %s %s", firstArg, secondArg, thirdArg);
				//	get name of room
				if (strcmp(secondArg, "NAME:") == 0){
					//create room
					strcpy(myRooms[roomCounter].name, thirdArg);
					//add to room array
					fflush(stdout);
					roomCounter++;
				}
				// get type of room
				if (strcmp(secondArg, "TYPE:") == 0){
					strcpy(myRooms[index].roomType, thirdArg);
					if(strcmp(thirdArg, "START_ROOM") == 0){
						visitedRooms[0].currentRoom = index;
					}
				}
				}
		}
		fclose(myFile);
		//free(result);
	}
}

/***********************************************************************
*  Function: 		addConnections()
*  Description:		adds connections to rooms
*  Parameters:   	C string for current directory
*  Pre-conditions:	compilation and execution of morand.buildrooms.c
*  Post-conditions:	None
*  Return:		None
************************************************************************/

void addConnections(char * myRoomDir)
{
	int i;
	// file pointer
	FILE * myFile;
	// string to hold file line
	char line[100];
	int index;
	// filepath name
	char result[MAX_FILEPATH];
	// variables when parsing file lines
	char firstArg[20];
	char secondArg[20];
	char thirdArg[20];
	int roomCounter = 0;
	// for each file in the directory, get the file name and store in result
	for(index = 0; index < MAX_ROOMS; index++){
		memset(result, '\0', sizeof(result));
		strcat(result, "./");
		strcat(result, myRoomDir);
		strcat(result, "/");
		strcat(result, myFileArray[index]);
		// open file
		if(	myFile = fopen(result, "r"))
		{
			while(fgets(line, sizeof(line), myFile) != NULL)
			{
				// clear parsing variables
				memset(thirdArg, '\0', sizeof(thirdArg));
				memset(secondArg, '\0', sizeof(secondArg));
				memset(firstArg, '\0', sizeof(firstArg));
				// read the line and store contents in parser variables
				sscanf(line, "%s %s %s", firstArg, secondArg, thirdArg);
				// create connections
				if(strcmp(firstArg, "CONNECTION") == 0){
					//get struct from room name in thirdArg
					for (i = 0; i < MAX_ROOMS; i++){
						if (strcmp(thirdArg, myRooms[i].name) == 0){
							myRooms[index].connectedRooms[myRooms[index].numConnections] = &myRooms[i];
						}
					}
					myRooms[index].numConnections++;
				}
			}//end of while line loop
		}//end of file loop, close file
		fclose(myFile);
	}//end of for loop
}


/***********************************************************************
*  Function: 		timeThread()
*  Description:		unlocks mutex to execute call to getTime()
*  Parameters:   	None
*  Pre-conditions:	pthread created in main for call to getTime,
*			main thread is locked
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void timeThread()
{
	// unlock mutex to activate 2nd thread call to getTime
	pthread_mutex_unlock(&mutex);
	// return to main thread, end 2nd thread
	pthread_join(tid[0], NULL);
	// lock mutex for main thread
	pthread_mutex_lock(&mutex);
	// recreate 2nd thread for subsequent "time" requests
	pthread_create(&(tid[0]), NULL, &getTime, NULL);
}



/************************************************************************
*************************************************************************
*************************************************************************
*************************************************************************
***********						      ***********
****				MAIN() 		                      ***
***********						      ***********
*************************************************************************
*************************************************************************
*************************************************************************
************************************************************************/
int main()
{
	// Create mutex
	initializeMutex();
	// Initialize rooms
	initializeRooms();

	// Get most recent directory and open it
	int i, j, k;
	char myRoomDir[MAX_CHARS];
	memset(myRoomDir, '\0', sizeof(myRoomDir));
	struct dirent * ent;
	getMostRecentDir(myRoomDir, ent);
	int mfaCount = 0;
	DIR * myDir;
	myDir = opendir(myRoomDir);
	if (myDir){
		readdir(myDir);
		while((ent = readdir(myDir)) != NULL){
			//omit objects directory and parent directory
				if(ent->d_name[0] != '.'){
					strcpy(myFileArray[mfaCount], ent->d_name);
					mfaCount++;
				}
		}
		// While directory is still open, create rooms and connections from each file
		createRooms(myRoomDir);
		addConnections(myRoomDir);
	}
	//close directory
	closedir(myDir);

	// Strings to print and hold buffers
	char possibleConnections[MAX_CHARS];
	char userEntry[MAX_CHARS];
	char userEntry2[MAX_CHARS];

	// Set while loop game over variable to false
	int gameOver = 1;
	// File pointer for writing and reading current time to/from
	FILE * timeFP;

	char timeBuffer[MAX_CHARS];
	char fileName[MAX_FILEPATH];
	strcpy(fileName, "currentTime.txt");
	while (gameOver == 1){
		printPossibleConnections(possibleConnections, userEntry);
		while(strcmp(userEntry, "time") == 0){
			// Thread and mutexes for getting current time
			timeThread();
			// open file to read from to
			memset(timeBuffer,'\0', sizeof(timeBuffer));
			timeFP = fopen(fileName,"r");
			fgets(timeBuffer, sizeof(timeBuffer), timeFP);
			printf("\n%s\n", timeBuffer);
			fclose(timeFP);
			// clear the userEntry buffer
			memset(userEntry, '\0', sizeof(userEntry));
			// Prompt user for next command
			whereToPrompt(userEntry);
		}

		// compare the user entry to possible rooms
		// if gameOver == 0, user found End Room, end game
		gameOver = compareEntry(userEntry, userEntry2);

	}
	pthread_mutex_unlock(&mutex);
	pthread_join(tid[0], NULL);
	pthread_mutex_destroy(&mutex);
	// will block exit until all created threads are done
	pthread_exit(NULL);
}//end of main

