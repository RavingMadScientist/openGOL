#include<stdio.h>
#include<string.h>
#define numCharTxts 59

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
        populateBasic(0, numCharTxts);    
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
//    printf("getCharTexture(%d)\n", ch);
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
    int fontLength = numCharTxts;
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
    charMap[14][2][2]=1;
    charMap[14][2][3]=1;
    charMap[14][1][2]=1;
    charMap[14][1][3]=1;
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
    charMap[16][7][0]=1;
    charMap[16][7][1]=1;
    charMap[16][7][2]=1;
    charMap[16][7][3]=1;   
    charMap[16][6][3]=1;   
    charMap[16][5][3]=1;
    charMap[16][4][3]=1;
    charMap[16][3][3]=1;
    charMap[16][2][3]=1;
    charMap[16][1][3]=1;
    charMap[16][1][2]=1;
    charMap[16][1][1]=1;
// 1
    charMap[17][1][1]=1;
    charMap[17][2][1]=1;
    charMap[17][3][1]=1;
    charMap[17][4][1]=1;
    charMap[17][5][1]=1;
    charMap[17][6][1]=1;
    charMap[17][7][1]=1;
    charMap[17][6][0]=1;
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
//:
    charMap[26][1][2]=1;
    charMap[26][1][3]=1;
    charMap[26][2][2]=1;
    charMap[26][2][3]=1;
    charMap[26][5][2]=1;
    charMap[26][5][3]=1;
    charMap[26][6][2]=1;
    charMap[26][6][3]=1;
//;
    charMap[27][2][2]=1;
    charMap[27][2][3]=1;
    charMap[27][1][2]=1;
    charMap[27][1][1]=1;
    charMap[27][1][1]=1;
    charMap[27][5][2]=1;
    charMap[27][5][3]=1;
    charMap[27][6][2]=1;
    charMap[27][6][3]=1;
//=
    charMap[29][4][0]=1;
    charMap[29][4][1]=1;
    charMap[29][4][2]=1;
    charMap[29][4][3]=1;
    charMap[29][1][0]=1;
    charMap[29][1][1]=1;
    charMap[29][1][2]=1;
    charMap[29][1][3]=1;
//A
    charMap[33][7][3]=1;
    charMap[33][6][2]=1;
    charMap[33][6][4]=1;
    charMap[33][5][2]=1;
    charMap[33][5][4]=1;
    charMap[33][3][2]=1;
    charMap[33][3][4]=1;
    charMap[33][4][1]=1;
    charMap[33][4][2]=1;
    charMap[33][4][3]=1;
    charMap[33][4][4]=1;
    charMap[33][4][5]=1;
    charMap[33][2][1]=1;
    charMap[33][1][0]=1;
    charMap[33][2][5]=1;
    charMap[33][1][6]=1;
//B
    charMap[34][7][0]=1;
    charMap[34][6][0]=1;
    charMap[34][5][0]=1;
    charMap[34][4][0]=1;
    charMap[34][3][0]=1;
    charMap[34][2][0]=1;
    charMap[34][1][0]=1;
    charMap[34][7][1]=1;
    charMap[34][7][2]=1;
    charMap[34][6][3]=1;
    charMap[34][5][2]=1;
    charMap[34][4][1]=1;
    charMap[34][4][2]=1;
    charMap[34][3][3]=1;
    charMap[34][2][2]=1;
    charMap[34][1][1]=1;
    charMap[34][1][2]=1;
//    charMap[34][][]=1;
//    charMap[34][][]=1;
//    charMap[34][][]=1;
//    charMap[34][][]=1;
//    charMap[34][][]=1;
//    charMap[34][][]=1;
//C
    charMap[35][4][0]=1;
    charMap[35][5][0]=1;
    charMap[35][3][0]=1;
    charMap[35][6][1]=1;
    charMap[35][7][2]=1;
    charMap[35][7][3]=1;
    charMap[35][6][4]=1;
    charMap[35][2][1]=1;
    charMap[35][1][2]=1;
//    charMap[35][][]=1;

