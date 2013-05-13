#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include<utility>
#include<vector>
#include<algorithm>
#include<functional>
#include <cmath>
#include <ctime>
#include <GL/glut.h>
using namespace std;

#define PI 3.141592
#define DEG2RAD(deg) (deg * PI / 180)

#define Proj_Id -100
#define Mir_Id 0
#define Blk_Id 100
#define No_Id 786
#define BUFSIZE 512
#define RENDER 1
#define SELECT 2
int INSECTS = 10;
// Function Declarations
void drawScene();
void drawBox(float,float);
void Insect();
void update(int value);
void randomize(int value);
void drawLine(std::vector < std::pair <std::pair<float,float>,std::pair<float,float> > > &,int,int);
void drawLightSource(float rad);
void drawTriangle();
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
std::pair<int,int> check_intersection(std::pair<float,float> ,std::pair<float,float> );
void startPicking();
void stopPicking();
void processHits2 (GLint, GLuint[], int);
void findLightSource(int,float);
void drawLight();
float findSlope(const std::pair<float,float> &,const std::pair<float,float> &);
void locateEndsOnProj(int, int);
float checkSide(float,float,float,float,float,float);
void computeReflection(std::pair<float,float> ,std::pair<float,float> ,std::pair<float,float> ,std::pair<float,float> );
void RotateEndPoint();
void myMouseMove(int,int);
void GetOGLPos(int, int);
void drawInsect();
std::pair<float,float> findIntersection(float,float ,float,float,float,float, float, float);

// Global Variables
GLuint selectBuf[BUFSIZE];
GLuint hits;
GLint numberOfNames=0;
GLuint names, *ptr, minZ,*ptrNames;
int stipple = 0x0101;
int Index=-1,Obj_Id=No_Id;
int mode = SELECT;
int RandomFlag=0;
int CursorFlag=0;
int TransportFlag=0;
int PlayFlag=0;
int cursorX,cursorY;
float rotAngle = 5;
float rotationSpeed = 5;
float translationSpeed = 0.1;
float projectorWidth = 10.5f;
float mirrorWidth = 2.5f;
float blockWidth = 4.5f;
float lightWidth = 1.5f;
float BallSize = 0.08;
float InsectSize = 0.08;
float InsectSizeFactor=0.00005;
int num_projectors,num_mirrors,num_blocks,num_insects;
int w,h;
float rad = 0.05f;
float MaxX=20,MinX=-20;
GLdouble posX, posY, posZ;
float a=-1,b=-3,c=1,e=-3;
float newN=10,newD=1.5;
//Miscelleneous Global Variables
int n;
vector < pair< pair<float,float>,pair<float,float> > > projector_pos;
vector < pair< pair<float,float>,pair<float,float> > > mirror_pos;
vector < pair< pair<float,float>,pair<float,float> > > block_pos;
vector < pair< pair<float,float>,pair<float,float> > > reflected_pos;

vector <float> Pdistance;
vector <int> num_lightRays;
vector <float> insect_size;

vector < pair<float,float> > lightSource;
vector < vector < pair<float,float> > > LightRays;

vector < pair<float,float> > insect_pos;
vector <int> removeI;


vector <int> proj_randomT;
vector <int> proj_randomR;
vector <int> mir_randomT;
vector <int> mir_randomR;
vector <int> blk_randomT;
vector <int> blk_randomR;
vector <int> insect_random;
float Width,Height;
#define LINESZ 1024
int main(int argc, char **argv) {
float x1,y_1,x2,y2,d;
	int i;
    float windowWidth ;
    float windowHeight;
	char buff[LINESZ];
	FILE *file;
    file = fopen(argv[1],"r");
	if(file == 0){
		printf( "Could not open file\n" );
		exit(0);
	}
	else{
		fgets (buff, LINESZ, file);
		fgets (buff, LINESZ, file);
		sscanf(buff,"%f %f ",&Width,&Height);
		MaxX=Width;
		MinX=-1*Width;
		fgets (buff, LINESZ, file);
		sscanf(buff,"%d",&num_projectors);
		LightRays.resize(num_projectors);
		lightSource.resize(num_projectors);
		
		for(i=0;i<num_projectors;i++){
			fgets (buff, LINESZ, file);
			sscanf(buff,"%f %f %f %f",&x1,&y_1,&x2,&y2);
			fgets (buff, LINESZ, file);
			sscanf(buff,"%f",&d);
			fgets (buff, LINESZ, file);
		    sscanf(buff,"%d",&n);
			projector_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
			Pdistance.push_back(d);
			num_lightRays.push_back(n);
			findLightSource(i,d);
			locateEndsOnProj(i,n);
		}
		fgets (buff, LINESZ, file);
		sscanf(buff,"%d",&num_blocks);
		
		for(i=0;i<num_blocks;i++){
			fgets (buff, LINESZ, file);
			sscanf(buff,"%f %f %f %f",&x1,&y_1,&x2,&y2);
			block_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
		}
		fgets (buff, LINESZ, file);
		sscanf(buff,"%d",&num_mirrors);
		
		for(i=0;i<num_mirrors;i++){
			fgets (buff, LINESZ, file);
			sscanf(buff,"%f %f %f %f",&x1,&y_1,&x2,&y2);
			mirror_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
		}
	}
	num_insects = INSECTS;
	insect_pos.clear();
	insect_pos.resize(num_insects);
	insect_size.resize(num_insects,InsectSize);
	srand(time(NULL));
	for(i=0;i<num_insects;i++){
		if(i%4==0)
			insect_pos[i]=make_pair(rand() % 4,rand() % 4);
		if(i%4==1)
			insect_pos[i]=make_pair(-1*rand() % 4,rand() % 4);
		if(i%4==2)
			insect_pos[i]=make_pair(-1*rand() % 4,-1*rand() % 4);
		if(i%4==3)
			insect_pos[i]=make_pair(rand() % 4,-1*rand() % 4);
	}
	//printf("Coordinates,DistanceOfLightSource of Projectors:");
	//for(i=0;i<num_projectors;i++){
	//	scanf("%f %f %f %f %f %d",&x1,&y_1,&x2,&y2,&d,&n);
	//		projector_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
	//		Pdistance.push_back(d);
	//		num_lightRays.push_back(n);
	//		findLightSource(i,d);
	//		locateEndsOnProj(i,n);
//	}
	
//	printf("Number of Mirrors:");
//	scanf("%d",&num_mirrors);
//	printf("Coordinates of Mirrors:");
//	for(i=0;i<num_mirrors;i++){
//		scanf("%f %f %f %f",&x1,&y_1,&x2,&y2);
//			mirror_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
//	}
	
//	printf("Number of Blocks:");
//	scanf("%d",&num_blocks);
//	printf("Coordinates of Blocks:");
//	for(i=0;i<num_blocks;i++){
//		scanf("%f %f %f %f",&x1,&y_1,&x2,&y2);
//			block_pos.push_back(make_pair(make_pair(x1,y_1),make_pair(x2,y2)));
//	}
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    w = glutGet(GLUT_SCREEN_WIDTH);
    h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w;
	windowHeight = h;
	glutInitWindowSize(w, h);
    glutInitWindowPosition((w-windowWidth)/ 2, (h - windowHeight) / 2);

    glutCreateWindow("Graphics : Assignment - Developed by Deepak Kathayat");  // Setup the window
    initRendering();
	
    // Register callbacks
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutMouseFunc(handleMouseclick);
	glutPassiveMotionFunc(myMouseMove);
    glutReshapeFunc(handleResize);
    glutTimerFunc(100, update,0);

    glutMainLoop();
   return 0;
}

