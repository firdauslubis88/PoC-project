/*
 *
 *	Fast Robust Matching Algorithm (User Interface)
 *
 *
 *	@Imaging Media Research Center @ KIST
 *	@date 07/29/11
 *	
 */


//-----------------------------------------------------------------------------
// includes

#ifdef WIN32
#include <windows.h>
#endif

#include "FastRobustMatching.h"

#include <stdio.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <opencv/highgui.h>

#include "cal3d2.h"
#include "ObjLoader.h"
#include "MMSystem.h"


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// definitions

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define CAMERA_WIDTH	640
#define CAMERA_HEIGHT	480

//#define FULL_SCREEN
//#define DRAW_MATCH_POINTS
#define MIN_MATCH_BUFFER	0
#define MAX_MATCH_BUFFER	30


// Video Data (Camera Unused)
//#define VIDEO_FILE	"./data/image_sequence.avi"

// AR Resources
#define USE_3D_MODEL	1
#define OBJ_FILE		"./data/kunjungjun.obj"
//#define OBJ_FILE		"./data/kunjungjun_2.obj"
#define CFG_FILE		"./data/king.cfg"
#define WAV_FILE		"./data/kun_sound.wav"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global variables

model obj_model;
Cal3d cfg_model("");


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GLUT callbacks and functions

void initGlut(int argc, char **argv);
void displayFunc(void);
void idleFunc(void);
void reshapeFunc(int width, int height);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
//-----------------------------------------------------------------------------

// other [OpenGL] functions
void countFrames(void);
void renderBitmapString(float x, float y, float z, void *font, char *string);
void renderTexture(float x, float y, IplImage *image, GLuint &textureID);

//-----------------------------------------------------------------------------

bool bFullsreen = false;
int nWindowID;

//-----------------------------------------------------------------------------

// parameters for the framecounter
char pixelstring[30];
int cframe = 0;
int elapsed_time = 0;
int timebase = 0;

//-----------------------------------------------------------------------------

// OpenCV variables

CvCapture *cvCapture = 0;

GLuint windowWidth;
GLuint windowHeight;

//IplImage* inputImage = NULL;
IplImage* newImage = NULL;
IplImage* newImageCrop = NULL;

GLuint inputImageTextureID;
GLuint cameraImageTextureID;

//-----------------------------------------------------------------------------

bool bInit = false;
int match_buffer = MIN_MATCH_BUFFER;

//-----------------------------------------------------------------------------

// Feb. 2011, Min-Hyuk Sung
void RedColor()		{ glColor4f(192.0f / 256.0f,   0.0f / 256.0f,   0.0f / 256.0f, 1.0f); }
void GreenColor()	{ glColor4f(  0.0f / 256.0f, 176.0f / 256.0f,  80.0f / 256.0f, 1.0f); }
void BlueColor()	{ glColor4f(  0.0f / 256.0f, 112.0f / 256.0f, 192.0f / 256.0f, 1.0f); }
void YellowColor()	{ glColor4f(255.0f / 256.0f, 192.0f / 256.0f,   0.0f / 256.0f, 1.0f); }
void VioletColor()	{ glColor4f(112.0f / 256.0f,  48.0f / 256.0f, 160.0f / 256.0f, 1.0f); }
void BlackColor()	{ glColor4f(  0.0f / 256.0f,  32.0f / 256.0f,  96.0f / 256.0f, 1.0f); }

//-----------------------------------------------------------------------------

void renderTexture(float x, float y, IplImage *image, GLuint &textureID) {
	glPushMatrix();
	glTranslatef(x, y, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);

	if(newImage->nChannels == 3)
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, image->width, image->height, 0, GL_BGR, GL_UNSIGNED_BYTE, image->imageData);
	else if(newImage->nChannels == 4)
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, image->width, image->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image->imageData);

	glBegin(GL_QUADS);
	glTexCoord2i(0,0);
	glVertex2i(0,0);
	glTexCoord2i(image->width,0);
	glVertex2i(image->width,0);
	glTexCoord2i(image->width,image->height);
	glVertex2i(image->width,image->height);
	glTexCoord2i(0,image->height);
	glVertex2i(0,image->height);

	/*
	glTexCoord2i(0,0);
	glVertex2i(0,image->height);
	glTexCoord2i(image->width,0);
	glVertex2i(image->width,image->height);
	glTexCoord2i(image->width,image->height);
	glVertex2i(image->width,0);
	glTexCoord2i(0,image->height);
	glVertex2i(0,0);
	*/
	glEnd();

	glPopMatrix();
}

