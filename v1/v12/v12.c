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

#define WIN_PREFIX "openGOL v1.1"
#define numObjTypes 5
#define numCharTxts 59
#define msgLength 26
#define msgRows 4
#define numDims 4


/* compile w gcc v12.c funcs1d.c funcs2d.c textLib.c -lm -lGLEW -lglut -lGLU -lGL -o v12


Converts the default organism from v1.1=Square to
v1.2=Cube

Grid is 3dified
*/
int currentWidth = 800;
int currentHeight = 800;
int winHandle = 0;
unsigned FrameCount = 0;
int gWidth = 11;
int gHeight = 11;
float step = 0.1;

double pi = 3.1415926535897932384626;
float stepTheta;// = pi/8.0;

char* loadFile;
int periodicMode = 0;
char* oType = "Cube";
int masterObj = 3;
char* objTypes[numObjTypes] = {"Grid", "Text", "Square","Cube", "Pacman"};

int evolveable[numObjTypes] = {0,0,0,1,0};

int autoMode = 0; 
int thisGen = 0;
int userStopped = 0; //p pauses evolve


//(#define numDims 4 already called)
int numObjs[numObjTypes] = {22, 26, 2, 2, 0};
int numVertices[numObjTypes] = {12, 4, 4, 24, 21};
float thresholds[numObjTypes] = {0.05, 0.05, 0.05, 0.05, 0.05};

GLuint OVertexShaderId, OFragmentShaderId, OProgramId;// organism rendering program memory pointers
GLuint TVertexShaderId, TFragmentShaderId, TProgramId; // text rendering program memory pointers

/*This is an array of pointers to floats. 
Each pointer must have its space allocated separately 

*/
GLfloat* cPointer[numObjTypes];
GLfloat* cColPointer[numObjTypes];
GLfloat* cTexPointer[numObjTypes];
GLfloat* oCenters[numObjTypes];
GLfloat* rPointer[numObjTypes];
GLuint shaderProgramIds[numObjTypes];

//blank grid
GLfloat** bgPointer;
int* updates;


// now spec the vertices for grid lines.
GLfloat* gVPointer; // These may be redundant with v1.0
GLfloat* gCPointer; // introduction of multidimensional
GLfloat* gTPointer;// VAO/VBO pointer arrays


//text parameters
float monoW = 8.0; //font character width (px)
float monoAR = 1.6;// font aspect ratio (h/l)
float xkern=-1.0; //font character kerning (px)
float ykern= 0.0; //font character kerning (px)
float lAlign=20.0; //leftAlign (px)
float topAlign=20.0; //(px)

GLfloat tsq[4][4] ;
int numTCoords = msgLength * msgRows * 4 * numDims;
// now spec the vertices for text squares.
GLfloat* tVPointer; // These may be redundant with v1.0
GLfloat* tCPointer; // introduction of multidimensional
GLfloat* tTPointer;// VAO/VBO pointer arrays

unsigned char localTexture[numCharTxts][8][8][4];
GLuint tex[numCharTxts];
GLuint txtOffsetLocation;
GLuint txtScaleLocation;
GLuint texLocation;

char printmsg[msgRows][msgLength];
char msgPrefix[msgRows][msgLength] = {"Generation:", "Orgs:", "Live Pct:", "abcdefghjklmostuvwxyz" };
float msgData[msgRows] = {1.0, 2.0, 3.1, 4.6};
char msgSuffix[msgRows][msgLength] = {" ", " ", "%", " " };


GLuint VaoId[numObjTypes+2]; // plus two since grid and text handled separately 
GLuint VtoId[1]; // Text objects ie anything with TProgramId
GLuint VboId[numObjTypes+2][3];
GLuint TextVboId[1][3];

GLuint offsetLocation; //pointer to navigation vec4
GLuint scaleLocation;
GLuint frustumLocation;
GLuint localFrustumLocation;
GLuint localVecLocation;
GLuint rotationXLocation;
GLuint rotationYLocation;
GLuint rotationZLocation;

GLuint rotationCamXLocation;
GLuint rotationCamYLocation;
GLuint rotationCamZLocation;

GLfloat scaleX, scaleY, scaleZ;
GLfloat scaleMag = 1.0;

