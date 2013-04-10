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

	// ofDisableSetupScreen();
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
	
	mFbo1.allocate(ofGetWidth(), ofGetHeight(),GL_RGBA);
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
	
	// for every triangle, we add up the s and t vector per vertex.

	// first find out how the triangles are drawn.
	
	if (mSphere.getMesh().hasIndices()){
		ofLogNotice() << "Vertices   " << mSphere.getMesh().getVertices().size();
		ofLogNotice() << "Normals:   " << mSphere.getMesh().getNormals().size();
		ofLogNotice() << "Texcoords: " << mSphere.getMesh().getTexCoords().size();
		ofLogNotice() << "Indices:   " << mSphere.getMesh().getIndices().size();
//		ofExit();
		
		// three indices at a time make a triangle.
		
		size_t vertexCount = mSphere.getMesh().getVertices().size();
		
		vector<ofVec3f> tanS(vertexCount, ofVec3f());	// pre-fill with vertexCount elements
		vector<ofVec3f> tanT(vertexCount, ofVec3f());

		tangents.resize(vertexCount, ofVec4f());
		
		for (int i = 0; i*3 < mSphere.getMesh().getIndices().size(); i++){

			// get the three vertex indices that form a triangle
			ofIndexType i1 = mSphere.getMesh().getIndices()[i*3];
			ofIndexType i2 = mSphere.getMesh().getIndices()[i*3 + 1];
			ofIndexType i3 = mSphere.getMesh().getIndices()[i*3 + 2];
			
			ofVec3f v1 = mSphere.getMesh().getVertex(i1);
			ofVec3f v2 = mSphere.getMesh().getVertex(i2);
			ofVec3f v3 = mSphere.getMesh().getVertex(i3);
			
			ofVec2f w1 = mSphere.getMesh().getTexCoord(i1);
			ofVec2f w2 = mSphere.getMesh().getTexCoord(i2);
			ofVec2f w3 = mSphere.getMesh().getTexCoord(i3);

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;

			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;
			
			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;
			
			float r = 1.0f / (s1 * t2 - s2 * t1);
			
			ofVec3f sDir ( r * (t2 * x1 - t1 * x2),
						   r * (t2 * y1 - t1 * y2),
						   r * (t2 * z1 - t1 * z2));
			
			ofVec3f tDir ( r * (s1 * x2 - s2 * x1),
						   r * (s1 * y2 - s2 * y1),
						   r * (s1 * z2 - s2 * z1));
			
			
			tanS[i1] += sDir;
			tanS[i2] += sDir;
			tanS[i3] += sDir;
			
			tanT[i1] += tDir;
			tanT[i2] += tDir;
			tanT[i3] += tDir;
			
		}
		

		
		for (size_t i = 0; i < vertexCount; i++){
			const ofVec3f& n = mSphere.getMesh().getNormals()[i];
			const ofVec3f& t = tanS[i];
			
			// Gram-Schmidt orthogonalize...
			
			tangents[i] = (t-n * n.dot(t)).getNormalized();
			
			// calculate handedness and store as w-cord
			
			tangents[i].w = (n.crossed(t).dot(tanT[i])) < 0.0f ? -1.0f : 1.0f;
			
			ofLogNotice() << tangents[i];
			
		}
		
		
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
	for (size_t i = 0; i<mSphere.getMesh().getVertices().size(); i++){
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

	for (int i = 0; i<mSphere.getMesh().getVertices().size(); i++){
		ofSetColor(0,0,255);
		ofLine(mSphere.getMesh().getVertex(i), mSphere.getMesh().getVertex(i) + 0.7 * mSphere.getMesh().getNormal(i));

		ofSetColor(0,255,0);
		ofLine(mSphere.getMesh().getVertex(i), mSphere.getMesh().getVertex(i) + 0.7 * ofVec3f(tangents[i]));

		ofSetColor(255,0,0);
		ofLine(mSphere.getMesh().getVertex(i), mSphere.getMesh().getVertex(i) + tangents[i].w * 0.7 * ofVec3f(tangents[i]).crossed(mSphere.getMesh().getNormal(i)));

		//		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
		// ofDrawBitmapString(ofToString(i), mSphere.getMesh().getVertices()[i]);
	}

	
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