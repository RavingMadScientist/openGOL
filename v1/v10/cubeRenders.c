#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

#include "funcs1d.h"
#include "funcs2d.h"
#include "textLib.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GL/glu.h>
#include<GL/gl.h>

#define WIN_PREFIX "openGOL v1.0"

int currentWidth = 800;
int currentHeight = 800;
int winHandle = 0;
unsigned FrameCount = 0;
int gWidth = 11;
int gHeight = 11;

int autoMode = 0; 
int thisGen = 0;
int userStopped = 0; //p pauses evolve



GLuint OVertexShaderId, OFragmentShaderId, OProgramId;// organism rendering program memory pointers
GLuint TVertexShaderId, TFragmentShaderId, TProgramId; // text rendering program memory pointers

GLuint VaoId[3]; // presently three Vertex Arrays (O,G,T), each divided into buffers

GLuint VboId[2]; //organism vertex array contains position and color buffers
GLuint VgoId[2]; //grid buffer pointers

GLuint VtoId[3]; // Text data has position, color, and texture buffers 
// note that Vto here describes the Text Vertex Array, NOT the triangle VA it is used for in gsquares91!

GLuint offsetLocation; //pointer to navigation vec4
GLuint scaleLocation;
GLfloat scaleX, scaleY, scaleZ;
GLfloat camOffset[4]={0.0, 0.0, 0.0, 0.0 };

GLuint txtOffsetLocation;
GLuint txtScaleLocation;
GLuint texLocation;

int mouseX;
int mouseY;

void Cleanup(void);
void DestroyShaders(void);
void DestroyVBO(void);
void evolveGame(void);

const GLchar* OVertexShader = 
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

const GLchar* TVertexShader = 
{
    "#version 330\n"\
    "layout(location=0) in vec4 in_Position;\n"\
    "layout(location=1) in vec4 in_Color;\n"\
    "layout(location=2) in vec2 tex_Coords;\n"\
    "uniform vec4 offset;\n"\
    "uniform vec4 scale;\n"\
    "//in vec2 texcoord;\n"\
    "out vec4 ex_Color;\n"\
    "out vec2 Tex_Coords;\n"\
    "void main(void)\n"\
    "{\n"\
    "   gl_Position = (scale*in_Position)+offset;\n"\
    "   ex_Color = in_Color;\n"\
    "   Tex_Coords = tex_Coords;\n"\
    "}\n"
};

const GLchar* OFragmentShader = 
{
    "#version 330\n"\
    "in vec4 ex_Color;\n"\
    "out vec4 out_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   out_Color = ex_Color;\n"\
    "}\n"
};

const GLchar* TFragmentShader = 
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


void CreateShaders(void) {
    printf("CreateShaders called\n");
    GLenum ErrorCheckValue = glGetError();

    OVertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(OVertexShaderId, 1, &OVertexShader, NULL);
    glCompileShader(OVertexShaderId);

    OFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(OFragmentShaderId, 1, &OFragmentShader, NULL);
    glCompileShader(OFragmentShaderId);

    OProgramId = glCreateProgram();
    glAttachShader(OProgramId, OVertexShaderId);
    glAttachShader(OProgramId, OFragmentShaderId);
    glLinkProgram(OProgramId);
    glUseProgram(OProgramId);    

    //specify a Uniform vec4 for moving our camera VertexShader offset around
    offsetLocation = glGetUniformLocation(OProgramId, "offset");
    scaleLocation = glGetUniformLocation(OProgramId, "scale");
    //should be global (is)

    TVertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(TVertexShaderId, 1, &TVertexShader, NULL);
    glCompileShader(TVertexShaderId);

    TFragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(TFragmentShaderId, 1, &TFragmentShader, NULL);
    glCompileShader(TFragmentShaderId);

    TProgramId = glCreateProgram();
    glAttachShader(TProgramId, TVertexShaderId);
    glAttachShader(TProgramId, TFragmentShaderId);
    glLinkProgram(TProgramId);
    glUseProgram(TProgramId); 

    txtOffsetLocation = glGetUniformLocation(TProgramId, "offset");
    txtScaleLocation = glGetUniformLocation(TProgramId, "scale");
    texLocation = glGetUniformLocation(TProgramId, "atexture");
    glUniform4f(txtOffsetLocation, -1.0, -0.8, 0.0, 0.0);
    glUniform4f(txtScaleLocation, 2.0/(float)currentWidth, -2.0/(float)currentHeight, 1.0, 1.0);

    glUseProgram(OProgramId); 

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

CreateVBO(){

//Part II. VBO construction and assignment logic.
// Instantiating our two VBOs...
    GLenum ErrorCheckValue = glGetError();
    glGenVertexArrays(3, VaoId);

//circles
    glBindVertexArray(VaoId[0]);

    glGenBuffers(2, VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Cols), Cols, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

//gridlines
    glBindVertexArray(VaoId[1]);

    glGenBuffers(2, VgoId);
    glBindBuffer(GL_ARRAY_BUFFER, VgoId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertices), gVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VgoId[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gColors), gColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);


//Text
    glBindVertexArray(VaoId[2]);
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

    glBindVertexArray(VaoId[0]);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }   

    }