GLfloat camOffset[4]={0.0, 0.0, 0.0, 0.0 };
GLfloat camScale[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat nullFrustum[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat thetaCentral[3] = {0.0, 0.0, 0.0};
GLfloat thetaLocal[3] = {0.0, 0.0, 0.0};

GLfloat rXMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
GLfloat rYMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
GLfloat rZMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

GLfloat rCamXMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
GLfloat rCamYMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
GLfloat rCamZMatPointer[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

GLuint txtOffsetLocation;
GLuint txtScaleLocation;
GLuint texLocation;

int mouseX;
int mouseY;

void RainbowMe(int, int);
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
    "uniform vec4 frustum;\n"\
    "uniform mat4 rotationX;\n"\
    "uniform mat4 rotationY;\n"\
    "uniform mat4 rotationZ;\n"\
    "uniform mat4 rotationCamX;\n"\
    "uniform mat4 rotationCamY;\n"\
    "uniform mat4 rotationCamZ;\n"\
    "vec4 localFrustum;\n"\
    "vec4 localVec;\n"\
    "out vec4 ex_Color;\n"\

    "void main(void)\n"\
    "{\n"\


    "   localVec = rotationCamX*rotationCamY*rotationCamZ*(rotationX*rotationY*rotationZ*(in_Position) + offset);\n"\
    "   localVec = localVec * scale;\n"\
    "   localFrustum.x = 1.2 - 0.6*localVec.z;\n"\
    "   localFrustum.y = 1.2 - 0.6*localVec.z;\n"\
    "   localFrustum.z = 1.0;\n"\
    "   localFrustum.w = 1.0;\n"\
    "   localVec = localVec * localFrustum;\n"\
    "   gl_Position = localVec;\n"\
    "   //gl_Position = (in_Position + offset) * scale;\n"\
    "   //gl_Position = in_Position+offset;\n"\
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

void loadProcTexture(){
    char font[] = "basic";
    setupTextLib(font);// textLib.c library function
    int i, j, k, row;
    for (i=0; i<numCharTxts; i++){
        getCharTexture(i, localTexture[i]);
        }
    float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glGenTextures(numCharTxts, tex);
    glActiveTexture(GL_TEXTURE0);
    for (i=0; i<numCharTxts; i++){
        j=i%3;
        color[0] = j==2? 0.0 : 1.0;
        color[1] = j==1? 0.0 : 1.0;
        color[2] = j==0? 1.0 : 0.0;

        glBindTexture(GL_TEXTURE_2D, tex[i]);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, localTexture[i]);
//        glGenerateMipmap(GL_TEXTURE_2D);
        }

    char localmsg[msgLength];
    int len; 
    for (row=0; row<msgRows; row++){
        float dataVal = msgData[row];
        if (row != 2){ // only the live pct is displayed as a float
            int dataChop = (int) dataVal;
//        sprintf(localmsg, "%s %d", msgPrefix[row], row);
            sprintf(localmsg, "%s %d", msgPrefix[row], dataChop);
            }
        else{
             sprintf(localmsg, "%s %.2f", msgPrefix[row], dataVal);           
            }
        sprintf(localmsg, "%s -%s", localmsg, msgSuffix[row]);
        len = strlen(localmsg);
        printf("msg[%d]: %s\n", row, localmsg);
//        for (i=0; i<len; i++){
//            printmsg[row][i]=localmsg[i];
//            }
        }
    }


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
    stepTheta = pi/24.0;
    offsetLocation = glGetUniformLocation(OProgramId, "offset");
    scaleLocation = glGetUniformLocation(OProgramId, "scale");
    frustumLocation = glGetUniformLocation(OProgramId, "frustum");
    localFrustumLocation = glGetUniformLocation(OProgramId, "localFrustum");
    localVecLocation = glGetUniformLocation(OProgramId, "localVec");

    rotationXLocation = glGetUniformLocation(OProgramId, "rotationX");
    rotationYLocation = glGetUniformLocation(OProgramId, "rotationY");
    rotationZLocation = glGetUniformLocation(OProgramId, "rotationZ");

    rotationCamXLocation = glGetUniformLocation(OProgramId, "rotationCamX");
    rotationCamYLocation = glGetUniformLocation(OProgramId, "rotationCamY");
    rotationCamZLocation = glGetUniformLocation(OProgramId, "rotationCamZ");

    //should be global (is)
    glUniform4f(offsetLocation, camOffset[0], camOffset[1], camOffset[2], camOffset[3]);

//    camScale[0] = scaleMag;
//    camScale[1] = scaleMag;
//    camScale[2] = scaleMag;
//defaults to 1.0,1.0,1.0. Changed by user nav and grid size
    glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);
 
    glUniform4f(frustumLocation, nullFrustum[0], nullFrustum[1], nullFrustum[2], nullFrustum[3]);


    glUniformMatrix4fv(rotationXLocation, 1, GL_FALSE, rXMatPointer);
    glUniformMatrix4fv(rotationYLocation, 1, GL_FALSE, rYMatPointer);
    glUniformMatrix4fv(rotationZLocation, 1, GL_FALSE, rZMatPointer);

    glUniformMatrix4fv(rotationCamXLocation, 1, GL_FALSE, rCamXMatPointer);
    glUniformMatrix4fv(rotationCamYLocation, 1, GL_FALSE, rCamYMatPointer);
    glUniformMatrix4fv(rotationCamZLocation, 1, GL_FALSE, rCamZMatPointer);


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
    shaderProgramIds[1] = TProgramId;
    shaderProgramIds[2] = OProgramId;
    shaderProgramIds[3] = OProgramId;
    shaderProgramIds[4] = OProgramId;

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }    
    }

