#include "ofxGL3Renderer.h"
#include "ofMesh.h"
#include "ofPath.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofMesh.h"
#include "ofBitmapFont.h"
#include "ofGLUtils.h"
#include "ofImage.h"
#include "ofFbo.h"
#include "ofShader.h"


//----------------------------------------------------------
ofxGL3Renderer::ofxGL3Renderer(bool useShapeColor)
: currentMatrix(&currentModelViewMatrix){
	bBackgroundAuto = true;

	linePoints.resize(2);
	rectPoints.resize(4);
	triPoints.resize(3);

	currentFbo = NULL;
	
	glGenVertexArrays(1, &defaultVAO);	// generate one default VAO into which we can store the VBOs for our built-in primitives.
	
}

//----------------------------------------------------------
void ofxGL3Renderer::update(){

}

// ----------------------------------------------------------------------

/// tig: The current GLSL shader should really be a member of the renderer,
/// since only openGL uses shaders, and different opengl versions allow
/// different shader language versions they can't be seen to be too far decoupled.

void ofxGL3Renderer::beginShader(shaderP_t shader_){

	if (shader_.get() == NULL ){
		ofLogWarning() << "cannot bind NULL shader";
		return;
	}
	
	if (currentShader.get() != NULL &&  shader_ != currentShader){
		// we currently have another shader bound.
		// let's end the current shader so that a new one can be bound.
		currentShader->end();
	}

	if (shader_ != currentShader) {
		// if we are about to bind a new shader, we first want to update the location cache.
		GLuint shaderProg = shader_->getProgram();
		// get uniform locations and cache them.
		// get attribute locations and cache them.
		shaderLocCache.locUniformProjectionMatrix	= glGetUniformLocation(shaderProg, "projectionMatrix");
		shaderLocCache.locUniformModelViewMatrix	= glGetUniformLocation(shaderProg, "modelViewMatrix");
		shaderLocCache.locAttributeVVertex			= glGetAttribLocation(shaderProg, "vVertex");
	}

	currentShader = shader_;

	currentShader->begin();
	// this will preset the current shader with the current matrices
	shaderSetupModelViewProjectionMatrices();
}

// ----------------------------------------------------------------------

void ofxGL3Renderer::endShader(){
	currentShader->end();
}

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------

