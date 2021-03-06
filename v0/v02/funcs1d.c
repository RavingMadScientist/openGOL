#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//local routine used by fillGrid to parse input file
void readFChars1D(char* fileName, float* gp, int gSize){
    //printf("rPointer = %d\n", gp);
    printf("opening specified file: %s\n", fileName);
    FILE * fp;
    fp = fopen(fileName, "r");
////    fp = fopen("inFile.txt", "r");
    if (fp == NULL) {
        printf ("error opening inFile\n");
        fclose(fp);
        }
    else {        
        int pos = 0;
        int thisChar= 0;
        char charOut='0';
        float thisVal;
        int dead = 0;
        while ((charOut != EOF) && (pos<gSize)){
            int delim = 0;
            thisVal = 0.0;
            char locString[50];
            char* lsp;
            lsp = locString;
            //printf("pos=%d, locString=%s\n", pos, locString);
            int locCount = 0;
            while (delim == 0){
                thisChar = getc(fp);            
                charOut = thisChar;
                if((locCount == 0)&&(charOut == EOF)){
                    dead = 1;
                    break;
                    }
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
                    thisVal = atof(locString);
                    //printf("val %d: %d\n",pos,thisVal );
                    delim=1;
                    }
                locCount ++;
                }
            //printf("val %d: %f(%c)\n",pos,thisVal, charOut );

            if ((charOut == EOF) || (dead == 1)) {
                break;
                }
            else{
                //printf("commit: %d->%f\n", pos, thisVal);
                *gp = (thisVal);
                gp++;            
                pos++;
                }
            locString[0] = '0';
            }
        fclose(fp);
        printf("clean break\n");
        //printf("rPointer = %d\n", gp);
        }
    printf("clean break from readFChars\n");
    }



void fillGrid1D(char* fileName, float* ggPointer, int gSize){
    printf("fillGrid initiated, size=%d\n", gSize);
    //printf("rPointer = %d\n", ggPointer);
    readFChars1D(fileName, ggPointer, gSize);

    
    //now we are going to print out the way the grid looks
    //printf("grid loaded by fillGrid:\n");
    int i;
    //int* locPointer;
    //locPointer = ggPointer;
//    float gval;
//    for (i=0; i<gSize; i++){
//        gval = *ggPointer;
//        printf("pos: %d\t val: %f\n",i,gval);
//        ggPointer++;
//        }
    printf("fillGrid complete\n");
    }
    //printf("fillGrid complete;");
//    return ggPointer;

    


void runGrid1D(float* ggPointer, int gSize) {

    printf("runGrid initiated\n");

    printf("grid loaded by runGrid:\n");
    int i;
    //int* locPointer;
    //locPointer = ggPointer;
    float gval;
    //printf("pointer at:%d\n",ggPointer);
    for (i=0; i<gSize; i++){
        gval = *ggPointer;
        printf("pos: %d\t val: %f\n",i,gval);
        ggPointer++;
        //printf("pointer at:%d\n",ggPointer);
        }

    printf("runGrid complete\n");
    }