// Function to draw objects on the screen
void drawScene() {
    int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	if(mode == SELECT && !RandomFlag && !CursorFlag)
		startPicking();

	glLoadIdentity();
    // Draw Projector
    glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(projectorWidth);
    drawLine(projector_pos,num_projectors,Proj_Id);
   
   //Draw Light Source
	drawLightSource(rad);
	
    // Draw Mirror
    glColor3f(0.0f, 1.0f, 0.0f);
	glLineWidth(mirrorWidth);
    drawLine(mirror_pos,num_mirrors,Mir_Id);

    // Draw Block
    glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(blockWidth);
    drawLine(block_pos,num_blocks,Blk_Id);

	//Draw Light Rays from projectors
    glColor3f(1.0f, 0.8f, 1.0f);
	glLineWidth(lightWidth);
	for(i=0;i<num_projectors;i++){
		LightRays[i].clear();
		locateEndsOnProj(i,num_lightRays[i]);
	}
    drawLight();
    
	//Draw reflected light from LightSources
	glColor3f(1.0f, 0.8f, 1.0f);
	glLineWidth(lightWidth);
    drawLine(reflected_pos,(int)reflected_pos.size(),No_Id);
	reflected_pos.clear();

	drawInsect();
	
	drawBox(Width,Height);
	if(mode == SELECT && !RandomFlag && !CursorFlag)
		stopPicking();
	else
    	glutSwapBuffers();

}

void drawBox(float l,float b){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f,-10.0f);
	glBegin(GL_QUADS);
		glVertex2f(-l/2 , -b/2 );
		glVertex2f( -l/2, b/2 );
		glVertex2f( l/2, b/2 );
		glVertex2f( l/2, -b/2);
	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}
void drawInsect(){
	int i,j,k;
	float x,y,sourceX=lightSource[Index].first,sourceY=lightSource[Index].second;
	float RaySlope,InsectSlope;
	if(PlayFlag==1){
	for(i=0;i<num_insects;i++){

		for(k=0;k<num_lightRays[Index];k++){
			x = LightRays[Index][k].first;
			y = LightRays[Index][k].second;
			
			RaySlope = findSlope(make_pair(sourceX,sourceY),make_pair(x,y));
		    InsectSlope	= findSlope(make_pair(sourceX,sourceY),make_pair(insect_pos[i].first,insect_pos[i].second));
			if((RaySlope - 0.09 <= InsectSlope)  && (InsectSlope <= RaySlope + 0.09))
				insect_size[i] = insect_size[i] - 30*InsectSizeFactor;
			else
				insect_size[i]+=InsectSizeFactor;
		}
		if(insect_size[i]<0.03)
			removeI.push_back(i);
			glPushMatrix();
			glTranslatef(0.0f, 0.0f,-10.0f);
			glTranslatef(insect_pos[i].first, insect_pos[i].second,0.0f);
			
			glBegin(GL_TRIANGLE_FAN);
	    	for(j=0 ; j<360 ; j++){
				if(j%2)
					glColor3f(0.137255f,0.556863f,0.419608f);
				else
					glColor3f(0.8f,0.196078f,0.6f);
        		
				glVertex2f(insect_size[i] * cos(DEG2RAD(j)), insect_size[i] * sin(DEG2RAD(j)));
			}
			glEnd();
			glPopMatrix();
		}
	for(i=0;i<(int)removeI.size();i++){
		insect_pos.erase(insect_pos.begin()+removeI[i]);
		insect_size.erase(insect_size.begin()+removeI[i]);
		num_insects-=1;
	}
	removeI.clear();
		
	}
}
void Insect() {
	
	if(PlayFlag==1){
	int i;
	insect_random.resize(num_insects);
	srand ( time(NULL) );
	for(i=0;i< (int) insect_random.size();i++){
		insect_random[i] = rand() % 4;
	}
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble x,y;

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );

		x = Width/2;
		y = Height/2;

	for(i=0;i<num_insects;i++){
		if(insect_random[i]==0){
			insect_pos[i].first -= translationSpeed;
			if((insect_pos[i].first>x) || (insect_pos[i].first < -1*x)){
			insect_pos[i].first += 5*translationSpeed;
			}
		}
		else if(insect_random[i]==1){
			insect_pos[i].first += translationSpeed;
			if((insect_pos[i].first>x) || (insect_pos[i].first < -1*x)){
				insect_pos[i].first -= 5*translationSpeed;
			}
		}
		else if(insect_random[i]==2){
			insect_pos[i].second += translationSpeed;
			if((insect_pos[i].second>y) || (insect_pos[i].second < -1*y)){
				insect_pos[i].second -= 5*translationSpeed;
			}
		}
		else if(insect_random[i]==3){
			insect_pos[i].second -= translationSpeed;
			if((insect_pos[i].second>y) || (insect_pos[i].second < -1*y)){
				insect_pos[i].second += 5*translationSpeed;
			}
		}
	}
/*		for(i=0;i<num_insects;i++){
			glPushMatrix();
			glTranslatef(0.0f, 0.0f,-10.0f);
			glTranslatef(insect_pos[i].first, insect_pos[i].second,0.0f);
			glBegin(GL_TRIANGLE_FAN);
			glColor3f(1.0f,1.0f,1.0f);
	    	for(j=0 ; j<360 ; j++) 
        		glVertex2f(InsectSize * cos(DEG2RAD(j)), InsectSize * sin(DEG2RAD(j)));
			glEnd();
			glPopMatrix();
		}*/
		}
}

