#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "heat.h"

void fixBoundaryConditions(float** simEnv){
	int x,y;

	// corners
	simEnv[0][0]						 = simEnv[1][1];
	simEnv[ENV_SIZE_X-1][0]				 = simEnv[ENV_SIZE_X-2][1];
	simEnv[ENV_SIZE_X-1][ENV_SIZE_Y-1]	 = simEnv[ENV_SIZE_X-2][ENV_SIZE_Y-2];
	simEnv[0][ENV_SIZE_Y-1]				 = simEnv[1][ENV_SIZE_Y-2];

	// top edge
	y = 0;
	for(x=1; x<(ENV_SIZE_X-1); x++)
		simEnv[x][y] = simEnv[x][y+1];
	// right edge
	x = ENV_SIZE_X-1;
	for(y=1; y<(ENV_SIZE_Y-1); y++)
		simEnv[x][y] = simEnv[x-1][y];
	// bottom edge
	y = ENV_SIZE_Y-1;
	for(x=(ENV_SIZE_X-1); x>0; x--)
		simEnv[x][y] = simEnv[x][y-1];
	// left edge
	x= 0;
	for(y=(ENV_SIZE_Y-1); y>0; y--)
		simEnv[x][y] = simEnv[x+1][y];
}