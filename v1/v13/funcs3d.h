/* There are some significant changes between funcs1d and
its rather ineffectual predecessor, ../2D.skelFuncs

for one, we no longer define the gridObj type, rather,
this is done in main(), and the library functions are 
only passed pointers to the array component of the gridObj

*/


//fParse function imbedded locally within fillGrid
int** fillGrid3D(char* fileName, int** ggPointer, int gridRows, int gridCols);

//runGrid still is not outputting files, only displaying results
void runGrid3D(int** ggPointer, int gridRows, int gridCols);

void evolveGame3D(float* rPointer, float*** b3DMPointer, int* updates, int gRows, int gCols, int gStacks, int periodicMode, float* dataPointer);
