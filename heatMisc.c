#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "heat.h"

void randomizeArray2D(float** array, int dim1, int dim2){
	int i, j;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			array[i][j] = ((float)rand()/(float)RAND_MAX);
		}
	}
}

void printArray1D(float* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%g ", array[i]);
	printf("\n");
}

void printArray1DInt(int* array, int dim1){
	int i;
	for (i=0; i<dim1; i++)
		printf("%i", array[i]);
	printf("\n");
}

void printArray2D(float** array, int dim1, int dim2){
	int i, j;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			printf("%g ", array[i][j]);
		}
		printf("\n");
	}
}

void printArray2DInt(int** array, int dim1, int dim2){
	int i, j;

	for (i=0; i<dim1; i++){
		for (j=0; j<dim2; j++){
			printf("%i", array[i][j]);
		}
		printf("\n");
	}
}

void copyArray2D(float** arrayOld, float** arrayNew, int dim1, int dim2){
	int i, j;
	for (i=0; i<dim1; i++)
		for (j=0; j<dim2; j++)
			arrayNew[i][j] = arrayOld[i][j];
}

void errorCase(const int errorCode){
	system("cat nagato");
	switch(errorCode){
		case ERR_INVALID_INPUT:
			printf("Error: invalid input\n");
			exit(-1);
		case ERR_MALLOC_FAIL:
			printf("Error: out of memory\n");
			exit(-1);
		case ERR_FILE_OPEN:
			printf("Error: file cannot be opened\n");
			exit(-1);
		case ERR_PGPLOT:
			printf("Error: cannot open pgplot window\n");
			exit(-1);
	}
}

void loadImage(float** simEnv, float* plotImg){
	int x,y;

	for (x=0; x<ENV_SIZE_X; x++)
		for (y=0; y<ENV_SIZE_Y; y++)
			plotImg[x*ENV_SIZE_Y + y] = 1*simEnv[x][y];
}

float** readCSV(const char filename[], const int lengthRow, const int lengthCol) {
	float** arrayWrite = allocateArray2D(lengthRow, lengthCol);

	char line[MAX_LINE];
	int i, j;
	float temp;
	
	// open input file, read only
	FILE *stream = fopen(filename, "r");

	if (stream == NULL)	
		errorCase(ERR_FILE_OPEN);

	for (i = 0; i < lengthRow; i++) {
		// grab next line of stream
		fgets(line, MAX_LINE, stream);

		// convert to float, everything before the comma delimiter
		temp = atof(strtok(line, ",\n"));
		for (j = 0; j < (lengthCol - 1); j++) {
			arrayWrite[i][j] = temp;		//save float in matrix
			temp = atof(strtok(NULL, ",\n"));	//keep reading from last position
		}
		arrayWrite[i][j] = temp;		//save last line
	}
	
	// cleanup
	fclose(stream);
	printf("%s read successful!\n", filename);
	return arrayWrite;
}