//D
    charMap[36][7][0]=1;
    charMap[36][6][0]=1;
    charMap[36][5][0]=1;
    charMap[36][4][0]=1;
    charMap[36][3][0]=1;
    charMap[36][2][0]=1;
    charMap[36][1][0]=1;
    charMap[36][7][2]=1;
    charMap[36][7][3]=1;
    charMap[36][6][4]=1;
    charMap[36][5][5]=1;
    charMap[36][4][5]=1;
    charMap[36][3][4]=1;
    charMap[36][2][3]=1;
    charMap[36][2][2]=1;
    charMap[36][1][2]=1;
    charMap[36][1][1]=1;

//E
    charMap[37][7][0]=1;
    charMap[37][6][0]=1;
    charMap[37][5][0]=1;
    charMap[37][4][0]=1;
    charMap[37][3][0]=1;
    charMap[37][2][0]=1;
    charMap[37][1][0]=1;
    charMap[37][7][1]=1;
    charMap[37][7][2]=1;
    charMap[37][7][3]=1;
    charMap[37][7][4]=1;
    charMap[37][4][1]=1;
    charMap[37][4][2]=1;
    charMap[37][4][3]=1;
    charMap[37][1][1]=1;
    charMap[37][1][2]=1;
    charMap[37][1][3]=1;
    charMap[37][1][4]=1;
//F
    charMap[38][7][0]=1;
    charMap[38][6][0]=1;
    charMap[38][5][0]=1;
    charMap[38][4][0]=1;
    charMap[38][3][0]=1;
    charMap[38][2][0]=1;
    charMap[38][1][0]=1;
    charMap[38][7][1]=1;
    charMap[38][7][2]=1;
    charMap[38][7][3]=1;
    charMap[38][7][4]=1;
    charMap[38][4][1]=1;
    charMap[38][4][2]=1;
    charMap[38][4][3]=1;
//G
    charMap[39][6][1]=1;
    charMap[39][7][2]=1;
    charMap[39][7][3]=1;
    charMap[39][6][4]=1;
    charMap[39][5][0]=1;
    charMap[39][4][0]=1;
    charMap[39][3][0]=1;
    charMap[39][2][1]=1;
    charMap[39][1][2]=1;
    charMap[39][1][3]=1;
    charMap[39][1][4]=1;
    charMap[39][2][4]=1;
    charMap[39][3][4]=1;
    charMap[39][4][4]=1;
    charMap[39][4][3]=1;

//H
    charMap[40][7][0]=1;
    charMap[40][6][0]=1;
    charMap[40][5][0]=1;
    charMap[40][4][0]=1;
    charMap[40][3][0]=1;
    charMap[40][2][0]=1;
    charMap[40][1][0]=1;
    charMap[40][7][3]=1;
    charMap[40][6][3]=1;
    charMap[40][5][3]=1;
    charMap[40][4][3]=1;
    charMap[40][3][3]=1;
    charMap[40][2][3]=1;
    charMap[40][1][3]=1;
    charMap[40][4][1]=1;
    charMap[40][4][2]=1;

//I
    charMap[41][7][3]=1;
    charMap[41][6][3]=1;
    charMap[41][5][3]=1;
    charMap[41][4][3]=1;
    charMap[41][3][3]=1;
    charMap[41][2][3]=1;
    charMap[41][1][3]=1;
    charMap[41][7][2]=1;
    charMap[41][7][4]=1;
    charMap[41][1][2]=1;
    charMap[41][1][4]=1;
    charMap[41][1][5]=1;
    charMap[41][1][1]=1;
//J
    charMap[42][7][3]=1;
    charMap[42][6][3]=1;
    charMap[42][5][3]=1;
    charMap[42][4][3]=1;
    charMap[42][3][3]=1;
    charMap[42][2][2]=1;
    charMap[42][1][1]=1;
    charMap[42][0][2]=1;
    charMap[42][7][4]=1;
    charMap[42][7][2]=1;

//K
    charMap[43][7][0]=1;
    charMap[43][6][0]=1;
    charMap[43][5][0]=1;
    charMap[43][4][0]=1;
    charMap[43][3][0]=1;
    charMap[43][2][0]=1;
    charMap[43][1][0]=1;
    charMap[43][4][1]=1;
    charMap[43][5][2]=1;
    charMap[43][6][2]=1;
    charMap[43][7][3]=1;
    charMap[43][4][2]=1;
    charMap[43][3][3]=1;
    charMap[43][2][3]=1;
    charMap[43][1][4]=1;

