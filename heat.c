#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "cpgplot.h"
#include "heat.h"

int main(){
	printf("\n====================================================================\n");
	printf("This program is able to simulate the diffusion of heat\n");
	printf("across a metal plate of size %i x %i\n", ENV_SIZE_X, ENV_SIZE_Y);
	printf("====================================================================\n");

	//==========================================================================
	//--------------------------SYSTEM INITIALIZATIONS--------------------------
	//==========================================================================
	
	// initialize random seed
	srand(time(NULL));

	// force print all outputs (remove stdout buffer)
	setbuf(stdout, NULL);

	// initialize pgplot window
	if (!cpgopen("/XWINDOW"))
		errorCase(ERR_PGPLOT);

	cpgpap(0.0, 0.6);						// set window size
	cpgsubp(1,3);						// subdivide window into panels
	// heatmap
	cpgpanl(1,1);
	cpgsvp(0.0, 1.0, 0.0, 1.0);
	cpgswin(0, ENV_SIZE_X, 0, ENV_SIZE_Y);
	// flux plot
	cpgpanl(1,2);
	cpgsvp(0.08, 0.92, 0.08, 0.92);
	cpgswin(LINE_PLOT_X1, LINE_PLOT_X2, FLUX_PLOT_Y1, FLUX_PLOT_Y2);
	cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
	cpglab("Time", "Flux", "");
	// heat plot
	cpgpanl(1,3);
	cpgsvp(0.08, 0.92, 0.08, 0.92);
	cpgswin(LINE_PLOT_X1, LINE_PLOT_X2, LINE_PLOT_Y1, LINE_PLOT_Y2);
	cpgbox("ABCINTS", 0.0, 0, "ABCINTS", 0.0, 0);
	cpglab("Time", "Total Heat", "");

	// initialize color table for pgplot display
  	float rl[9] = {-0.5, 0.0, 0.17, 0.33, 0.50, 0.67, 0.83, 1.0, 1.7};
  	float rr[9] = { 0.0, 0.0,  0.0,  0.0,  0.6,  1.0,  1.0, 1.0, 1.0};
  	float rg[9] = { 0.0, 0.0,  0.0,  1.0,  1.0,  1.0,  0.6, 0.0, 1.0};
  	float rb[9] = { 0.0, 0.3,  0.8,  1.0,  0.3,  0.0,  0.0, 0.0, 1.0};
  	cpgctab(rl, rr, rg, rb, 512,  1.0, 0.5);
	cpgscr(10, 0.0, 0.0, 1.0);
	cpgscr(11, 1.0, 0.0, 0.0);
	cpgsfs(3);


	//==========================================================================
	//--------------------------VARIABLE INITIALIZATIONS------------------------
	//==========================================================================

	// generic variables
	int i, j, k;						// counters

	// simulation environment
	float** simEnvEven = allocateArray2D(ENV_SIZE_X, ENV_SIZE_Y);
	float** simEnvOdd = allocateArray2D(ENV_SIZE_X, ENV_SIZE_Y);
	float* simLocal = allocateArray1D(5);

	// mnist handwritten numbers
	float** mnistDatabase = readCSV("mnist_train_100.csv", 100, 785);
	for (i=0; i<100; i++)
		for (j=0; j<785; j++)
			mnistDatabase[i][j] = mnistDatabase[i][j]/255.0;

	// current location and time
	int x,y,z;
	int t, tGlobal;

	// student number
	int studentNumbRaw;
	int studentNumbWorking;
	int studentNumb[7];

	// rates
	float rateDiff = 0.2;
	float delta;

	// flux variables
	float flux;
	float fluxTotal;
	float fluxAverage;
	float fluxHeat;
	float totalHeat;
	int x1, x2, y1, y2;

	// background heat
	float bgHeat;

	// tracking variables
	float totalHeatOld;
	float totalHeatPre;
	float tGlobalOld;
	float fluxOld;

	// pgplot variables
	float* plotImg = allocateArray1D(ENV_SIZE_TOTAL);
	float TR[6] = {0, 0, 1, ENV_SIZE_Y, -1, 0};
	float plotMinBound = 0;
	float plotMaxBound = 1;

	//==========================================================================
	//--------------------------------SETUP-------------------------------------
	//==========================================================================
	
	// ask for student number
	printf("Please enter your student number:\n");
	if (scanf("%i", &studentNumbRaw) == 0)
		errorCase(ERR_INVALID_INPUT);
	studentNumbWorking = studentNumbRaw;
	for (i=0; i<SN_LENGTH; i++){
		studentNumb[6-i] = studentNumbWorking%10;
		studentNumbWorking /= 10;
	}
	printf("\nYour student number is:\n");
	for (i=0; i<SN_LENGTH; i++)
		printf("%i", studentNumb[i]);
	printf("\n\n");

	// set and print diffusion rate based on last digit of student number
	rateDiff = ((((float)(studentNumb[6]))/10.0)*0.19)+0.01;
	printf("Your Diffusion Rate is: \n%f\n\n", rateDiff);

	// set and print background heat added based on last 4 digits of student number
	studentNumbRaw -= 1410000;
	bgHeat = ((float)((studentNumbRaw%97)%10));
	bgHeat += ((float)((studentNumbRaw%101)%8))*10;
	bgHeat /= 100;
	printf("Your Background Heat is: \n%f\n\n", bgHeat*100);

	// set and print domain for calculating flux
	// x1, y1 based on last four digits of student number
	x1 = studentNumbRaw % ENV_SIZE_X;
	y1 = studentNumbRaw % ENV_SIZE_Y;
	// x2, y2 based on last four digits of student number
	x2 = x1 + (studentNumbRaw % (97));
	if (x2 >= ENV_SIZE_X)
		x2 = ENV_SIZE_X - 1;
	y2 = y1 + (studentNumbRaw % (29));
	if (y2 >= ENV_SIZE_Y)
		y2 = ENV_SIZE_Y - 1;
	printf("Your Domain is: \n(%i, %i) X (%i, %i)\n\n", x1, y1, x2, y2);

	// environment initialization:
	// select digits and place into environment
	for (i=0; i<SN_LENGTH; i++){
		if (studentNumb[i] == 0)
			z = 0;
		else if (studentNumb[i] == 1)
			z = 13;
		else if (studentNumb[i] == 2)
			z = 27;
		else if (studentNumb[i] == 3)
			z = 33;
		else if (studentNumb[i] == 4)
			z = 44;
		else if (studentNumb[i] == 5)
			z = 55;
		else if (studentNumb[i] == 6)
			z = 60;
		else if (studentNumb[i] == 7)
			z = 71;
		else if (studentNumb[i] == 8)
			z = 81;
		else
			z = 89;

		for (x=0; x<28; x++)
			for (y=0; y<28; y++) {
				simEnvEven[x+(i*28)+1][y+1] = mnistDatabase[z][y*28+x] + bgHeat;
				if (simEnvEven[x+(i*28)+1][y+1] > 1.0)
					simEnvEven[x+(i*28)+1][y+1] = 1.0;
			}
	}


	//==========================================================================
	//--------------------------ACTUAL CODE-------------------------------------
	//==========================================================================

	// initialize display
	fixBoundaryConditions(simEnvEven);
	copyArray2D(simEnvEven, simEnvOdd, ENV_SIZE_X, ENV_SIZE_Y);
	loadImage(simEnvEven, plotImg);
	cpgpanl(1,1);
	cpgsvp(0.0, 1.0, 0.0, 1.0);
	cpgswin(0, ENV_SIZE_X, 0, ENV_SIZE_Y);
	cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);
	cpgrect(x1, x2, y1, y2);

	// initialize trackers
	tGlobalOld = 0;
	fluxOld = 0;
	totalHeatOld = 0;
	for (x=x1; x<=x2; x++)
		for (y=y1; y<=y2; y++)
			totalHeatOld += simEnvEven[x][y];

	// initial delay to visualize starting matrix
	for (t=0; t<500000000; t++){}
	
	t = 0;
	tGlobal = 0;
	flux = 0;
	fluxAverage = 0;
	fluxTotal = 0;
	while(1){
		flux = 0;
		cpgpanl(1,1);
		cpgsvp(0.0, 1.0, 0.0, 1.0);
		cpgswin(0, ENV_SIZE_X, 0, ENV_SIZE_Y);

		// calculate heat changes using numeric methods
		fixBoundaryConditions(simEnvEven);

		//simEnvEven[50][15] = 100;
		//simEnvEven[60][15] = -10;

		copyArray2D(simEnvEven, simEnvOdd, ENV_SIZE_X, ENV_SIZE_Y);

		for (x=1; x<(ENV_SIZE_X-1); x++)
			for (y=1; y<(ENV_SIZE_Y-1); y++)
				if ((x+y)%2 == 0) {
					delta = rateDiff*(simEnvEven[x][y+1] - 2*simEnvEven[x][y] + simEnvEven[x][y-1]);
					simEnvOdd[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
					delta = rateDiff*(simEnvEven[x+1][y] - 2*simEnvEven[x][y] + simEnvEven[x-1][y]);
					simEnvOdd[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
				}
		for (x=1; x<(ENV_SIZE_X-1); x++)
			for (y=1; y<(ENV_SIZE_Y-1); y++)
				if ((x+y)%2 == 1) {
					delta = rateDiff*(simEnvOdd[x][y+1] - 2*simEnvOdd[x][y] + simEnvOdd[x][y-1]);
					simEnvOdd[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
					delta = rateDiff*(simEnvOdd[x+1][y] - 2*simEnvOdd[x][y] + simEnvOdd[x-1][y]);
					simEnvOdd[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
				}
		loadImage(simEnvOdd, plotImg);
		cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);
		cpgrect(x1, x2, y1, y2);
		fluxTotal += flux;
		tGlobal++;

		flux = 0;

		//simEnvOdd[50][15] = 100;
		//simEnvOdd[60][15] = -10;

		fixBoundaryConditions(simEnvOdd);
		
		for (x=1; x<(ENV_SIZE_X-1); x++)
			for (y=1; y<(ENV_SIZE_Y-1); y++)
				if ((x+y)%2 == 1) {
					delta = rateDiff*(simEnvOdd[x][y+1] - 2*simEnvOdd[x][y] + simEnvOdd[x][y-1]);
					simEnvEven[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
					delta = rateDiff*(simEnvOdd[x+1][y] - 2*simEnvOdd[x][y] + simEnvOdd[x-1][y]);
					simEnvEven[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
				}
		for (x=1; x<(ENV_SIZE_X-1); x++)
			for (y=1; y<(ENV_SIZE_Y-1); y++)
				if ((x+y)%2 == 0) {
					delta = rateDiff*(simEnvEven[x][y+1] - 2*simEnvEven[x][y] + simEnvEven[x][y-1]);
					simEnvEven[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
					delta = rateDiff*(simEnvEven[x+1][y] - 2*simEnvEven[x][y] + simEnvEven[x-1][y]);
					simEnvEven[x][y] += delta;
					if (INSIDE_BOX)
						flux += delta;
				}
		loadImage(simEnvEven, plotImg);
		cpgimag(plotImg, ENV_SIZE_Y, ENV_SIZE_X, 1, ENV_SIZE_Y, 1, ENV_SIZE_X, plotMinBound, plotMaxBound, TR);
		cpgrect(x1, x2, y1, y2);
		fluxTotal += flux;
		tGlobal++;



		// flux line plot
		cpgpanl(1,2);
		cpgsvp(0.08, 0.92, 0.08, 0.92);
		cpgswin(LINE_PLOT_X1, LINE_PLOT_X2, FLUX_PLOT_Y1, FLUX_PLOT_Y2);
		cpgmove(tGlobalOld, fluxOld);
		cpgdraw(tGlobal, flux);

		// heat line plot
		totalHeat = 0;
		for (x=x1; x<=x2; x++)
			for (y=y1; y<=y2; y++)
				totalHeat += simEnvEven[x][y];
		cpgpanl(1,3);
		cpgsvp(0.08, 0.92, 0.08, 0.92);
		cpgswin(LINE_PLOT_X1, LINE_PLOT_X2, LINE_PLOT_Y1, LINE_PLOT_Y2);
		cpgmove(tGlobalOld, totalHeatOld);
		cpgdraw(tGlobal, totalHeat);

		// set trackers
		tGlobalOld = tGlobal;
		totalHeatOld = totalHeat;
		fluxOld = flux;

		if (tGlobal%100 == 0) {
			totalHeat = 0;
			for (x=x1; x<=x2; x++)
				for (y=y1; y<=y2; y++)
					totalHeat += simEnvEven[x][y];
			fluxAverage = fluxTotal/tGlobal;
			fluxHeat = totalHeat - totalHeatPre;
			printf("Total Heat: %f \n Current Divergence: %f \n Current Flux:       %f\n\n", totalHeat, flux, fluxHeat);
		}

		totalHeatPre = 0;
		for (x=x1; x<=x2; x++)
			for (y=y1; y<=y2; y++)
				totalHeatPre += simEnvEven[x][y];
	}
}