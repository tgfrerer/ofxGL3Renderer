#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	glGetError();
	
	mPassThrough = ofPtr<ofShader>(new ofShader);
	
	string result = (mPassThrough->load("passthrough")) ? "Loaded passthrough shader successfully" : "Awww. problem loading passthrough shader.";
	ofLogNotice() << result;
	
	GL3Renderer = ofPtr<ofxGL3Renderer>(new ofxGL3Renderer());
	
	ofSetCurrentRenderer(GL3Renderer);
	
	mCam1.setupPerspective(false, 60, 0.1, 200);
	mCam1.setGlobalPosition(5, 5, 20);
	
	mCam1.disableMouseInput();
	mCam1.enableMouseInput();
	mCam1.setTranslationKey('m');
	mCam1.setDistance(20);

//	mPassThrough->printActiveAttributes();
//	mPassThrough->printActiveUniforms();

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
	ofSetColor(255,0,0);
	
	ofPushMatrix();
	ofTranslate(-4, 0);
	ofCircle(0, 0, 5);
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