// Feb. 2012, Min-Hyuk Sung
void render_3D_model(unsigned int width, unsigned int height) {

	// July 2011, Min-Hyuk Sung
	unsigned int size = min(width, height);

	glPushMatrix();
	glTranslatef(0.5*width, 0.5*height, 0);
	glTranslatef(0, 0.4*height, 50);
	glRotatef(180, 1, 0, 0);
	glScalef(0.025*size, 0.025*size, 0.025*size);
	//glTranslatef(12, -48, 12);
	//glScalef(3, 3, 3);

	glPushMatrix();
	glTranslatef(0, 0.006*height, 20);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.02*size, 0.02*size, 0.02*size);
	cfg_model.Render();
	glPopMatrix();

	glPushMatrix();
	obj_model.draw();
	glPopMatrix();

	glPopMatrix();
}

//-----------------------------------------------------------------------------
void displayFunc(void) {

	if(!bInit) {
		bInit = true;
	}

	newImage = cvQueryFrame( cvCapture );

	if( !( (newImage->width > 0) && (newImage->height > 0) ) )	return;

	// Aug. 2011, Min-hyuk Sung
	// For image ratio problem...
	if(newImage->width > CAMERA_WIDTH || newImage->height > CAMERA_HEIGHT)
	{
		newImageCrop = cvCreateImage(cvSize(CAMERA_WIDTH, CAMERA_HEIGHT),
			newImage->depth, newImage->nChannels);

		// ROI is defined in 'initGlut' function
		cvCopy(newImage, newImageCrop, NULL);
	}
	else
	{
		newImageCrop = newImage;
	}

	//glViewport(0,0,windowWidth,windowHeight);			// Reset The Current Viewport

	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// -- draw textures -- //
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0.0,(GLdouble)newImage->width,0.0,(GLdouble)newImage->height);	
	//gluOrtho2D(0.0,windowWidth,0.0,windowHeight);
	// A Texture is needed to be flipped
	gluOrtho2D(0.0,CAMERA_WIDTH,CAMERA_HEIGHT,0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// camera image
	renderTexture(0, 0, newImageCrop, cameraImageTextureID);

	// input image
	//renderTexture(newImage->width, 0, inputImage, inputImageTextureID);

	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	// ---- //

	// Feb. 2011, Min-Hyuk Sung
	bool isMatched = FastRobustMatching::matching(newImageCrop);
	
	if( isMatched ) match_buffer = min(match_buffer + 1, MAX_MATCH_BUFFER);
	else			match_buffer = max(match_buffer - 1, MIN_MATCH_BUFFER);

	// Dec. 2011, Min-Hyuk Sung
	//
#ifdef DRAW_MATCH_POINTS
	FastRobustMatching::MatchPair pairs;
	FastRobustMatching::get_matched_inlier_pairs(pairs);
	glPointSize(8);
	glBegin(GL_POINTS);
	GreenColor();
	for(FastRobustMatching::MatchPair::iterator it = pairs.begin(); it != pairs.end(); ++it)
		glVertex2d(it->second.x, it->second.y);
	glEnd();
#endif
	//

	if( isMatched )
	{
		double corners[4][2];
		FastRobustMatching::get_box_corners(corners);
		glLineWidth(4);
		glBegin(GL_LINE_LOOP);
		RedColor();
		for(int i = 0; i < 4; i++)
			glVertex2d(corners[i][0], corners[i][1]);
		glEnd();
	}

	// -- draw models -- //
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();
	// In order to use OpenGL function (gluPerspective),
	// Modelview Matrix should be transformed from (x, y, z) -> (x, -y, -z)
	// 'fovy' should be obtained from the 'real' camera focal length.
	// Please see 'FastRobust.cpp' and 'ATANCamera.cc'
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,10.0f,5000.0f);
	//glLoadMatrixd(FastRobustMatching::get_frustum_matrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixd(FastRobustMatching::get_modelview_matrix());

	if( isMatched )
	{
		/*
		double corners[4][3];
		FastRobust::GetBoxCorners3D(corners);
		glLineWidth(4);
		glBegin(GL_LINE_LOOP);
		GreenColor();
		for(int i = 0; i < 4; i++)
			glVertex3d(corners[i][0], corners[i][1], corners[i][2]);
		glEnd();
		*/

		//double axes[4][3];
		//match->GetAxes(axes);

		/*
		glLineWidth(4);
		glBegin(GL_LINES);
		BlueColor();
		glVertex3d(0, 0, 0);
		glVertex3d(0.5, 0, 0);
		glEnd();

		glBegin(GL_LINES);
		GreenColor();
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0.5, 0);
		glEnd();

		glBegin(GL_LINES);
		RedColor();
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 0.5);
		glEnd();
		*/

		glEnable( GL_DEPTH_TEST );
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		/*
		GLfloat	lightpos[4] = { 50.0, 150.0, 100.0, 1.0 }; 
		float viewerPosition[3]		= { 0.0, 0.0, -10 };
		GLfloat teapotSize = 20;
		//GLfloat	lightpos[4] = { 5.0, 15.0, 10.0, 1.0 }; 
		//float viewerPosition[3]		= { 0.0, 0.0, -0.1 };
		//GLfloat teapotSize = 0.2;

		/*
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef( viewerPosition[0], viewerPosition[1], viewerPosition[2] );
		glutSolidTeapot(teapotSize);
		glPopMatrix();
		*/

#if USE_3D_MODEL
		// Set target image size
		//render_3D_model(640, 480);
		render_3D_model(1280, 960);
#else
		GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,-200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

		glTranslatef( 320, 240, 0 );
		glutSolidCube(200.0);
		//glTranslatef( 0.0, 0.0, 0.05 );
		//glutSolidCube(0.1);
#endif
		glDisable( GL_DEPTH_TEST );
	}
	// ---- //