void display(){
    ++FrameCount;
    //printf("display-%d\n", FrameCount);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(OProgramId); //cam shaders,
    glBindVertexArray(VaoId[0]);
    int baseIndex;
            drawSquare(c);

    glUseProgram(TProgramId);
    glBindVertexArray(VaoId[2]);
    glBindVertexArray(VaoId[0]);       
    glutSwapBuffers();
    if((FrameCount == 1)&&(thisGen==0)){
        printf("fc1\n");
        glutPostRedisplay();
        }

void IdleFunction(void){
    //glutPostRedisplay();
    int dumb = 0;
    }
void ResizeFunction(int Width, int Height){
    currentWidth = Width;
    currentHeight = Height;

    float cWidth = (float)Width;
    float cHeight = (float)Height;
    float newYOffset = -1.0 + cHeight / 400.0;

    glUseProgram(TProgramId);
    glUniform4f(txtOffsetLocation, -1.0, newYOffset, 0.0, 0.0); 
    glutPostRedisplay();
//    glViewport(0,0,currentWidth, currentHeight);
    }

void TimerFunction(int Value){
    if (0 != Value) {
        char* TempString = (char*) malloc(512 + strlen(WIN_PREFIX));
         sprintf(TempString, "%s: %d fps@ %d x %d . Rect Coords(%d,%d,%f,%f)",WIN_PREFIX,FrameCount * 4, currentWidth, currentHeight, mouseX, mouseY, lAlign, topAlign );        
           glutSetWindowTitle(TempString);
        free(TempString);
        }
    FrameCount = 0;
    if (Value == 99){
        evolveGame();
        }
    else{
        glutTimerFunc(250, TimerFunction, 1);
        }
    }

void evolveCube(){


    }

void mouseControl(int button, int state, int x, int y){
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
	  // Store the clicked point in the points array after correcting
	  // from event to OpenGL co-ordinates.    
        mouseX = x;
        mouseY = y;
        }
    }

void KeyboardFunction(unsigned char Key, int X, int Y){
    int updates[numOrgs];
    switch (Key) {

        case 'f':{ 
            evolveGame();   
            break;
            }
        default:{
            break;}
        }

void specialKeyInput(int key, int x, int y){
    glUseProgram(OProgramId);
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
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
    glutPostRedisplay();

}

void main(int argc, char**argv) {
    for (i=1; i<argc; i++){
        if ( strcmp(argv[i], "-g") == 0){
            if ( (i+1)<argc ){
                gWidth = atoi(argv[i+1]);
                }
            gHeight = (i+2) < argc ? atoi(argv[i+2]) : gWidth;
            printf ("Grid Supplied: %d x %d\n", gWidth, gHeight);

            }

        if ( strcmp(argv[i], "-a") == 0){
            if ((i+1)<argc){
                autoMode = atoi(argv[i+1]);
                }
            else{
                autoMode = -1; // inf loop
                }
            }

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
    glutMouseFunc(mouseControl); 
    glutKeyboardFunc(KeyboardFunction);
    glutSpecialFunc(specialKeyInput);
    glutCloseFunc(Cleanup);

    glewExperimental = GL_TRUE;
    GLenum GlewInitResult;
    GlewInitResult = glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CreateShaders();
    CreateVBO();
    glClearColor(0.0,0.0,0.0,0.0); //RGBA
    glutMainLoop();

    }

void Cleanup(void) {
    glDeleteTextures(numCharTxts, tex);
    DestroyShaders();
    DestroyVBO();
    }

void DestroyShaders(void) {

    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);
    glDetachShader(TProgramId, TVertexShaderId);
    glDetachShader(TProgramId, TFragmentShaderId);

    glDeleteShader(TFragmentShaderId);
    glDeleteShader(TVertexShaderId);
    glDeleteProgram(TProgramId);    

    glDetachShader(OProgramId, OVertexShaderId);
    glDetachShader(OProgramId, OFragmentShaderId);

    glDeleteShader(OFragmentShaderId);
    glDeleteShader(OVertexShaderId);
    glDeleteProgram(OProgramId);    

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

void DestroyVBO(void) {
    GLenum ErrorCheckValue = glGetError();


    glBindVertexArray(VaoId[2]);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(3, VtoId);

    glBindVertexArray(VaoId[1]);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(2, VgoId);

    glBindVertexArray(VaoId[0]);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(2, VboId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