void ofxGL3Renderer::draw(ofMesh & vertexData, bool useColors, bool useTextures, bool useNormals){
	if(vertexData.getNumVertices()){
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &vertexData.getVerticesPointer()->x);
//		glEnableVertexAttribArray(0);
	}
	if(vertexData.getNumNormals() && useNormals){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(ofVec3f), &vertexData.getNormalsPointer()->x);
	}
	if(vertexData.getNumColors() && useColors){
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT, sizeof(ofFloatColor), &vertexData.getColorsPointer()->r);
	}

	if(vertexData.getNumTexCoords() && useTextures){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(ofVec2f), &vertexData.getTexCoordsPointer()->x);
	}

	if(vertexData.getNumIndices()){
#ifdef TARGET_OPENGLES
		glDrawElements(ofGetGLPrimitiveMode(vertexData.getMode()), vertexData.getNumIndices(),GL_UNSIGNED_SHORT,vertexData.getIndexPointer());
#else
		glDrawElements(ofGetGLPrimitiveMode(vertexData.getMode()), vertexData.getNumIndices(),GL_UNSIGNED_INT,vertexData.getIndexPointer());
#endif
	}else{
		glDrawArrays(ofGetGLPrimitiveMode(vertexData.getMode()), 0, vertexData.getNumVertices());
	}
	if(vertexData.getNumColors()){
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(vertexData.getNumNormals()){
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(vertexData.getNumTexCoords()){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors, bool useTextures, bool useNormals){
		if (bSmoothHinted) startSmoothing();
#ifndef TARGET_OPENGLES
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, ofGetGLPolyMode(renderType));
		draw(vertexData,useColors,useTextures,useNormals);
		glPopAttrib(); //TODO: GLES doesnt support polygon mode, add renderType to gl renderer?
#else
		if(vertexData.getNumVertices()){
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), vertexData.getVerticesPointer());
		}
		if(vertexData.getNumNormals() && useNormals){
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, vertexData.getNormalsPointer());
		}
		if(vertexData.getNumColors() && useColors){
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT, sizeof(ofFloatColor), vertexData.getColorsPointer());
		}

		if(vertexData.getNumTexCoords() && useTextures){
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, vertexData.getTexCoordsPointer());
		}

		GLenum drawMode;
		switch(renderType){
		case OF_MESH_POINTS:
			drawMode = GL_POINTS;
			break;
		case OF_MESH_WIREFRAME:
			drawMode = GL_LINES;
			break;
		case OF_MESH_FILL:
			drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
			break;
		default:
			drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
			break;
		}

		if(vertexData.getNumIndices()){
			glDrawElements(drawMode, vertexData.getNumIndices(),GL_UNSIGNED_SHORT,vertexData.getIndexPointer());
		}else{
			glDrawArrays(drawMode, 0, vertexData.getNumVertices());
		}
		if(vertexData.getNumColors() && useColors){
			glDisableClientState(GL_COLOR_ARRAY);
		}
		if(vertexData.getNumNormals() && useNormals){
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		if(vertexData.getNumTexCoords() && useTextures){
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
#endif
		if (bSmoothHinted) endSmoothing();
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(vector<ofPoint> & vertexData, ofPrimitiveMode drawMode){
	if(!vertexData.empty()) {
		if (bSmoothHinted) startSmoothing();
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &vertexData[0].x);
		glDrawArrays(ofGetGLPrimitiveMode(drawMode), 0, vertexData.size());
		if (bSmoothHinted) endSmoothing();
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofPolyline & poly){
	if(!poly.getVertices().empty()) {
		// use smoothness, if requested:
		if (bSmoothHinted) startSmoothing();

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &poly.getVertices()[0].x);
		glDrawArrays(poly.isClosed()?GL_LINE_LOOP:GL_LINE_STRIP, 0, poly.size());

		// use smoothness, if requested:
		if (bSmoothHinted) endSmoothing();
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofPath & shape){
	ofColor prevColor;
	if(shape.getUseShapeColor()){
		prevColor = ofGetStyle().color;
	}
	if(shape.isFilled()){
		ofMesh & mesh = shape.getTessellation();
		if(shape.getUseShapeColor()){
			setColor( shape.getFillColor() * ofGetStyle().color,shape.getFillColor().a/255. * ofGetStyle().color.a);
		}
		draw(mesh);
	}
	if(shape.hasOutline()){
		float lineWidth = ofGetStyle().lineWidth;
		if(shape.getUseShapeColor()){
			setColor( shape.getStrokeColor() * ofGetStyle().color, shape.getStrokeColor().a/255. * ofGetStyle().color.a);
		}
		setLineWidth( shape.getStrokeWidth() );
		vector<ofPolyline> & outlines = shape.getOutline();
		for(int i=0; i<(int)outlines.size(); i++)
			draw(outlines[i]);
		setLineWidth(lineWidth);
	}
	if(shape.getUseShapeColor()){
		setColor(prevColor);
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh){
	if(image.isUsingTexture()){
		ofTexture& tex = image.getTextureReference();
		if(tex.bAllocated()) {
			tex.drawSubsection(x,y,z,w,h,sx,sy,sw,sh);
		} else {
			ofLogWarning() << "ofxGL3Renderer::draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofFloatImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh){
	if(image.isUsingTexture()){
		ofTexture& tex = image.getTextureReference();
		if(tex.bAllocated()) {
			tex.drawSubsection(x,y,z,w,h,sx,sy,sw,sh);
		} else {
			ofLogWarning() << "ofxGL3Renderer::draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::draw(ofShortImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh){
	if(image.isUsingTexture()){
		ofTexture& tex = image.getTextureReference();
		if(tex.bAllocated()) {
			tex.drawSubsection(x,y,z,w,h,sx,sy,sw,sh);
		} else {
			ofLogWarning() << "ofxGL3Renderer::draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::setCurrentFBO(ofFbo * fbo){
	currentFbo = fbo;
}

//----------------------------------------------------------
void ofxGL3Renderer::pushView() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	ofRectangle currentViewport;
	currentViewport.set(viewport[0], viewport[1], viewport[2], viewport[3]);
	viewportHistory.push(currentViewport);


	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();*/


	// done like this cause i was getting GL_STACK_UNDERFLOW
	// should ofPush/PopMatrix work the same way, what if it's mixed with glPush/PopMatrix
	matrixMode(OF_MATRIX_PROJECTION);
	pushMatrix();	// will push projection matrix
	matrixMode(OF_MATRIX_MODELVIEW);
	pushMatrix();	// will push modelview matrix
	
//	ofMatrix4x4 m;
//	glGetFloatv(GL_PROJECTION_MATRIX,m.getPtr());
//	projectionStack.push(m);
//	glGetFloatv(GL_MODELVIEW_MATRIX,m.getPtr());
//	modelViewStack.push(m);
}


//----------------------------------------------------------
void ofxGL3Renderer::popView() {
	if( viewportHistory.size() ){
		ofRectangle viewRect = viewportHistory.top();
		viewport(viewRect.x, viewRect.y, viewRect.width, viewRect.height,false);
		viewportHistory.pop();
	}

	/*glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();*/

	// done like this cause i was getting GL_STACK_UNDERFLOW
	// should ofPush/PopMatrix work the same way, what if it's mixed with glPush/PopMatrix
//	glMatrixMode(GL_PROJECTION);
//	if(!projectionStack.empty()){
//		glLoadMatrixf(projectionStack.top().getPtr());
//		projectionStack.pop();
//	}else{
//		ofLogError() << "popView: couldn't pop projection matrix, stack empty. probably wrong anidated push/popView";
//	}
//	glMatrixMode(GL_MODELVIEW);
//	if(!modelViewStack.empty()){
//		glLoadMatrixf(modelViewStack.top().getPtr());
//		modelViewStack.pop();
//	}else{
//		ofLogError() << "popView: couldn't pop modelView matrix, stack empty. probably wrong anidated push/popView";
//	}
	
	matrixMode(OF_MATRIX_PROJECTION);
	popMatrix();	// will pop projection matrix
	matrixMode(OF_MATRIX_MODELVIEW);
	popMatrix();	// will pop modelview matrix

}

//----------------------------------------------------------
void ofxGL3Renderer::viewport(ofRectangle viewport_){
	viewport(viewport_.x, viewport_.y, viewport_.width, viewport_.height,true);
}

//----------------------------------------------------------
void ofxGL3Renderer::viewport(float x, float y, float width, float height, bool invertY) {
	if(width == 0) width = ofGetWindowWidth();
	if(height == 0) height = ofGetWindowHeight();

	if (invertY){
		if(currentFbo){
			y = currentFbo->getHeight() - (y + height);
		}else{
			y = ofGetWindowHeight() - (y + height);
		}
	}
	glViewport(x, y, width, height);	
}

//----------------------------------------------------------
ofRectangle ofxGL3Renderer::getCurrentViewport(){

	// I am using opengl calls here instead of returning viewportRect
	// since someone might use glViewport instead of ofViewport...

	GLint viewport[4];					// Where The Viewport Values Will Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);
	ofRectangle view;
	view.x = viewport[0];
	view.y = viewport[1];
	view.width = viewport[2];
	view.height = viewport[3];
	return view;

}

//----------------------------------------------------------
int ofxGL3Renderer::getViewportWidth(){
	GLint viewport[4];					// Where The Viewport Values Will Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);
	return viewport[2];
}

//----------------------------------------------------------
int ofxGL3Renderer::getViewportHeight(){
	GLint viewport[4];					// Where The Viewport Values Will Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);
	return viewport[3];
}

//----------------------------------------------------------
void ofxGL3Renderer::setCoordHandedness(ofHandednessType handedness) {
	coordHandedness = handedness;
}

//----------------------------------------------------------
ofHandednessType ofxGL3Renderer::getCoordHandedness() {
	return coordHandedness;
}

//----------------------------------------------------------
void ofxGL3Renderer::setupScreenPerspective(float width, float height, ofOrientation orientation, bool vFlip, float fov, float nearDist, float farDist) {
	if(width == 0) width = ofGetWidth();
	if(height == 0) height = ofGetHeight();

	float viewW = ofGetViewportWidth();
	float viewH = ofGetViewportHeight();

	float eyeX = viewW / 2;
	float eyeY = viewH / 2;
	float halfFov = PI * fov / 360;
	float theTan = tanf(halfFov);
	float dist = eyeY / theTan;
	float aspect = (float) viewW / viewH;

	if(nearDist == 0) nearDist = dist / 10.0f;
	if(farDist == 0) farDist = dist * 10.0f;

	matrixMode(OF_MATRIX_PROJECTION);
	loadIdentityMatrix();	// WE DON'T NEED THAT.
		
	ofMatrix4x4 persp;
	persp.makePerspectiveMatrix(fov, aspect, nearDist, farDist);
	loadMatrix( persp );
	//gluPerspective(fov, aspect, nearDist, farDist);


	matrixMode(OF_MATRIX_MODELVIEW);
	loadIdentityMatrix();	// WE DON'T NEED THAT.
	
	ofMatrix4x4 lookAt;
	lookAt.makeLookAtViewMatrix( ofVec3f(eyeX, eyeY, dist),  ofVec3f(eyeX, eyeY, 0),  ofVec3f(0, 1, 0) );
	loadMatrix( lookAt );
	//gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0, 0, 1, 0);

	//note - theo checked this on iPhone and Desktop for both vFlip = false and true
	if(ofDoesHWOrientation()){
		if(vFlip){
			scale(1, -1, 1);
			translate(0, -height, 0);
		}
	}else{
		if( orientation == OF_ORIENTATION_UNKNOWN ) orientation = ofGetOrientation();
		switch(orientation) {
			case OF_ORIENTATION_180:
				rotate(-180, 0, 0, 1);
				if(vFlip){
					scale(1, -1, 1);
					translate(-width, 0, 0);
				}else{
					translate(-width, -height, 0);
				}

				break;

			case OF_ORIENTATION_90_RIGHT:
				rotate(-90, 0, 0, 1);
				if(vFlip){
					scale(-1, 1, 1);
				}else{
					scale(-1, -1, 1);
					translate(0, -height, 0);
				}
				break;

			case OF_ORIENTATION_90_LEFT:
				rotate(90, 0, 0, 1);
				if(vFlip){
					scale(-1, 1, 1);
					translate(-width, -height, 0);
				}else{
					scale(-1, -1, 1);
					translate(-width, 0, 0);
				}
				break;

			case OF_ORIENTATION_DEFAULT:
			default:
				if(vFlip){
					scale(1, -1, 1);
					translate(0, -height, 0);
				}
				break;
		}
	}

}

//----------------------------------------------------------
void ofxGL3Renderer::setupScreenOrtho(float width, float height, ofOrientation orientation, bool vFlip, float nearDist, float farDist) {
	if(width == 0) width = ofGetWidth();
	if(height == 0) height = ofGetHeight();
	
	float viewW = ofGetViewportWidth();
	float viewH = ofGetViewportHeight();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ofSetCoordHandedness(OF_RIGHT_HANDED);
	if(vFlip) {
		ofMatrix4x4 ortho = ofMatrix4x4::newOrthoMatrix(0, width, height, 0, nearDist, farDist);
		ofSetCoordHandedness(OF_LEFT_HANDED);
	}
	
	ofMatrix4x4 ortho = ofMatrix4x4::newOrthoMatrix(0, viewW, 0, viewH, nearDist, farDist);
	glMultMatrixf(ortho.getPtr());	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//note - theo checked this on iPhone and Desktop for both vFlip = false and true
	if(ofDoesHWOrientation()){
		if(vFlip){
			glScalef(1, -1, 1);
			glTranslatef(0, -height, 0);
		}
	}else{
		if( orientation == OF_ORIENTATION_UNKNOWN ) orientation = ofGetOrientation();
		switch(orientation) {
			case OF_ORIENTATION_180:
				glRotatef(-180, 0, 0, 1);
				if(vFlip){
					glScalef(1, -1, 1);
					glTranslatef(-width, 0, 0);
				}else{
					glTranslatef(-width, -height, 0);
				}

				break;

			case OF_ORIENTATION_90_RIGHT:
				glRotatef(-90, 0, 0, 1);
				if(vFlip){
					glScalef(-1, 1, 1);
				}else{
					glScalef(-1, -1, 1);
					glTranslatef(0, -height, 0);
				}
				break;

			case OF_ORIENTATION_90_LEFT:
				glRotatef(90, 0, 0, 1);
				if(vFlip){
					glScalef(-1, 1, 1);
					glTranslatef(-width, -height, 0);
				}else{
					glScalef(-1, -1, 1);
					glTranslatef(-width, 0, 0);
				}
				break;

			case OF_ORIENTATION_DEFAULT:
			default:
				if(vFlip){
					glScalef(1, -1, 1);
					glTranslatef(0, -height, 0);
				}
				break;
		}
	}

}

//----------------------------------------------------------
//Resets openGL parameters back to OF defaults
void ofxGL3Renderer::setupGraphicDefaults(){

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//----------------------------------------------------------
void ofxGL3Renderer::setupScreen(){
	setupScreenPerspective();	// assume defaults
}

//----------------------------------------------------------
void ofxGL3Renderer::setCircleResolution(int res){
	if((int)circlePolyline.size()!=res+1){
		circlePolyline.clear();
		circlePolyline.arc(0,0,0,1,1,0,360,res);
		circlePoints.resize(circlePolyline.size());
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::setSphereResolution(int res) {
	if(sphereMesh.getNumVertices() == 0 || res != ofGetStyle().sphereResolution) {
		int n = res * 2;
		float ndiv2=(float)n/2;
    
		/*
		 Original code by Paul Bourke
		 A more efficient contribution by Federico Dosil (below)
		 Draw a point for zero radius spheres
		 Use CCW facet ordering
		 http://paulbourke.net/texture_colour/texturemap/
		 */
		
		float theta2 = TWO_PI;
		float phi1 = -HALF_PI;
		float phi2 = HALF_PI;
		float r = 1.f; // normalize the verts //
    
		sphereMesh.clear();
    sphereMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
		int i, j;
    float theta1 = 0.f;
		float jdivn,j1divn,idivn,dosdivn,unodivn=1/(float)n,t1,t2,t3,cost1,cost2,cte1,cte3;
		cte3 = (theta2-theta1)/n;
		cte1 = (phi2-phi1)/ndiv2;
		dosdivn = 2*unodivn;
		ofVec3f e,p,e2,p2;
    
		if (n < 0){
			n = -n;
			ndiv2 = -ndiv2;
		}
		if (n < 4) {n = 4; ndiv2=(float)n/2;}
    if(r <= 0) r = 1;
		
		t2=phi1;
		cost2=cos(phi1);
		j1divn=0;
    
    ofVec3f vert, normal;
    ofVec2f tcoord;
		
		for (j=0;j<ndiv2;j++) {
			t1 = t2;
			t2 += cte1;
			t3 = theta1 - cte3;
			cost1 = cost2;
			cost2 = cos(t2);
			e.y = sin(t1);
			e2.y = sin(t2);
			p.y = r * e.y;
			p2.y = r * e2.y;
			
			idivn=0;
			jdivn=j1divn;
			j1divn+=dosdivn;
			for (i=0;i<=n;i++) {
				t3 += cte3;
				e.x = cost1 * cos(t3);
				e.z = cost1 * sin(t3);
				p.x = r * e.x;
				p.z = r * e.z;
				
				normal.set( e.x, e.y, e.z );
				tcoord.set( idivn, jdivn );
				vert.set( p.x, p.y, p.z );
				
				sphereMesh.addNormal(normal);
				sphereMesh.addTexCoord(tcoord);
				sphereMesh.addVertex(vert);
				
				e2.x = cost2 * cos(t3);
				e2.z = cost2 * sin(t3);
				p2.x = r * e2.x;
				p2.z = r * e2.z;
				
				normal.set(e2.x, e2.y, e2.z);
				tcoord.set(idivn, j1divn);
				vert.set(p2.x, p2.y, p2.z);
				
				sphereMesh.addNormal(normal);
				sphereMesh.addTexCoord(tcoord);
				sphereMesh.addVertex(vert);
				
				idivn += unodivn;
				
			}
		}
	}
}

//our openGL wrappers

// ----------------------------------------------------------------------

inline void ofxGL3Renderer::shaderSetupModelViewProjectionMatrices() {
	/// tig:
	// this can be optimised. we really only want to apply these if they have changed.
	// oh, and we want to cache the uniform locations, eventually.
	if (currentShader.get()==NULL) return;

	// apply current matrices to current shader, using cached unform locations
	glUniformMatrix4fv(shaderLocCache.locUniformProjectionMatrix, 1, GL_FALSE, currentProjectionMatrix.getPtr());
	glUniformMatrix4fv(shaderLocCache.locUniformModelViewMatrix, 1, GL_FALSE, currentModelViewMatrix.getPtr());
}

// ----------------------------------------------------------------------

inline void ofxGL3Renderer::shaderApplyModelViewProjectionMatrices(){
	if (currentShader.get()==NULL) return;
	// apply current matrix to current shader, using cached unform locations
	if (currentMatrixMode == OF_MATRIX_PROJECTION) 	glUniformMatrix4fv(shaderLocCache.locUniformProjectionMatrix, 1, GL_FALSE, currentProjectionMatrix.getPtr());
	if (currentMatrixMode == OF_MATRIX_MODELVIEW) glUniformMatrix4fv(shaderLocCache.locUniformModelViewMatrix, 1, GL_FALSE, currentModelViewMatrix.getPtr());
}

// ----------------------------------------------------------------------

//----------------------------------------------------------
void ofxGL3Renderer::pushMatrix(){
	if (currentMatrixMode == OF_MATRIX_MODELVIEW){
		modelViewMatrixStack.push(currentModelViewMatrix);
	} else if (currentMatrixMode == OF_MATRIX_PROJECTION){
		projectionMatrixStack.push(currentProjectionMatrix);
	} else if (currentMatrixMode == OF_MATRIX_TEXTURE){
		textureMatrixStack.push(currentTextureMatrix);
	}
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::popMatrix(){

	if (currentMatrixMode == OF_MATRIX_MODELVIEW && !modelViewMatrixStack.empty()){
		currentModelViewMatrix = modelViewMatrixStack.top();
		modelViewMatrixStack.pop();
	} else if (currentMatrixMode == OF_MATRIX_PROJECTION && !projectionMatrixStack.empty()){
		currentProjectionMatrix = projectionMatrixStack.top();
		projectionMatrixStack.pop();
	} else if (currentMatrixMode == OF_MATRIX_TEXTURE && !textureMatrixStack.empty()){
		currentTextureMatrix = textureMatrixStack.top();
		textureMatrixStack.pop();
	} else {
		ofLogWarning() << "ofxGL3Renderer: Empty matrix stack, cannot pop any further.";
	}
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::translate(const ofVec3f& p){
	currentMatrix->glTranslate(p);
	shaderApplyModelViewProjectionMatrices();
}


//----------------------------------------------------------
void ofxGL3Renderer::translate(float x, float y, float z){
	currentMatrix->glTranslate(x, y, z);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::scale(float xAmnt, float yAmnt, float zAmnt){
	currentMatrix->glScale(xAmnt, yAmnt, zAmnt);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::rotate(float degrees, float vecX, float vecY, float vecZ){
	currentMatrix->glRotate(degrees, vecX, vecY, vecZ);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::rotateX(float degrees){
	currentMatrix->glRotate(degrees, 1, 0, 0);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::rotateY(float degrees){
	currentMatrix->glRotate(degrees, 0, 1, 0);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::rotateZ(float degrees){
	currentMatrix->glRotate(degrees, 0, 0, 1);
	shaderApplyModelViewProjectionMatrices();
}

//same as ofRotateZ
//----------------------------------------------------------
void ofxGL3Renderer::rotate(float degrees){
	currentMatrix->glRotate(degrees, 0, 0, 1);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::matrixMode(ofMatrixMode mode){
	currentMatrixMode = mode;

	if (currentMatrixMode == OF_MATRIX_MODELVIEW){
		currentMatrix = &currentModelViewMatrix;
	} else if (currentMatrixMode == OF_MATRIX_PROJECTION){
		currentMatrix = &currentProjectionMatrix;
	} else if (currentMatrixMode == OF_MATRIX_TEXTURE){
		currentMatrix = &currentTextureMatrix;
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::loadIdentityMatrix (void){
	currentMatrix->makeIdentityMatrix();
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::loadMatrix (const ofMatrix4x4 & m){
	currentMatrix->set(m);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::loadMatrix (const float *m){
	currentMatrix->set(m);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::multMatrix (const ofMatrix4x4 & m){
	currentMatrix->preMult(m);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::multMatrix (const float *m){
	currentMatrix->preMult(m);
	shaderApplyModelViewProjectionMatrices();
}

//----------------------------------------------------------
void ofxGL3Renderer::setColor(const ofColor & color){
	setColor(color.r,color.g,color.b,color.a);
}

//----------------------------------------------------------
void ofxGL3Renderer::setColor(const ofColor & color, int _a){
	setColor(color.r,color.g,color.b,_a);
}

//----------------------------------------------------------
void ofxGL3Renderer::setColor(int _r, int _g, int _b){
	glColor4f(_r/255.f,_g/255.f,_b/255.f,1.f);
}


//----------------------------------------------------------
void ofxGL3Renderer::setColor(int _r, int _g, int _b, int _a){
	glColor4f(_r/255.f,_g/255.f,_b/255.f,_a/255.f);
}

//----------------------------------------------------------
void ofxGL3Renderer::setColor(int gray){
	setColor(gray, gray, gray);
}

//----------------------------------------------------------
void ofxGL3Renderer::setHexColor(int hexColor){
	int r = (hexColor >> 16) & 0xff;
	int g = (hexColor >> 8) & 0xff;
	int b = (hexColor >> 0) & 0xff;
	setColor(r,g,b);
}

//----------------------------------------------------------
void ofxGL3Renderer::clear(float r, float g, float b, float a) {
	glClearColor(r / 255., g / 255., b / 255., a / 255.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------------------------------------------------------
void ofxGL3Renderer::clear(float brightness, float a) {
	clear(brightness, brightness, brightness, a);
}

//----------------------------------------------------------
void ofxGL3Renderer::clearAlpha() {
	glColorMask(0, 0, 0, 1);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMask(1, 1, 1, 1);
}

//----------------------------------------------------------
void ofxGL3Renderer::setBackgroundAuto(bool bAuto){
	bBackgroundAuto = bAuto;
}

//----------------------------------------------------------
bool ofxGL3Renderer::bClearBg(){
	return bBackgroundAuto;
}

//----------------------------------------------------------
ofFloatColor & ofxGL3Renderer::getBgColor(){
	return bgColor;
}

//----------------------------------------------------------
void ofxGL3Renderer::background(const ofColor & c){
	bgColor = c;
	glClearColor(bgColor[0],bgColor[1],bgColor[2], bgColor[3]);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------------------------------------------------------
void ofxGL3Renderer::background(float brightness) {
	background(brightness);
}

//----------------------------------------------------------
void ofxGL3Renderer::background(int hexColor, float _a){
	background ( (hexColor >> 16) & 0xff, (hexColor >> 8) & 0xff, (hexColor >> 0) & 0xff, _a);
}

//----------------------------------------------------------
void ofxGL3Renderer::background(int r, int g, int b, int a){
	background(ofColor(r,g,b,a));
}

//----------------------------------------------------------
void ofxGL3Renderer::setFillMode(ofFillFlag fill){
	bFilled = fill;
}

//----------------------------------------------------------
ofFillFlag ofxGL3Renderer::getFillMode(){
	return bFilled;
}

//----------------------------------------------------------
void ofxGL3Renderer::setRectMode(ofRectMode mode){
	rectMode = mode;
}

//----------------------------------------------------------
ofRectMode ofxGL3Renderer::getRectMode(){
	return rectMode;
}

//----------------------------------------------------------
void ofxGL3Renderer::setLineWidth(float lineWidth){
	glLineWidth(lineWidth);
}

//----------------------------------------------------------
void ofxGL3Renderer::setLineSmoothing(bool smooth){
	bSmoothHinted = smooth;
}


//----------------------------------------------------------
void ofxGL3Renderer::startSmoothing(){
	#ifndef TARGET_OPENGLES
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
	#endif

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	

	//why do we need this?
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//----------------------------------------------------------
void ofxGL3Renderer::endSmoothing(){
	#ifndef TARGET_OPENGLES
		glPopAttrib();
	#endif
}

//----------------------------------------------------------
void ofxGL3Renderer::setBlendMode(ofBlendMode blendMode){
	switch (blendMode){

		case OF_BLENDMODE_ALPHA:{
			glEnable(GL_BLEND);
			#ifndef TARGET_OPENGLES
				glBlendEquation(GL_FUNC_ADD);
			#endif
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}

		case OF_BLENDMODE_ADD:{
			glEnable(GL_BLEND);
			#ifndef TARGET_OPENGLES
				glBlendEquation(GL_FUNC_ADD);
			#endif
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}

		case OF_BLENDMODE_MULTIPLY:{
			glEnable(GL_BLEND);
			#ifndef TARGET_OPENGLES
				glBlendEquation(GL_FUNC_ADD);
			#endif
			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA /* GL_ZERO or GL_ONE_MINUS_SRC_ALPHA */);
			break;
		}

		case OF_BLENDMODE_SCREEN:{
			glEnable(GL_BLEND);
			#ifndef TARGET_OPENGLES
				glBlendEquation(GL_FUNC_ADD);
			#endif
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
			break;
		}

		case OF_BLENDMODE_SUBTRACT:{
			glEnable(GL_BLEND);
		#ifndef TARGET_OPENGLES
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
		#else
			ofLog(OF_LOG_WARNING, "OF_BLENDMODE_SUBTRACT not currently supported on OpenGL/ES");
		#endif
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}


		default:
			break;
	}
}

//----------------------------------------------------------
void ofxGL3Renderer::enablePointSprites(){
#ifdef TARGET_OPENGLES
	glEnable(GL_POINT_SPRITE_OES);
	glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
	// does look like this needs to be enabled in ES because
	// it is always eneabled...
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

#else
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
}

//----------------------------------------------------------
void ofxGL3Renderer::disablePointSprites(){
#ifdef TARGET_OPENGLES
	glDisable(GL_POINT_SPRITE_OES);
#else
	glDisable(GL_POINT_SPRITE);
#endif
}

//----------------------------------------------------------
void ofxGL3Renderer::drawLine(float x1, float y1, float z1, float x2, float y2, float z2){
	linePoints[0].set(x1,y1,z1);
	linePoints[1].set(x2,y2,z2);

	// use smoothness, if requested:
	if (bSmoothHinted) startSmoothing();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &linePoints[0].x);
	glDrawArrays(GL_LINES, 0, 2);

	// use smoothness, if requested:
	if (bSmoothHinted) endSmoothing();

}

//----------------------------------------------------------
void ofxGL3Renderer::drawRectangle(float x, float y, float z,float w, float h){

	if (rectMode == OF_RECTMODE_CORNER){
		rectPoints[0].set(x,y,z);
		rectPoints[1].set(x+w, y, z);
		rectPoints[2].set(x+w, y+h, z);
		rectPoints[3].set(x, y+h, z);
	}else{
		rectPoints[0].set(x-w/2.0f, y-h/2.0f, z);
		rectPoints[1].set(x+w/2.0f, y-h/2.0f, z);
		rectPoints[2].set(x+w/2.0f, y+h/2.0f, z);
		rectPoints[3].set(x-w/2.0f, y+h/2.0f, z);
	}

	// bind vertex array
	glBindVertexArray(defaultVAO);

	rectVbo.setVertexData(&rectPoints[0], 4, GL_DYNAMIC_DRAW);
	

	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) startSmoothing();

	glBindBuffer(GL_ARRAY_BUFFER, rectVbo.getVertId()); // bind to triangle vertices
	glEnableVertexAttribArray(0);							// activate attribute 0 in shader
	glVertexAttribPointer(shaderLocCache.locAttributeVVertex, 3, GL_FLOAT,GL_FALSE,0,0);
	
	glDrawArrays((bFilled == OF_FILLED) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
	
	glDisableVertexAttribArray(0);			// disable vertex attrib array.
	glBindBuffer(GL_ARRAY_BUFFER,0);		// unbind by binding to zero

	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) endSmoothing();

}

//----------------------------------------------------------
void ofxGL3Renderer::drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3){
	triPoints[0].set(x1,y1,z1);
	triPoints[1].set(x2,y2,z2);
	triPoints[2].set(x3,y3,z3);

	// bind vertex array
	glBindVertexArray(defaultVAO);
	
	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) startSmoothing();
	triangleVbo.setVertexData(&triPoints[0], 3, GL_DYNAMIC_DRAW);

//	applyModelViewProjectionMatrices();

	
	glBindBuffer(GL_ARRAY_BUFFER, triangleVbo.getVertId()); // bind to triangle vertices
	glEnableVertexAttribArray(0);							// activate attribute 0 in shader
	glVertexAttribPointer(shaderLocCache.locAttributeVVertex,3,GL_FLOAT,GL_FALSE,0,0);
	
	glDrawArrays((bFilled == OF_FILLED) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 3);

	glDisableVertexAttribArray(0);			// disable vertex attrib array.
	glBindBuffer(GL_ARRAY_BUFFER,0);		// unbind by binding to zero
	
	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) endSmoothing();

}

//----------------------------------------------------------
void ofxGL3Renderer::drawCircle(float x, float y, float z,  float radius){
	vector<ofPoint> & circleCache = circlePolyline.getVertices();
	for(int i=0;i<(int)circleCache.size();i++){
		circlePoints[i].set(radius*circleCache[i].x+x,radius*circleCache[i].y+y,z);
	}

	// bind default vertex array
	glBindVertexArray(defaultVAO);

	circleVbo.setVertexData(&circlePoints[0].x, 3, circlePoints.size(), GL_DYNAMIC_DRAW, sizeof(ofVec3f));

	glBindBuffer(GL_ARRAY_BUFFER, circleVbo.getVertId());		// bind the circle vertex vbo
	glEnableVertexAttribArray(0);								// we assume vertex data goes in attribute position zero on the current vertex shader
	glVertexAttribPointer(shaderLocCache.locAttributeVVertex, 3,GL_FLOAT,GL_FALSE,0,0);
	
	// DRAW !!!
	glDrawArrays((bFilled == OF_FILLED) ? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, circlePoints.size());

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);							// unbind the circle vertex vbo
	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) endSmoothing();

}

//----------------------------------------------------------
void ofxGL3Renderer::drawSphere(float x, float y, float z, float radius) {
    
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glScalef(radius, radius, radius);
    if(bFilled) {
        sphereMesh.draw();
    } else {
        sphereMesh.drawWireframe();
    }
    glPopMatrix();
    glDisable(GL_NORMALIZE);
    
}

//----------------------------------------------------------
void ofxGL3Renderer::drawEllipse(float x, float y, float z, float width, float height){
	float radiusX = width*0.5;
	float radiusY = height*0.5;
	vector<ofPoint> & circleCache = circlePolyline.getVertices();
	for(int i=0;i<(int)circleCache.size();i++){
		circlePoints[i].set(radiusX*circlePolyline[i].x+x,radiusY*circlePolyline[i].y+y,z);
	}

	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) startSmoothing();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &circlePoints[0].x);
	glDrawArrays((bFilled == OF_FILLED) ? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, circlePoints.size());

	// use smoothness, if requested:
	if (bSmoothHinted && bFilled == OF_OUTLINE) endSmoothing();

}

//----------------------------------------------------------
void ofxGL3Renderer::drawString(string textString, float x, float y, float z, ofDrawBitmapMode mode){
	// this is copied from the ofTrueTypeFont
	//GLboolean blend_enabled = glIsEnabled(GL_BLEND); //TODO: this is not used?
	GLint blend_src, blend_dst;
	glGetIntegerv( GL_BLEND_SRC, &blend_src );
	glGetIntegerv( GL_BLEND_DST, &blend_dst );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	int len = (int)textString.length();
	//float yOffset = 0;
	float fontSize = 8.0f;
	bool bOrigin = false;

	float sx = 0;
	float sy = -fontSize;


	///////////////////////////
	// APPLY TRANSFORM / VIEW
	///////////////////////////
	//

	bool hasModelView = false;
	bool hasProjection = false;
	bool hasViewport = false;

	ofRectangle rViewport;
	
#ifdef TARGET_OPENGLES
	if(mode == OF_BITMAPMODE_MODEL_BILLBOARD) {
		mode = OF_BITMAPMODE_SIMPLE;
	}
#endif

	switch (mode) {

		case OF_BITMAPMODE_SIMPLE:

			sx += x;
			sy += y;
			break;

		case OF_BITMAPMODE_SCREEN:

			hasViewport = true;
			ofPushView();

			rViewport = ofGetWindowRect();
			ofViewport(rViewport);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(-1, 1, 0);
			glScalef(2/rViewport.width, -2/rViewport.height, 1);

			ofTranslate(x, y, 0);
			break;

		case OF_BITMAPMODE_VIEWPORT:

			rViewport = ofGetCurrentViewport();

			hasProjection = true;
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

			hasModelView = true;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glTranslatef(-1, 1, 0);
			glScalef(2/rViewport.width, -2/rViewport.height, 1);

			ofTranslate(x, y, 0);
			break;

		case OF_BITMAPMODE_MODEL:

			hasModelView = true;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			ofTranslate(x, y, z);
			ofScale(1, -1, 0);
			break;

		case OF_BITMAPMODE_MODEL_BILLBOARD:
			//our aim here is to draw to screen
			//at the viewport position related
			//to the world position x,y,z

			// ***************
			// this will not compile for opengl ES
			// ***************
#ifndef TARGET_OPENGLES
			//gluProject method
			GLdouble modelview[16], projection[16];
			GLint view[4];
			double dScreenX, dScreenY, dScreenZ;
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			glGetDoublev(GL_PROJECTION_MATRIX, projection);
			glGetIntegerv(GL_VIEWPORT, view);
			view[0] = 0; view[1] = 0; //we're already drawing within viewport
			gluProject(x, y, z, modelview, projection, view, &dScreenX, &dScreenY, &dScreenZ);

			if (dScreenZ >= 1)
				return;
			
			rViewport = ofGetCurrentViewport();

			hasProjection = true;
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

			hasModelView = true;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glTranslatef(-1, -1, 0);
			glScalef(2/rViewport.width, 2/rViewport.height, 1);

			glTranslatef(dScreenX, dScreenY, 0);
            
            if(currentFbo == NULL) {
                glScalef(1, -1, 1);
            } else {
                glScalef(1,  1, 1); // invert when rendering inside an fbo
            }
            
#endif
			break;

		default:
			break;
	}
	//
	///////////////////////////


	// (c) enable texture once before we start drawing each char (no point turning it on and off constantly)
	//We do this because its way faster
	ofDrawBitmapCharacterStart(textString.size());

	for(int c = 0; c < len; c++){
		if(textString[c] == '\n'){

			sy += bOrigin ? -1 : 1 * (fontSize*1.7);
			if(mode == OF_BITMAPMODE_SIMPLE) {
				sx = x;
			} else {
				sx = 0;
			}

			//glRasterPos2f(x,y + (int)yOffset);
		} else if (textString[c] >= 32){
			// < 32 = control characters - don't draw
			// solves a bug with control characters
			// getting drawn when they ought to not be
			ofDrawBitmapCharacter(textString[c], (int)sx, (int)sy);
						
			sx += fontSize;
		}
	}
	//We do this because its way faster
	ofDrawBitmapCharacterEnd();


	if (hasModelView)
		glPopMatrix();

	if (hasProjection)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	if (hasViewport)
		ofPopView();

	glBlendFunc(blend_src, blend_dst);
}
