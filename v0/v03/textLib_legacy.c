#include<stdio.h>
#define numCharTxts 4

unsigned char gText[numCharTxts][8][8][4];
int posMap[numCharTxts][8][8];

char colorOn[4] = {0x00, 0xFF, 0x00, 0xFF};
char colorOff[4] = {0x00, 0x00, 0x00, 0x00};


int chn, i, j;

void setupTextLib(){
    for(chn=0; chn<numCharTxts; chn++){
        for (i=0; i<8; i++){
            for(j=0; j<8; j++){
                posMap[chn][i][j] = 0;
                }
            } 
        }
    
//'g'
    posMap[0][0][0]=1;
    posMap[0][0][1]=1;
    posMap[0][0][2]=1;
    posMap[0][1][2]=1;
    posMap[0][2][2]=1;
    posMap[0][3][2]=1;
    posMap[0][4][2]=1;
    posMap[0][5][2]=1;
    posMap[0][5][1]=1;
    posMap[0][5][0]=1;
    posMap[0][4][0]=1;
    posMap[0][3][0]=1;
    posMap[0][3][1]=1;
//h
    posMap[1][1][0]=1;
    posMap[1][2][0]=1;
    posMap[1][3][0]=1;
    posMap[1][4][0]=1;
    posMap[1][4][1]=1;
    posMap[1][4][2]=1;
    posMap[1][3][2]=1;
    posMap[1][2][2]=1;
    posMap[1][1][2]=1;
    posMap[1][5][0]=1;
    posMap[1][6][0]=1;
//e
    posMap[2][1][0]=1;
    posMap[2][1][1]=1;
    posMap[2][1][2]=1;
    posMap[2][2][0]=1;
    posMap[2][3][0]=1;
    posMap[2][3][1]=1;
    posMap[2][3][2]=1;
    posMap[2][4][2]=1;
    posMap[2][5][2]=1;
    posMap[2][5][1]=1;
    posMap[2][5][0]=1;
    posMap[2][4][0]=1;
//n
    posMap[3][1][0]=1;
    posMap[3][2][0]=1;
    posMap[3][3][0]=1;
    posMap[3][4][0]=1;    
    posMap[3][4][1]=1;
    posMap[3][4][2]=1;
    posMap[3][4][3]=1;
    posMap[3][3][3]=1;
    posMap[3][2][3]=1;
    posMap[3][1][3]=1;


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