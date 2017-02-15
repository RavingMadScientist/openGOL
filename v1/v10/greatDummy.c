#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<math.h>


#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<GL/glu.h>
#include<GL/gl.h>

#define WIN_PREFIX "openGOL v1.0"
#define numObjTypes 2


/* This script is written as a skeleton designed to maximize
 the ease and generality with which a graphics idea can be implemented in OpenGL
The user is free to define any number of ObjectTypes,
and the pointer-handling is as close to automatic as possible


*/
int currentWidth = 800;
int currentHeight = 800;
int winHandle = 0;
unsigned FrameCount = 0;
int gWidth = 11;
int gHeight = 11;
float step = 0.1;


int autoMode = 0; 
int thisGen = 0;
int userStopped = 0; //p pauses evolve


int numObjs[numObjTypes] = {1, 2};
char* objTypes[numObjTypes] = {"Square","Cube"};

GLuint OVertexShaderId, OFragmentShaderId, OProgramId;// organism rendering program memory pointers
GLuint TVertexShaderId, TFragmentShaderId, TProgramId; // text rendering program memory pointers


GLfloat* cPointer[numObjTypes];
GLfloat* cColPointer[numObjTypes];
GLfloat* cTexPointer[numObjTypes];
GLfloat* oCenters[numObjTypes];
GLfloat* rPointer[numObjTypes];
GLuint shaderProgramIds[numObjTypes];

int numVertices[numObjTypes] = {4, 8};

GLuint VaoId[numObjTypes]; 
GLuint VboId[numObjTypes][3];

GLuint offsetLocation; //pointer to navigation vec4
GLuint scaleLocation;
GLfloat scaleX, scaleY, scaleZ;
GLfloat scaleMag = 1.0;
GLfloat camOffset[4]={0.0, 0.0, 0.0, 0.0 };
GLfloat camScale[4] = {1.0, 1.0, 1.0, 1.0};

GLuint txtOffsetLocation;
GLuint txtScaleLocation;
GLuint texLocation;

int mouseX;
int mouseY;

void Cleanup(void);
void DestroyShaders(void);
void DestroyVBO(void);
void evolveCube(void);

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
    glUniform4f(offsetLocation, camOffset[0], camOffset[1], camOffset[2], camOffset[3]);

    camScale[0] = scaleMag;
    camScale[1] = scaleMag;
    camScale[2] = scaleMag;
    glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);


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

    shaderProgramIds[0] = OProgramId;
    shaderProgramIds[1] = OProgramId;




    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

int initSquare(int oI, int oP, int memIndex){
    printf("initSquare initiated:%d,%d,%d\n", oI, oP, memIndex);
    int i, j, k, o;
    float R;

    k=memIndex/2;
    
    for (i=0; i<4; i++){
        j=memIndex+4*i;
        cColPointer[oI][j]=0.25;
        cColPointer[oI][j+1]=0.88;
        cColPointer[oI][j+2]=0.96;
        cColPointer[oI][j+3]=1.0;        
        }
    printf("cColPointer assigned\n");

    float xv, yv, zv;
    R = rPointer[oI][oP] * 0.5;
        printf("radius[%d][%d]=%f\n", oI, oP, rPointer[oI][oP]);

    for(i=0; i<(3*(oP+1)); i++){
        printf("oCenters[%d][%d]=%f\n", oI, i, oCenters[oI][i]);
        }
    xv = oCenters[oI][3*oP];
    yv=oCenters[oI][3*oP+1];
    zv=oCenters[oI][3*oP+2];
    int mi = memIndex;
    float lv;
    float sq[4][4] = {{xv-R, yv-R, zv, 1.0},{xv-R, yv+R, zv, 1.0},{xv+R, yv+R, zv, 1.0},{xv+R, yv-R, zv, 1.0}};
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
//            printf("sq[%d][%d]=cPointer[%d][%d]=%f\n", i, j, oI, mi, lv);
//            printf("prior value was %f\n", cPointer[oI][mi]);
//            printf("cColPointer = %f\n", cColPointer[oI][mi]);
            cPointer[oI][mi]=lv;
            mi++;
            }
        }
    return mi;

    }


int initCube(int oI, int oP, int memIndex){
    printf("initCube initiated:%d,%d,%d\n", oI, oP, memIndex);
    int i, j, k, o;
    float R;

    k=memIndex/2;
    
    for (i=0; i<numVertices[oI]; i++){
        j=memIndex+4*i;
        cColPointer[oI][j]=1.0/((float)i+1.0);
        cColPointer[oI][j+1]= ( (float) abs(i-numVertices[oI]/2) ) * 2.0 / (float) numVertices[oI];
        cColPointer[oI][j+2]=(float) i / (float) numVertices[oI];
        cColPointer[oI][j+3]=1.0;        
        }
    printf("cColPointer assigned\n");

    float xv, yv, zv;
    R = rPointer[oI][oP] * 0.5;
        printf("radius[%d][%d]=%f\n", oI, oP, rPointer[oI][oP]);

    for(i=0; i<(3*(oP+1)); i++){
        printf("oCenters[%d][%d]=%f\n", oI, i, oCenters[oI][i]);
        }
    xv = oCenters[oI][3*oP];
    yv=oCenters[oI][3*oP+1];
    zv=oCenters[oI][3*oP+2];
    int mi = memIndex;
    float lv;
//    float sq[8][4] = {{xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
//        {xv-R, yv-R, zv+R, 1.0},{xv-R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0}};
    float sq[8][4] = {{xv+R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv+R, yv+0.5*R, zv-R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
        {xv-2*R, yv-R, zv+R, 1.0},{xv-R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0}};

    for (i=0; i<8; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
//            printf("sq[%d][%d]=cPointer[%d][%d]=%f\n", i, j, oI, mi, lv);
//            printf("prior value was %f\n", cPointer[oI][mi]);
//            printf("cColPointer = %f\n", cColPointer[oI][mi]);
            cPointer[oI][mi]=lv;
            mi++;
            }
        }
    return mi;

    }