int initSquare(int oI, int oP, int memIndex){
//    printf("initSquare initiated:%d,%d,%d\n", oI, oP, memIndex);
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
//    printf("cColPointer assigned\n");

    float xv, yv, zv;
//    printf("rPointer @%d\n",rPointer);
//    printf("%rPointer[%d] @%d\n",oI, rPointer[oI]);
//    printf("[%d][%d]: \n",oI, oP);
//    printf("%f\n", rPointer[oI][oP]);
    R = rPointer[oI][oP] * 0.5;
//    printf("radius[%d][%d]=%f\n", oI, oP, rPointer[oI][oP]);

//    for(i=0; i<(3*(oP+1)); i++){
//        printf("oCenters[%d][%d]=%f\n", oI, i, oCenters[oI][i]);
//        }
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
    int i, j, k, o, nv;
    float R;
    nv = numVertices[oI];
    k=memIndex/2;
    
    for (i=0; i<nv; i++){
        j=memIndex+4*i;
        cColPointer[oI][j]=0.5/((float)i+1.0);
        cColPointer[oI][j+1]= ( (float) abs(i-nv/2) ) * 2.0 / (float) nv;
        cColPointer[oI][j+2]=(float) i / (float) nv;
        cColPointer[oI][j+3]=1.0;        
        }
    printf("cColPointer assigned\n");

    float xv, yv, zv;
    R = rPointer[oI][oP] * 0.5;
        printf("radius[%d][%d]=%f\n", oI, oP, rPointer[oI][oP]);

//    for(i=0; i<(3*(oP+1)); i++){
//        printf("oCenters[%d][%d]=%f\n", oI, i, oCenters[oI][i]);
//        }
    xv = oCenters[oI][3*oP];
    yv=oCenters[oI][3*oP+1];
    zv=oCenters[oI][3*oP+2];
    int mi = memIndex;
    float lv;
//    float sq[8][4] = {{xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
//        {xv-R, yv-R, zv+R, 1.0},{xv-R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0}};

// front, back, right, left, bottom, top
    float sq[24][4] = {{xv-R, yv-R, zv+R, 1.0},{xv-R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},\
        {xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
        {xv+R, yv-R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},\
        {xv-R, yv+R, zv+R, 1.0},{xv-R, yv-R, zv+R, 1.0},{xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},\
        {xv-R, yv-R, zv-R, 1.0},{xv-R, yv-R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
        {xv+R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv-R, yv+R, zv+R, 1.0}};


    for (i=0; i<nv; i++){
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

int initPacman(int oI, int oP, int memIndex){



    }

//color scheme
//void RainbowMe(int memOut, int seed){ 
//    //printf("cirColor %d:%f | ",memOut,cColPointer[memOut]);
//    int i, j, q;
////    cColPointer[memOut] = 1.0;
//    cColPointer[memOut]=1.0;
//    for(i=0; i<3; i++){
//        q=i+seed+memOut;
//        j=i+memOut;
//        if((q%16)%5 == 0){
//            cColPointer[j] = 1.0;
//            }
//        else if(q%4 == 3){
//            cColPointer[j] = 1.0;
//            }
//        else if ( ((q/4)%4) == 3){
//            cColPointer[j] = 1.0;
//            }
//        else{
//            cColPointer[j] = 0.0;        
//            }
//        }
//    }

int initOrg(char* orgType, int oTypeIndex, int oPosIndex, int memIndex){

    glUseProgram(shaderProgramIds[oTypeIndex]);
//    printf("program %d\n", shaderProgramIds[oTypeIndex]);

    int memIn = memIndex;    
    int memOut = memIn;

//    printf("initOrg creating ");
    if (strcmp(orgType, "Square") == 0){
//        printf("Square\n");
        memOut = initSquare(oTypeIndex, oPosIndex, memIn);
        }
    else if (strcmp(orgType, "Cube") == 0){
//        printf("Cube\n");
        memOut = initCube(oTypeIndex, oPosIndex, memIn);

        }
    else{
        int abc = 0;
//        printf("Object not recognized by initOrg object automation: %s\n", orgType);
    }    

    return memOut;
    }

CreateVBO(){
//Part I. laying down the rotatable object...
    printf("CreateVBO() Initiated\n");
    int loc;
    int c, d, i, j, k;
    float xVal, yVal;
    float zVal = 0.0;
    int nObs;
    char* oType;
    for (c=0; c<numObjTypes; c++){
        loc=0;
        oType = objTypes[c];
        nObs = numObjs[c];
        //printf("init:objTypes[%d]\n", c);
        for(d=0; d<nObs; d++){
//            printf("obj[%d][%d] preparing init\n", c, d);
            loc = initOrg(oType, c, d, loc);
//            printf("initOrg returned obj[%d][%d]->%d\n", c, d, loc);
            }        
        }

// At present, we still do the gridlines manually after the for (obj in objTypes) loop
//    printf("calcing grid coord vertices\n");
    int numGCoords = gHeight * gWidth * numDims * 2;
    GLfloat gVertices[numGCoords];
    GLfloat gColors[numGCoords];
    float gwMax = ((float)gWidth-1.0)/2.0; 
    float ghMax = ((float)gHeight-1.0)/2.0;


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
//    printf("grid coord vertices complete\n");


//and text too, because the legacy function WORKS :)
// text squares
//    printf("calcing text coord vertices\n");
    float xv, yv, zv, lv, ifloat;
    int o, row;

    GLfloat txtVertices[numTCoords];
    GLfloat txtColors[numTCoords];
    GLfloat txtTextures[numTCoords/2];

    tVPointer=txtVertices;
    tCPointer=txtColors;
    tTPointer=txtTextures;
    for(row=0; row<msgRows; row++){
        for (i=0; i<msgLength; i++){
            //printf( "i=%d\n", i);
            //xv=lAlign+monoW;
            xv=(float)((2*monoW+xkern)*i)+ lAlign + monoW;
            //yv = topAlign-(monoW*monoAR*row);
            yv = (float)(topAlign+2*(monoAR*monoW+ykern)*row);//+ (float)(row*monoAR*monoW);
            zv = -0.1;
            //i=(int) i;
            //printf("txtSQ [%d][%d] ", row, i );
            //printf(" centered at(%f, %f, %f)\n", xv, yv, zv);  
            tsq[0][0]=xv-monoW;
            tsq[0][1]=yv-(monoW*monoAR);
            tsq[0][2]=zv;
            tsq[0][3]=1.0;
            tsq[1][0]=xv-monoW;
            tsq[1][1]=yv+(monoW*monoAR);
            tsq[1][2]=zv;
            tsq[1][3]=1.0;
            tsq[2][0]=xv+monoW;
            tsq[2][1]=yv+(monoW*monoAR);
            tsq[2][2]=zv;
            tsq[2][3]=1.0;
            tsq[3][0]=xv+monoW;
            tsq[3][1]=yv-(monoW*monoAR);
            tsq[3][2]=zv;
            tsq[3][3]=1.0;
            for (j=0; j<4; j++){
                for (k=0; k<4; k++){
                    o=16*msgLength*row+16*i+4*j+k;            
                    lv=tsq[j][k];
                    txtVertices[o]=lv;
                    //printf("txtVertices[%d]=%f\n", o, lv);
                    }
                }
            }
    //txt texture Coords
        for (i=0; i<msgLength; i++){
            k = 8*row*msgLength+8*i;
            txtTextures[k] = 0.0;
            txtTextures[k+1] = 1.0;
            txtTextures[k+2] = 0.0;
            txtTextures[k+3] = 0.0;
            txtTextures[k+4] = 1.0;
            txtTextures[k+5] = 0.0;
            txtTextures[k+6] = 1.0;
            txtTextures[k+7] = 1.0;
            for (j=0; j<4; j++){
                k=16*msgLength*row+16*i+4*j;
                txtColors[k]=1.0;
                txtColors[k+1]=1.0;
                txtColors[k+2]=1.0;
                txtColors[k+3]=1.0;
                }    
            }

        }
//        printf("text coord vertices complete\n");


//Part II. VBO construction and assignment logic.
// Instantiating our two VBOs...
    GLenum ErrorCheckValue = glGetError();
    glGenVertexArrays(numObjTypes, VaoId);
    int q;
    for (q=2; q<numObjTypes; q++){
//safety first!
        glUseProgram(shaderProgramIds[q]);
         // gridLines and Text are 0 and 1
        int dummyLength = 4*numVertices[q] * numObjs[q];
        float dummy[dummyLength];
        printf("binding VaoId[%d]\n",q);
        printf( "dummyLength=%d vertices\n", dummyLength);
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
        printf("VaoId[%d] Initialization complete\n",q);
        }
//gridLines
    glUseProgram(OProgramId);
    glBindVertexArray(VaoId[0]);
    glGenBuffers(3, VboId[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[0][0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertices), gVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[0][1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gColors), gColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
//no grid texture info yet

//Text
    glUseProgram(TProgramId);
    glBindVertexArray(VaoId[1]);
    glGenBuffers(3, VboId[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1][0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtVertices), txtVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1][1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtColors), txtColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VboId[1][2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(txtTextures), txtTextures, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);


    glUseProgram(OProgramId);
    glBindVertexArray(VaoId[0]);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR) {
        fprintf(stderr,"ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
        exit(-1);
        }   
    }


void drawTextsq(int row, int c){
    //printf("Drawing Square %d\n", c);

    //draw operation
    int baseIndex = 16;
    int start = (4*(row*msgLength+c));
    glDrawArrays(GL_TRIANGLE_FAN, start, 4);

    //data recovery operation
//    GLfloat dummyIn[baseIndex];
//    int punchIn = sizeof(dummyIn);
//    int arrayPosition = (row*msgLength + c);    
//    punchIn *= arrayPosition;
////    printf("row, pos, punchIn: %d, %d, %d\n", row, c, punchIn);
//    glBindVertexArray(VaoId[1] );
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[1][0]);
//
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyIn), dummyIn);
    }

int drawCube(int oTypeIndex, int oPosIndex, int memIndex){
    int nv = numVertices[oTypeIndex];
    int drawStart = nv*oPosIndex;
    int baseIndex = 4*nv;
    int i;
    //data recovery

//    GLfloat dummyBack[baseIndex];

//    int punchIn = sizeof(dummyBack);
//    punchIn *= oPosIndex;
//    glBindVertexArray(VaoId[oTypeIndex]);
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
//
//    int i;
//    for(i=0; i<baseIndex; i++){
//        printf("cPointer[%d,%d]=%f\n", oTypeIndex, memIndex+ i, cPointer[oTypeIndex][memIndex+i] );
//        printf("drawCube[%d,%d]V%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[2+oTypeIndex][1]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
//    for(i=0; i<baseIndex; i++){
//        printf("drawCube[%d,%d]C%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[2+oTypeIndex][0]);

    //draw operation
    for(i=0; i<6; i++)      {
        glDrawArrays(GL_TRIANGLE_FAN, drawStart+i*nv/6, nv/6);
        }

    int mi = memIndex+baseIndex;
    return mi;
    }

int drawSquare(int oTypeIndex, int oPosIndex, int memIndex){

    int start = 4*oPosIndex;
    int baseIndex = 16;

    //data recovery script
//    int first = baseIndex*oPosIndex;
//    GLfloat dummyBack[baseIndex];
//
//    int punchIn = sizeof(dummyBack);
//    punchIn *= oPosIndex;
//    glBindVertexArray(VaoId[oTypeIndex]);
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
//    int i;
//    for(i=0; i<baseIndex; i++){
//        printf("drawSquare[%d,%d]V%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][1]);
//    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
//    for(i=0; i<baseIndex; i++){
//        printf("drawSquare[%d,%d]C%d=%f\n", oTypeIndex, oPosIndex, i, dummyBack[i] );
//        }
//    glBindBuffer(GL_ARRAY_BUFFER, VboId[oTypeIndex][0]);

    glDrawArrays(GL_TRIANGLE_FAN, start, numVertices[oTypeIndex]);



//    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

//    printf("drawSquare[%d,%d] returns\n", oTypeIndex, oPosIndex);
    }



int drawOrg(char* orgType, int oTypeIndex, int oPosIndex, int memIndex){
    glUseProgram(shaderProgramIds[oTypeIndex]);
    int memIn = memIndex;    
    int memOut = memIn;
    if (strcmp(orgType, "Square") == 0){
        printf("drawOrg sees Square[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = drawSquare(oTypeIndex, oPosIndex, memIndex);
        }
    else if (strcmp(orgType, "Cube") == 0){
        printf("drawOrg sees Cube[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = drawCube(oTypeIndex, oPosIndex, memIndex);
        }
    else{
        printf("draw-Object not recognized: %s\n", orgType);
    }    

    return memOut;
    }



void display(){
    ++FrameCount;
    printf("display fc=%d\n", FrameCount);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
//    printf("display %d\n", FrameCount);
    int i, j, z, loc;
    for (i=0; i<numObjTypes; i++){
        glUseProgram(shaderProgramIds[i]); //cam shaders,
        glBindVertexArray(VaoId[i]);

        loc=0;
        z = numObjs[i];
        printf("numObjs[%d]=%d\n", i, z);
        for (j = 0; j<z; j++){

//            if(rPointer[i][j]>=thresholds[i]){
//                printf("display[%d,%d]\n", i, j);
//                drawOrg(objTypes[i], i, j, loc);
//                }    
            if(i==masterObj){
                printf("display masterObj[%d,%d]\n", i, j);
                loc = drawOrg(objTypes[i], i, j, loc);
            }
            else if(i==2){
                printf("display[%d,%d]\n", i, j);
                loc = drawOrg(objTypes[i], i, j, loc);
                }    
            } 
        }

////bind gridlines VAO
    glUseProgram(OProgramId);
    glBindVertexArray(VaoId[0]);
//draw vertical gridlines
    int g;
    int baseIndex;
    for(g=0; g<gWidth; g++){
        baseIndex = gHeight * g;
        glDrawArrays(GL_LINE_STRIP, baseIndex, gHeight);
        }
//draw horizontal gridlines
    for(g=0; g<gHeight; g++){
        baseIndex = gWidth * g;
        glDrawArrays(GL_LINE_STRIP, gHeight*gWidth+baseIndex, gWidth);
        }

//draw Text
    glUseProgram(TProgramId);
    glBindVertexArray(VaoId[1]);
    int q, r, row, len;
    char localmsg[msgLength];
    char thisChar;
//at present, we are going to recalculate the message for every display iteration, 
//but for optimization this function will probably be offloaded
    for (row=0; row<msgRows; row++){
        float dataVal = msgData[row];
        if (row != 2){ // only the live pct is displayed as a float
            int dataChop = (int) dataVal;
            sprintf(localmsg, "%s %d", msgPrefix[row], dataChop);
            }
        else{
             sprintf(localmsg, "%s %.2f", msgPrefix[row], dataVal);           
            }
        sprintf(localmsg, "%s%s", localmsg, msgSuffix[row]);
        len = strlen(localmsg);
        printf("msg[%d]: %s\n", row, localmsg);
        for (i=0; i<len; i++){
            printmsg[row][i]=localmsg[i];
            }
        }
    if (FrameCount == 1){
    printf("display reads printMsg=%s\n%s\n%s\n%s\n", printmsg[0], printmsg[1], printmsg[2], printmsg[3]);
        }
    for(row=0; row<msgRows; row++){
        for (q=0; q<msgLength; q++){
            //j=q%numCharTxts;
            thisChar=printmsg[row][q];
            r=(int) thisChar;
            r=toupper(r)-32; // basic(font)-specific indexing
            r= r<0? 0 : r; 
            //printf("%d.%d: %c->%d->", row, q, thisChar, r);
            r=r%numCharTxts;
            //printf("%d\n", r);
            
            glBindTexture(GL_TEXTURE_2D, tex[r]);
            drawTextsq(row, q);
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
    glUseProgram(OProgramId); //(default) 
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
        evolveGame();
        }
    else{
        glutTimerFunc(250, TimerFunction, 1);
        }
    }

int refreshSquare(int oTypeIndex, int oPosIndex, int memIndex){
    //printf("updating square %d\n", op);
    int ot = oTypeIndex;
    int op = oPosIndex;
    int mi = memIndex;
    int baseIndex = numVertices[ot] * numDims;    
    int first = baseIndex * op;
    
    float lv, xv, yv, zv;
    float R = rPointer[ot][op] * 0.5;
    xv = oCenters[ot][3*op];
    yv=oCenters[ot][3*op+1];
    zv=oCenters[ot][3*op+2];
    float sq[4][4] = {{xv-R, yv-R, zv, 1.0},{xv-R, yv+R, zv, 1.0},{xv+R, yv+R, zv, 1.0},{xv+R, yv-R, zv, 1.0}};
    int pos = first;
    int i, j;
    for (i=0; i<4; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
            cPointer[ot][pos]=lv;
            pos++;
            }
        }
    GLfloat dummyOut[baseIndex];
    GLfloat dummyBack[baseIndex];
    for (i=0; i<baseIndex; i++){
        dummyOut[i] = cPointer[ot][first+i];    
        }
    int punchIn = sizeof(dummyOut) * op;
    glBindVertexArray(VaoId[ot]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[ot][0]);
    glBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyOut), dummyOut);

    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return pos;
    }

int refreshCube(int oTypeIndex, int oPosIndex, int memIndex){
    //printf("updating Cube %d\n", op);
    int ot = oTypeIndex;
    int op = oPosIndex;
    int mi = memIndex;
    int nv = numVertices[ot];
    int baseIndex = nv * numDims;    
    int first = baseIndex * op;
    
    float lv, xv, yv, zv;
    float R = rPointer[ot][op] * 0.5;
    
    xv = oCenters[ot][3*op];
    yv=oCenters[ot][3*op+1];
    zv=oCenters[ot][3*op+2];

// front, back, right, left, bottom, top
    float sq[24][4] = {{xv-R, yv-R, zv+R, 1.0},{xv-R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},\
        {xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
        {xv+R, yv-R, zv-R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv+R, yv+R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},\
        {xv-R, yv+R, zv+R, 1.0},{xv-R, yv-R, zv+R, 1.0},{xv-R, yv-R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},\
        {xv-R, yv-R, zv-R, 1.0},{xv-R, yv-R, zv+R, 1.0},{xv+R, yv-R, zv+R, 1.0},{xv+R, yv-R, zv-R, 1.0},\
        {xv+R, yv+R, zv+R, 1.0},{xv+R, yv+R, zv-R, 1.0},{xv-R, yv+R, zv-R, 1.0},{xv-R, yv+R, zv+R, 1.0}};


    int pos = first;
    int i, j;
    for (i=0; i<nv; i++){
        for (j=0; j<4; j++){
            lv=sq[i][j];
            cPointer[ot][pos]=lv;
            pos++;
            }
        }
    GLfloat dummyOut[baseIndex];
    GLfloat dummyBack[baseIndex];
    for (i=0; i<baseIndex; i++){
        dummyOut[i] = cPointer[ot][first+i];    
        }
    int punchIn = sizeof(dummyOut) * op;
    glBindVertexArray(VaoId[ot]);
    glBindBuffer(GL_ARRAY_BUFFER, VboId[ot][0]);
    glBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyOut), dummyOut);

    glGetBufferSubData(GL_ARRAY_BUFFER, punchIn, sizeof(dummyBack), dummyBack);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return pos;
    }


int refreshOrg(char* orgType, int oTypeIndex, int oPosIndex, int memIndex){
    glUseProgram(shaderProgramIds[oTypeIndex]);
    int memIn = memIndex;    
    int memOut;
    if (strcmp(orgType, "Square") == 0){
//        printf("drawOrg sees Cube[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = refreshSquare(oTypeIndex, oPosIndex, memIndex);
        }
    else if (strcmp(orgType, "Cube") == 0){
//        printf("drawOrg sees Cube[%d,%d]@%d\n", oTypeIndex, oPosIndex, memIndex);
        memOut = refreshCube(oTypeIndex, oPosIndex, memIndex);
        }
    else{
        printf("draw-Object not recognized: %s\n", orgType);
    }    

    return memOut;
    }

void evolveGame(){

    int i, j, k;
    printf("evolve[\n");
    for(i=0; i<numObjTypes; i++){
        printf("%d->%d", i, evolveable[i]);
        if (evolveable[i] > 0){
            int nO = numObjs[i];
            int updates[nO];
            for (j=0; j<nO; j++){
                updates[j]=0;
                }
            evolveGame2D(rPointer[i], bgPointer, updates, gHeight, gWidth, periodicMode, msgData);  
    //            for (i=0; i<numOrgs; i++){
    //                printf("evolved state[%d]=%f\n", i, rPointer[i]);
    //                }
            int loc = 0;
            for (j=0; j<nO; j++){
                if (updates[j] == 1){
      //          printf("update: r[%d]=%f\n", i, rPointer[i]);
                    loc = refreshOrg(objTypes[i], i, j, loc);
                    }
                }
            }
        }

    thisGen++;
    glutPostRedisplay();  

    if ((autoMode > 0) && thisGen < autoMode){
        // for human-viewable output, we throttle the program
        //in automode, by running it through timer delay
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
    printf("SupFOOOOOOO\n");
    printf("%d\n\n",Key);
    printf("%d\n\n",Key);
    printf("%d\n\n",Key);
    printf("%d\n\n",Key);
    switch (Key) {

        case 'f':{ 
            evolveGame();   
            break;
            }
        case '+':{ 
            printf("+\n");   
            //scaleMag*=(6.0/5.0);
            camScale[0] *=(6.0/5.0);
            camScale[1] *=(6.0/5.0);
            camScale[2] *=(6.0/5.0);
            glUseProgram(OProgramId);
            glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);
            glutPostRedisplay();
            break;
            }
        case '-':{ 
            printf("-\n");   
            //scaleMag*=(5.0/6.0);
            camScale[0] *=(5.0/6.0);
            camScale[1] *=(5.0/6.0);
            camScale[2] *=(5.0/6.0);
            glUseProgram(OProgramId);
            glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], camScale[3]);
            glutPostRedisplay();
            break;
            }

        case 'h':{ 
            thetaCentral[0] = 0.0;
            thetaCentral[1] = 0.0;
            thetaCentral[2] = 0.0;

            rXMatPointer[5] = 1.0;
            rXMatPointer[6] = 0.0;
            rXMatPointer[9] = 0.0;
            rXMatPointer[10] = 1.0;

            rYMatPointer[0] = 1.0;
            rYMatPointer[2] = 0.0;
            rYMatPointer[8] = 0.0;
            rYMatPointer[10] = 1.0;

            rZMatPointer[0] = 1.0;
            rZMatPointer[1] = 0.0;
            rZMatPointer[4] = 0.0;
            rZMatPointer[5] = 1.0;

            thetaLocal[0] = 0.0;
            thetaLocal[1] = 0.0;
            thetaLocal[2] = 0.0;

            rCamXMatPointer[5] = 1.0;
            rCamXMatPointer[6] = 0.0;
            rCamXMatPointer[9] = 0.0;
            rCamXMatPointer[10] = 1.0;

            rCamYMatPointer[0] = 1.0;
            rCamYMatPointer[2] = 0.0;
            rCamYMatPointer[8] = 0.0;
            rCamYMatPointer[10] = 1.0;

            rCamZMatPointer[0] = 1.0;
            rCamZMatPointer[1] = 0.0;
            rCamZMatPointer[4] = 0.0;
            rCamZMatPointer[5] = 1.0;

            camOffset[0] = 0.0;
            camOffset[1] = 0.0;       
            camOffset[2] = 0.0;

            glUseProgram(OProgramId);
            glUniformMatrix4fv(rotationXLocation, 1, GL_FALSE, rXMatPointer);
            glUniformMatrix4fv(rotationYLocation, 1, GL_FALSE, rYMatPointer);
            glUniformMatrix4fv(rotationZLocation, 1, GL_FALSE, rZMatPointer);

            glUniformMatrix4fv(rotationCamXLocation, 1, GL_FALSE, rXMatPointer);
            glUniformMatrix4fv(rotationCamYLocation, 1, GL_FALSE, rYMatPointer);
            glUniformMatrix4fv(rotationCamZLocation, 1, GL_FALSE, rZMatPointer);


            glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[3], 0.0);  

            glutPostRedisplay();
            break;
            }   

        case '2':{ 
            int k = glutGetModifiers();
            printf("2 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
                thetaLocal[0]+=stepTheta;
                float tc = thetaLocal[0];
                rCamXMatPointer[5] = cos(tc);
                rCamXMatPointer[6] = sin(tc);
                rCamXMatPointer[9] = -1*sin(tc);
                rCamXMatPointer[10] = cos(tc);
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rXMatPointer[%d]=%f\n", i,  rXMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationCamXLocation, 1, GL_FALSE, rCamXMatPointer);

                }
            else{
                thetaCentral[0]+=stepTheta;
                float tc = thetaCentral[0];
                rXMatPointer[5] = cos(tc);
                rXMatPointer[6] = sin(tc);
                rXMatPointer[9] = -1*sin(tc);
                rXMatPointer[10] = cos(tc);
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rXMatPointer[%d]=%f\n", i,  rXMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationXLocation, 1, GL_FALSE, rXMatPointer);
                }
            glutPostRedisplay();
            break;
            }    

        case '8':{ 
            int k = glutGetModifiers();
            printf("8 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
                thetaLocal[0]-=stepTheta;
                float tc = thetaLocal[0];
                rCamXMatPointer[5] = cos(tc);
                rCamXMatPointer[6] = sin(tc);
                rCamXMatPointer[9] = -1*sin(tc);
                rCamXMatPointer[10] = cos(tc);
                int i;
                glUniformMatrix4fv(rotationCamXLocation, 1, GL_FALSE, rCamXMatPointer);
                }
            else{
                thetaCentral[0]-=stepTheta;
                float tc = thetaCentral[0];
                rXMatPointer[5] = cos(tc);
                rXMatPointer[6] = sin(tc);
                rXMatPointer[9] = -1*sin(tc);
                rXMatPointer[10] = cos(tc);
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rXMatPointer[%d]=%f\n", i,  rXMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationXLocation, 1, GL_FALSE, rXMatPointer);
                }
            glutPostRedisplay();
            break;            
            }

        case '4':{ 
            int k = glutGetModifiers();
            printf("4 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
                thetaLocal[1]+=stepTheta;
                float tc = thetaLocal[1];
                rCamYMatPointer[0] = cos(tc);
                rCamYMatPointer[2] = sin(tc);
                rCamYMatPointer[8] = -1*sin(tc);
                rCamYMatPointer[10] = cos(tc);
                int i;
                glUniformMatrix4fv(rotationCamYLocation, 1, GL_FALSE, rCamYMatPointer);
                }
            else{
                thetaCentral[1]+=stepTheta;
                float tc = thetaCentral[1];
                rYMatPointer[0] = cos(tc);
                rYMatPointer[2] = sin(tc);
                rYMatPointer[8] = -1*sin(tc);
                rYMatPointer[10] = cos(tc);
        
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rYMatPointer[%d]=%f\n", i,  rYMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationYLocation, 1, GL_FALSE, rYMatPointer);
                }
            glutPostRedisplay();
            break;
            }    

        case '6':{ 
            int k = glutGetModifiers();
            printf("6 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
                thetaLocal[1]-=stepTheta;
                float tc = thetaLocal[1];
                rCamYMatPointer[0] = cos(tc);
                rCamYMatPointer[2] = sin(tc);
                rCamYMatPointer[8] = -1*sin(tc);
                rCamYMatPointer[10] = cos(tc);
                int i;
                glUniformMatrix4fv(rotationCamYLocation, 1, GL_FALSE, rCamYMatPointer);
                }
            else{
                thetaCentral[1]-=stepTheta;
                float tc = thetaCentral[1];
                rYMatPointer[0] = cos(tc);
                rYMatPointer[2] = sin(tc);
                rYMatPointer[8] = -1*sin(tc);
                rYMatPointer[10] = cos(tc);
        
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rYMatPointer[%d]=%f\n", i,  rYMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationYLocation, 1, GL_FALSE, rYMatPointer);
                }
            glutPostRedisplay();
            break;            
            }

        case '7':{ 

            int k = glutGetModifiers();
            printf("7 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
            thetaLocal[2]-=stepTheta;
            float tc = thetaLocal[2];
            rCamZMatPointer[0] = cos(tc);
            rCamZMatPointer[1] = -1*sin(tc);
            rCamZMatPointer[4] = sin(tc);
            rCamZMatPointer[5] = cos(tc);
            glUniformMatrix4fv(rotationCamZLocation, 1, GL_FALSE, rCamZMatPointer);

                }
            else{ // central rotation
            thetaCentral[2]-=stepTheta;
            float tc = thetaCentral[2];
            printf("%f\n",tc);
            rZMatPointer[0] = cos(tc);
            rZMatPointer[1] = -1*sin(tc);
            rZMatPointer[4] = sin(tc);
            rZMatPointer[5] = cos(tc);
            int i;
//            for(i=0; i<16; i++){        
//                printf("rZMatPointer[%d]=%f\n", i,  rZMatPointer[i]);
//                }
            glUniformMatrix4fv(rotationZLocation, 1, GL_FALSE, rZMatPointer);   
                }

            glutPostRedisplay();


            break;
            }    

        case '9':{ 
            int k = glutGetModifiers();
            printf("9 pressed with modifier %d\n",k);
            glUseProgram(OProgramId);
            if (k>=4){ // local Camera rotation
            thetaLocal[2]+=stepTheta;
            float tc = thetaLocal[2];
            rCamZMatPointer[0] = cos(tc);
            rCamZMatPointer[1] = -1*sin(tc);
            rCamZMatPointer[4] = sin(tc);
            rCamZMatPointer[5] = cos(tc);
            glUniformMatrix4fv(rotationCamZLocation, 1, GL_FALSE, rCamZMatPointer);

                }
            else{

                thetaCentral[2]+=stepTheta;
                float tc = thetaCentral[2];
                rZMatPointer[0] = cos(tc);
                rZMatPointer[1] = -1*sin(tc);
                rZMatPointer[4] = sin(tc);
                rZMatPointer[5] = cos(tc);
                int i;
    //            for(i=0; i<16; i++){        
    //                printf("rZMatPointer[%d]=%f\n", i,  rZMatPointer[i]);
    //                }
                glUniformMatrix4fv(rotationZLocation, 1, GL_FALSE, rZMatPointer);
                }        
            glutPostRedisplay();
            break;               
            }

        case '0':{ 
            camOffset[2]+=step;
            glUseProgram(OProgramId);
            glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[2], 0.0);  
            glutPostRedisplay();
            break;
            }

        case '.':{ 
            camOffset[2]-=step;
            glUseProgram(OProgramId);
            glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[2], 0.0);  
            glutPostRedisplay();
            break;
            }

        default:{
            break;}
        }
    }

