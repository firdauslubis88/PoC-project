#pragma once

#include "ofMain.h"
#include "MSAPhysics3D.h"

using namespace msa::physics;

#define UNITS(s)                (s * ofGetWidth() / 1280.0) // scale to uniform units

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

#define	SPRING_MIN_WIDTH		UNITS(1)
#define SPRING_MAX_WIDTH		UNITS(3)

#define NODE_MIN_RADIUS			UNITS(5)
#define NODE_MAX_RADIUS			UNITS(15)

#define MIN_MASS				1
#define MAX_MASS				3

#define MIN_BOUNCE				0.2
#define MAX_BOUNCE				0.9

#define	FIX_PROBABILITY			10		// % probability of a particle being fixed on creation

#define FORCE_AMOUNT			UNITS(10)

#define EDGE_DRAG				0.98

#define	GRAVITY					1

#define MAX_ATTRACTION			10
#define MIN_ATTRACTION			3

#define SECTOR_COUNT			1		// currently there is a bug at sector borders, so setting this to 1

class ofApp : public ofBaseApp{

	public:

		bool				mouseAttract = false;
		bool				doMouseXY = false;		// pressing left mmouse button moves mouse in XY plane
		bool				doMouseYZ = false;		// pressing right mouse button moves mouse in YZ plane
		bool				doRender = true;
		int					forceTimer = false;

		float				rotSpeed = 0;
		float				mouseMass = 1;

		int             	width;
		int         		height;

		ofImage				ballImage;
		World3D_ptr         world;
		Particle3D_ptr      mouseNode;

		void setupLighting();

		void setup();
		void addRandomParticle();
		void addRandomSpring();
		void killRandomParticle();
		void killRandomSpring();
		void toggleMouseAttract();
		void addRandomForce(float f);
		void lockRandomParticles();
		void unlockRandomParticles();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void mouseReleased();
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void initScene();
		
};
