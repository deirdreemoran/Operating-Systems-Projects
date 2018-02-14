#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
//#include <fcnt1.h>


#define MAX_CONNECTIONS 6
#define MAX_ROOMS 7
#define INITIAL_ROOM_TYPES 3
#define TOTAL_ROOMS 10

struct room
{
	char * name;
	int numConnections;
	int count;
	int roomID;
	char * roomType;
	int connectedRooms[6];
} myRooms[7];

void initializeRooms(){
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


int * randomNumberGenerator(int size){
		int i, j, k;

	int ranlist[size];
	int ranNum;
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






static void assignRandomRoomNames(char **rmNames){
		int i, j, k;

	int * ranNumList;
	ranNumList = randomNumberGenerator(10);
	// Assign random room names to 7 rooms
	for(i = 0; i < MAX_ROOMS; i++){
		myRooms[i].name = rmNames[ranNumList[i]];
	}
}

static void assignRandomRoomTypes(char ** rmTypes){
		int i, j, k;

	// Generate another array of unique random numbers to assign random room types
	int * ranNumList2;
	ranNumList2 = randomNumberGenerator(3);
	// Assign random room for first 3 rooms (guarentees 1 start room, 1 end room)
	for(j = 0; j < MAX_ROOMS; j++){
		//Assign random rooms to first 3 random structs
		if (j < INITIAL_ROOM_TYPES){
			myRooms[j].roomType = rmTypes[ranNumList2[j]];
		}
		//Assign midroom roomtype to remaining rooms
		if (j >= INITIAL_ROOM_TYPES){
			myRooms[j].roomType = rmTypes[2];
		}
	}
}

void assignRandomNumRoomConnections(){
	// Generate list of random numbers for # room connections
		int i, j, k, r;

		srand((unsigned)time(NULL));
		int peter[7];
		int * ranNumList4;
		for (k = 0; k < MAX_ROOMS; k++){
			int r = rand() % 6 + 1;
		 	do
				r = rand() % 6 + 1;
			while (r < INITIAL_ROOM_TYPES);
			peter[k] = r;
			// Assign random number of connections to a each room
			myRooms[k].numConnections = r;
	}

}



void assignRandomRoomConnections(){
	int i, j;
	int randomN;
	int w;
	for (i = 0; i < MAX_ROOMS; i++){
		j = 0;
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

					myRooms[i].connectedRooms[myRooms[i].count] = myRooms[randomN].roomID;
					myRooms[i].count++;
					for (w = 0; w < j; w++){
						if(myRooms[i].connectedRooms[w] == myRooms[randomN].roomID)
						{
							myRooms[i].count--;
							w = j - 1;
							j--;
						}
					}
			}

}
}



void createRoomFiles(char ** flNames){
	int i, j, k;
	FILE * myFile;
		char fileBuffer[50];
		int pid = getpid();
		sprintf(fileBuffer, "morand.rooms.%d", pid);
		mkdir (fileBuffer, 0755);
		char fileBuffer2[50];
		for (i = 0; i < MAX_ROOMS; i++){
			snprintf(fileBuffer2, sizeof(fileBuffer2), "morand.rooms.%d/%s", pid, flNames[i]);
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








int main(){

		srand((unsigned)time(NULL));
	int ranNum;
	int i, j, k;

	// Array of hardcoded file names
	char * fileNames[7];
	char fNames[7][100] = { "file1", "file2", "file3", "file4", "file5", "file6", "file7" };
	// Array of hardcoded roomTypes
	char types[3][11] = { "START_ROOM", "END_ROOM", "MID_ROOM" };
	// Array of 3 ptrs to chars
	char * roomTypes[3];
	char buffer[100];
	// Array of hardcoded room names
	char * roomNames[10];
	char names[10][100] = {"Attic", "Basement", "Kitchen", "Loft", "Bedroom", "Den", "Bathroom", "Study", "Closet", "Stable" };
	// Fill array of ptrs to point to hardcoded room names
	for (i = 0; i < TOTAL_ROOMS; i++)
	{
		roomNames[i] = names[i];
	}
	for (i = 0; i < INITIAL_ROOM_TYPES; i++){
		roomTypes[i] = types[i];
	}
	for (i = 0; i < MAX_ROOMS; i++){
			fileNames[i] = fNames[i];
	}

	//Initialize rooms
	initializeRooms();

	// Generate an array of unique random numbers to assign random room names
	assignRandomRoomNames(roomNames);

		// Generate another array of unique random numbers to assign random room types

	assignRandomRoomTypes(roomTypes);
	// ?????????room ids ??
// Assign random number of room connections
	assignRandomNumRoomConnections();

//------------------------------------------------------------------------------------
assignRandomRoomConnections();

createRoomFiles(fileNames);


}