void randomize(int value){
	int i;

	if(RandomFlag==1){
	proj_randomT.resize(num_projectors);
	proj_randomR.resize(num_projectors);
	mir_randomT.resize(num_mirrors);
	mir_randomR.resize(num_mirrors);
	blk_randomT.resize(num_blocks);
	blk_randomR.resize(num_blocks);
	srand ( time(NULL) );
	for(i=0;i< (int) proj_randomT.size();i++){
		proj_randomT[i] = rand() % 4;
	}
	for(i=0;i<(int)mir_randomT.size();i++){
		mir_randomT[i] = rand() % 4;
	}
	for(i=0;i<(int)blk_randomT.size();i++){
		blk_randomT[i] = rand() % 4;
	}
	for(i=0;i<(int)proj_randomR.size();i++){
		proj_randomR[i] = rand() % 2;
	}
	for(i=0;i<(int)mir_randomR.size();i++){
		mir_randomR[i] = rand() % 2;
	}
	for(i=0;i<(int)blk_randomR.size();i++){
		blk_randomR[i] = rand() % 2;
	}
	}
}

void GetOGLPos(int x, int y){
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );
		
		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

}
void myMouseMove( int x, int y)
{   if(CursorFlag==1){
	float tempX,tempY;
	GetOGLPos(x,y);
	tempX = posX;
	tempY = posY;
	float r,MidX,MidY,theta,rotAngle,X1,Y1,X2,Y2,slope1,slope2;
	int i;
	for(i=0;i<num_projectors;i++){
		
		MidX = (projector_pos[i].first.first+projector_pos[i].second.first)/2;
		MidY = (projector_pos[i].first.second+projector_pos[i].second.second)/2;
		slope1 = (lightSource[i].second - MidY)/(lightSource[i].first-MidX);
		slope2 = (lightSource[i].second - tempX)/(lightSource[i].first-tempY);
		rotAngle = atan2((slope1-slope2),(1+(slope1*slope2)));
		if(rotAngle<0)
			rotAngle+=2*PI;
	    X1 = lightSource[i].first;
	    Y1 = lightSource[i].second;
	
		X2 = projector_pos[i].second.first;
		Y2 = projector_pos[i].second.second;
		theta = atan2((Y2-Y1),(X2-X1));
		r = sqrt( pow(X2-X1,2) + pow(Y2-Y1,2) );
		projector_pos[i].second.first = lightSource[i].first + (r*cos(DEG2RAD(theta+rotAngle))) ;
		projector_pos[i].second.second = lightSource[i].second + (r*sin(DEG2RAD(theta+rotAngle))) ;
		
		X2 = projector_pos[i].first.first;
		Y2 = projector_pos[i].first.second;
		theta = atan2((Y2-Y1),(X2-X1));
		r = sqrt( pow(X2-X1,2) + pow(Y2-Y1,2) );
		
		projector_pos[i].first.first = lightSource[i].first + (r*cos(DEG2RAD(theta+rotAngle))) ;
		projector_pos[i].first.second = lightSource[i].second + (r*sin(DEG2RAD(theta+rotAngle))) ;
	}
	}
}

