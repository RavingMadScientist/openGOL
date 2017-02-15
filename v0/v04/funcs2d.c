#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//local routine used by fillGrid to parse input file
void readFChars(char* fileName, int** gp, int gridRows, int gridCols){
    int gSize = gridRows * gridCols;
    printf("opening specified file: %s\n", fileName);
    FILE * fp;
    fp = fopen(fileName, "r");
//    fp = fopen("inFile.txt", "r");
    if (fp == NULL) {
        printf ("error opening inFile\n");
        fclose(fp);
        }
    else {    
        int pos = 0;
        int row = 0;
        int col = 0;

        int thisChar= 0;
        char charOut;
        int thisVal;
        while ((thisChar != EOF) && (pos<gSize)){
            int delim = 0;
            thisVal = 0;
            char locString[50];
            char* lsp;
            lsp = locString;
            while (delim == 0){
                thisChar = getc(fp);            
                charOut = thisChar;
                //printf("char %d: %d=%c\n",pos, thisChar, charOut);
                if ((charOut!='\t')&&(charOut!='\n')&&(charOut!=',')&&(charOut!=';')&&(charOut!=EOF)  ){
                    if(charOut!=' '){
                        *lsp = charOut;
                        lsp++;
                        }
                    //printf("locString %d: %s\n",pos,locString );

                    }                
                else{
                    //printf("locString %d: %s\n",pos,locString );
                    thisVal = atoi(locString);
                    //printf("val %d: %d\n",pos,thisVal );
                    delim=1;
                    }
                }
            //printf("val %d: %d\n",pos,thisVal );
            
        // the pointer handling is a bit trickier for the 2d case.
        // we will need to convert pos to row, col

//            *gp = thisVal;
//            gp++;  
            col = pos % gridCols;
            row = pos / gridCols;
            gp[row][col] = thisVal;   
            printf("val [%d][%d]: \t%d\n",row,col,thisVal );    
            pos++;
            }
        fclose(fp);
        printf("clean break\n");
        }

    }



int** fillGrid(char* fileName, int** ggPointer, int gridRows, int gridCols){

    printf("fillGrid initiated\n");
    readFChars(fileName, ggPointer, gridRows, gridCols);
    //now we are going to print out the way the grid looks
    printf("grid loaded by fillGrid:\n");
    int i,j;
    //int* locPointer = ggPointer;
    int gval;
    for (i=0; i<gridRows; i++){
        for (j=0; j<gridCols; j++){
            gval = ggPointer[i][j];
            printf("grid[%d][%d]:\t%d\n",i,j,gval);
            }        
        }

//    for (i=0; i<gSize; i++){
//        gval = *ggPointer;
//        printf("pos: %d\t val: %d\n",i,gval);
//        ggPointer++;
//        }
    printf("fillGrid complete;");
    return ggPointer;

    }


void runGrid2D(int** ggPointer, int gridRows, int gridCols) {

    printf("runGrid initiated\n");

    printf("grid loaded by runGrid:\n");
    int i,j;
    //int* locPointer;
    //locPointer = ggPointer;
    int gval;
    for (i=0; i<gridRows; i++){
        for (j=0; j<gridCols; j++){
            gval = ggPointer[i][j];
            printf("grid[%d][%d]:\t%d\n",i,j,gval);
            }        
        }

    //printf("pointer at:%d\n",ggPointer);
//    for (i=0; i<gSize; i++){
//        gval = *ggPointer;
//        printf("pos: %d\t val: %d\n",i,gval);
//        ggPointer++;
//        //printf("pointer at:%d\n",ggPointer);
//        }

    printf("runGrid complete\n");
    }