//L
    charMap[44][7][0]=1;
    charMap[44][6][0]=1;
    charMap[44][5][0]=1;
    charMap[44][4][0]=1;
    charMap[44][3][0]=1;
    charMap[44][2][0]=1;
    charMap[44][1][0]=1;
    charMap[44][1][1]=1;
    charMap[44][1][2]=1;
    charMap[44][1][3]=1;

//M
    charMap[45][7][0]=1;
    charMap[45][6][0]=1;
    charMap[45][5][0]=1;
    charMap[45][4][0]=1;
    charMap[45][3][0]=1;
    charMap[45][2][0]=1;
    charMap[45][1][0]=1;
    charMap[45][7][4]=1;
    charMap[45][6][4]=1;
    charMap[45][5][4]=1;
    charMap[45][4][4]=1;
    charMap[45][3][4]=1;
    charMap[45][2][4]=1;
    charMap[45][1][4]=1;

    charMap[45][6][1]=1;
    charMap[45][5][1]=1;
    charMap[45][4][1]=1;
    charMap[45][3][2]=1;
    charMap[45][2][2]=1;
    charMap[45][1][2]=1;
//    charMap[45][2][3]=1;
//    charMap[45][3][3]=1;
    charMap[45][4][3]=1;
    charMap[45][5][3]=1;
    charMap[45][6][4]=1;

//N
    charMap[46][7][0]=1;
    charMap[46][6][0]=1;
    charMap[46][5][0]=1;
    charMap[46][4][0]=1;
    charMap[46][3][0]=1;
    charMap[46][2][0]=1;
    charMap[46][1][0]=1;

    charMap[46][6][1]=1;
    charMap[46][5][1]=1;
    charMap[46][4][2]=1;
    charMap[46][3][2]=1;
    charMap[46][1][3]=1;
    charMap[46][2][3]=1;
    charMap[46][3][3]=1;
    charMap[46][4][3]=1;
    charMap[46][5][3]=1;
    charMap[46][6][3]=1;
    charMap[46][7][3]=1;
//O
    charMap[47][7][2]=1;
    charMap[47][6][1]=1;
    charMap[47][6][3]=1;
    charMap[47][5][0]=1;
    charMap[47][4][0]=1;
    charMap[47][3][0]=1;
    charMap[47][2][1]=1;
    charMap[47][1][2]=1;
    charMap[47][2][3]=1;
    charMap[47][3][4]=1;
    charMap[47][4][4]=1;
    charMap[47][5][4]=1;
//P
    charMap[48][7][0]=1;
    charMap[48][6][0]=1;
    charMap[48][5][0]=1;
    charMap[48][4][0]=1;
    charMap[48][3][0]=1;
    charMap[48][2][0]=1;
    charMap[48][1][0]=1;
    charMap[48][7][1]=1;
    charMap[48][7][2]=1;
    charMap[48][6][3]=1;
    charMap[48][5][2]=1;
    charMap[48][4][1]=1;
//Q
    charMap[49][7][2]=1;
    charMap[49][6][1]=1;
    charMap[49][6][3]=1;
    charMap[49][5][0]=1;
    charMap[49][4][0]=1;
    charMap[49][3][0]=1;
    charMap[49][2][1]=1;
    charMap[49][1][2]=1;
    charMap[49][2][3]=1;
    charMap[49][3][4]=1;
    charMap[49][4][4]=1;
    charMap[49][5][4]=1;
    charMap[49][0][5]=1;
    charMap[49][1][4]=1;
    charMap[49][1][3]=1;
    charMap[49][2][2]=1;
//R
    charMap[50][7][0]=1;
    charMap[50][6][0]=1;
    charMap[50][5][0]=1;
    charMap[50][4][0]=1;
    charMap[50][3][0]=1;
    charMap[50][2][0]=1;
    charMap[50][1][0]=1;
    charMap[50][7][1]=1;
    charMap[50][7][2]=1;
    charMap[50][6][3]=1;
    charMap[50][5][2]=1;
    charMap[50][4][1]=1;
    charMap[50][3][2]=1;
    charMap[50][2][3]=1;
    charMap[50][1][4]=1;