void specialKeyInput(int key, int x, int y){
    printf("%d\n",key);
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
    glUniform4f(offsetLocation, -1.0*camOffset[0], -1.0*camOffset[1], -1.0*camOffset[2], 0.0);  

    glutPostRedisplay();

}

void main(int argc, char**argv) {
    int i,j,k;
    int externFile = 0;
    int typeSpecd = 0;

    for (i=1; i<argc; i++){

        if ( strcmp(argv[i], "-a") == 0){
            if ((i+1)<argc){
                autoMode = atoi(argv[i+1]);
                }
            else{
                autoMode = -1; // inf loop
                }
            }
        if (( strcmp(argv[i], "-f") == 0) && ((i+1)<argc)){
            externFile = 1;
            loadFile = argv[i+1];
            }

        if ( strcmp(argv[i], "-g") == 0){
            if ( (i+1)<argc ){
                gWidth = atoi(argv[i+1]);
                }
            gHeight = (i+2) < argc ? atoi(argv[i+2]) : gWidth;
            printf ("Grid Supplied: %d x %d\n", gWidth, gHeight);

            }
        if ( strcmp(argv[i], "-p") == 0){
            periodicMode = 1;
            } 

        if (( strcmp(argv[i], "-t") == 0) && ((i+1)<argc)){
            typeSpecd = 1;
            oType = argv[i+1];
            }

        }
// now decide which object(s) to evolve based on -t specs
    //check for valid organism type requested
    if (typeSpecd == 1){
        int typeFound = 0;
        for (i=0; i<numObjTypes; i++){
            if (strcmp(oType, objTypes[i]) == 0){
                typeFound = 1;
                masterObj = i;
                int s;
                for(s=0; s<numObjTypes; s++){
                    evolveable[s] = (s==i) ? 1 : 0 ;
                    }
                }
            }
        oType = (typeFound == 1) ? oType : objTypes[masterObj];
        }
    printf("oType is %s\n", oType);
    printf("masterObj = %d\n", masterObj);

//    if (strcmp(oType, "Square") == 0){
//        //numVertices = 4;
//        }
//    if (strcmp(oType, "Cube") == 0){
//        //numVertices = 4;
//        }
//    if (strcmp(oType, "Pacman") == 0){
//        //numVertices = 21;
//        } 


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


    loadProcTexture();
//    glFrontFace​(GL_CCW​);
    CreateShaders();

    if (externFile == 0){
        loadFile = "blub.txt";
        }
    printf("Input File: %s\n", loadFile);
//allocate array space for state pointers


    int numGCoords = gHeight * gWidth * numDims * 2;
    GLfloat gVertices[numGCoords];
    GLfloat gColors[numGCoords];

    gVPointer = gVertices;
    gCPointer = gColors;

//organism routine
    int q = masterObj;
    numObjs[q] = gWidth*gHeight;
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
    oCenters[q]=cents;

    int xStart, yStart;
    yStart = ( (gHeight%2) == 0) ? 1.0 : (float) (-1*(gHeight-1)/2);
    xStart = ( (gWidth%2) == 0) ? 1.0 : (float) (-1*(gWidth-1)/2);
    int loc = 0;
    for(i=0; i<gHeight; i++){
        for (j = 0; j<gWidth; j++){
            oCenters[q][loc] = xStart + (float)j;
            loc ++;
            oCenters[q][loc] = yStart + (float)i;
            loc++;
            oCenters[q][loc] = 0.0;
            loc++; 
            }
        }
//    for(i=0; i<3; i++){
//        printf("oCenters[%d][%d]=%f\n", q, i, oCenters[q][i]);
//        }
    int nO = numObjs[q];
    GLfloat cRads[nO];
    cRads[0] = 0.6;
 

    fillGrid1D(loadFile, cRads, numObjs[q], msgData);

    rPointer[q] = cRads;  
//    for(i=0; i<nO; i++){
//        rPointer[q][i] = cRads[i];
//    
//        }

//    printf("rPointer[%d]:\n",q);
//    for(i=0; i<numObjs[q]; i++){
//        printf("rP[%d][%d]sh=%f", q, i, cRads[i]);
//        printf("rP[%d][%d]=%f", q, i, rPointer[q][i]);
//        } 



//Cube routine, converted to squares (should be invisible)
    q=2;
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

    camScale[0] = 1.0/ (float)((gWidth+1)/2);
    camScale[1] = 1.0/ (float)((gHeight+1)/2);
    camScale[2] = 0.1;
    glUniform4f(scaleLocation, camScale[0], camScale[1], camScale[2], 1.0);  


    CreateVBO();


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