int initOrg(char* orgType, int oTypeIndex, int oPosIndex, int memIndex){
    printf("initOrg creating ");
    int memIn = memIndex;    
    int memOut;
    if (strcmp(orgType, "Square") == 0){
        memOut = initSquare(oTypeIndex, oPosIndex, memIndex);
        }
    if (strcmp(orgType, "Cube") == 0){
        memOut = initCube(oTypeIndex, oPosIndex, memIndex);
        }
    else{
        printf("Object not recognized: %s\n", orgType);
    }    

    return memOut;
    }

CreateVBO(){
//Part I. laying down the rotatable object...
    printf("CreateVBO() Initiated\n");
    int loc;
    int c, d;
    int nOb;
    char* oType;
    for (c=0; c<numObjTypes; c++){
        loc=0;
        nOb = numObjs[c];
        //printf("init:objTypes[%d]\n", c);
        for(d=0; d<nOb; d++){
            //printf("obj[%d][%d] preparing init\n", c, d);
            oType = objTypes[c];
            loc = initOrg(oType, c, d, loc);
            //printf("initOrg returned obj[%d][%d]->%d\n", c, d, loc);
            }        
        }

//Part II. VBO construction and assignment logic.
// Instantiating our two VBOs...
    GLenum ErrorCheckValue = glGetError();
    glGenVertexArrays(numObjTypes, VaoId);
    int q;
    for (q=0; q<numObjTypes; q++){
        int dummyLength = 4*numVertices[q] * numObjs[q];
        float dummy[dummyLength];

        glBindVertexArray(VaoId[q]);
        glGenBuffers(3, VboId[q]); // location, color, texture
        glBindBuffer(GL_ARRAY_BUFFER, VboId[q][0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(dummy), cPointer[q], GL_STREAM_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VboId[q][1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(dummy), cColPointer[q], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VboId[q][2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(dummy)/2, cTexPointer[q], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);

        }
    glBindVertexArray(VaoId[0]);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }   
    }

int drawCube(int oTypeIndex, int oPosIndex, int memIndex){

    int drawStart = numVertices[oTypeIndex]*oPosIndex;
    int baseIndex = 4*numVertices[oTypeIndex];
    GLfloat dummyBack[baseIndex];

    int punchIn = sizeof(dummyBack);
    punchIn *= oPosIndex;
    glBindVertexArray(VaoId[oTypeIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
    glDrawArrays(GL_TRIANGLE_FAN, drawStart, numVertices[oTypeIndex]/2);
    glDrawArrays(GL_TRIANGLE_FAN, drawStart+ numVertices[oTypeIndex]/2, numVertices[oTypeIndex]/2);
    }

int drawSquare(int oTypeIndex, int oPosIndex, int memIndex){

    int start = 4*oPosIndex;
    int baseIndex = 16;
    int first = baseIndex*oPosIndex;
    GLfloat dummyBack[baseIndex];

    int punchIn = sizeof(dummyBack);
    punchIn *= oPosIndex;
    glBindVertexArray(VaoId[oTypeIndex]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);
    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
    int i;
//    for(i=0; i<baseIndex; i++){
//        printf("drawCube[%d,%d]V%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][1]);
    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
//    for(i=0; i<baseIndex; i++){
//        printf("drawCube[%d,%d]C%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);

    glDrawArrays(GL_TRIANGLE_FAN, start, numVertices[oTypeIndex]);

//    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

//    printf("drawCube[%d,%d] returns\n", oTypeIndex, oPosIndex);
    }



int drawOrg(char* orgType, int oTypeIndex, int oPosIndex, int memIndex){
    glUseProgram(shaderProgramIds[oTypeIndex]);
    int memIn = memIndex;    
    int memOut;
    if (strcmp(orgType, "Square") == 0){
//        printf("drawOrg sees Cube[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = drawSquare(oTypeIndex, oPosIndex, memIndex);
        }
    else if (strcmp(orgType, "Cube") == 0){
//        printf("drawOrg sees Cube[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = drawCube(oTypeIndex, oPosIndex, memIndex);
        }
    else{
        printf("draw-Object not recognized: %s\n", orgType);
    }    

    return memOut;
    }

void display(){
    ++FrameCount;
    //printf("display-%d\n", FrameCount);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
//    printf("display %d\n", FrameCount);
    int i, j, z, loc;
    for (i=0; i<numObjTypes; i++){
        glUseProgram(shaderProgramIds[i]); //cam shaders,
        glBindVertexArray(VaoId[i]);
        z = numObjs[i];
        for (j = 0; j<z; j++){
//            printf("display[%d,%d]\n", i, j);
            loc=0;
            drawOrg(objTypes[i], i, j, loc);    
            } 
        }

    
    glutSwapBuffers();
    if((FrameCount == 1)&&(thisGen==0)){
        printf("fc1\n");
        glutPostRedisplay();
        }
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
         sprintf(TempString, "%s: %d fps@ %d x %d .MouseAt %d,%d",WIN_PREFIX,FrameCount * 4, currentWidth, currentHeight, mouseX, mouseY);        
           glutSetWindowTitle(TempString);
        free(TempString);
        }
    FrameCount = 0;
    if (Value == 99){
        evolveCube();
        }
    else{
        glutTimerFunc(250, TimerFunction, 1);
        }
    }

void evolveCube(){

    thisGen++;
    glutPostRedisplay();  

    if ((autoMode > 0) && thisGen < autoMode){
        //evolveGame(); // run it through TimerFunc so the user can watch the evolution
        glutTimerFunc(250, TimerFunction, 99); //99 is the magic code
        }

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

    switch (Key) {

        case 'f':{ 
            evolveCube();   
            break;
            }
        case '+':{ 
            printf("+\n");   
            scaleMag*=(6.0/5.0);
            camScale[0] = scaleMag;
            camScale[1] = scaleMag;
            camScale[2] = scaleMag;
            glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);
            glutPostRedisplay();
            break;
            }
        case '-':{ 
            printf("-\n");   
            scaleMag*=(5.0/6.0);
            camScale[0] = scaleMag;
            camScale[1] = scaleMag;
            camScale[2] = scaleMag;
            glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);
            glutPostRedisplay();
            break;
            }
        default:{
            break;}
        }
    }

void specialKeyInput(int key, int x, int y){
    glUseProgram(OProgramId);
    if(key == GLUT_KEY_UP){
        camOffset[1]+=step;
        }    
    if(key == GLUT_KEY_DOWN){
        camOffset[1]-=step;
        }  
    if(key == GLUT_KEY_LEFT){
        camOffset[0]-=step;
        }  
    if(key == GLUT_KEY_RIGHT){
        camOffset[0]+=step;
        }  
    glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[3], 0.0);  

    glutPostRedisplay();

}