// Function to handle all calculations in the scene
// updated evry 10 milliseconds
void update(int value) {

		int i;
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLdouble x,y;

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );

		x = Width/2;
		y = Height/2;
	if(stipple>0)
		stipple = stipple+0x1010;
	else
		stipple = 0;
	if(RandomFlag==1){
	randomize(0);
	for(i=0;i<num_projectors;i++){
		if(proj_randomT[i]==0){
			projector_pos[i].first.first -= translationSpeed;
			projector_pos[i].second.first -= translationSpeed;
			if(projector_pos[i].first.first>=x || projector_pos[i].first.first <= -1*x){
			projector_pos[i].first.first += 5*translationSpeed;
			projector_pos[i].second.first += 5*translationSpeed;
			}
			if(projector_pos[i].second.first>=x || projector_pos[i].second.first <= -1*x){
			projector_pos[i].first.first += 5*translationSpeed;
			projector_pos[i].second.first += 5*translationSpeed;
		}
		}
		else if(proj_randomT[i]==1){
			projector_pos[i].first.first += translationSpeed;
			projector_pos[i].second.first += translationSpeed;
			if(projector_pos[i].first.first>x || projector_pos[i].first.first < -1*x){
			projector_pos[i].first.first -= 5*translationSpeed;
			projector_pos[i].second.first -= 5*translationSpeed;
		}

			if(projector_pos[i].second.first>x || projector_pos[i].second.first < -1*x){
			projector_pos[i].first.first -= 5*translationSpeed;
			projector_pos[i].second.first -= 5*translationSpeed;
		}
		}
		else if(proj_randomT[i]==2){
			projector_pos[i].first.second += translationSpeed;
			projector_pos[i].second.second += translationSpeed;
			if(projector_pos[i].first.second>y || projector_pos[i].first.second < -1*y){
			projector_pos[i].first.second -= 5*translationSpeed;
			projector_pos[i].second.second -= 5*translationSpeed;
		}
			
			if(projector_pos[i].second.second>y || projector_pos[i].second.second < -1*y){
			projector_pos[i].first.second -= 5*translationSpeed;
			projector_pos[i].second.second -= 5*translationSpeed;
			}
		}
		else if(proj_randomT[i]==3){
			projector_pos[i].first.second -= translationSpeed;
			projector_pos[i].second.second -= translationSpeed;
			if(projector_pos[i].first.second>y || projector_pos[i].first.second < -1*y){
			projector_pos[i].first.second += 5*translationSpeed;
			projector_pos[i].second.second += 5*translationSpeed;
			}
			if(projector_pos[i].second.second>y || projector_pos[i].second.second < -1*y){
			projector_pos[i].first.second += 5*translationSpeed;
			projector_pos[i].second.second += 5*translationSpeed;
			}
		}
		if(proj_randomR[i]==0){
			Obj_Id=Proj_Id;
			Index=i;
		    rotAngle = abs(rotAngle);
			RotateEndPoint();
		}
		else if(proj_randomR[i]==1){
			Obj_Id=Proj_Id;
			Index=i;
		    rotAngle = -1* abs(rotAngle);
			RotateEndPoint();
		}
	}
	for(i=0;i<num_mirrors;i++){
		if(mir_randomT[i]==0){
			mirror_pos[i].first.first -= translationSpeed;
			mirror_pos[i].second.first -= translationSpeed;
			if(mirror_pos[i].first.first>x || mirror_pos[i].first.first < -1*x){
			mirror_pos[i].first.first += 5*translationSpeed;
			mirror_pos[i].second.first += 5*translationSpeed;
			}
			if(mirror_pos[i].second.first>x || mirror_pos[i].second.first < -1*x){
			mirror_pos[i].first.first += 5*translationSpeed;
			mirror_pos[i].second.first += 5*translationSpeed;
		}
		}
		else if(mir_randomT[i]==1){
			mirror_pos[i].first.first += translationSpeed;
			mirror_pos[i].second.first += translationSpeed;
			if(mirror_pos[i].first.first>x || mirror_pos[i].first.first < -1*x){
			mirror_pos[i].first.first -= 5*translationSpeed;
			mirror_pos[i].second.first -= 5*translationSpeed;
			}
			if(mirror_pos[i].second.first>x || mirror_pos[i].second.first < -1*x){
			mirror_pos[i].first.first -= 5*translationSpeed;
			mirror_pos[i].second.first -= 5*translationSpeed;
		}
		}
		else if(mir_randomT[i]==2){
			mirror_pos[i].first.second += translationSpeed;
			mirror_pos[i].second.second += translationSpeed;
			if(mirror_pos[i].first.second>y || mirror_pos[i].first.second < -1*y){
			mirror_pos[i].first.second -= 5*translationSpeed;
			mirror_pos[i].second.second -= 5*translationSpeed;
		}
			
			if(mirror_pos[i].second.second>y || mirror_pos[i].second.second < -1*y){
			mirror_pos[i].first.second -= 5*translationSpeed;
			mirror_pos[i].second.second -= 5*translationSpeed;
			}
		}
		else if(mir_randomT[i]==3){
			mirror_pos[i].first.second -= translationSpeed;
			mirror_pos[i].second.second -= translationSpeed;
			if(mirror_pos[i].first.second>y || mirror_pos[i].first.second < -1*y){
			mirror_pos[i].first.second += 5*translationSpeed;
			mirror_pos[i].second.second += 5*translationSpeed;
		}
			
			if(mirror_pos[i].second.second>y || mirror_pos[i].second.second < -1*y){
			mirror_pos[i].first.second += 5*translationSpeed;
			mirror_pos[i].second.second += 5*translationSpeed;
			}
		}
		if(mir_randomR[i]==0){
			Obj_Id=Mir_Id;
			Index=i;
		    rotAngle = abs(rotAngle);
			RotateEndPoint();
		}
		else if(mir_randomR[i]==1){
			Obj_Id=Mir_Id;
			Index=i;
		    rotAngle = -1 * abs(rotAngle);
			RotateEndPoint();
		}
	}
	for(i=0;i<num_blocks;i++){
		if(blk_randomT[i]==0){
			block_pos[i].first.first -= translationSpeed;
			block_pos[i].second.first -= translationSpeed;
			if(block_pos[i].first.first>x || block_pos[i].first.first < -1*x){
			block_pos[i].first.first += 5*translationSpeed;
			block_pos[i].second.first += 5*translationSpeed;
			}
			if(block_pos[i].second.first>x || block_pos[i].second.first < -1*x){
			block_pos[i].first.first += 5*translationSpeed;
			block_pos[i].second.first += 5*translationSpeed;
		}
		}
		else if(blk_randomT[i]==1){
			block_pos[i].first.first += translationSpeed;
			block_pos[i].second.first += translationSpeed;
			if(block_pos[i].first.first>x || block_pos[i].first.first < -1*x){
			block_pos[i].first.first -= 5*translationSpeed;
			block_pos[i].second.first -= 5*translationSpeed;
			}
			if(block_pos[i].second.first>x || block_pos[i].second.first < -1*x){
			block_pos[i].first.first -= 5*translationSpeed;
			block_pos[i].second.first -= 5*translationSpeed;
		}
		}
		else if(blk_randomT[i]==2){
			block_pos[i].first.second += translationSpeed;
			block_pos[i].second.second += translationSpeed;
			if(block_pos[i].first.second>y || block_pos[i].first.second < -1*y){
			block_pos[i].first.second -= 5*translationSpeed;
			block_pos[i].second.second -= 5*translationSpeed;
		}
			
			if(block_pos[i].second.second>y || block_pos[i].second.second < -1*y){
			block_pos[i].first.second -= 5*translationSpeed;
			block_pos[i].second.second -= 5*translationSpeed;
			}
		}
		else if(blk_randomT[i]==3){
			block_pos[i].first.second -= translationSpeed;
			block_pos[i].second.second -= translationSpeed;
			if(block_pos[i].first.second>y || block_pos[i].first.second < -1*y){
			block_pos[i].first.second += 5*translationSpeed;
			block_pos[i].second.second += 5*translationSpeed;
		}
			
			if(block_pos[i].second.second>y || block_pos[i].second.second < -1*y){
			block_pos[i].first.second += 5*translationSpeed;
			block_pos[i].second.second += 5*translationSpeed;
			}
		}
		if(blk_randomR[i]==0){
			Obj_Id=Blk_Id;
			Index=i;
		    rotAngle = abs(rotAngle);
			RotateEndPoint();
		}
		else if(blk_randomR[i]==1){
			Obj_Id=Blk_Id;
			Index=i;
		    rotAngle = -1* abs(rotAngle);
			RotateEndPoint();
		}
	}
	}
	if(PlayFlag==1){
		Insect();
	}
	glutTimerFunc(100,update,0);
	glutPostRedisplay();
}
std::pair<int,int> check_intersection( std::pair<float,float> p1, std::pair<float,float> p2){
	int i,id=No_Id,I=No_Id;
	float xA,yA,xB,yB,d,MinD=10000;
	pair <float,float> Pt;
	/*for(i=0;i<num_projectors;i++){
	  xA = projector_pos[i].first.first;
	  yA = projector_pos[i].first.second;
	  xB = projector_pos[i].second.first;
	  yB = projector_pos[i].second.second;
	  if(((checkSide(xA,yA,xB,yB,p1.first,p1.second)*checkSide(xA,yA,xB,yB,p2.first,p2.second))<0) && ((checkSide(p1.first,p1.second,p2.first,p2.second,xA,yA)*checkSide(p1.first,p1.second,p2.first,p2.second,xB,yB))<0)){
	  return make_pair(Proj_Id,i);
	  }
	  }*/
	for(i=0;i<num_blocks;i++){
		xA = block_pos[i].first.first;
		yA = block_pos[i].first.second;
		xB = block_pos[i].second.first;
		yB = block_pos[i].second.second;
		if(((checkSide(xA,yA,xB,yB,p1.first,p1.second)*checkSide(xA,yA,xB,yB,p2.first,p2.second))<0) && ((checkSide(p1.first,p1.second,p2.first,p2.second,xA,yA)*checkSide(p1.first,p1.second,p2.first,p2.second,xB,yB))<0)){
			Pt = findIntersection(xA,yA,xB,yB,p1.first,p1.second,p2.first,p2.second);
			d = sqrt( ( (Pt.first-p1.first)*(Pt.first-p1.first) ) + ( (Pt.second-p1.second)*(Pt.second-p1.second) ) );
			if(d<MinD){
			MinD = d;
			id = Blk_Id;
			I = i;
			}
			}
		}
	for(i=0;i<num_mirrors;i++){
		xA = mirror_pos[i].first.first;
		yA = mirror_pos[i].first.second;
		xB = mirror_pos[i].second.first;
		yB = mirror_pos[i].second.second;
		if(((checkSide(xA,yA,xB,yB,p1.first,p1.second)*checkSide(xA,yA,xB,yB,p2.first,p2.second))<0) && ((checkSide(p1.first,p1.second,p2.first,p2.second,xA,yA)*checkSide(p1.first,p1.second,p2.first,p2.second,xB,yB))<0)){
			Pt = findIntersection(xA,yA,xB,yB,p1.first,p1.second,p2.first,p2.second);
			d = sqrt( ( (Pt.first-p1.first)*(Pt.first-p1.first) ) + ( (Pt.second-p1.second)*(Pt.second-p1.second) ) );
			if(d<MinD){
			MinD = d;
			id = Mir_Id;
			I = i;
			}
		}
	}
	if(id==Mir_Id){
		if(checkSide(mirror_pos[I].first.first,mirror_pos[I].first.second,mirror_pos[I].second.first,mirror_pos[I].second.second,p1.first,p1.second)<0)
			computeReflection(p1,p2,mirror_pos[I].first,mirror_pos[I].second);
	}
	return make_pair(id,I);
}

