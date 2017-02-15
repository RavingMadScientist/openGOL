#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GL/glu.h>
#include<GL/gl.h>

#include"textLib.h"

#define WIN_PREFIX "HELLOW ibo3a"

/*chapter2_.c is a direct extension of the work in ../ogl01, which culminated in chapter1x.c
it follows Tut.2 from openglbook.com

compile w gcc chapter2a.c -lGLEW -lglut -lGLU -lGL -o chapter2a

2b and 2d demonstrate the drawing of squares using GL_TRIANGLES and GL_TRIANGLE_STRIP, respectively
requiring 6 vs. 4 vertices
2f introduces the custom data struct Vertex, which packages color info with vertex coords
*/


int currentWidth = 600;
int currentHeight = 600;
int winHandle = 0;
unsigned FrameCount = 0;

int numDims = 4;
float R;
GLfloat sq[4][4] ;
int numTSquares = 5;
int numTCoords;// = numTSquares * 4 * numDims; //four vertices per square

GLfloat tri[3][4];
int numTris = 7;
float Rt;
int numTriCoords;

//some new shit here
GLuint VertexShaderId, FragmentShaderId, ProgramId; //removed in 2f, idk y
GLuint VaoId[2];
GLuint VboId[3];
GLuint VtoId[3];
GLuint tex;
GLuint texLocation; // fragment shader uniform identifier
static unsigned char chessboard[64][64][4]; 
unsigned char localTexture[8][8][4];


const GLchar* VertexShader = 
{
    "#version 330\n"\
    "layout(location=0) in vec4 in_Position;\n"\
    "layout(location=1) in vec4 in_Color;\n"\
    "layout(location=2) in vec2 tex_Coords;\n"\
    "//in vec2 texcoord;\n"\
    "out vec4 ex_Color;\n"\
    "out vec2 Tex_Coords;\n"\
    "void main(void)\n"\
    "{\n"\
    "   gl_Position = in_Position;\n"\
    "   ex_Color = in_Color;\n"\
    "   Tex_Coords = tex_Coords;\n"\
    "}\n"
};

const GLchar* FragmentShader = 
{
    "#version 330\n"\
    "in vec4 ex_Color;\n"\
    "in vec2 Tex_Coords;\n"\
    "out vec4 out_Color;\n"\
    "uniform sampler2D aTexture;\n"\
    "void main(void)\n"\
    "{\n"\
    "   //out_Color = texture(aTexture, Tex_Coords) * ex_Color ;\n"\
    "   out_Color = texture(aTexture, Tex_Coords) ;\n"\
    "}\n"
};

//void Initialize(int, char*[]);
//void InitWindow(int, char*[]);
//void ResizeFunction(int, int);
//void RenderFunction(void);
void TimerFunction(int); //defined after main
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);

void createChessboard(void){
   int i, j;
   for (i = 0; i < 64; i++) 
      for (j = 0; j < 64; j++) 
         if ( ( ((i/8)%2) && ((j/8)%2) ) || ( !((i/8)%2) && !((j/8)%2) ) )
		 {
            chessboard[i][j][0] = 0x00;
            chessboard[i][j][1] = 0x00;
            chessboard[i][j][2] = 0x00;
            chessboard[i][j][3] = 0xFF;
		 }
		 else
		 {
            chessboard[i][j][0] = 0xFF;
            chessboard[i][j][1] = 0xFF;
            chessboard[i][j][2] = 0xFF;
            chessboard[i][j][3] = 0xFF;
		 }
    }

void loadProcTexture(){
    setupTextLib();
    getCharTexture('q', localTexture);
    int i, j, k;
    for (i=0; i<8; i++){
        for (j=0; j<8; j++){
            printf("localTexture[%d,%d]=( ",i,j);
            for (k=0; k<4; k++){
                printf("%d, ", localTexture[i][j][k]);
                }
            printf(")\n");
            }   
        }

    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //float pixels[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,0.0f, 0.0f, 0.0f};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

//chessboard
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, chessboard);
// charText
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, localTexture);

    glGenerateMipmap(GL_TEXTURE_2D);
    }

void ResizeFunction(int Width, int Height){
    currentWidth = Width;
    currentHeight = Height;
    glViewport(0,0,currentWidth, currentHeight);
// glViewport syntax: (XLowerLeft, YLowerLeft, Width, Height);
    }


void drawSquare(int c){
    printf("Drawing Square %d\n", c);
    int baseIndex = 16;
    int start = 4*c;
    glDrawArrays(GL_TRIANGLE_FAN, start, 4);
    GLfloat dummyIn[baseIndex];
    int punchIn = sizeof(dummyIn);
    punchIn *= c;
    glBindBuffer(GL_ARRAY_BUFFER, VtoId[0]);

    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyIn), dummyIn);
    int i, j;
//        printf("square displayVals[%d]:\n",c);
//    for(i=0; i<4; i++){
//        for(j=0; j<4; j++){
//            printf("%f, ", dummyIn[4*i+j]);
//            }
//        printf("\n");
//        }

    }

