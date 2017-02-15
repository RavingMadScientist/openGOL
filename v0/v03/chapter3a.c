#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GL/glu.h>
#include<GL/gl.h>

#define WIN_PREFIX "HELLOW ibo3a"

/*chapter2_.c is a direct extension of the work in ../ogl01, which culminated in chapter1x.c
it follows Tut.2 from openglbook.com

compile w gcc chapter2a.c -lGLEW -lglut -lGLU -lGL -o chapter2a

2b and 2d demonstrate the drawing of squares using GL_TRIANGLES and GL_TRIANGLE_STRIP, respectively
requiring 6 vs. 4 vertices
2f introduces the custom data struct Vertex, which packages color info with vertex coords
*/

typedef struct{
    float XYZW[4];
    float RGBA[4];
    } Vertex;

int currentWidth = 600;
int currentHeight = 600;
int winHandle = 0;
unsigned FrameCount = 0;

int numDims = 4;
float R;
GLfloat sq[4][4] ;
int numTSquares = 5;

int numTCoords;// = numTSquares * 4 * numDims; //four vertices per square


//some new shit here
GLuint VertexShaderId, FragmentShaderId, ProgramId, VboId, BufferId, IndexBufferId, ColorBufferId; //removed in 2f, idk y
GLuint VaoId[2];
GLuint VtoId, VtoColId;
const GLchar* VertexShader = 
{
    "#version 330\n"\
    "layout(location=0) in vec4 in_Position;\n"\
    "layout(location=1) in vec4 in_Color;\n"\
    "out vec4 ex_Color;\n"\

    "void main(void)\n"\
    "{\n"\
    "   gl_Position = in_Position;\n"\
    "   ex_Color = in_Color;\n"\
    "   ex_Color.a = 0.4;\n"\
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


void ResizeFunction(int Width, int Height){
    currentWidth = Width;
    currentHeight = Height;
    glViewport(0,0,currentWidth, currentHeight);
// glViewport syntax: (XLowerLeft, YLowerLeft, Width, Height);
    }


void drawSquare(int c){
    printf("Drawing Square\n");
    int baseIndex = 16;
    int start = 4*c;
    glDrawArrays(GL_TRIANGLE_FAN, start, 4);
    GLfloat dummyIn[baseIndex];
    int punchIn = sizeof(dummyIn);
    punchIn *= c;
glBindBuffer(GL_ARRAY_BUFFER, VtoId);

    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyIn), dummyIn);
    int i, j;
        printf("square displayVals[%d]:\n",c);
    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            printf("%f, ", dummyIn[4*i+j]);
            }
        printf("\n");
        }

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
    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);

    glBindVertexArray(VaoId[1]);
    int q;
    for (q=0; q<numTSquares; q++){
        drawSquare(q);
        }
    glBindVertexArray(VaoId[0]);       
    glutSwapBuffers();
//    glFlush();
//    glutPostRedisplay(); //relation to glFlush unknown
	}

void IdleFunction(void){
    //glutPostRedisplay();

    int q;
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

    CreateShaders();
    CreateVBO();
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
    DestroyShaders();
    DestroyVBO();
    }

void CreateVBO(void) {
    printf("CreateVBO called\n");
//    GLfloat Vertices[] = {
//        -0.8f, 0.8f, 0.0f, 1.0f,
//        0.8f, 0.8f, 0.0f, 1.0f,
//        -0.8f, -0.8f, 0.0f, 1.0f,
//        0.8f, -0.8f, 0.0f, 1.0f
//        };
//
    GLfloat Colors[] = {
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0
        };
//    Vertex Vertices[] = {
//        {{-0.8f, -0.8f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
//        {{0.0f, 0.8f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},        
//        {{0.8f, -0.8f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
//        };
    Vertex Vertices[] =
    {
        	{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        	// Top
        	{ { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        	{ { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        	{ { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        	{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        	// Bottom
        	{ { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        	{ { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        	{ { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        	{ { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        	// Left
        	{ { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        	{ { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        	{ { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        	{ { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        	// Right
        	{ { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        	{ { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        	{ { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
        	{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
    };

    GLubyte Indices[] = {
        	// Top
        	0, 1, 3,
        	0, 3, 2,
        	3, 1, 4,
        	3, 4, 2,
        	// Bottom
        	0, 5, 7,
        	0, 7, 6,
        	7, 5, 8,
        	7, 8, 6,
        	// Left
        	0, 9, 11,
        	0, 11, 10,
        	11, 9, 12,
        	11, 12, 10,
        	// Right
        	0, 13, 15,
        	0, 15, 14,
        	15, 13, 16,
        	15, 16, 14
    };

    GLenum ErrorCheckValue = glGetError();

    const size_t BufferSize = sizeof(Vertices);
    const size_t VertexSize = sizeof(Vertices[0]);
    const size_t RgbOffset = sizeof(Vertices[0].XYZW);

    numTCoords = numTSquares * 4 * numDims;
// now spec the vertices for text squares.
    GLfloat txtVertices[numTCoords];
    GLfloat txtColors[numTCoords];

    int i, j, k, o;
    float xv, yv, zv, lv;

    R = 0.05f;

    //float sq[4][4];

    for (i=0; i<numTSquares; i++){
        float ifloat = (float)i;
        xv= -1.0 + 0.2 * ifloat + R;
        yv = 0.8;
        zv = 0.0;
 printf("R,xv+r, xv-R=(%f, %f, %f)\n", R, xv+R, xv-R);
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


                printf("sq[%d][%d][%d] = %f\n", i, j, k, lv);
                txtVertices[o]=lv;
                }
            }
        }

    for (i=0; i<numTSquares; i++){
        for (j=0; j<4; j++){
            k=16*i+4*j;
            txtColors[k]=0.0;
            txtColors[k+1]=1.0;
            txtColors[k+2]=0.5;
            txtColors[k+3]=1.0;
            }    
    
        }


    glGenVertexArrays(2, VaoId); // change this to reflect num. vertex arrays bound to your buffer.
    glBindVertexArray(VaoId[0]);


//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(0);
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
    glEnableVertexAttribArray(0);    
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


//    glGenBuffers(1, &ColorBufferId);
//    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(1);

    glBindVertexArray(VaoId[1]);
    glGenBuffers(1, &VtoId);
    glBindBuffer(GL_ARRAY_BUFFER, VtoId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtVertices), txtVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VtoColId);
    glBindBuffer(GL_ARRAY_BUFFER, VtoColId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtColors), txtColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);


    glBindVertexArray(VaoId[0]);



    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

void DestroyVBO(void) {

    GLenum ErrorCheckValue = glGetError();

    glBindVertexArray(VaoId[1]);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VtoColId);
    glDeleteBuffers(1, &VtoColId);
    glBindBuffer(GL_ARRAY_BUFFER, VtoId);
    glDeleteBuffers(1, &VtoId);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &IndexBufferId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, VaoId);

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