void evolveGame2D(float* rPointer, float** bgPointer, int* updates, int gRows, int gCols, int periodicMode, float* dataPointer){
    //printf("periodic = %d\n",periodicMode);
    int gSize = gRows * gCols;
    int lives = 0;
    float threshold= 0.05; // primarily for output stats
    int i,j,k,n,x,y;
//    for (i=0; i<gSize; i++){
//        printf("evolveGame input@pos %d: %f\n", i, rPointer[i]);
//        }
    k=0;
    for (i=0; i<gRows; i++){
        for (j=0; j<gCols; j++){
            //bgPointer[i][j] = 0.0;
            bgPointer[i][j] = rPointer[k];
            k++;   
            }
        }
 //    for (i=0; i<gRows; i++){
//        for (j=0; j<gCols; j++){
//            float scoree=0;
//            scoree = bgPointer[i][j];
////            printf("input blankGrid@pos (%d, %d): %f\n", i, j, bgPointer[i][j]);
//            printf("input blankGrid@pos (%d, %d): %f\n", i, j, scoree);
//
//            }
//        }
// and now the actual logic loops!
    k=0;
    float score;
    float convertScore;
    if (periodicMode == 1){
        int loci[8], locj[8];
        for (i=0; i<gRows; i++){
            if (i == 0){
                loci[0] = gRows-1;  
                loci[1] = gRows-1;
                loci[2] = gRows-1;          
                }
            else{
                loci[0] = i-1;  
                loci[1] = i-1;
                loci[2] = i-1;                  
                }
            loci[3] = i;
            loci[4] = i;
            if (i == (gRows-1)){
                loci[5] = 0;  
                loci[6] = 0;
                loci[7] = 0;          
                }
            else{
                loci[5] = i+1;  
                loci[6] = i+1;
                loci[7] = i+1;                  
                }           
 
            for (j=0; j<gCols; j++){
                //bgPointer[i][j] = 0.0;
                if (j == 0){
                    locj[0] = gCols-1;  
                    locj[3] = gCols-1;
                    locj[5] = gCols-1;          
                    }
                else{
                    locj[0] = j-1;  
                    locj[3] = j-1;
                    locj[5] = j-1;                  
                    }
                locj[1] = j;
                locj[6] = j;
                if (j == (gCols-1)){
                    locj[2] = 0;  
                    locj[4] = 0;
                    locj[7] = 0;          
                    }
                else{
                    locj[2] = j+1;  
                    locj[4] = j+1;
                    locj[7] = j+1;                  
                    }                     
                score = 0.0;
                //printf("Position %d at Grid(%d,%d): ", k, i, j);
                for (n=0; n<8; n++){
                    x=loci[n];
                    y=locj[n];
                    //printf("%d:(%d,%d) ",n , x, y);
                    score += bgPointer[x][y];
                    }
                //printf(" \n");
                convertScore = ((score < 1.95) || (score>3.05)) ? 0.0 : 1.0;
                updates[k] = abs(convertScore - rPointer[k]) > .05? 1: 0;                                
                rPointer[k] = convertScore;
                if (convertScore >= threshold){
                    lives+=1;
                    }
                //printf("Position %d at Grid(%d,%d): %f neighbors -> %f \n", k, i, j, score, convertScore);
                k++;   
                }   
            }
        }
    else{
        //printf("nonPeriodic\n");
        int loci[8], locj[8];
        for (i=0; i<gRows; i++){
            if (i == 0){
                loci[0] = -1;  
                loci[1] = -1;
                loci[2] = -1;          
                }
            else{
                loci[0] = i-1;  
                loci[1] = i-1;
                loci[2] = i-1;                  
                }
            loci[3] = i;
            loci[4] = i;
            if (i == (gRows-1)){
                loci[5] = -1;  
                loci[6] = -1;
                loci[7] = -1;          
                }
            else{
                loci[5] = i+1;  
                loci[6] = i+1;
                loci[7] = i+1;                  
                }           
 
            for (j=0; j<gCols; j++){
                //bgPointer[i][j] = 0.0;
                if (j == 0){
                    locj[0] = -1;  
                    locj[3] = -1;
                    locj[5] = -1;          
                    }
                else{
                    locj[0] = j-1;  
                    locj[3] = j-1;
                    locj[5] = j-1;                  
                    }
                locj[1] = j;
                locj[6] = j;
                if (j == (gCols-1)){
                    locj[2] = -1;  
                    locj[4] = -1;
                    locj[7] = -1;          
                    }
                else{
                    locj[2] = j+1;  
                    locj[4] = j+1;
                    locj[7] = j+1;                  
                    }                     
                score = 0.0;
                //printf("Position %d at Grid(%d,%d): ", k, i, j);
                for (n=0; n<8; n++){
                    x=loci[n];
                    y=locj[n];
                    //printf("%d:(%d,%d) ",n , x, y);
                    if((x>=0)&&(y>=0)){
                        score += bgPointer[x][y];
                        }                    
                    }
                //printf(" \n");
                convertScore = ((score < 1.95) || (score>3.05)) ? 0.0 : 1.0;
                updates[k] = abs(convertScore - rPointer[k]) > .05? 1: 0;                                
                rPointer[k] = convertScore;
                //printf("Position %d at Grid(%d,%d): %f neighbors -> %f \n", k, i, j, score, convertScore);
                if (convertScore >= threshold){
                    lives+=1;
                    }
                k++;   
                
                }   
            }        
        }  

    float pct;
    int intpct;
    pct = 100.0* (float) lives / (float) gSize;
    intpct = (int) (pct*100.0);
    pct = (float) intpct / 100.0;
    dataPointer[0]=dataPointer[0]+1;
    dataPointer[1]=lives;
    dataPointer[2]=pct;
    dataPointer[3]=99.1;
    dataPointer+=3;
    *dataPointer = 77.1;

    }