void main(int argc, char**argv) {
    int i;
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



//    glFrontFace​(GL_CCW​);
    CreateShaders();
//allocate array space for state pointers

//square routine
    int q = 0;

    int numoFloats = numVertices[q] * numObjs[q] * 4;
    GLfloat verts[numoFloats]; 
    for(i=0; i<numoFloats; i++){
        verts[i] = (i+1)%4 == 0 ? 1.0: 0.0;
        }
    cPointer[q] = verts;

    GLfloat colors[numoFloats]; 
    cColPointer[q] = colors;
    
    GLfloat txts[numoFloats/2]; 
    cTexPointer[q] = txts;

    int numCCoords = numObjs[q] * 3;
    GLfloat cents[numCCoords]; 
    for(i=0; i<numCCoords; i++){
        cents[i] = (i+1)%4 == 0 ? 1.0: 0.0;
        }
    oCenters[q] = cents;        
//    for(i=0; i<3; i++){
//        printf("oCenters[%d][%d]=%f\n", q, i, oCenters[q][i]);
//        }
    GLfloat cRads[numObjs[q]];
    cRads[0] = 0.6;
    rPointer[q] = cRads;    
//Cube routine
    q=1;
    numoFloats = numVertices[q] * numObjs[q] * 4;
    GLfloat vertc[numoFloats]; 
    cPointer[q] = vertc;

    GLfloat colorc[numoFloats]; 
    cColPointer[q] = colorc;
    
    GLfloat txtc[numoFloats/2]; 
    cTexPointer[q] = txtc;

    numCCoords = numObjs[q] * 3;
    GLfloat centc[numCCoords]; 
    for(i=0; i<numObjs[q]; i++){
//        centc[i] = (i+1)%4 == 0 ? 1.0: 0.0; // just initialization, initCube routine handles assignment
        centc[3*i] = 0.4*(float)i;
        centc[3*i+1] = 0.1;
        centc[3*i+2] = 0.0;
        }
    oCenters[q] = centc;        
//    for(i=0; i<3; i++){
//        printf("oCenters[%d][%d]=%f\n", q, i, oCenters[q][i]);
//        }
    GLfloat cRadc[numObjs[q]];
    cRadc[0] = 0.2;
    cRadc[1] = 0.4;
    rPointer[q] = cRadc;    
//end Cube loop


    CreateVBO();
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

    int i;
    for(i=numObjTypes-1; i>=0; i--){

        glBindVertexArray(VaoId[i]);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(3, VboId[i]);
    }

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

