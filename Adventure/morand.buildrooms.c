/***********************************************************************
*
*  Author: Deirdre Moran
*  Program: morand.buildrooms.c
*  Date: 2/13/2018
*  Description: Builds room files for adventure game incorporating
*		file reading and writing, threading, and mutexes.
*
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Max rooms connections
#define MAX_CONNECTIONS 6
// Max number of selected rooms in game
#define MAX_ROOMS 7
// Number of room types
#define NUM_ROOM_TYPES 3
// Number of total rooms
#define TOTAL_ROOMS 10


/***********************************************************************
*  Structure:		room
*  Description:  	Information for graph of rooms
*  name:  		name of room
*  numConnections: 	number of rooms connected to this room
*  count:		count of rooms
*  roomID:		room ID for non-string room identification
*  roomType:  		type of room (START_ROOM, MID_ROOM, END_ROOM)
*  connectedRooms:	array of pointers to connected room structures
************************************************************************/
struct room
{
	char * name;
	int numConnections;
	int count;
	int roomID;
	char * roomType;
	int connectedRooms[6];
} myRooms[7];


/***********************************************************************
*  Function: 		initializeRooms()
*  Description:		Sets myRoom struct to 0/NULL
*  Parameters:  	None
*  Pre-conditions:	myRoom struct memory allocated
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void initializeRooms()
{
	int i, j, k;
	for (i = 0; i < MAX_ROOMS; i++){
		myRooms[i].name = calloc(16, sizeof(char));
		strcpy(myRooms[i].name, " ");
		myRooms[i].numConnections = 0;
		myRooms[i].count = 0;
		for (j = 0; j < MAX_CONNECTIONS; j++){
			myRooms[i].connectedRooms[j] = 99;
		}
	}
	k = 0;
	for (j = 40; j < 47; j++){
		myRooms[k].roomID = j;
		k++;
	}
}


/***********************************************************************
*  Function: 		randomNumberGenerator()
*  Description:		generates array of random numbers
*  Parameters:  	number of random numbers to generate
*  Pre-condition:	size of array to generate
*  Post-conditions:	None
*  Return:		array of random numbers
************************************************************************/
int * randomNumberGenerator(int size)
{
	int i, j;
	int ranlist[size];
	int ranNum;
	// create unique array of random numbers in specified size range
	ranNum = rand() % size;
	for (i = 0; i < size; i++){
		ranlist[i] = size + 1;
	}
	ranlist[0] = ranNum;
	int checkRan = 0;
	ranNum = rand() % size;
	for (j = 1; j < size; j++){
		for (i = 0; i < size; i++){
			if (ranNum == ranlist[i])
			{
				checkRan++;
			}
		}
		if (checkRan == 0)
		{
			ranlist[j] = ranNum;
		}
		else if (checkRan != 0){
			checkRan = 0;
			j--;
			ranNum = rand() % size;
		}
	}
	int * result;
	result = ranlist;
	return result;
}


/***********************************************************************
*  Function: 		assignRandomRoomNames()
*  Description:		Generates array of unique random numbers to
*			assign random room names
*  Parameters:   	Pointers to hardcoded room names
*  Pre-conditions:	hardcoded room names created
*  Post-conditions:	None
*  Return:		None
************************************************************************/
static void assignRandomRoomNames(char **rmNames)
{
	int i, j, k;
	int * ranNumList;
	ranNumList = randomNumberGenerator(10);
	// Assign random room names to # of rooms
	for(i = 0; i < MAX_ROOMS; i++){
		myRooms[i].name = rmNames[ranNumList[i]];
	}
}


/***********************************************************************
*  Function: 		assignRandomRoomTypes()
*  Description:		Generate an array of unique random numbers
*			and assign random room types
*  Parameters:   	Pointers to hardcoded room types
*  Pre-conditions:	myRoom struct memory set aside
*  Post-conditions:	None
*  Return:		None
************************************************************************/
static void assignRandomRoomTypes(char ** rmTypes)
{
	int i, j, k;
	// Generate another array of unique random numbers to assign random room types
	int * ranNumList2;
	ranNumList2 = randomNumberGenerator(3);
	// Assign random room for first 3 rooms (guarentees 1 start room, 1 end room)
	for(j = 0; j < MAX_ROOMS; j++){
		//Assign random rooms to first 3 random structs
		if (j < NUM_ROOM_TYPES){
			myRooms[j].roomType = rmTypes[ranNumList2[j]];
		}
		//Assign midroom roomtype to remaining rooms
		if (j >= NUM_ROOM_TYPES){
			myRooms[j].roomType = rmTypes[2];
		}
	}
}

/***********************************************************************
*  Function: 		assignRandomNumRoomConnections()
*  Description:		Assigns random number of room connections
*  Parameters:   	None
*  Pre-conditions:	myRoom struct memory set aside
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void assignRandomNumRoomConnections(){
	// Generate list of random numbers for # room connections
	int i, r;
	srand((unsigned)time(NULL));
	for (i = 0; i < MAX_ROOMS; i++){
		int r = rand() % 6 + 1;
	 	do
			r = rand() % 6 + 1;
		while (r < NUM_ROOM_TYPES);

		// Assign random number of connections to a each room
		myRooms[i].numConnections = r;
	}
}


/***********************************************************************
*  Function: 		assignRandomRoomConnections()
*  Description:		Assigns random room connections
*  Parameters:   	None
*  Pre-conditions:	myRoom struct memory set aside
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void assignRandomRoomConnections()
{
	int i, j, k;
	int randomN;
	//for each room
	for (i = 0; i < MAX_ROOMS; i++){
		for(j = 0; j < MAX_CONNECTIONS; j++){
			// Generate random number between 0 and 7
			randomN = (rand() % (7));

			if(randomN == i && i != 6)
			{
				randomN++;
			}
			if (randomN == i && i == 6){
				randomN--;
			}
			// assign random room as connected room
			myRooms[i].connectedRooms[myRooms[i].count] = myRooms[randomN].roomID;
			myRooms[i].count++;
			for (k = 0; k < j; k++){
				if(myRooms[i].connectedRooms[k] == myRooms[randomN].roomID)
				{
					myRooms[i].count--;
					k = j - 1;
					j--;
				}
			}
		}
	}
}

/***********************************************************************
*  Function: 		createRoomFiles()
*  Description:		Creates room files and writes info to for game play
*  Parameters:   	pointers to hardcoded file names
*  Pre-conditions:	myRooms struct memory set aside
*  Post-conditions:	None
*  Return:		None
************************************************************************/
void createRoomFiles(char ** flNames){
	int i, j, k;
	// file pointer
	FILE * myFile;
	char fileBuffer[50];
	// get process id
	int pid = getpid();
	// create directory name
	sprintf(fileBuffer, "morand.rooms.%d", pid);
	// create directory
	mkdir (fileBuffer, 0755);
	char fileBuffer2[50];
	// create seven files from f1Names file names
	for (i = 0; i < MAX_ROOMS; i++){
		snprintf(fileBuffer2, sizeof(fileBuffer2), "morand.rooms.%d/%s", pid, flNames[i]);
		// open file and write contents of myRoom to it
		myFile = fopen(fileBuffer2, "w");
		fprintf(myFile, "ROOM NAME: %s\n", myRooms[i].name);
		for (j = 0; j < myRooms[i].numConnections; j++){
			fprintf(myFile, "CONNECTION %d: ", j + 1);
			for(k = 0; k < MAX_ROOMS; k++){
				if(myRooms[k].roomID == myRooms[i].connectedRooms[j]){
					fprintf(myFile, "%s\n", myRooms[k].name);
				}
			}
		}
		fprintf(myFile, "ROOM TYPE: %s\n", myRooms[i].roomType);
	}
	fclose(myFile);
}




/************************************************************************
*************************************************************************
*************************************************************************
*************************************************************************
***********			       			      ***********
****				  MAIN()		  	      ***
***********						      ***********
*************************************************************************
*************************************************************************
*************************************************************************
************************************************************************/
int main()
{
	// Seed random number
	srand((unsigned)time(NULL));
	int ranNum;
	int i, j, k;

	// Array of hardcoded file names
	char * fileNames[7];
	char fNames[7][100] = { "file1", "file2", "file3", "file4", "file5", "file6", "file7" };
	// Array of 3 ptrs to chars to hold roomtype string
	char * roomTypes[3];
	// Array of hardcoded roomTypes
	char types[3][11] = { "START_ROOM", "END_ROOM", "MID_ROOM" };
	// Pointers to room names and array of hardcoded room names
	char * roomNames[10];
	char names[10][100] = {"Attic", "Basement", "Kitchen", "Loft", "Bedroom", "Den", "Bathroom", "Study", "Closet", "Stable" };
	// Fill array of ptrs to point to hardcoded room names, room types, and file names
	for (i = 0; i < TOTAL_ROOMS; i++)
	{
		roomNames[i] = names[i];
	}
	for (i = 0; i < NUM_ROOM_TYPES; i++){
		roomTypes[i] = types[i];
	}
	for (i = 0; i < MAX_ROOMS; i++){
			fileNames[i] = fNames[i];
	}
	//Initialize rooms
	initializeRooms();
	// Generate an array of unique random numbers and assign random room names
	assignRandomRoomNames(roomNames);
	// Generate an array of unique random numbers and assign random room types
	assignRandomRoomTypes(roomTypes);
	// Assign random number of room connections
	assignRandomNumRoomConnections();
	// Assign random room connections
	assignRandomRoomConnections();
	// Create room files for game play
	createRoomFiles(fileNames);
}