//S
    charMap[51][7][2]=1;
    charMap[51][7][3]=1;
    charMap[51][6][4]=1;
    charMap[51][7][1]=1;
    charMap[51][6][0]=1;
    charMap[51][5][0]=1;
    charMap[51][4][1]=1;
    charMap[51][3][2]=1;
    charMap[51][3][3]=1;
    charMap[51][2][4]=1;
    charMap[51][1][3]=1;
    charMap[51][0][2]=1;
    charMap[51][0][1]=1;
    charMap[51][1][0]=1;
//T
    charMap[52][7][3]=1;
    charMap[52][6][3]=1;
    charMap[52][5][3]=1;
    charMap[52][4][3]=1;
    charMap[52][3][3]=1;
    charMap[52][2][3]=1;
    charMap[52][1][3]=1;
    charMap[52][7][2]=1;
    charMap[52][7][4]=1;
    charMap[52][7][1]=1;
    charMap[52][7][5]=1;


//U
    charMap[53][7][0]=1;
    charMap[53][6][0]=1;
    charMap[53][5][0]=1;
    charMap[53][4][0]=1;
    charMap[53][3][0]=1;
    charMap[53][2][1]=1;
    charMap[53][1][2]=1;
    charMap[53][1][3]=1;
    charMap[53][2][4]=1;
    charMap[53][3][5]=1;
    charMap[53][4][5]=1;
    charMap[53][5][5]=1;
    charMap[53][6][5]=1;
    charMap[53][7][5]=1;
//V
    charMap[54][6][0]=1;
    charMap[54][5][1]=1;
    charMap[54][4][1]=1;
    charMap[54][3][1]=1;
    charMap[54][2][2]=1;
    charMap[54][1][2]=1;
    charMap[54][3][2]=1;
    charMap[54][4][3]=1;
    charMap[54][5][3]=1;
    charMap[54][6][4]=1;

//W
    charMap[55][6][0]=1;
    charMap[55][5][1]=1;
    charMap[55][4][1]=1;
    charMap[55][3][1]=1;
    charMap[55][2][2]=1;
    charMap[55][1][2]=1;
    charMap[55][2][3]=1;
    charMap[55][3][3]=1;
    charMap[55][2][4]=1;
    charMap[55][2][5]=1;
    charMap[55][3][5]=1;
    charMap[55][4][5]=1;
    charMap[55][5][5]=1;
    charMap[55][6][6]=1;
//X
    charMap[56][7][0]=1;
    charMap[56][6][1]=1;
    charMap[56][5][1]=1;
    charMap[56][4][2]=1;
    charMap[56][3][2]=1;
    charMap[56][2][3]=1;
    charMap[56][1][4]=1;
    charMap[56][1][0]=1;
    charMap[56][2][1]=1;
    charMap[56][3][1]=1;
    charMap[56][5][2]=1;
    charMap[56][6][3]=1;
    charMap[56][7][4]=1;
//Y
    charMap[57][1][3]=1;
    charMap[57][2][3]=1;
    charMap[57][3][3]=1;
    charMap[57][4][4]=1;
    charMap[57][4][2]=1;
    charMap[57][5][4]=1;
    charMap[57][5][2]=1;
    charMap[57][6][5]=1;
    charMap[57][6][1]=1;
    charMap[57][7][6]=1;
    charMap[57][7][0]=1;

//Z
    charMap[58][7][1]=1;
    charMap[58][7][2]=1;
    charMap[58][7][3]=1;
    charMap[58][7][4]=1;
    charMap[58][6][3]=1;
    charMap[58][5][3]=1;
    charMap[58][4][2]=1;
    charMap[58][3][2]=1;
    charMap[58][2][1]=1;
    charMap[58][1][1]=1;
    charMap[58][1][2]=1;
    charMap[58][1][3]=1;
    charMap[58][1][4]=1;

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
                //printf("posMap[%d][%d][%d] = %d\n",i,j,k, charMap[index][j][k]);    
                posMap[i][j][k] = charMap[index][j][k];
                }
            }        
        }   
}