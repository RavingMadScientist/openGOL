/*
In this directory, we will store usable versions of openGOL

compile w gcc v01.c funcs2d.c -lm -lGLEW -lglut -lGLU -lGL -o v01

And here we have a fully functioning incarnation,
"-p" flag sets periodic boundary conditions,
just hit the f key to evolve.

*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include "funcs1d.h"
#include "funcs2d.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GL/glu.h>
#include<GL/gl.h>

#define WIN_PREFIX "openGOL v0.1"

int currentWidth = 800;
int currentHeight = 800;
int winHandle = 0;
unsigned FrameCount = 0;
int c,g,i,j,k = 0; //local counter vars
float xVal, yVal, zVal, theta, R; //local circle vars
float rVal, gVal, bVal; //local color vars

// user parameter input defaults
int gWidth = 11;
int gHeight = 11;
char* loadFile;
int periodicMode = 0;
char* oType = "pacman";
char* oList[2]   = {"pacman", "square"};
//
static const float PI = 3.14159265;
static int numDims = 4; //homogeneous vector coords
int numOrgs = 2; //gets autoreplaced by input file parser
int numoFloats = 121; // also gets updated quickly
int numVertices = 21; //GL_TRIANGLE_FAN circle approx



GLfloat* cPointer;
GLfloat* cColPointer;

int* orgLoc;
GLfloat* oCenters;
GLfloat* rPointer;

GLfloat** bgPointer;
int* updates;



GLuint VertexShaderId, FragmentShaderId, ProgramId, VaoId[2], VboId, ColorBufferId;
GLuint VgoId, VgoCId; //grid buffer pointers

GLuint offsetLocation; //pointer to navigation vec4
GLuint scaleLocation;
GLfloat scaleX, scaleY, scaleZ;
GLfloat camOffset[4]={0.0, 0.0, 0.0, 0.0 };

// assert functions defined after main
void RainbowMe(int, int);
void Cleanup(void);
void DestroyShaders(void);
void DestroyVBO(void);
// define shaders
const GLchar* VertexShader = 
{
    "#version 330\n"\
    "layout(location=0) in vec4 in_Position;\n"\
    "layout(location=1) in vec4 in_Color;\n"\
    "uniform vec4 offset;\n"\
    "uniform vec4 scale;\n"\

    "out vec4 ex_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   gl_Position = (in_Position + offset) * scale;\n"\
    "   ex_Color = in_Color;\n"\
    "}\n"
};

const GLchar* FragmentShader = 
{
    "#version 330\n"\
    "in vec4 ex_Color;\n"\
    "out vec4 out_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   out_Color = ex_Color;\n"\
    "}\n"
};

void CreateShaders(void) {
    printf("CreateShaders called\n");
    GLenum ErrorCheckValue = glGetError();

    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
    glUseProgram(ProgramId);    

    //specify a Uniform vec4 for moving our VertexShader offset around
    offsetLocation = glGetUniformLocation(ProgramId, "offset");
    scaleLocation = glGetUniformLocation(ProgramId, "scale");
    //should be global (is)

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }


CreateVBO(GLfloat* cPointer, GLfloat* cColPointer, GLfloat* oCenters, GLfloat* rlPointer) {
//    float gval;   
//    printf("%d\n", rPointer); 
//    for (i=0; i<numOrgs; i++){
//        gval = rlPointer[i];
//        printf ("VBO rad[%d] = %f\n", i, gval);
//        //rlPointer++;
//        }


//Part I. Vertex Construction Logic

//"Organism" Drawer (generalized, essentially reads from a C imitation of a dict the specific draw routine)
    int loc = 0;
    for (c=0; c<numOrgs; c++){
        loc = initOrg(oType, c, loc);        
        }

// For WHATEVER reason, seems you have to copy your vertex arrays to local storage
// in order to bind to VBO
    GLfloat Verts[numoFloats];
    GLfloat Cols[numoFloats]; 
    for(i=0; i<numoFloats; i++){
        Verts[i] = cPointer[i];
        Cols[i]=cColPointer[i];
        }

// gridLines (locally defined array since static) 
    int numGCoords = gHeight * gWidth * numDims * 2;
    GLfloat gVertices[numGCoords];
    GLfloat gColors[numGCoords];
    float gwMax = ((float)gWidth-1.0)/2.0; 
    float ghMax = ((float)gHeight-1.0)/2.0;
    zVal = 0.0;

//vertical gridline vertices
    for(i=0; i<gWidth; i++){
        xVal = (gwMax - (float)i); 
        for(j=0; j<gHeight; j++){
            yVal = (ghMax-(float)j);
            gVertices[4*(gHeight*i+j)] = xVal;
            gVertices[4*(gHeight*i+j)+1] = yVal;
            gVertices[4*(gHeight*i+j)+2] = zVal;
            gVertices[4*(gHeight*i+j)+3] = 1.0;
            }
        }
//horizontal gridline vertices
    int baseIndex = 4* gWidth * gHeight;
    for(i=0; i<gHeight; i++){
        yVal = (ghMax - (float)i); 
        for(j=0; j<gWidth; j++){
            xVal =  (gwMax-(float)j);
            gVertices[baseIndex+4*(gHeight*i+j)] = xVal;
            gVertices[baseIndex+4*(gHeight*i+j)+1] = yVal;
            gVertices[baseIndex+4*(gHeight*i+j)+2] = zVal;
            gVertices[baseIndex+4*(gHeight*i+j)+3] = 1.0;
            }
        }
// no variation in gridline color (white)
    for(i=0; i<numGCoords;i++){
        gColors[i] = 1.0;
        } 

//Part II. VBO construction and assignment logic.
// Instantiating our two VBOs...
    GLenum ErrorCheckValue = glGetError();
    glGenVertexArrays(2, VaoId);

//circles
    glBindVertexArray(VaoId[0]);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Cols), Cols, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
//gridlines
    glBindVertexArray(VaoId[1]);

    glGenBuffers(1, &VgoId);
    glBindBuffer(GL_ARRAY_BUFFER, VgoId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertices), gVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VgoCId);
    glBindBuffer(GL_ARRAY_BUFFER, VgoCId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gColors), gColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(VaoId[0]);


    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }   
    }

//call the proper routine to create an organism of desired type
int initOrg(char* orgType, int oIndex, int memIndex){
    printf("initOrg(%s),%d\n", orgType, oIndex);
    int memIn = memIndex;    
    int memOut;
    if (strcmp(orgType, "pacman") == 0){
        memOut = initPacman(oIndex, memIndex);
        }
    if (strcmp(orgType, "square") == 0){
        memOut = initSquare(oIndex, memIndex);
        }

//    float postVal;
//    printf("initOrg recd from initPacman cPointer[%d:%d]=",memIn, memOut);
//    for (i=memIn; i<memOut; i++){
//        postVal = cPointer[i];
//        printf("%f,",postVal);
//        }        
//    printf("\n");
    return memOut;
    }
int initPacman(int orgIndex, int memIndex){
    printf("initPacman(%d)\n", orgIndex);
    int mi = memIndex;
    for(i=0; i<numVertices; i++){
        mi=memIndex+4*i;
        RainbowMe(mi, 0);
        }
    int o = orgIndex;
    mi = memIndex;
    i=orgIndex;
    printf("initPacman-oCenter[%d]=(%f,%f,%f)\n",i, oCenters[3*i], oCenters[3*i+1], oCenters[3*1+2]);
    i=mi;
    printf("cPointer[%d]=%f\n",i, cPointer[i]);

    xVal = oCenters[3*o];
    cPointer[mi] = xVal;
    mi++;
    yVal = oCenters[3*o+1];
    cPointer[mi] = yVal;
    mi++;
    zVal = oCenters[3*o+2];
    cPointer[mi] = zVal;
    mi++;
    cPointer[mi] = 1.0; //homogeneous coords
    mi ++;
    
    R = rPointer[o] * 0.5;
//    //and now we input the peripheral coordinates
//    printf("numVertices=%d\n",numVertices);
    for (i=1; i<numVertices; i++){
        theta = (float)(i-1.0)* 2.0*PI/(float)(numVertices-1.0);

        xVal = oCenters[3*o]+R*cos(theta);
        cPointer[mi] = xVal;
        mi++;

        yVal = oCenters[3*o+1]+R*sin(theta);                
        cPointer[mi] = yVal;
        mi++;
        cPointer[mi] = zVal;
        mi++;
        cPointer[mi] = 1.0;
        mi++;
        }
        //}
//    if (rPointer[o] > 0.1){
//        printf("valid Circle(#%d) specd at (%f, %f, %f): R=%f\n", c, oCenters[3*c], oCenters[3*c+1], oCenters[3*c+2], rPointer[c]);
//        }
    printf("initPacman(%d) returns\n", orgIndex);
    return mi;
    }

//color scheme
void RainbowMe(int memOut, int seed){ 
    //printf("cirColor %d:%f | ",memOut,cColPointer[memOut]);
    int i, j, q;
//    cColPointer[memOut] = 1.0;
    cColPointer[memOut]=1.0;
    for(i=0; i<3; i++){
        q=i+seed+memOut;
        j=i+memOut;
        if((q%16)%5 == 0){
            cColPointer[j] = 1.0;
            }
        else if(q%4 == 3){
            cColPointer[j] = 1.0;
            }
        else if ( ((q/4)%4) == 3){
            cColPointer[j] = 1.0;
            }
        else{
            cColPointer[j] = 0.0;        
            }
        }
    }
int initSquare(int orgIndex, int memIndex){
    printf("initSquare(%d)\n", orgIndex);
    int i, j, o;
    for (i=0; i<numVertices; i++){
        j=memIndex+4*i;
        cColPointer[j]=0.0;
        cColPointer[j+1]=1.0;
        cColPointer[j+2]=0.0;
        cColPointer[j+3]=1.0;
        }
    float xv, yv, zv;
    o=orgIndex;
    R = rPointer[o] * 0.5;
    xv = oCenters[3*o];
    yv=oCenters[3*o+1];
    zv=oCenters[3*o+2];
    int mi = memIndex;
    float lv;
    float sq[4][4] = {{xv-R, yv-R, zv, 1.0},{xv-R, yv+R, zv, 1.0},{xv+R, yv+R, zv, 1.0},{xv+R, yv-R, zv, 1.0}};
//    sq[0]={xv-R, yv-R, zv, 1.0};
//    sq[1]={xv-R, yv+R, zv, 1.0};
//    sq[2]={xv+R, yv-R, zv, 1.0};
//    sq[3]={xv+R, yv+R, zv, 1.0};
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
            cPointer[mi]=lv;
            mi++;
            }
        }

    printf("initSquare(%d) returns\n", orgIndex);
    return mi;
    }


void drawCircle(int c){
    
    int baseIndex = numVertices*c;
    glDrawArrays(GL_TRIANGLE_FAN, baseIndex, (numVertices- numVertices/6));
    glDrawArrays(GL_LINES, baseIndex + (numVertices - numVertices/6), numVertices/6);
        }
void drawCircleSkel ( int c){
    int baseIndex = numVertices*c;
    glDrawArrays(GL_TRIANGLE_STRIP, baseIndex, numVertices);
    }

void drawSquare(int c){
    
    int baseIndex = numVertices*c;
    glDrawArrays(GL_TRIANGLE_FAN, baseIndex, numVertices);
    }


void display(){
    ++FrameCount;
    //printf("display-%d\n", FrameCount);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// bind circles VAO
    glBindVertexArray(VaoId[0]);
// draw each circle (pacman)
    int baseIndex;
    float threshold = 0.05;
    for (c = 0; c<numOrgs; c++){
        if ( rPointer[c] > threshold){
            
            drawSquare(c);
            }
        }
////bind gridlines VAO
    glBindVertexArray(VaoId[1]);
//draw vertical gridlines
    for(g=0; g<gWidth; g++){
        baseIndex = gHeight * g;
        glDrawArrays(GL_LINE_STRIP, baseIndex, gHeight);
        }
//draw horizontal gridlines
    for(g=0; g<gHeight; g++){
        baseIndex = gWidth * g;
        glDrawArrays(GL_LINE_STRIP, gHeight*gWidth+baseIndex, gWidth);
        }

    glutSwapBuffers();
    }

void IdleFunction(void){
    //glutPostRedisplay();
    int dumb = 0;
    }

void ResizeFunction(int Width, int Height){
    currentWidth = Width;
    currentHeight = Height;
    glViewport(0,0,currentWidth, currentHeight);
    }



void TimerFunction(int Value){
    if (0 != Value) {
        char* TempString = (char*) malloc(512 + strlen(WIN_PREFIX));
        sprintf(TempString, "%s: %d fps@ %d x %d",WIN_PREFIX,FrameCount * 4, currentWidth, currentHeight);        
        glutSetWindowTitle(TempString);
        free(TempString);
        }
    FrameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
    }

void moveCircle(int thisCircle, GLfloat x, GLfloat y){
    int baseIndex = numVertices * numDims;
    int first = baseIndex * thisCircle;
    int loc;
    for(i=0; i<numVertices; i++){
        loc = first + 4*i;
        cPointer[loc]+=x;
        cPointer[loc+1]+=y;
        }

    GLfloat dummy[baseIndex];
    for (i=0; i<baseIndex; i++){
        dummy[i]=cPointer[first+i];
        }

    glBindVertexArray(VaoId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(dummy), dummy);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glutPostRedisplay();
    }



void refreshCircle(int thisCircle, GLfloat rVal){
    printf("hello%d\n", thisCircle);

    int baseIndex = numVertices * numDims;
    int first = baseIndex * thisCircle;
    int loc;
    GLfloat xVal, yVal;
    int i;
    printf("%d,%d,%d: r= %f\n", numVertices, baseIndex, first, rVal);

    float xc, yc;
    xc=cPointer[first];
    yc=cPointer[first+1];
    float theta = 0.0;
    for (i=0; i<numVertices; i++){
        theta = (float)(i-1.0)* 2.0*PI/(float)(numVertices-1.0);
        xVal = xc + rVal*cos(theta);
        yVal = yc + rVal*sin(theta);
        loc = first + 4*i;    
        //loc = 4*i;     
        //printf("%d/%d:", i, loc);
        xVal = cPointer[loc];
        yVal = cPointer[loc+1];
        //printf("%d|(%f,%f)-> ", i, xVal, yVal);       
        cPointer[loc] = xVal;
        cPointer[loc+1] = yVal;
        xVal = cPointer[loc];
        yVal = cPointer[loc+1];
        //printf("(%f,%f)\n", cPointer[loc], cPointer[loc+1]);
        }
    //printf("\n");

    GLfloat dummyIn[baseIndex];
    GLfloat dummyOut[baseIndex];
    GLfloat dummyBackIn[baseIndex];

    for (i=0; i<baseIndex; i++){
        dummyOut[i] = cPointer[first+i];    
        }
//
//    for (i=0; i<numVertices; i++){
//        loc = first + 4*i;
//        printf("(%f,%f)\n", cPointer[loc], cPointer[loc+1]);
//        dummyOut[4*i]=cPointer[loc];
//        dummyOut[4*i+1] = cPointer[loc+1];
//        dummyOut[4*i+2] = 0.0;
//        dummyOut[4*i+3] = 1.0;
//        }

//    for(i=0; i<numVertices; i++){
//       printf("dummyOut%d:V%d=(%f,%f,%f,%f)\n",thisCircle,i, dummyOut[4*i], dummyOut[4*i+1], dummyOut[4*i+2], dummyOut[4*i+3] );
//    }
    int punchIn = sizeof(dummyIn) * thisCircle;

    glBindVertexArray(VaoId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
//    glBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummy), dummy);
    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyIn), dummyIn);
    glBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyOut), dummyOut);
    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBackIn), dummyBackIn);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

void refreshSquare(int op){
    //printf("updating square %d\n", op);
    int baseIndex = numVertices * numDims;    
    int first = baseIndex * op;
    
    float lv, xv, yv, zv;
    R = rPointer[op] * 0.5;
    xv = oCenters[3*op];
    yv=oCenters[3*op+1];
    zv=oCenters[3*op+2];
    float sq[4][4] = {{xv-R, yv-R, zv, 1.0},{xv-R, yv+R, zv, 1.0},{xv+R, yv+R, zv, 1.0},{xv+R, yv-R, zv, 1.0}};
    int pos = first;
    int i, j;
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
            cPointer[pos]=lv;
            pos++;
            }
        }
    GLfloat dummyOut[baseIndex];
    for (i=0; i<baseIndex; i++){
        dummyOut[i] = cPointer[first+i];    
        }
    int punchIn = sizeof(dummyOut) * op;
    glBindVertexArray(VaoId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyOut), dummyOut);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

void refreshOrg(int op){
    //printf("refreshing %d\n", op);
    if (strcmp(oType, "pacman") == 0){
        refreshCircle(op, rPointer[op]);
        }
    if (strcmp(oType, "square") == 0){
        refreshSquare(op);
        }
    }

void KeyboardFunction(unsigned char Key, int X, int Y){
    int updates[numOrgs];
    switch (Key) {

        case 'a':{ 
            moveCircle(0,-1.0, 0.0);  
            break;        
            }
        case 'c':{ 
            camOffset[0]++;
            glUseProgram(ProgramId);
            glUniform4f(offsetLocation, -1*camOffset[0], -1*camOffset[2], -1*camOffset[3], 0.0);  
            glBindBuffer(GL_ARRAY_BUFFER, VboId);
            glutPostRedisplay();
            break;        
            }

        case 'd':{
            moveCircle(0, 1.0, 0.0); 
            break;
            } 

        case 'f':{ 
            printf("f\n");
            for (i=0; i<numOrgs; i++){
                updates[i]=0;;
                }
            evolveGame2D(rPointer, bgPointer, updates, gHeight, gWidth, periodicMode);  
            for (i=0; i<numOrgs; i++){
                //printf("evolved state[%d]=%f\n", i, rPointer[i]);
                }
            for (i=0; i<numOrgs; i++){
                if (updates[i] == 1){
                    //printf("update: r[%d]=%f\n", i, rPointer[i]);
                    refreshOrg(i);
                    }
                }
            glutPostRedisplay();     
            }

//        case 'g':{ 
//            printf("g\n");  
//            break;        
//            }

        case 's':{ 
            moveCircle(0, 0.0, -1.0); 
            break;        
            }

        case 'v':{ 
            camOffset[0]--;
            glUseProgram(ProgramId);
            glUniform4f(offsetLocation, -1*camOffset[0], -1*camOffset[2], -1*camOffset[3], 0.0);  
            glBindBuffer(GL_ARRAY_BUFFER, VboId);
            glutPostRedisplay();
            break;        
            }
        case 'w':{ 
            moveCircle(0, 0.0, 1.0);  
            break;        
            }

        default:
            break;
        }
    }

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y){
    glUseProgram(ProgramId);
    if(key == GLUT_KEY_UP){
        camOffset[1]++;
        }    
    if(key == GLUT_KEY_DOWN){
        camOffset[1]--;
        }  
    if(key == GLUT_KEY_LEFT){
        camOffset[0]--;
        }  
    if(key == GLUT_KEY_RIGHT){
        camOffset[0]++;
        }  
    glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[3], 0.0);  
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glutPostRedisplay();

}

void main(int argc, char**argv) {

//flag parsing
    int externFile = 0;
    int typeSpecd = 0;
    for (i=1; i<argc; i++){
        if ( strcmp(argv[i], "-p") == 0){
            periodicMode = 1;
            }                
        if ( strcmp(argv[i], "-g") == 0){
            if ( (i+1)<argc ){
                gWidth = atoi(argv[i+1]);
                }
            gHeight = (i+2) < argc ? atoi(argv[i+2]) : gWidth;
            printf ("Grid Supplied: %d x %d\n", gWidth, gHeight);

            }
//        else{
//            printf("%s\n", argv[i]);
//            }
        if (( strcmp(argv[i], "-f") == 0) && ((i+1)<argc)){
            externFile = 1;
            loadFile = argv[i+1];
            }
        if (( strcmp(argv[i], "-t") == 0) && ((i+1)<argc)){
            typeSpecd = 1;
            oType = argv[i+1];
            }
        }
    //check for valid organism type requested
    if (typeSpecd == 1){
        int typeFound = 0;
        for (i=0; i<2; i++){
            if (strcmp(oType, oList[i]) == 0){
                typeFound = 1;
                }
            }
        oType = (typeFound == 1) ? oType : oList[0];
        }
    printf("oType is %s\n", oType);
    if (strcmp(oType, "square") == 0){
        numVertices = 4;
        }
    if (strcmp(oType, "pacman") == 0){
        numVertices = 21;
        }

    glutInit(&argc, argv);    
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(currentWidth,currentHeight);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //this flag turns the window transparent when decommented
    winHandle = glutCreateWindow(WIN_PREFIX);
    printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

//assign callbacks
    glutReshapeFunc(ResizeFunction);
    glutDisplayFunc(display);
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);
    glutKeyboardFunc(KeyboardFunction);
    glutSpecialFunc(specialKeyInput);
    glutCloseFunc(Cleanup);

    glewExperimental = GL_TRUE;
    GLenum GlewInitResult;
    GlewInitResult = glewInit();

    CreateShaders();


//initialization for CreateVBO() call

    //if (externFile > 0 ){
    if (externFile == 0){
        loadFile = "blub.txt";
        }
    printf("Input File: %s\n", loadFile);

    // set our global pointers arrays of the proper size,
    // given the input specs
    numOrgs = gWidth * gHeight;
    numoFloats = numVertices * numDims * numOrgs;

    GLfloat vert[numoFloats];
    cPointer = vert;

    GLfloat colors[numoFloats];
    cColPointer = colors;

    int orgLocArray[numOrgs];
    orgLoc = orgLocArray;
 
    GLfloat cCenterArray[numOrgs * 3];
    oCenters = cCenterArray;

    GLfloat cRads[numOrgs];
    GLfloat* rlPointer;
    rlPointer = cRads;

    int upVec[numOrgs];
    updates = upVec;

    int xStart, yStart;
    yStart = ( (gHeight%2) == 0) ? 1.0 : (float) (-1*(gHeight-1)/2);
    xStart = ( (gWidth%2) == 0) ? 1.0 : (float) (-1*(gWidth-1)/2);
    int loc = 0;
    for(i=0; i<gHeight; i++){
        for (j = 0; j<gWidth; j++){
            oCenters[loc] = xStart + (float)j;
            loc ++;
            oCenters[loc] = yStart + (float)i;
            loc++;
            oCenters[loc] = 0.0;
            loc++; 
            }
        }
//    for(i=0; i<numOrgs; i++){
//        printf("oCenter[%d]=(%f,%f,%f)\n",i, oCenters[3*i], oCenters[3*i+1], oCenters[3*1+2]);
//        }
// interesting idea- store existence information in Radius array (r=0?)
//coup de Grace...
//think it will make sense to 1Dify the linkPractice info for output, even if 2/3D is better for calculation.

    rPointer = cRads;
    //printf("rPointer = %d\n", cRads);
    fillGrid1D(loadFile, rPointer, numOrgs);
//    for(i=0; i<numOrgs; i++){
//        printf("rPointer[%d]=%f\n", i, rPointer[i]);
//        }

        //}
//    else{
//        numoFloats = numVertices * numDims * numOrgs;
//        GLfloat vert[numoFloats];
//        //cPointer defined globally for subroutine access
//        cPointer = vert;
//
//    
//        GLfloat cCenterArray[] = {-0.2, -0.2, 0.0, 0.1, 0.1, 0.0};
//        oCenters = cCenterArray;
//        GLfloat cRads[] = {0.1,0.2};// interesting idea- store existence information in Radius array (r=0?)
//
//        rPointer = cRads; 
//       }

    GLfloat cCols[numoFloats];
    cColPointer = cCols;

    scaleX = 1.0/ (float)((gWidth+1)/2);
    scaleY = 1.0/ (float)((gHeight+1)/2);
    glUniform4f(scaleLocation, scaleX, scaleY, 1.0, 1.0);  


    CreateVBO(cPointer, cColPointer, oCenters, cRads);


//initialize blankGrid for evolveGame operations
    GLfloat blankGrid[gHeight][gWidth];
    GLfloat* bPointer[gHeight];
    for(i=0; i<gHeight; i++){
        bPointer[i]=blankGrid[i];
        }
    bgPointer = bPointer;

    glClearColor(0.0,0.0,0.0,0.0); //RGBA
    glutMainLoop();

    }

void Cleanup(void) {
    DestroyShaders();
    DestroyVBO();
    }

void DestroyShaders(void) {

    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);
    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);
    glDeleteProgram(ProgramId);    

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

void DestroyVBO(void) {
    GLenum ErrorCheckValue = glGetError();
    glBindVertexArray(VaoId[1]);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VgoCId);
    glDeleteBuffers(1, &VgoId);

    glBindVertexArray(VaoId[0]);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

