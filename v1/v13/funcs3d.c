#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//local routine used by fillGrid to parse input file
void readFChars3D(char* fileName, int** gp, int gridRows, int gridCols){
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



int** fillGrid3D(char* fileName, int** ggPointer, int gridRows, int gridCols){

    printf("fillGrid initiated\n");
    readFChars3D(fileName, ggPointer, gridRows, gridCols);
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


void runGrid3D(int** ggPointer, int gridRows, int gridCols) {

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

void evolveGame3D(float* rPointer, float*** b3DMPointer, int* updates, int gRows, int gCols, int gStacks, int periodicMode, float* dataPointer){
    printf("periodic = %d\n",periodicMode);
    int gSize = gRows * gCols * gStacks;
    int lives = 0;
    float threshold= 0.05; // primarily for output stats
    int i,j,k,m,n,x,y,z;
//    for (i=0; i<gSize; i++){
//        printf("evolveGame input@pos %d: %f\n", i, rPointer[i]);
//        }
    m=0;
    for (i=0; i<gStacks; i++){
        for (j=0; j<gCols; j++){
            for (k=0; k<gRows; k++){
            //bgPointer[i][j] = 0.0;
//                if((i==0) || i==(gStacks-1)){
//                    printf("rPointer[%d] = %f\n", m, rPointer[m]);
//                    }
                b3DMPointer[i][j][k] = rPointer[m];
//                if((i==0) || i==(gStacks-1)){
//                    printf("b3DMPointer[%d][%d][%d] = %f\n", i, j, k, b3DMPointer[i][j][k]);
//                    }
                m++;
                }   
            }
        }
    printf("evolve loop 3Dgrid assignment complete\n");

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
    m=0;
    float score;
    float convertScore;
    if (periodicMode == 1){
//        int loci[8], locj[8]; 3D now, homes
        int loci[26], locj[26], lock[26];
        for(i=0; i<gStacks; i++){
            if (i == 0){
                loci[0] = gStacks-1;  
                loci[1] = gStacks-1;
                loci[2] = gStacks-1;    
                loci[3] = gStacks-1;  
                loci[4] = gStacks-1;
                loci[5] = gStacks-1;  
                loci[6] = gStacks-1;  
                loci[7] = gStacks-1;
                loci[8] = gStacks-1;        
                }
            else{
                loci[0] = i-1;  
                loci[1] = i-1;
                loci[2] = i-1;    
                loci[3] = i-1;  
                loci[4] = i-1;
                loci[5] = i-1;  
                loci[6] = i-1;  
                loci[7] = i-1;
                loci[8] = i-1;                 
                }
            loci[9] = i;  
            loci[10] = i;  
            loci[11] = i;
            loci[12] = i;    
            loci[13] = i;  
            loci[14] = i;
            loci[15] = i;  
            loci[16] = i;  
            if (i == (gStacks-1)){

                loci[17] = 0;         
                loci[18] = 0;
                loci[19] = 0; 
                loci[20] = 0;  
                loci[21] = 0;
                loci[22] = 0;    
                loci[23] = 0;  
                loci[24] = 0;
                loci[25] = 0;  
                }
            else{
                loci[17] = i+1;         
                loci[18] = i+1;
                loci[19] = i+1; 
                loci[20] = i+1;  
                loci[21] = i+1;
                loci[22] = i+1;    
                loci[23] = i+1;  
                loci[24] = i+1;
                loci[25] = i+1;        
                }             
 
            for (j=0; j<gCols; j++){
                //bgPointer[i][j] = 0.0;
                if (j == 0){
                    locj[0] = gCols-1;  
                    locj[1] = gCols-1;
                    locj[2] = gCols-1;    
                    locj[9] = gCols-1;  
                    locj[10] = gCols-1;
                    locj[11] = gCols-1;  
                    locj[17] = gCols-1;  
                    locj[18] = gCols-1;
                    locj[19] = gCols-1;        
                    }
                else{
                    locj[0] = j-1;  
                    locj[1] = j-1;
                    locj[2] = j-1;    
                    locj[9] = j-1;  
                    locj[10] = j-1;
                    locj[11] = j-1;  
                    locj[17] = j-1;  
                    locj[18] = j-1;
                    locj[19] = j-1;                 
                    }               
                locj[3] = j;  
                locj[4] = j;  
                locj[5] = j;
                locj[12] = j;    
                locj[13] = j;  
                locj[20] = j;
                locj[21] = j;  
                locj[22] = j;  
                if (j == (gCols-1)){
    
                    locj[6] = 0;         
                    locj[7] = 0;
                    locj[8] = 0; 
                    locj[14] = 0;  
                    locj[15] = 0;
                    locj[16] = 0;    
                    locj[23] = 0;  
                    locj[24] = 0;
                    locj[25] = 0;  
                    }
                else{
                    locj[6] = j+1;         
                    locj[7] = j+1;
                    locj[8] = j+1; 
                    locj[14] = j+1;  
                    locj[15] = j+1;
                    locj[16] = j+1;    
                    locj[23] = j+1;  
                    locj[24] = j+1;
                    locj[25] = j+1;        
                    }      
                for (k=0; k<gRows; k++){
                    //bgPointer[i][j] = 0.0;
                    if (k == 0){
                        lock[0] = gRows-1;  
                        lock[3] = gRows-1;
                        lock[6] = gRows-1;    
                        lock[9] = gRows-1;  
                        lock[12] = gRows-1;
                        lock[14] = gRows-1;  
                        lock[17] = gRows-1;  
                        lock[20] = gRows-1;
                        lock[23] = gRows-1;        
                        }
                    else{
                        lock[0] = k-1;  
                        lock[3] = k-1;
                        lock[6] = k-1;    
                        lock[9] = k-1;  
                        lock[12] = k-1;
                        lock[14] = k-1;  
                        lock[17] = k-1;  
                        lock[20] = k-1;
                        lock[23] = k-1;                 
                        }               
                    lock[1] = k;  
                    lock[4] = k;  
                    lock[7] = k;
                    lock[10] = k;    
                    lock[15] = k;  
                    lock[18] = k;
                    lock[21] = k;  
                    lock[24] = k;  
                    if (k == (gRows-1)){
        
                        lock[2] = 0;         
                        lock[5] = 0;
                        lock[8] = 0; 
                        lock[11] = 0;  
                        lock[13] = 0;
                        lock[16] = 0;    
                        lock[19] = 0;  
                        lock[22] = 0;
                        lock[25] = 0;  
                        }
                    else{
                        lock[2] = k+1;         
                        lock[5] = k+1;
                        lock[8] = k+1; 
                        lock[11] = k+1;  
                        lock[13] = k+1;
                        lock[16] = k+1;    
                        lock[19] = k+1;  
                        lock[22] = k+1;
                        lock[25] = k+1;        
                        }  

                    score = 0.0;
                    for (n=0; n<26; n++){
                        z=loci[n];
                        y=locj[n];
                        x=lock[n]; 
                        //printf("%d:(%d,%d) ",n , x, y);
                        score += b3DMPointer[z][y][x];
                        }
                //printf(" \n");
                    convertScore = ((score < 1.95) || (score>3.05)) ? 0.0 : 1.0;
                    updates[m] = abs(convertScore - rPointer[m]) > .05? 1: 0;                                
                    rPointer[m] = convertScore;
                    if (convertScore >= threshold){
                        lives+=1;
                        }
                //printf("Position %d at Grid(%d,%d): %f neighbors -> %f \n", k, i, j, score, convertScore);
                    m++;
                    }   
                }   
            }
        }

    else{
        //printf("nonPeriodic\n");
                
        int loci[26], locj[26], lock[26];
        for(i=0; i<gStacks; i++){
   //         printf("i=%d\n", i);
            if (i == 0){
                loci[0] = -1;  
                loci[1] = -1;
                loci[2] = -1;    
                loci[3] = -1;  
                loci[4] = -1;
                loci[5] = -1;  
                loci[6] = -1;  
                loci[7] = -1;
                loci[8] = -1;        
                }
            else{
                loci[0] = i-1;  
                loci[1] = i-1;
                loci[2] = i-1;    
                loci[3] = i-1;  
                loci[4] = i-1;
                loci[5] = i-1;  
                loci[6] = i-1;  
                loci[7] = i-1;
                loci[8] = i-1;                 
                }
            loci[9] = i;  
            loci[10] = i;  
            loci[11] = i;
            loci[12] = i;    
            loci[13] = i;  
            loci[14] = i;
            loci[15] = i;  
            loci[16] = i;  
            if (i == (gStacks-1)){

                loci[17] = -1;         
                loci[18] = -1;
                loci[19] = -1; 
                loci[20] = -1;  
                loci[21] = -1;
                loci[22] = -1;    
                loci[23] = -1;  
                loci[24] = -1;
                loci[25] = -1;  
                }
            else{
                loci[17] = i+1;         
                loci[18] = i+1;
                loci[19] = i+1; 
                loci[20] = i+1;  
                loci[21] = i+1;
                loci[22] = i+1;    
                loci[23] = i+1;  
                loci[24] = i+1;
                loci[25] = i+1;        
                }             
 
            for (j=0; j<gCols; j++){
    //            printf("j=%d\n", j);
                //bgPointer[i][j] = 0.0;
                if (j == 0){
                    locj[0] = -1;  
                    locj[1] = -1;
                    locj[2] = -1;    
                    locj[9] = -1;  
                    locj[10] = -1;
                    locj[11] = -1;  
                    locj[17] = -1;  
                    locj[18] = -1;
                    locj[19] = -1;        
                    }
                else{
                    locj[0] = j-1;  
                    locj[1] = j-1;
                    locj[2] = j-1;    
                    locj[9] = j-1;  
                    locj[10] = j-1;
                    locj[11] = j-1;  
                    locj[17] = j-1;  
                    locj[18] = j-1;
                    locj[19] = j-1;                 
                    }               
                locj[3] = j;  
                locj[4] = j;  
                locj[5] = j;
                locj[12] = j;    
                locj[13] = j;  
                locj[20] = j;
                locj[21] = j;  
                locj[22] = j;  
                if (j == (gCols-1)){
    
                    locj[6] = -1;         
                    locj[7] = -1;
                    locj[8] = -1; 
                    locj[14] = -1;  
                    locj[15] = -1;
                    locj[16] = -1;    
                    locj[23] = -1;  
                    locj[24] = -1;
                    locj[25] = -1;  
                    }
                else{
                    locj[6] = j+1;         
                    locj[7] = j+1;
                    locj[8] = j+1; 
                    locj[14] = j+1;  
                    locj[15] = j+1;
                    locj[16] = j+1;    
                    locj[23] = j+1;  
                    locj[24] = j+1;
                    locj[25] = j+1;        
                    }      
                for (k=0; k<gRows; k++){
     //               printf("k=%d\n", k);
                    //bgPointer[i][j] = 0.0;
                    if (k == 0){
                        lock[0] = -1;  
                        lock[3] = -1;
                        lock[6] = -1;    
                        lock[9] = -1;  
                        lock[12] = -1;
                        lock[14] = -1;  
                        lock[17] = -1;  
                        lock[20] = -1;
                        lock[23] = -1;        
                        }
                    else{
                        lock[0] = k-1;  
                        lock[3] = k-1;
                        lock[6] = k-1;    
                        lock[9] = k-1;  
                        lock[12] = k-1;
                        lock[14] = k-1;  
                        lock[17] = k-1;  
                        lock[20] = k-1;
                        lock[23] = k-1;                 
                        }               
                    lock[1] = k;  
                    lock[4] = k;  
                    lock[7] = k;
                    lock[10] = k;    
                    lock[15] = k;  
                    lock[18] = k;
                    lock[21] = k;  
                    lock[24] = k;  
                    if (k == (gRows-1)){
        
                        lock[2] = -1;         
                        lock[5] = -1;
                        lock[8] = -1; 
                        lock[11] = -1;  
                        lock[13] = -1;
                        lock[16] = -1;    
                        lock[19] = -1;  
                        lock[22] = -1;
                        lock[25] = -1;  
                        }
                    else{
                        lock[2] = k+1;         
                        lock[5] = k+1;
                        lock[8] = k+1; 
                        lock[11] = k+1;  
                        lock[13] = k+1;
                        lock[16] = k+1;    
                        lock[19] = k+1;  
                        lock[22] = k+1;
                        lock[25] = k+1;        
                        }  
//                   printf("k=%d, ckpt 1\n", k);
                    score = 0.0;
                    float locRad;
                    //printf("Position %d at Grid(%d,%d): ", k, i, j);
                    for (n=0; n<26; n++){
                        locRad=0.0;
                        z=loci[n];
                        y=locj[n];
                        x = lock[n];
//                        printf("c1,n=%d: zyx=(%d,%d,%d)\n", n,z,y,x);               
                        //printf("%d:(%d,%d) ",n , x, y);
                        if((x>=0)&&(y>=0)&&(z>=0)){
                            locRad= b3DMPointer[z][y][x];
                            score+=locRad;
                            if (locRad > 0.05){   
  //                              printf("%d,%d,%d:n[%d]=%f->%f\n", i,j,k,n,locRad,score);
                                }
                            }   
 //                       printf("c2,n=%d\n", n);                      
                        }
  //                  printf("k=%d, ckpt 2\n", k);
                    //printf(" \n");
                    convertScore = ((score < 1.95) || (score>3.05)) ? 0.0 : 1.0;
                    updates[m] = abs(convertScore - rPointer[m]) > .05? 1: 0;                                
                    rPointer[m] = convertScore;
                    //printf("Position %d at Grid(%d,%d): %f neighbors -> %f \n", k, i, j, score, convertScore);
                    if (convertScore >= threshold){
                        lives+=1;
                        }
                    m++;   
                    }
                }   
            }        
        }  

    printf("evolveGame3D logic complete\n");

    float pct;
    int intpct;
    pct = 100.0* (float) lives / (float) gSize;
    intpct = (int) (pct*100.0);
    pct = (float) intpct / 100.0;
    dataPointer[0]=dataPointer[0]+1;
    dataPointer[1]=lives;
    dataPointer[2]=pct;
    dataPointer[3]=99.1;
//    dataPointer+=3;
//    *dataPointer = 77.1;
    printf("evolveGame3D logic Complete\n");
    }