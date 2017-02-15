/* There are some significant changes between funcs1d and
its rather ineffectual predecessor, ../2D.skelFuncs

for one, we no longer define the gridObj type, rather,
this is done in main(), and the library functions are 
only passed pointers to the array component of the gridObj

*/

//fParse function imbedded locally within fillGrid
void fillGrid1D(char* fileName, float* ggPointer, int gSize);

//runGrid still is not outputting files, only displaying results
void runGrid1D(float* ggPointer, int gSize);