#ifdef WAV_FILE
	if( match_buffer == 1 )	// New start of match sequence
		PlaySound("./data/kun_sound.wav", 0, SND_ASYNC);
	if( match_buffer == 0 )
		PlaySound(NULL, 0, 0);
#endif

	if(newImage != newImageCrop)
	{
		cvReleaseImage(&newImageCrop);
		// newImage is not released (refer to 'cvQueryFrame' function)
	}

	countFrames();

	glutSwapBuffers();
}

//-----------------------------------------------------------------------------
void initGlut(int argc, char **argv) {

	// GLUT Window Initialization:
	glutInit (&argc, argv);

	/*
	inputImage = cvLoadImage(IMAGE_FILE);
	if(!inputImage)
	{
		printf("error: A input image does not exist: %s\n", IMAGE_FILE);
		system("pause");
		exit(-1);
	}
	*/

	// initialize 1st camera on the bus
	cvCapture = cvCreateCameraCapture(1);
#ifndef VIDEO_FILE
	cvCapture = cvCaptureFromCAM(0);
#else
	cvCapture = cvCaptureFromAVI(VIDEO_FILE);
#endif
	if(!cvCapture)
	{
		printf("error: A camera does not exist\n");
		system("pause");
		exit(-1);
	}
	cvSetCaptureProperty(cvCapture, CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
	cvSetCaptureProperty(cvCapture, CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

	// Aug. 2011, Min-hyuk Sung
	// For image ratio problem...
	newImage = cvQueryFrame( cvCapture );
	if(newImage->width > CAMERA_WIDTH || newImage->height > CAMERA_HEIGHT)
	{
		int width_offset = (newImage->width - CAMERA_WIDTH) / 2;
		int height_offset = (newImage->height - CAMERA_HEIGHT) / 2;
		cvSetImageROI(newImage, cvRect(width_offset, height_offset, CAMERA_WIDTH, CAMERA_HEIGHT));
	}

	//windowWidth = inputImage->width + CAMERA_WIDTH;
	//windowHeight = max(inputImage->height, CAMERA_HEIGHT);
	windowWidth = CAMERA_WIDTH;
	windowHeight = CAMERA_HEIGHT;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	nWindowID = glutCreateWindow ("FastRobust");

#if defined(FULL_SCREEN) && defined(WIN32)
	HWND hwnd = FindWindow(NULL, "FastRobust");
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_MAXIMIZE);
	SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, SWP_SHOWWINDOW);
#endif

	// Register callbacks:
	glutDisplayFunc		(displayFunc);
	glutReshapeFunc		(reshapeFunc);
	glutKeyboardFunc	(keyboardFunc);
	glutSpecialFunc		(specialFunc);
	glutMouseFunc		(mouseFunc);
	glutMotionFunc		(mouseMotionFunc);
	glutIdleFunc		(idleFunc);

	// initialze OpenGL texture		
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	glGenTextures(1, &inputImageTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, inputImageTextureID);

	glGenTextures(1, &cameraImageTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cameraImageTextureID);

	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
#if USE_3D_MODEL
	// July 2011, Min-Hyuk Sung
	// ---- 3D Model Initialization ---- //
	if( !obj_model.Load(OBJ_FILE) || !cfg_model.Load(CFG_FILE) )
	{
		system("pause");
		exit(-1);
	}
	// ---- //
#endif

	// Dec. 2011, Min-Hyuk Sung
	if(argc < 2)
	{
		std::cerr << "error: Feature database file is required." << std::endl;
		system("pause");
		exit(-1);
	}
	std::string feature_file = std::string(argv[1]);
	//

	//Feb. 2011, Min-Hyuk Sung
	if( !FastRobustMatching::initialize(feature_file.c_str(), CAMERA_WIDTH, CAMERA_HEIGHT) )
	{
		system("pause");
		exit(-1);
	}
}

