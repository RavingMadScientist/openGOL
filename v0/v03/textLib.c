#include<stdio.h>
#include<string.h>
#define numCharTxts 24

unsigned char gText[numCharTxts][8][8][4];
int posMap[numCharTxts][8][8];

char colorOn[4] = {0x00, 0xFF, 0x00, 0xFF};
char colorOff[4] = {0x00, 0x00, 0x00, 0x00};


int chn, i, j;

void populateBasic(int firstChar, int numChars);
void setupTextLib(char* font){
    for(chn=0; chn<numCharTxts; chn++){
        for (i=0; i<8; i++){
            for(j=0; j<8; j++){
                posMap[chn][i][j] = 0;
                }
            } 
        }

//    printf("font %c not yet supported\n", font);  

    if (strcmp(font, "basic")==0){
        printf("basic font selected\n");
        populateBasic(6, numCharTxts);    
        }
    else{
        printf("font %s not yet supported\n", font);    
        }


    for (chn = 0; chn < numCharTxts; chn++){    
        for (i=0; i<8; i++){
            for(j=0; j<8; j++){
                if(posMap[chn][i][j] == 1){
                    gText[chn][i][j][0]=colorOn[0];
                    gText[chn][i][j][1]=colorOn[1];
                    gText[chn][i][j][2]=colorOn[2];
                    gText[chn][i][j][3]=colorOn[3];
                    }
                else{
                    gText[chn][i][j][0]=colorOff[0];
                    gText[chn][i][j][1]=colorOff[0];
                    gText[chn][i][j][2]=colorOff[0];
                    gText[chn][i][j][3]=colorOff[0];
                    }
                }
            } 
        }
    }

void getCharTexture(int ch, unsigned char localTex[8][8][4] ){
    printf("getCharTexture(%d)\n", ch);
    int i, j, k;
    for (i=0; i<8; i++){
        for (j=0; j<8; j++){
            for (k=0; k<4; k++){
                localTex[i][j][k] = gText[ch][i][j][k];
                //printf("gText[%d,%d,%d]=%d | ",i,j,k,gText[ch][i][j][k]);
                }
            }   
        }
    //return gText;
    }