void computeReflection(std::pair<float,float> p1,std::pair<float,float> p2,std::pair<float,float> p3,std::pair<float,float> p4){
	int i,id=No_Id,I;
	float x,y,x1,y_1,x2,y2,x3,y3,x4,y4,den,z,MinD=100000;
	float xA,yA,xB,yB,FinalX,FinalY;
	float a1,b1,a2,b2,lambda,slope;
	float yMin,yMax,X,Y;
	pair <float,float> Pt;
	x1 = p1.first; //lightSource
	y_1 = p1.second;//lightSource
	x2 = p2.first; //far away pt
	y2 = p2.second; //far away pt
	x3 = p3.first;
	y3 = p3.second;
	x4 = p4.first;
	y4 = p4.second;
	den = ((x1-x2)*(y3-y4)) - ((y_1-y2)*(x3-x4));

	x = ( ((x1*y2) - (y_1*x2)) * (x3-x4))  -  ( (x1-x2) * ((x3*y4)-(y3*x4)) );
	x = x/den;
	y = ( ((x1*y2) - (y_1*x2)) * (y3-y4))  -  ( (y_1-y2) * ((x3*y4)-(y3*x4)) );
	y = y/den;

	a1 = (y2-y_1)/(x2-x1);
	b1 = -1;
	a2 = (y4-y3)/(x4-x3);
	b2 = -1;
	lambda = -2 * ((a1*a2)+(b1*b2));
	lambda = lambda/((a2*a2)+(b2*b2));
	slope = -1* ((a1+(lambda*a2))/(b1+lambda*b2));
			
	yMin = (slope * (MinX - x)) + y;
	yMax = (slope * (MaxX - x)) + y;
	if(checkSide(x3,y3,x4,y4,x1,y_1)*checkSide(x3,y3,x4,y4,MinX,yMin)>0){
		X=MinX;
		Y=yMin;
	}
	else{X=MaxX;
		Y=yMax;
	}
	FinalX=X;
	FinalY=Y;
	//check intersection with the block
	for(i=0;i<num_blocks;i++){
		xA = block_pos[i].first.first;
		yA = block_pos[i].first.second;
		xB = block_pos[i].second.first;
		yB = block_pos[i].second.second;
		if(((checkSide(xA,yA,xB,yB,x,y)*checkSide(xA,yA,xB,yB,X,Y))<0) && ((checkSide(x,y,X,Y,xA,yA)*checkSide(x,y,X,Y,xB,yB))<0)){
			Pt = findIntersection(xA,yA,xB,yB,x,y,X,Y);
			z = sqrt( ( (Pt.first-x)*(Pt.first-x) ) + ( (Pt.second-y)*(Pt.second-y) ) );
			if(z<MinD){
			MinD = z;
			FinalX=Pt.first;
			FinalY=Pt.second;
			}
		}
	}

	//check intersection with mirror
	for(i=0;i<num_mirrors;i++){
		xA = mirror_pos[i].first.first;
		yA = mirror_pos[i].first.second;
		xB = mirror_pos[i].second.first;
		yB = mirror_pos[i].second.second;
		if( ((y-y_1)/(x-x1)) ==(((Y-y_1))/(X-x1)));
		else{
		if(((checkSide(xA,yA,xB,yB,x,y)*checkSide(xA,yA,xB,yB,X,Y))<0) && ((checkSide(x,y,X,Y,xA,yA)*checkSide(x,y,X,Y,xB,yB))<0)){
			Pt = findIntersection(xA,yA,xB,yB,x,y,X,Y);
			z = sqrt( ( (Pt.first-x)*(Pt.first-x) ) + ( (Pt.second-y)*(Pt.second-y) ) );
			if(z<0.1);
			else{
			if(z<MinD && z!=0){
			MinD = z;
			FinalX=Pt.first;
			FinalY=Pt.second;
			id=Mir_Id;
			I=i;
			}
			}
			}
		}
	}
	if(id==Mir_Id){
		if(checkSide(mirror_pos[I].first.first,mirror_pos[I].first.second,mirror_pos[I].second.first,mirror_pos[I].second.second,FinalX,FinalY)<0)
			computeReflection(make_pair(x,y),make_pair(FinalX,FinalY),mirror_pos[I].first,mirror_pos[I].second);
	}
	/*glPushMatrix();
	glTranslatef(0.0f, 0.0f,-10.0f);
	if(TransportFlag==1){
		glEnable( GL_LINE_STIPPLE );
		glLineStipple( 1, 0x1C47+stipple );
	}
	glBegin(GL_LINES);
	glColor3f(0.8f,0.2f,0.8f);	
	glVertex2f(x, y);
	glColor3f(0.2f,0.8f,0.2f);	
	glVertex2f(FinalX,FinalY);
	glEnd();
	glDisable( GL_LINE_STIPPLE );
	glPopMatrix();*/
//	else
//		reflected_pos.push_back(make_pair(make_pair(x,y),make_pair(X,Y)));
		reflected_pos.push_back(make_pair(make_pair(x,y),make_pair(FinalX,FinalY)));
}
void drawLine(std::vector< pair <pair<float,float>,pair<float,float> > > & V, int num_objects, int Ob_Id) {
  int i;
  	if(Ob_Id!=No_Id)
		glPushName(Ob_Id);
	for(i=0;i<num_objects;i++){
  			if(Ob_Id!=No_Id)
				glPushName(i);
			glPushMatrix();
			glTranslatef(0.0f,0.0f,-10.0f);
			if(Ob_Id==No_Id){
				if(TransportFlag==1){
				glEnable( GL_LINE_STIPPLE );
				glLineStipple( 1, 0x1C47+stipple );
				}
				glBegin(GL_LINES);
				glColor3f(0.917647f,0.917647f,0.678431f);	
				glVertex2f(V[i].first.first, V[i].first.second);
				glColor3f(1.0f,0.43f,0.786078f);	
	    		glVertex2f(V[i].second.first, V[i].second.second);
				glEnd();
				glDisable( GL_LINE_STIPPLE );
			}
			else
			{
				if(Obj_Id==Ob_Id && Index==i)
					glColor3f(0.517647f,0.617647f,1.0f);	
			glBegin(GL_LINES);
				glVertex2f(V[i].first.first, V[i].first.second);
	    		glVertex2f(V[i].second.first, V[i].second.second);
			glEnd();
			}
			glPopMatrix();
  	if(Ob_Id!=No_Id)
		glPopName();
	}
  	if(Ob_Id!=No_Id)
		glPopName();
}

