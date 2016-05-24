#define SN_LENGTH 7

#define ENV_SIZE_X 198				//(28*SN_LENGTH+2)
#define ENV_SIZE_Y 30
#define ENV_SIZE_TOTAL (ENV_SIZE_X*ENV_SIZE_Y)

#define INSIDE_BOX (x>=x1 && x<=x2 && y>=y1 && y<=y2)
#define LINE_PLOT_X1 0
#define LINE_PLOT_X2 10000
#define LINE_PLOT_Y1 0
#define LINE_PLOT_Y2 300

#define FLUX_PLOT_Y1 -0.05
#define FLUX_PLOT_Y2 0.05

#define ERR_INVALID_INPUT 1
#define ERR_MALLOC_FAIL 2
#define ERR_FILE_OPEN 3
#define ERR_PGPLOT 4

#define MAX_LINE 10000


// generic array allocations (square lattice)
float* allocateArray1D(const int dim1);
int* allocateArray1DInt(const int dim1);
float** allocateArray2D(const int dim1, const int dim2);
int** allocateArray2DInt(const int dim1, const int dim2);
float*** allocateArray3D(const int dim1, const int dim2, const int dim3);
int*** allocateArray3dInt(const int dim1, const int dim2, const int dim3);

// generic array operations
void randomizeArray2D(float** array, int dim1, int dim2);
void printArray1D(float* array, int dim1);
void printArray1DInt(int* array, int dim1);
void printArray2D(float** array, int dim1, int dim2);
void printArray2DInt(int** array, int dim1, int dim2);
void copyArray2D(float** arrayOld, float** arrayNew, int dim1, int dim2);

// calculations
void fixBoundaryConditions(float** simEnv);
void loadImage(float** simEnv, float* plotImg);

// miscellaneous
void errorCase(const int errorCode);
float** readCSV(const char filename[], const int lengthRow, const int lengthCol);