void drawTriangle(int c){
    printf("Drawing Triangle %d\n", c);
    int baseIndex = 12;
    int start = 3*c;
    glDrawArrays(GL_TRIANGLES, start, 3);
    GLfloat dummyIn[baseIndex];
    int punchIn = sizeof(dummyIn);
    punchIn *= c;
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyIn), dummyIn);
    int i, j;
//        printf("triangle displayVals[%d]:\n",c);
//    for(i=0; i<3; i++){
//        for(j=0; j<4; j++){
//            printf("%f, ", dummyIn[4*i+j]);
//            }
//        printf("\n");
//        }

    }
void display() {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
//    glutSwapBuffers();
//    glutPostRedisplay();
//    printf("display-func called\n");

//    glClearColor(1.0f,0.1f,0.0f,0.0f); //RGB
//    glClear(GL_COLOR_BUFFER_BIT); //sets screen to desired color
    ++FrameCount;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(VaoId[0]);
    //glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
    int q;
//    for (q=0; q<numTris; q++){
//        drawTriangle(q);
//        }
    
    glBindVertexArray(VaoId[1]);

    for (q=0; q<numTSquares; q++){
        drawSquare(q);
        }
    glBindVertexArray(VaoId[0]);       
    glutSwapBuffers();
    if(FrameCount == 1){
        printf("fc1\n");
        glutPostRedisplay();
        }
//    glFlush();
//    glutPostRedisplay(); //relation to glFlush unknown
	}

void IdleFunction(void){
    //glutPostRedisplay();

    int q = 0;
    }




void main(int argc, char**argv) {
    glutInit(&argc, argv);  //initializes FreeGLUT library
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
//    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowPosition(100,100);
//    glutInitWindowSize(currentWidth,currentHeight);
    glutInitWindowSize(currentWidth,currentHeight);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //this flag turns the window transparent when decommented
//    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA); //GLUT_DOUBLE flag seems to be responsible culprit provoking window transparency
//source of earlier transparency bug was REQUIREMENT to call glutSwapBuffers in Display function if dual buffers are used

    printf("preparing to glutCreateWindow\n");
    winHandle = glutCreateWindow(WIN_PREFIX);
    printf("glutCreateWindow created\n");
/*
    if(winHandle < 1) {
        fprintf(stderr,"ERROR: Could not create a new rendering window.\n");
        exit(EXIT_FAILURE);
        }
*/
    printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
    glutReshapeFunc(ResizeFunction);
    //glutDisplayFunc(RenderFunction);
    glutDisplayFunc(display);
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);
    glutCloseFunc(Cleanup);

    glewExperimental = GL_TRUE;
    GLenum GlewInitResult;
    GlewInitResult = glewInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    createChessboard();
    loadProcTexture();


    CreateShaders();
    CreateVBO();
    glUniform1i(texLocation, tex);
//    printf("CreateVBO returned to display OK\n");
    glClearColor(0.0,0.0,0.0,0.0); //RGB

    glutMainLoop();
    //exit(EXIT_SUCCESS);

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

void Cleanup(void) {
    glDeleteTextures(1, &tex);
    DestroyShaders();
    DestroyVBO();
    }

