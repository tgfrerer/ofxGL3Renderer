#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	shouldDrawString = false;
	glGetError();

	mFlatNormalShader = ofPtr<ofShader>(new ofShader);
	mFlatNormalShader->load("withNormals");
	
	ProgrammableGLRenderer = ofPtr<ofProgrammableGLRenderer>(new ofProgrammableGLRenderer());

	ofSetCurrentRenderer(ProgrammableGLRenderer);
//	ProgrammableGLRenderer->setup();

	ofDisableSetupScreen();
	mCam1.setupPerspective(false, 60, 0.1, 200);
	mCam1.setGlobalPosition(5, 5, 20);
	
	mCam1.disableMouseInput();
	mCam1.enableMouseInput();
	mCam1.setTranslationKey('m');
	mCam1.setDistance(20);

	
	ofDisableArbTex();
	mImg1.loadImage("earthNormalMap_2048.png");
	ofEnableArbTex();
	
	ofEnableArbTex();
	ofSetSphereResolution(20);
	
	mFbo1.allocate(ofGetViewportWidth(), ofGetViewportHeight(),GL_RGBA, 6);
	ofSetupScreenPerspective(ofGetViewportWidth(), ofGetViewportHeight(), OF_ORIENTATION_DEFAULT, false);
	ofGetWidth();
	
	// create cube mesh
	
	ofVec3f vertices[] = {
		
		ofVec3f(-1, -1,  1),		// front square vertices
		ofVec3f( 1, -1,  1),
		ofVec3f( 1,  1,  1),
		ofVec3f(-1,  1,  1),
		
		ofVec3f(-1, -1, -1),		// back square vertices
		ofVec3f( 1, -1, -1),
		ofVec3f( 1,  1, -1),
		ofVec3f(-1,  1, -1),
		
	};
	
	ofIndexType indices[] = {
		// -- winding is counter-clockwise (facing camera)
		0,1,2,		// pos z
		0,2,3,
		1,5,6,		// pos x
		1,6,2,
		2,6,7,		// pos y
		2,7,3,
		
		// -- winding is clockwise (facing away from camera)
		3,4,0,		// neg x
		3,7,4,
		4,5,1,		// neg y
		4,1,0,
		5,7,6,		// neg z
		5,4,7,
	};
	
	ofVec3f normals[] = {
		ofVec3f( 0,  0,  1),
		ofVec3f( 1,  0,  0),
		ofVec3f( 0,  1,  0),
		ofVec3f(-1,  0,  0),
		ofVec3f( 0, -1,  0),
		ofVec3f( 0,  0, -1),
		ofVec3f(1,0,0), // can be anything, will not be used
		ofVec3f(1,0,0), //  -- " --
	};
	
	mMshCube.addVertices(vertices, 8);
	mMshCube.addNormals(normals,8);
	mMshCube.addIndices(indices, 3*2*6);

//	mLight1.setParent(mCam1);
	
	mLight1.setGlobalPosition(9,
							  3.0,
							  0);

	mSphere.set(5,5, OF_PRIMITIVE_TRIANGLES);
	vector<ofVec3f> norm =  mSphere.getMesh().getNormals();
	vector<ofVec2f> tex = mSphere.getMesh().getTexCoords();
	
	if (mSphere.getMesh().hasIndices()){
		ofLogNotice() << "vertices" << mSphere.getMesh().getVertices().size();
		ofLogNotice() << "normals" << mSphere.getMesh().getNormals().size();
		ofLogNotice() << "texcoords: " << mSphere.getMesh().getTexCoords().size();
		ofLogNotice() << "has Indice:" << mSphere.getMesh().getIndices().size();
//		ofExit();
	}
	
	
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle( "GLFW Window: " + ofToString(ofGetFrameRate(),2) + "fps");
}

//--------------------------------------------------------------
void testApp::draw(){


	ProgrammableGLRenderer->startRender();
	
	mFbo1.begin(false);
	

	ofClear(64);
	ofBackgroundGradient(ofColor(240), ofColor(255,0,0));
    mCam1.begin();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	
	

	ofFill();
	ofSetColor(255);

	mLight1.setGlobalPosition(sin( (ofGetElapsedTimeMillis()%5000) * TWO_PI / 5000.0 ) * 9,
							  3.0,
							  cos( (ofGetElapsedTimeMillis()%5000) * TWO_PI / 5000.0 ) * 9);

	ofPushMatrix();
	ofMultMatrix( mLight1.getGlobalTransformMatrix() );
	ofDrawBox(0.1);
	ofPopMatrix();
	
	ofPushMatrix();
	mFlatNormalShader->begin();
	ofVec4f lightPos = mLight1.getGlobalPosition();
	lightPos.w = 1.0;	// this is a position !!!
	lightPos = lightPos * mCam1.getGlobalTransformMatrix().getInverse();
 
	mFlatNormalShader->setUniform4fv("lightPositionInCameraSpace", &lightPos.x);
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);		// OpenGL default is GL_LAST_VERTEX_CONVENTION

	mFlatNormalShader->setUniformTexture("src_tex_unit0", mImg1, 0);
//	ofDrawIcoSphere(0, 0, 0, 5);
//	ofDrawSphere(5);
	mSphere.draw();
	glDisable(GL_CULL_FACE);
	for (int i = 0; i<mSphere.getMesh().getVertices().size(); i++){
//		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
		// ofDrawBitmapString(ofToString(i), mSphere.getMesh().getVertices()[i]);
	}
	
//	ofDrawBitmapString("test", ofVec3f(0));
	
//	ofPushMatrix();
//	ofScale(5, 5, 5);
//	// 	ofDrawBox(5);
//	mMshCube.draw();
//	ofPopMatrix();

	mFlatNormalShader->end();
	
	ofSetColor(255);
	ofLine(mLight1.getGlobalPosition(),ofVec3f(0,0,0));
	ofPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	
	ofSetColor(255);
	ofDrawBitmapString("inside camera draw 100,100,0", ofVec3f(100,100,0));
	mCam1.end();
	ofSetColor(255);
	ofDrawBitmapString("inside fbo draw 100,100,0", ofVec3f(100,100,0));
	mFbo1.end();

	ofSetColor(255);
	ofClear(255,0,0);
	ofDisableAlphaBlending();

	mFbo1.getTextureReference().draw(0, 0);

	ofDisableAlphaBlending();

	ofSetColor(255);
	ofDrawBitmapString("standard draw 100,100,0", ofVec3f(100,100,0));
	ProgrammableGLRenderer->finishRender();
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//	ofLogNotice() << "key pressed:  " << char(key);

	if (key == ' '){
		glGetError();

		mFlatNormalShader = ofPtr<ofShader>(new ofShader);
		string result = (mFlatNormalShader->load("bump")) ? "Loaded bump shader successfully" : "Awww. problem loading bump shader.";
		ofLogNotice() << result;
		
		mFlatNormalShader->printActiveUniforms();
		mFlatNormalShader->printActiveAttributes();
	}
	
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
	mFbo1.allocate(ofGetViewportWidth(), ofGetViewportHeight());
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	ofLogNotice() << "gotMessage:" << msg.message;
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	ofLogNotice() << dragInfo.position;
}