//-----------------------------------------------------------------------------

void idleFunc(void) {
#if USE_3D_MODEL
	// July 2011
	cfg_model.Update();
#endif
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------

void reshapeFunc(int width, int height) {

	glViewport(0, 0, width, height);

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f,100.0f);
	*/

	glMatrixMode(GL_MODELVIEW);

}

//-----------------------------------------------------------------------------


// mouse callback
void mouseFunc(int button, int state, int x, int y) {
	
}

//-----------------------------------------------------------------------------

void mouseMotionFunc(int x, int y) {
	
}

//-----------------------------------------------------------------------------

void keyboardFunc(unsigned char key, int x, int y) {
    
	switch(key) {
		
		// -----------------------------------------

#ifdef WIN32
		// exit on escape
		case '\033':

			if(bInit) {
				glDeleteTextures(1, &inputImageTextureID);
				glDeleteTextures(1, &cameraImageTextureID);
				cvReleaseCapture( &cvCapture );
			}
			exit(0);
			break;
#endif

		// -----------------------------------------
			
		// switch to fullscreen
		case 'f':

			bFullsreen = !bFullsreen;
			if(bFullsreen) {
				glutFullScreen();
				windowWidth = glutGet(GLUT_SCREEN_WIDTH);
				windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
			}
			else {
				glutSetWindow(nWindowID);
				glutPositionWindow(100, 100);
				glutReshapeWindow(CAMERA_WIDTH, CAMERA_HEIGHT);
				windowWidth = CAMERA_WIDTH;
				windowHeight = CAMERA_HEIGHT;
			}
			break;

		// -----------------------------------------
	}
}

//-----------------------------------------------------------------------------

void specialFunc(int key, int x, int y) {
	//printf("key pressed: %d\n", key);
}

//-----------------------------------------------------------------------------

void countFrames(void)  {

	elapsed_time=glutGet(GLUT_ELAPSED_TIME);
	cframe++;
	if (elapsed_time - timebase > 50) {
		sprintf(pixelstring, "fps: %4.2f", cframe*1000.0/(elapsed_time-timebase));		
		timebase = elapsed_time;
		cframe = 0;
	// Draw status text and uni-logo:
	} 
	glDisable(GL_LIGHTING);
	glColor4f(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 200, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// render the string
	renderBitmapString(5,5,0.0,GLUT_BITMAP_HELVETICA_10,pixelstring);
		
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------

void renderBitmapString(float x, float y, float z, void *font, char *string) {
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

//-----------------------------------------------------------------------------


void main(int argc, char **argv) {

	initGlut(argc, argv);
	glutMainLoop();
}



