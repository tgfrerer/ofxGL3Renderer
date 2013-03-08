#pragma once

#include "ofMain.h"
#include "ofxGL3Renderer.h"
#include "ofProgrammableGLRenderer.h"

class testApp : public ofBaseApp{

	ofPtr<ofShader>	mPassThrough;

	ofEasyCam						mCam1;
	ofPtr<ofProgrammableGLRenderer> ProgrammableGLRenderer;
	ofPtr<ofxGL3Renderer>			GL3Renderer;
	
	ofImage							mImg1;
	
	ofFbo							mFbo1;
	
	bool shouldDrawString			;
	
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