void CreateVBO(void) {
    printf("CreateVBO called\n");
    int i, j, k, o;
    float xv, yv, zv, lv, ifloat;

    numTriCoords = numTris * 3 * numDims;
    GLfloat triVertices[numTriCoords];
    GLfloat triColors[numTriCoords];     
    GLfloat triTextures[numTriCoords/2];  
    Rt = 0.3;
    for (i=0; i<numTris; i++){
         ifloat = (float)i;
        xv= -1.0 + 0.2 * ifloat + R;
        yv = 0.6;
        zv = 0.0;
         //printf("Rt,xv+Rt, yv+Rt=(%f, %f, %f)\n", R, xv+Rt, yv+Rt);
        tri[0][0]=xv-Rt;
        tri[0][1]=yv-Rt;
        tri[0][2]=zv;
        tri[0][3]=1.0;
        tri[1][0]=xv;
        tri[1][1]=yv+Rt;
        tri[1][2]=zv;
        tri[1][3]=1.0;
        tri[2][0]=xv+Rt;
        tri[2][1]=yv-Rt;
        tri[2][2]=zv;
        tri[2][3]=1.0;
        for (j=0; j<3; j++){
            for (k=0; k<4; k++){
                o=12*i+4*j+k;            
                lv=tri[j][k];
                //printf("tri[%d][%d][%d] = %f\n", i, j, k, lv);
                triVertices[o]=lv;
                }
            }
        }

    for (i=0; i<numTris; i++){
        k = 6*i;
            //texture Coords
        triTextures[k] = 0.0;
        triTextures[k+1] = 0.0;
        triTextures[k+2] = 0.5;
        triTextures[k+3] = 1.0;
        triTextures[k+4] = 1.0;
        triTextures[k+5] = 0.0;
        //color Coords
        for (j=0; j<3; j++){
            k=12*i+4*j;
            triColors[k]=0.8;
            triColors[k+1]=0.0;
            triColors[k+2]=0.0;
            triColors[k+3]=0.8;
            }    
    
        }

    numTCoords = numTSquares * 4 * numDims;
// now spec the vertices for text squares.
    GLfloat txtVertices[numTCoords];
    GLfloat txtColors[numTCoords];
    GLfloat txtTextures[numTCoords/2];

    R = 0.05f;

    //float sq[4][4];

    for (i=0; i<numTSquares; i++){
        ifloat = (float)i;
        xv= -1.0 + 0.2 * ifloat + R;
        yv = 0.8;
        zv = 0.0;
         //printf("R,xv+r, xv-R=(%f, %f, %f)\n", R, xv+R, xv-R);
        //sq = {{xv-R, yv-R, zv, 1.0},{xv-R, yv+R, zv, 1.0},{xv+R, yv+R, zv, 1.0},{xv+R, yv-R, zv, 1.0}};
        //sq[0] = {xv-R, yv-R, zv, 1.0};
        //sq[1] = {xv-R, yv+R, zv, 1.0};
        //sq[2] = {xv+R, yv+R, zv, 1.0};
        //sq[3] = {xv+R, yv-R, zv, 1.0};
        sq[0][0]=xv-R;
        sq[0][1]=yv-R;
        sq[0][2]=zv;
        sq[0][3]=1.0;
        sq[1][0]=xv-R;
        sq[1][1]=yv+R;
        sq[1][2]=zv;
        sq[1][3]=1.0;
        sq[2][0]=xv+R;
        sq[2][1]=yv+R;
        sq[2][2]=zv;
        sq[2][3]=1.0;
        sq[3][0]=xv+R;
        sq[3][1]=yv-R;
        sq[3][2]=zv;
        sq[3][3]=1.0;
        for (j=0; j<4; j++){
            for (k=0; k<4; k++){
                o=16*i+4*j+k;            
                lv=sq[j][k];


                //printf("sq[%d][%d][%d] = %f\n", i, j, k, lv);
                txtVertices[o]=lv;
                }
            }
        }

    for (i=0; i<numTSquares; i++){
//texture Coords
        k = 8*i;
            //texture Coords
//        txtTextures[k] = 0.0;
//        txtTextures[k+1] = 0.0;
//        txtTextures[k+2] = 0.0;
//        txtTextures[k+3] = 1.0;
//        txtTextures[k+4] = 1.0;
//        txtTextures[k+5] = 1.0;
//        txtTextures[k+6] = 1.0;
//        txtTextures[k+7] = 0.0;
        txtTextures[k] = 0.0;
        txtTextures[k+1] = 0.0;
        txtTextures[k+2] = 0.0;
        txtTextures[k+3] = 1.1;
        txtTextures[k+4] = 1.1;
        txtTextures[k+5] = 1.1;
        txtTextures[k+6] = 1.1;
        txtTextures[k+7] = 0.0;
        for (j=0; j<4; j++){
            k=16*i+4*j;
            txtColors[k]=1.0;
            txtColors[k+1]=1.0;
            txtColors[k+2]=1.0;
            txtColors[k+3]=1.0;
            }    
    
        }

    GLenum ErrorCheckValue = glGetError();
    glGenVertexArrays(2, VaoId); // change this to reflect num. vertex arrays bound to your buffer.
    glBindVertexArray(VaoId[0]);

    glGenBuffers(3, VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);    

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triColors), triColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triTextures), triTextures, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

//    glGenBuffers(1, &ColorBufferId);
//    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(1);

    glBindVertexArray(VaoId[1]);
    glGenBuffers(3, VtoId);
    glBindBuffer(GL_ARRAY_BUFFER, VtoId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtVertices), txtVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VtoId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtColors), txtColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VtoId[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtTextures), txtTextures, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    //glBindVertexArray(VaoId[0]);



    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

void DestroyVBO(void) {
    printf("destroy VBO\n");
    GLenum ErrorCheckValue = glGetError();

    glBindVertexArray(VaoId[1]);

//    glDisableVertexAttribArray(1);
//    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ARRAY_BUFFER, VtoId[1]);
//    glDeleteBuffers(1, &VtoColId);
//    glBindBuffer(GL_ARRAY_BUFFER, VtoId);
    glDeleteBuffers(3, VtoId);
//    glDeleteBuffers(1, &VtoId[1]);
//    glDeleteBuffers(1, &VtoId[0]);


//    glDeleteBuffers(1, &ColorBufferId);


    glBindVertexArray(VaoId[0]);

//    glDisableVertexAttribArray(1);
//    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDeleteBuffers(1, &IndexBufferId);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VboId);
//    glDeleteBuffers(1, &VboId[1]);
//    glDeleteBuffers(1, &VboId[0]);

    glDeleteBuffers(3, VboId);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDeleteVertexArrays(2, VaoId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

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

    texLocation = glGetUniformLocation(ProgramId, "atexture");


    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
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