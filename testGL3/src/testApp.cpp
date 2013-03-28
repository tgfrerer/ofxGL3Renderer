#include "testApp.h"

 #define USE_PROGRAMMABLE_GL

//--------------------------------------------------------------
void testApp::setup(){
	
	shouldDrawString = false;
#ifdef USE_PROGRAMMABLE_GL
	glGetError();

	mFlatNormalShader = ofPtr<ofShader>(new ofShader);
	string result = (mFlatNormalShader->load("withNormals")) ? "Loaded flat normals shader successfully" : "Awww. problem loading flat normals shader.";
	ofLogNotice() << result;
	
	ofLogNotice() << "normals shader";
	mFlatNormalShader->printActiveAttributes();
	mFlatNormalShader->printActiveUniforms();
	
	ProgrammableGLRenderer = ofPtr<ofProgrammableGLRenderer>(new ofProgrammableGLRenderer());

	ofSetCurrentRenderer(ProgrammableGLRenderer);
	ProgrammableGLRenderer->setup();
#endif

	mCam1.setupPerspective(true, 60, 0.1, 200);
	mCam1.setGlobalPosition(5, 5, 20);
	
	mCam1.disableMouseInput();
	mCam1.enableMouseInput();
	mCam1.setTranslationKey('m');
	mCam1.setDistance(20);

	ofEnableNormalizedTexCoords();
	mImg1.loadImage("stolen_pony.jpg");
	ofDisableNormalizedTexCoords();
	
	ofEnableArbTex();
	
	ofSetSphereResolution(20);
	
	mFbo1.allocate(ofGetViewportWidth(), ofGetViewportHeight(),GL_RGBA, 6);
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle( "GLFW Window: " + ofToString(ofGetFrameRate(),2) + "fps");
}

//--------------------------------------------------------------
void testApp::draw(){

//	ofSetupScreenPerspective(ofGetWidth(), ofGetHeight(), OF_ORIENTATION_DEFAULT, false);

#ifdef USE_PROGRAMMABLE_GL
	ProgrammableGLRenderer->startRender();
#endif
	
	
	mFbo1.begin(false);

    mCam1.begin();

	glEnable(GL_DEPTH_TEST);

	if(ofGetCoordHandedness()==OF_LEFT_HANDED)
	{
		glCullFace(GL_FRONT);
	}
	else {
		glCullFace(GL_BACK);
	}
		
	glEnable(GL_CULL_FACE);
	
	ofClear(128);


	ofNoFill();
	ofSetColor(255);

	
	ofPushMatrix();
	ofDrawAxis(10);
	ofDrawGrid(20);
	
	ofPushMatrix();
	ofTranslate(-4, 0);
	ofCircle(0, 0, 4);
	ofPopMatrix();
	
	ofNoFill();
	
	ofPushMatrix();
	ofTranslate(-4, 0);
	ofSetColor(0,255,0);
	ofCircle(0, 0, 6);
	ofPopMatrix();
	
	ofPushMatrix();
	ofPushMatrix();
	ofTranslate(3, -6);
	ofSetColor(0,0,255);
	ofRect(0, 0, 4, 12);
	ofPopMatrix();
	ofPushMatrix();
	ofTranslate(8, -2);
	ofSetColor(0,255,255);
	ofRect(0, 0, 3, 3);
	ofPopMatrix();
	ofTranslate(8, 2);
	ofSetColor(255,255,255);
	ofTriangle(ofVec3f(0,4), ofVec3f(0,0), ofVec3f(4,4));
	ofPopMatrix();

	ofNoFill();
	ofSetColor(255);

	ofPushMatrix();
	ofTranslate(ofVec3f(0,0,-1));
	mImg1.draw(0,0,10,10);
	ofPopMatrix();


	ofSetColor(0, 0, 255);
	glDisable(GL_CULL_FACE);
	ofDrawBitmapString("The (quick) brown \nfox jumps ^over^ \nthe *lazy* dog :-)", ofVec3f(2,2,0));
	glEnable(GL_CULL_FACE);
	ofSetColor(255);
#ifdef USE_PROGRAMMABLE_GL
	mFlatNormalShader->begin();
#endif
	
	ofPushMatrix();
	ofFill();
	ofDrawSphere(-4, 0, 0, 3);
	ofPushMatrix();
	ofFill();
	ofTranslate(0, - 8);
	ofScale(20.0, 1.0, 0.2);
	ofDrawBox(1);
	ofPopMatrix();
	ofPopMatrix();

	ofDrawCone(4, 0, 0, 0.5, 2);
#ifdef USE_PROGRAMMABLE_GL
	mFlatNormalShader->end();
#endif
	ofPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	
	vector<int> a;
	
	
	mCam1.end();
	mFbo1.end();

	ofSetColor(255);
	ofClear(255,0,0);
	ofDisableAlphaBlending();

	mFbo1.getTextureReference().draw(0, 0);

	ofDisableAlphaBlending();

#ifdef USE_PROGRAMMABLE_GL
	ProgrammableGLRenderer->finishRender();
#endif
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//	ofLogNotice() << "key pressed:  " << char(key);

#ifdef USE_PROGRAMMABLE_GL
	if (key == ' '){
		glGetError();

		mFlatNormalShader = ofPtr<ofShader>(new ofShader);
		string result = (mFlatNormalShader->load("passthrough")) ? "Loaded passthrough shader successfully" : "Awww. problem loading passthrough shader.";
		ofLogNotice() << result;
	}
#endif
	
	if (key == 'x'){
		shouldDrawString ^= true;
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
//	ofLogNotice() << "key released: " << char(key);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
//	ofLogNotice() << "mouseDragged";
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

//	ofLogNotice() << x << "|" << y;
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	mFbo1.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	ofLogNotice() << "gotMessage:" << msg.message;
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}