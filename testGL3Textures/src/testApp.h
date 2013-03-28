#pragma once

#include "ofMain.h"
#include "ofProgrammableGLRenderer.h"

class testApp : public ofBaseApp{

	ofPtr<ofShader>	mFlatNormalShader;

	ofEasyCam						mCam1;
	ofPtr<ofProgrammableGLRenderer> ProgrammableGLRenderer;
	ofImage							mImg1;
	ofFbo							mFbo1;
	ofNode							mLight1;
	ofVboMesh						mMshCube;

	ofSpherePrimitive				mSphere;
	
	bool shouldDrawString;

	GLuint biNormalVboId;
	GLuint tangentVboId;
	
	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