void drawLightSource(float rad) {
	int i;
	lightSource.clear();
	lightSource.resize(num_projectors);
	for(i=0;i<num_projectors;i++){
		findLightSource(i,Pdistance[i]);
		glColor3f(0.85f,0.85f,0.10f);
		glPushMatrix();
		glTranslatef(0.0f, 0.0f,-10.0f);
		glTranslatef(lightSource[i].first, lightSource[i].second,0.0f);
		glBegin(GL_TRIANGLE_FAN);
	    for(int j=0 ; j<360 ; j++) 
        	glVertex2f(BallSize * cos(DEG2RAD(j)), BallSize * sin(DEG2RAD(j)));
        glEnd();
	//	glPointSize(PointSize);
	//	glBegin(GL_POINTS);
	//	glVertex2f(lightSource[i].first, lightSource[i].second);
	//	glEnd();
		glPopMatrix();
	}
}

void locateEndsOnProj(int i, int n){
	int j;
	float xA,xB,yA,yB;
	xA = projector_pos[i].first.first;
	xB = projector_pos[i].second.first;
	yA = projector_pos[i].first.second;
	yB = projector_pos[i].second.second;
	LightRays[i].resize(n);

	for(j=1;j<n+1;j++){
		LightRays[i][j-1].first = (float)( (j*xB) + ( (n-j+1)*xA) ) / (float)(n+1);
		LightRays[i][j-1].second = (float)( (j*yB) + ( (n-j+1)*yA) ) / (float)(n+1);
	}
	return;
}

float findSlope(const std::pair<float,float> &p1,const std::pair<float,float> &p2){
	return (float)(p2.second - p1.second)/(float)(p2.first - p1.first);
}
void drawLight() {
	int i,j;
	pair <float,float> Pt;
	float x,y,x1,y_1,x2,y2,x3,y3,x4,y4;
	for(i=0;i<num_projectors;i++){
		for(j=0;j<(int)LightRays[i].size();j++){
			float slope = findSlope(make_pair(lightSource[i].first,lightSource[i].second),LightRays[i][j]);
			if(!isnan(slope)){
				if(LightRays[i][j].first<lightSource[i].first){
					//draw ray into -ve infinity
					x = MinX;
					y = (slope * (MinX - lightSource[i].first)) + lightSource[i].second;
				}
				else{//draw ray into +ve infinity
					x = MaxX;
					y = (slope * (MaxX - lightSource[i].first)) + lightSource[i].second;
				}
				std::pair <int,int> Id = check_intersection(make_pair(lightSource[i].first,lightSource[i].second),make_pair(x,y));
				if(Id.first!=No_Id){
					//find intersection point and draw line segment
					x1 = lightSource[i].first;
					y_1 = lightSource[i].second;
					x2 = x;
					y2 = y;
					if(Id.first==Proj_Id){
						x3 = projector_pos[Id.second].first.first;
						y3 = projector_pos[Id.second].first.second;
						x4 = projector_pos[Id.second].second.first;
						y4 = projector_pos[Id.second].second.second;
					}
					else if(Id.first==Mir_Id){
						x3 = mirror_pos[Id.second].first.first;
						y3 = mirror_pos[Id.second].first.second;
						x4 = mirror_pos[Id.second].second.first;
						y4 = mirror_pos[Id.second].second.second;
					}
					else if(Id.first==Blk_Id){
						x3 = block_pos[Id.second].first.first;
						y3 = block_pos[Id.second].first.second;
						x4 = block_pos[Id.second].second.first;
						y4 = block_pos[Id.second].second.second;
					}
					Pt = findIntersection(x1,y_1,x2,y2,x3,y3,x4,y4);
					x = Pt.first;
					y = Pt.second;
				}
				glPushMatrix();
				glTranslatef(0.0f, 0.0f,-10.0f);
				if(TransportFlag==1){
					glEnable( GL_LINE_STIPPLE );
					glLineStipple( 1, 0x1C47+stipple );
				}
				glBegin(GL_LINES);
				glColor3f(0.917647f,0.917647f,0.678431f);	
				glVertex2f(lightSource[i].first, lightSource[i].second);
				glColor3f(1.0f,0.43f,0.786078f);	
				glVertex2f(x,y);
				glEnd();
				glDisable( GL_LINE_STIPPLE );
				glPopMatrix();
			}
		}
	}
}

std::pair<float,float> findIntersection(float x1,float y_1,float x2,float y2,float x3,float y3,float x4,float y4){
	float den = ((x1-x2)*(y3-y4)) - ((y_1-y2)*(x3-x4));

	float x = ( ((x1*y2) - (y_1*x2)) * (x3-x4))  -  ( (x1-x2) * ((x3*y4)-(y3*x4)) );
	x = x/den;
	float y = ( ((x1*y2) - (y_1*x2)) * (y3-y4))  -  ( (y_1-y2) * ((x3*y4)-(y3*x4)) );
	y = y/den;
	return make_pair(x,y);
}
void drawTriangle() {

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
}

// Initializing some openGL 3D rendering options
void initRendering() {

    glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
    glEnable(GL_COLOR_MATERIAL);    // Enable coloring
    glEnable(GL_LINE_SMOOTH);    // Enable line lines with correct filtering
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}

