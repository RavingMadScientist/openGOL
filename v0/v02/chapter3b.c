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
//some new shit here
GLuint VertexShaderId, FragmentShaderId, ProgramId, VaoId, VboId, BufferId, IndexBufferId[2], ActiveIndexBuffer = 0;//, ColorBufferId; removed in 2f, idk y


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

//    glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
//    if (ActiveIndexBuffer == 0){
//        glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, NULL);    
//        }
//    else {
//        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);
//    }

    glColor3d(1.0, 0.0, 0.0);
    glRasterPos2d(0, 0);


    char* aString = "SCORE!";
    int z;
    for (z=0; z<6; z++ ){

        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, aString[z]);
        }
//    glLoadIdentity();  
//    glColor3f(0.0,0.0,0.0);
//    glWindowPos2f(100.0,100.0);
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,'S');
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,'C');
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,'O');
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,'R');
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,'E');
//    glutBitmapCharacter (GLUT_BITMAP_8_BY_13,':');
    glFlush(); 
    glutSwapBuffers();
//    glFlush();
//    glutPostRedisplay(); //relation to glFlush unknown
	}

void IdleFunction(void){
    glutPostRedisplay();
    }

void KeyboardFunction(unsigned char Key, int X, int Y){
    switch (Key) {
        case 'T':
        case 't': {
            ActiveIndexBuffer = (ActiveIndexBuffer == 1? 0: 1);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ActiveIndexBuffer]);
            break;
            }
        default:
            break;
        }
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
    glutKeyboardFunc(KeyboardFunction);

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


void CreateVBO(void) {
    printf("CreateVBO called\n");
//    GLfloat Vertices[] = {
//        -0.8f, 0.8f, 0.0f, 1.0f,
//        0.8f, 0.8f, 0.0f, 1.0f,
//        -0.8f, -0.8f, 0.0f, 1.0f,
//        0.8f, -0.8f, 0.0f, 1.0f
//        };
//
//    GLfloat Colors[] = {
//        1.0, 0.0, 0.0, 1.0,
//        0.0, 1.0, 0.0, 1.0,
//        0.0, 0.0, 1.0, 1.0,
//        1.0, 1.0, 1.0, 1.0
//        };
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

    GLubyte AlternateIndices[] = {
  // Outer square border:
        3, 4, 16,
        3, 15, 16,
        15, 16, 8,
        15, 7, 8,
        7, 8, 12,
        7, 11, 12,
        11, 12, 4,
        11, 3, 4,
  // Inner square
        0, 11, 3,
        0, 3, 15,
        0, 15, 7,
        0, 7, 11
    };


    GLenum ErrorCheckValue = glGetError();

    const size_t BufferSize = sizeof(Vertices);
    const size_t VertexSize = sizeof(Vertices[0]);
    const size_t RgbOffset = sizeof(Vertices[0].XYZW);



    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, BufferId);
    glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
    glEnableVertexAttribArray(0);    
    glEnableVertexAttribArray(1);
    
//    glGenBuffers(1, &IndexBufferId);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
    glGenBuffers(2, IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(AlternateIndices), AlternateIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[0]);

//    glGenBuffers(1, &ColorBufferId);
//    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
//    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(1);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

void DestroyVBO(void) {

    GLenum ErrorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &BufferId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    glDeleteBuffers(1, &IndexBufferId);
    glDeleteBuffers(2, IndexBufferId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }
