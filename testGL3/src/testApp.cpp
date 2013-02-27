#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	glGetError();
	
	mPassThrough = ofPtr<ofShader>(new ofShader);
	
	string result = (mPassThrough->load("passthrough")) ? "OK" : "NOPE";
	ofLogNotice() << result;
	
	GL3Renderer = ofPtr<ofxGL3Renderer>(new ofxGL3Renderer());
	
	ofSetCurrentRenderer(GL3Renderer);
	
	mProjection.makePerspectiveMatrix(60, ofGetWidth()/(float)ofGetHeight(), 0.1, 200);
	mModelview.makeIdentityMatrix();
	mModelview.translate(-5, -5, -20);
	
	mCam1.setupPerspective(false, 60, 0.1, 200);
	mCam1.setGlobalPosition(5, 5, 20);

	
	mCam1.disableMouseInput();
	mCam1.enableMouseInput();
	mCam1.setTranslationKey('m');
	mCam1.setDistance(20);

	mPassThrough->printActiveAttributes();
	mPassThrough->printActiveUniforms();

}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle( "GLFW Window: " + ofToString(ofGetFrameRate(),2) + "fps");
}

//--------------------------------------------------------------
void testApp::draw(){

	mCam1.begin();
	
	GL3Renderer->beginShader(mPassThrough);
	
	ofClear(0,0,0);

	ofFill();
	
	ofPushMatrix();
	ofTranslate(-3, 0);
	ofCircle(0, 0, 5);
	ofPopMatrix();
	
	ofNoFill();

	ofPushMatrix();
	ofTranslate(-3, 0);
	ofCircle(0, 0, 6);
	ofPopMatrix();
	
	ofTriangle(ofVec3f(0,0), ofVec3f(7,0), ofVec3f(0,7));
	
	GL3Renderer->endShader();
	mCam1.end();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//	ofLogNotice() << "key pressed:  " << char(key);
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}