void startPicking() {

	GLint viewport[4];
	float ratio;
	glSelectBuffer(BUFSIZE,selectBuf);
	
	glGetIntegerv(GL_VIEWPORT,viewport);
	
	glRenderMode(GL_SELECT);
	
	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(cursorX,viewport[3]-cursorY,5,5,viewport);
	ratio = (viewport[2]+0.0) / viewport[3];
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void stopPicking() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	if (hits != 0){
		processHits2(hits,selectBuf,0);
	}
	mode = RENDER;
}

void processHits2 (GLint hits, GLuint buffer[], int sw){
	int i;
	ptr = (GLuint *) buffer;
	minZ = 0xffffffff;
	for (i = 0; i < hits; i++) {
	    names = *ptr;
	    ptr++;
	    if (*ptr < minZ) {
			numberOfNames = names;
			minZ = *ptr;
			ptrNames = ptr+2;
		}
		ptr += names+2;
	}
	if (numberOfNames > 0) {
		ptr = ptrNames;
		Obj_Id = *ptr;
		Index = *(ptr+1);
	}
}

void findLightSource(int i,float d){
	float x,y; //midpoints of the line segment
	float xA,xB,yA,yB; //points of line segment
	float tempx,tempy;
	xA = projector_pos[i].first.first;
	xB = projector_pos[i].second.first;
	x = (xA+xB)/2;

	yA = projector_pos[i].first.second;
	yB = projector_pos[i].second.second;
	y = (yA+yB)/2;
	
	float cosine = cos(atan2(xA-xB,yB-yA));
	float sine = sin(atan2(xA-xB,yB-yA));

	tempx = x + d*cosine;
	tempy = y + d*sine;
	
	float Check_Side = checkSide(xA,yA,xB,yB,tempx,tempy);
	if(Check_Side<0){
		lightSource[i].first=tempx;
		lightSource[i].second=tempy;
	}
	else{
		lightSource[i].first=x-(d*cosine);
		lightSource[i].second=y-(d*sine);
	}
	return;
}

float checkSide(float xA,float yA,float xB,float yB,float tempx,float tempy){
   return (xB - xA) * (tempy - yA) - (yB - yA) * (tempx - xA);
}
void RotateEndPoint(){
	float X1,Y1,X2,Y2,theta,r;
	if(Obj_Id==Proj_Id){
	    X1 = projector_pos[Index].first.first;
	    Y1 = projector_pos[Index].first.second;
		X2 = projector_pos[Index].second.first;
		Y2 = projector_pos[Index].second.second;
		theta = atan2((Y2-Y1),(X2-X1));
		r = sqrt( pow(X2-X1,2) + pow(Y2-Y1,2) );
		projector_pos[Index].second.first = X1 + (r*cos(DEG2RAD(theta+rotAngle))) ;
		projector_pos[Index].second.second = Y1 + (r*sin(DEG2RAD(theta+rotAngle))) ;
	}
	else if(Obj_Id==Mir_Id){
	    X1 = mirror_pos[Index].first.first;
	    Y1 = mirror_pos[Index].first.second;
		X2 = mirror_pos[Index].second.first;
		Y2 = mirror_pos[Index].second.second;
		theta = atan2((Y2-Y1),(X2-X1));
		r = sqrt( pow(X2-X1,2) + pow(Y2-Y1,2) );
		mirror_pos[Index].second.first = X1 + (r*cos(DEG2RAD(theta+rotAngle))) ;
		mirror_pos[Index].second.second = Y1 + (r*sin(DEG2RAD(theta+rotAngle))) ;
	}
	else if(Obj_Id==Blk_Id){
	    X1 = block_pos[Index].first.first;
	    Y1 = block_pos[Index].first.second;
		X2 = block_pos[Index].second.first;
		Y2 = block_pos[Index].second.second;
		theta = atan2((Y2-Y1),(X2-X1));
		r = sqrt( pow(X2-X1,2) + pow(Y2-Y1,2) );
		block_pos[Index].second.first = X1 + (r*cos(DEG2RAD(theta+rotAngle))) ;
		block_pos[Index].second.second = Y1 + (r*sin(DEG2RAD(theta+rotAngle))) ;
	}
}