//This will be the format of any font we may decide to use...
void populateBasic(int firstChar, int numChars){
    int fontLength = 30;
    if (firstChar+numChars > fontLength){
        numChars = fontLength;
        }
    int i, j, k, chn, index;
    int charMap[fontLength][8][8];

    for(chn=0; chn<fontLength; chn++){
        for (i=0; i<8; i++){
            for(j=0; j<8; j++){
                charMap[chn][i][j] = 0;
                }
            } 
        }
//!
    charMap[1][0][0]=1;
    charMap[1][2][0]=1;
    charMap[1][3][0]=1;
    charMap[1][4][0]=1;
    charMap[1][5][0]=1;    
    charMap[1][6][0]=1; 
//"
    charMap[2][7][0]=1;
    charMap[2][6][0]=1;
    charMap[2][5][0]=1;
    charMap[2][7][2]=1;
    charMap[2][6][2]=1;
    charMap[2][5][2]=1;
//#
    charMap[3][1][1]=1;
    charMap[3][2][1]=1;
    charMap[3][3][1]=1;
    charMap[3][4][1]=1;
    charMap[3][5][1]=1;
    charMap[3][1][3]=1;
    charMap[3][2][3]=1;
    charMap[3][3][3]=1;
    charMap[3][4][3]=1;
    charMap[3][5][3]=1;
    charMap[3][4][0]=1;
    charMap[3][4][1]=1;
    charMap[3][4][2]=1;
    charMap[3][4][3]=1;
    charMap[3][4][4]=1;
    charMap[3][2][0]=1;
    charMap[3][2][1]=1;
    charMap[3][2][2]=1;
    charMap[3][2][3]=1;
    charMap[3][2][4]=1;
//%
    charMap[5][7][0]=1;
    charMap[5][7][1]=1;
    charMap[5][7][2]=1;
    charMap[5][6][2]=1;
    charMap[5][5][2]=1;
    charMap[5][5][1]=1;
    charMap[5][5][0]=1;
    charMap[5][6][0]=1;
    charMap[5][0][0]=1;
    charMap[5][1][1]=1;
    charMap[5][2][2]=1;
    charMap[5][3][2]=1;
    charMap[5][4][3]=1;
    charMap[5][5][4]=1;    
    charMap[5][6][4]=1;
    charMap[5][7][5]=1;
    charMap[5][2][4]=1;
    charMap[5][2][5]=1;
    charMap[5][2][6]=1;
    charMap[5][1][6]=1;
    charMap[5][0][6]=1;
    charMap[5][0][5]=1;
    charMap[5][0][4]=1;
    charMap[5][1][4]=1;
// &
    charMap[6][4][1]=1;
    charMap[6][3][1]=1;
    charMap[6][2][1]=1;
    charMap[6][1][1]=1;
    charMap[6][2][2]=1;
    charMap[6][3][3]=1;
    charMap[6][3][2]=1;
    charMap[6][3][0]=1;
// '
    charMap[7][7][0]=1;
    charMap[7][6][0]=1;
    charMap[7][5][0]=1;
//(

//)

//*
    charMap[10][7][3]=1;
    charMap[10][6][3]=1;
    charMap[10][5][3]=1;
    charMap[10][5][2]=1;
    charMap[10][6][1]=1;
    charMap[10][5][4]=1;
    charMap[10][6][5]=1;
    charMap[10][4][2]=1;
    charMap[10][3][1]=1;
    charMap[10][4][4]=1;
    charMap[10][3][5]=1;
//+
    charMap[11][3][3]=1;
    charMap[11][4][3]=1;
    charMap[11][5][3]=1;
    charMap[11][1][3]=1;
    charMap[11][2][3]=1;
    charMap[11][3][4]=1;
    charMap[11][3][5]=1;
    charMap[11][3][2]=1;
    charMap[11][3][1]=1;
//-
    charMap[13][1][3]=1;
    charMap[13][2][3]=1;
    charMap[13][3][3]=1;
    charMap[13][4][3]=1;
    charMap[13][5][3]=1;

//.
    charMap[14][0][0]=1;
    charMap[14][0][1]=1;
    charMap[14][1][0]=1;
    charMap[14][1][1]=1;
// /
    charMap[15][0][0]=1;
    charMap[15][1][1]=1;
    charMap[15][2][2]=1;
    charMap[15][3][3]=1;
    charMap[15][4][4]=1;
    charMap[15][5][5]=1;
// 0
    charMap[16][1][0]=1;
    charMap[16][2][0]=1;
    charMap[16][3][0]=1;
    charMap[16][4][0]=1;
    charMap[16][5][0]=1;
    charMap[16][6][0]=1;
    charMap[16][6][1]=1;
    charMap[16][6][2]=1;
    charMap[16][6][3]=1;    
    charMap[16][5][3]=1;
    charMap[16][4][3]=1;
    charMap[16][3][3]=1;
    charMap[16][2][3]=1;
    charMap[16][1][3]=1;
    charMap[16][1][2]=1;
    charMap[16][1][1]=1;
// 1
    charMap[17][1][0]=1;
    charMap[17][2][0]=1;
    charMap[17][3][0]=1;
    charMap[17][4][0]=1;
    charMap[17][5][0]=1;
//2
    charMap[18][6][0]=1;
    charMap[18][7][1]=1;
    charMap[18][7][2]=1;
    charMap[18][6][3]=1;
    charMap[18][5][3]=1;
    charMap[18][4][2]=1;
    charMap[18][3][2]=1;
    charMap[18][2][1]=1;
    charMap[18][1][0]=1;
    charMap[18][1][1]=1;
    charMap[18][1][2]=1;
    charMap[18][1][3]=1;
    charMap[18][1][4]=1;

//3
    charMap[19][6][0]=1;
    charMap[19][7][1]=1;
    charMap[19][7][2]=1;
    charMap[19][6][3]=1;
    charMap[19][5][3]=1;
    charMap[19][4][2]=1;
    charMap[19][4][1]=1;

    charMap[19][3][3]=1;
    charMap[19][2][3]=1;
    charMap[19][1][2]=1;
    charMap[19][1][1]=1;
    charMap[19][2][0]=1;

//4
    charMap[20][4][3]=1;
    charMap[20][5][3]=1;
    charMap[20][6][3]=1;
    charMap[20][3][3]=1;
    charMap[20][2][3]=1;
    charMap[20][1][3]=1;
    charMap[20][5][2]=1;
    charMap[20][4][1]=1;
    charMap[20][3][0]=1;
    charMap[20][3][1]=1;
    charMap[20][3][2]=1;
    charMap[20][3][4]=1;

    charMap[20][2][3]=1;
    charMap[20][1][3]=1;
//5
    charMap[21][6][3]=1;
    charMap[21][6][2]=1;
    charMap[21][6][1]=1;
    charMap[21][5][1]=1;
    charMap[21][4][1]=1;
    charMap[21][4][2]=1;
    charMap[21][3][3]=1;
    charMap[21][2][4]=1;
    charMap[21][1][3]=1;
    charMap[21][0][2]=1;
    charMap[21][1][1]=1;

//6
    charMap[22][6][4]=1;
    charMap[22][6][3]=1;
    charMap[22][5][2]=1;
    charMap[22][4][1]=1;
    charMap[22][3][1]=1;
    charMap[22][2][0]=1;
    charMap[22][1][1]=1;
    charMap[22][0][2]=1;
    charMap[22][1][3]=1;
    charMap[22][2][4]=1;
    charMap[22][3][3]=1;
    charMap[22][4][2]=1;
//7
    charMap[23][6][0]=1;
    charMap[23][6][1]=1;
    charMap[23][6][2]=1;
    charMap[23][6][3]=1;
    charMap[23][5][2]=1;
    charMap[23][4][2]=1;
    charMap[23][3][1]=1;
    charMap[23][2][1]=1;
    charMap[23][1][0]=1;
//8
    charMap[24][7][3]=1;
    charMap[24][6][2]=1;
    charMap[24][6][4]=1;
    charMap[24][5][2]=1;    
    charMap[24][5][4]=1;
    charMap[24][4][3]=1;
    charMap[24][3][2]=1;
    charMap[24][3][4]=1;
    charMap[24][2][1]=1;
    charMap[24][2][5]=1;
    charMap[24][1][2]=1;
    charMap[24][1][4]=1;
    charMap[24][0][3]=1;
//9
    charMap[25][7][3]=1;
    charMap[25][6][2]=1;
    charMap[25][6][4]=1;
    charMap[25][5][2]=1;    
    charMap[25][5][4]=1;
    charMap[25][4][3]=1;
    charMap[25][4][4]=1;
    charMap[25][3][4]=1;
    charMap[25][2][3]=1;
    charMap[25][1][2]=1;
    charMap[25][1][1]=1;

////'g'
//    posMap[0][0][0]=1;
//    posMap[0][0][1]=1;
//    posMap[0][0][2]=1;
//    posMap[0][1][2]=1;
//    posMap[0][2][2]=1;
//    posMap[0][3][2]=1;
//    posMap[0][4][2]=1;
//    posMap[0][5][2]=1;
//    posMap[0][5][1]=1;
//    posMap[0][5][0]=1;
//    posMap[0][4][0]=1;
//    posMap[0][3][0]=1;
//    posMap[0][3][1]=1;
////h
//    posMap[1][1][0]=1;
//    posMap[1][2][0]=1;
//    posMap[1][3][0]=1;
//    posMap[1][4][0]=1;
//    posMap[1][4][1]=1;
//    posMap[1][4][2]=1;
//    posMap[1][3][2]=1;
//    posMap[1][2][2]=1;
//    posMap[1][1][2]=1;
//    posMap[1][5][0]=1;
//    posMap[1][6][0]=1;
////e
//    posMap[2][1][0]=1;
//    posMap[2][1][1]=1;
//    posMap[2][1][2]=1;
//    posMap[2][2][0]=1;
//    posMap[2][3][0]=1;
//    posMap[2][3][1]=1;
//    posMap[2][3][2]=1;
//    posMap[2][4][2]=1;
//    posMap[2][5][2]=1;
//    posMap[2][5][1]=1;
//    posMap[2][5][0]=1;
//    posMap[2][4][0]=1;
////n
//    posMap[3][1][0]=1;
//    posMap[3][2][0]=1;
//    posMap[3][3][0]=1;
//    posMap[3][4][0]=1;    
//    posMap[3][4][1]=1;
//    posMap[3][4][2]=1;
//    posMap[3][4][3]=1;
//    posMap[3][3][3]=1;
//    posMap[3][2][3]=1;
//    posMap[3][1][3]=1;



//assignment of font charMap data into global posMap...
    for (i=0; i<numChars; i++){
        for (j=0; j<8; j++){
            for (k=0; k<8; k++){
                index = (i+firstChar)%fontLength;
                printf("posMap[%d][%d][%d] = %d\n",i,j,k, charMap[index][j][k]);    
                posMap[i][j][k] = charMap[index][j][k];
                }
            }        
        }   
}