// Function called when the window is resized
void handleResize(int w, int h) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / (float)h, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void handleKeypress1(unsigned char key, int x, int y) {
	int i;
	if (key == 27) {
	FILE *f;
    f = fopen("WorldState","w");
	fprintf(f,"# State of the World\n");
	fprintf(f,"%f %f #Width and Height of the World\n",Width,Height);
	fprintf(f,"%d #Number of projectors\n",num_projectors);
	for(i=0;i<num_projectors;i++){
		fprintf(f,"%f %f %f %f #Project %d line: x1 y1 x2 y2\n",projector_pos[i].first.first,projector_pos[i].first.second,projector_pos[i].second.first,projector_pos[i].second.second,i+1);
		fprintf(f,"%f #Distance to the point light source\n",Pdistance[i]);
		fprintf(f,"%d #Number of pixels in the Projector\n",num_lightRays[i]);
	}
		fprintf(f,"%d #Number of Blocks\n",num_blocks);
	for(i=0;i<num_blocks;i++){
		fprintf(f,"%f %f %f %f #Block %d: x1 y1 x2 y2\n",block_pos[i].first.first,block_pos[i].first.second,block_pos[i].second.first,block_pos[i].second.second,i+1);
	}
		fprintf(f,"%d #Number of Mirrors\n",num_mirrors);
	for(i=0;i<num_mirrors;i++){
		fprintf(f,"%f %f %f %f #Mirror %d: x1 y1 x2 y2\n",mirror_pos[i].first.first,mirror_pos[i].first.second,mirror_pos[i].second.first,mirror_pos[i].second.second,i+1);
	}
	fclose(f);
	exit(0);     // escape key is pressed
	}
	else if (key == 102){ //f is pressed, increase speed
		translationSpeed = translationSpeed + 0.1;
		rotAngle = rotAngle + rotationSpeed;
	}
	else if (key == 115){ //s is pressed, decrease speed
		translationSpeed = translationSpeed - 0.1;
		rotAngle = rotAngle - rotationSpeed;
		if(rotAngle <= -360)
			rotAngle = -1*(rotAngle+360);
		if(translationSpeed<=0)
			translationSpeed = 0.1;
	}
	else if (key == 108){  //r is pressed, rotation to the right
			rotAngle = abs(rotAngle);
			RotateEndPoint();
		}

	else if (key == 114){ //l is pressed, rotation to the left
			rotAngle = -1 * abs(rotAngle);
			RotateEndPoint();
		}
	else if(key == 'g' && CursorFlag==0){//enter cursor mode
			CursorFlag=1;
		}
		else if(key == 'g' && CursorFlag==1){//exit cursor mode
			CursorFlag=0;
		}
			
		else if(key == 'z' && RandomFlag==0){ //z is pressed, enter random mode
			RandomFlag=1;
		}
		else if(key == 'z' && RandomFlag==1){ //z is pressed, exit random mode
			RandomFlag=0;
		}
	else if(key == 't' && TransportFlag==0){
			TransportFlag=1;
	}
	else if(key == 't' && TransportFlag==1){
			TransportFlag=0;
	}
	else if(key == 'b' ){
		block_pos.push_back(make_pair(make_pair(-1,0),make_pair(1,0)));
		num_blocks+=1;
		Obj_Id=Blk_Id;
		Index = num_blocks-1;
	}
	if(key == 'm' ){
		mirror_pos.push_back(make_pair(make_pair(-1,0),make_pair(1,0)));
		num_mirrors+=1;
		Obj_Id=Mir_Id;
		Index = num_mirrors-1;
	}
	if(key == 'p' ){
		num_projectors+=1;
		projector_pos.push_back(make_pair(make_pair(a,b),make_pair(c,e)));
		Pdistance.push_back(newD);
		num_lightRays.push_back(newN);
		lightSource.resize(num_projectors);
		LightRays.resize(num_projectors);
		findLightSource(num_projectors-1,newD);
		locateEndsOnProj(num_projectors-1,newN);
	}
	if(key == 'd' && Obj_Id!=No_Id){
		if(Obj_Id==Proj_Id){
			num_projectors-=1;
			projector_pos.erase(projector_pos.begin()+Index);
			Pdistance.erase(Pdistance.begin()+Index);
			num_lightRays.erase(num_lightRays.begin()+Index);
			lightSource.resize(num_projectors);
			LightRays.resize(num_projectors);
		}
		if(Obj_Id==Mir_Id){
			num_mirrors-=1;
			mirror_pos.erase(mirror_pos.begin()+Index);
		}
		if(Obj_Id==Blk_Id){
			num_blocks-=1;
			block_pos.erase(block_pos.begin()+Index);
		}
	}
	else if(key == 'P' && PlayFlag==0){
		PlayFlag=1;
		mirror_pos.clear();
		num_mirrors=0;
		block_pos.clear();
		num_blocks=0;
		projector_pos.resize(1);
		num_projectors=1;
		Pdistance.resize(1);
		num_lightRays.resize(1);
		lightSource.resize(1);
		LightRays.resize(1);
		Obj_Id=Proj_Id;
		Index=0;
	num_insects = INSECTS;
	removeI.clear();
	insect_pos.clear();
	insect_pos.resize(num_insects);
	insect_size.resize(num_insects,InsectSize);
	srand(time(NULL));
	for(i=0;i<num_insects;i++){
		if(i%4==0)
			insect_pos[i]=make_pair(rand() % 4,rand() % 4);
		if(i%4==1)
			insect_pos[i]=make_pair(-1*rand() % 4,rand() % 4);
		if(i%4==2)
			insect_pos[i]=make_pair(-1*rand() % 4,-1*rand() % 4);
		if(i%4==3)
			insect_pos[i]=make_pair(rand() % 4,-1*rand() % 4);
	}
	//printf("Coordinates,DistanceOfLightSource of Projectors:");

	}
	else if(key == 'P' && PlayFlag==1){
		PlayFlag=0;
		insect_pos.clear();
		insect_size.clear();
		removeI.clear();
	}
}

void handleKeypress2(int key, int x, int y) {
	int i;
	if(numberOfNames>0){
		if (key == GLUT_KEY_LEFT){
			if(Obj_Id==Proj_Id){
				projector_pos[Index].first.first -= translationSpeed;
				projector_pos[Index].second.first -= translationSpeed;
				lightSource[Index].first -= translationSpeed;
				for(i=0;i<(int)LightRays[Index].size();i++)
					LightRays[Index][i].first -= translationSpeed;
			}
			else if(Obj_Id==Mir_Id){
				mirror_pos[Index].first.first -= translationSpeed;
				mirror_pos[Index].second.first -= translationSpeed;
			}
			else if(Obj_Id==Blk_Id){
				block_pos[Index].first.first -= translationSpeed;
				block_pos[Index].second.first -= translationSpeed;
			}
		}
		if (key == GLUT_KEY_RIGHT){
			if(Obj_Id==Proj_Id){
				projector_pos[Index].first.first += translationSpeed;
				projector_pos[Index].second.first += translationSpeed;
				lightSource[Index].first += translationSpeed;
				for(i=0;i<(int)LightRays[Index].size();i++)
					LightRays[Index][i].first += translationSpeed;
			}
			else if(Obj_Id==Mir_Id){
				mirror_pos[Index].first.first += translationSpeed;
				mirror_pos[Index].second.first += translationSpeed;
			}
			else if(Obj_Id==Blk_Id){
				block_pos[Index].first.first += translationSpeed;
				block_pos[Index].second.first += translationSpeed;
			}
		}
		if (key == GLUT_KEY_UP){
			if(Obj_Id==Proj_Id){
				projector_pos[Index].first.second += translationSpeed;
				projector_pos[Index].second.second += translationSpeed;
				lightSource[Index].second += translationSpeed;
				for(i=0;i<(int)LightRays[Index].size();i++)
					LightRays[Index][i].second += translationSpeed;
			}
			else if(Obj_Id==Mir_Id){
				mirror_pos[Index].first.second += translationSpeed;
				mirror_pos[Index].second.second += translationSpeed;
			}
			else if(Obj_Id==Blk_Id){
				block_pos[Index].first.second += translationSpeed;
				block_pos[Index].second.second += translationSpeed;
			}
		}

		if (key == GLUT_KEY_DOWN){
			if(Obj_Id==Proj_Id){
				projector_pos[Index].first.second -= translationSpeed;
				projector_pos[Index].second.second -= translationSpeed;
				lightSource[Index].second -= translationSpeed;
				for(i=0;i<(int)LightRays[Index].size();i++)
					LightRays[Index][i].second -= translationSpeed;
			}
			else if(Obj_Id==Mir_Id){
				mirror_pos[Index].first.second -= translationSpeed;
				mirror_pos[Index].second.second -= translationSpeed;
			}
			else if(Obj_Id==Blk_Id){
				block_pos[Index].first.second -= translationSpeed;
				block_pos[Index].second.second -= translationSpeed;
			}
		}
	}
}

void handleMouseclick(int button, int state, int x, int y) {

	if (state == GLUT_DOWN)
	{
		cursorX = x;
		cursorY = y;
		mode = SELECT;
		//if (button == GLUT_LEFT_BUTTON)
		//	theta += 15;
		//else if (button == GLUT_RIGHT_BUTTON)
		//	theta -= 15;
	}
}
