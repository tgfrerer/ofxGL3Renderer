#pragma once
#include "ofBaseTypes.h"
#include "ofPolyline.h"
#include "ofMatrix4x4.h"
#include "ofVbo.h"

#include <stack>
class ofShapeTessellation;
class ofMesh;
class ofFbo;
class ofShader;

class ofxGL3Renderer: public ofBaseRenderer{

	typedef ofPtr<ofShader> shaderP_t;
	shaderP_t currentShader;
	
	void shaderSetupModelViewProjectionMatrices();
	void shaderUploadModelViewProjectionMatrices();

	ofFloatColor fgColor;
	
	void shaderSetupColor();
	void shaderUploadColor();
	
	GLuint defaultVAO;

	ofVbo circleVbo;
	ofVbo triangleVbo;
	ofVbo rectVbo;
	ofVbo lineVbo;
	ofVbo vertexDataVbo;
	ofVbo meshVbo;
	
	struct {
		GLuint locUniformProjectionMatrix;
		GLuint locUniformModelViewMatrix;
		GLuint locUniformColor;
		GLuint locAttributePosition;
		GLuint locAttributeNormal;
		GLuint locAttributeColor;
		GLuint locAttributeTexCoord;
	} shaderLocCache;
	
	void preparePrimitiveDraw(ofVbo& vbo);
	void finishPrimitiveDraw();
	
public:
	ofxGL3Renderer(bool useShapeColor=true);
	~ofxGL3Renderer(){}

	string getType(){ return "GL"; }

	void setCurrentFBO(ofFbo * fbo);

	void update();
	void draw(ofMesh & vertexData, bool useColors=true, bool useTextures=true, bool useNormals = true);
	void draw(ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors=true, bool useTextures = true, bool useNormals=true);
	void draw(ofPolyline & poly);
	void draw(ofPath & path);
	void draw(vector<ofVec3f> & vertexData, ofPrimitiveMode drawMode);
	void draw(ofImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh);
	void draw(ofFloatImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh);
	void draw(ofShortImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh);

	bool rendersPathPrimitives(){
		return false;
	}
	
	//--------------------------------------------

	void		beginShader(shaderP_t shader_);
	void		endShader();

	shaderP_t	getCurrentShader(){ return currentShader; };

	//--------------------------------------------

	// transformations
	void pushView();
	void popView();

	// setup matrices and viewport (upto you to push and pop view before and after)
	// if width or height are 0, assume windows dimensions (ofGetWidth(), ofGetHeight())
	// if nearDist or farDist are 0 assume defaults (calculated based on width / height)
	void viewport(ofRectangle viewport);
	void viewport(float x = 0, float y = 0, float width = 0, float height = 0, bool invertY = true);
	void setupScreenPerspective(float width = 0, float height = 0, ofOrientation orientation = OF_ORIENTATION_UNKNOWN, bool vFlip = true, float fov = 60, float nearDist = 0, float farDist = 0);
	void setupScreenOrtho(float width = 0, float height = 0, ofOrientation orientation = OF_ORIENTATION_UNKNOWN, bool vFlip = true, float nearDist = -1, float farDist = 1);
	ofRectangle getCurrentViewport();
	int getViewportWidth();
	int getViewportHeight();

	void setCoordHandedness(ofHandednessType handedness);
	ofHandednessType getCoordHandedness();

	//our openGL wrappers
	void pushMatrix();
	void popMatrix();
	void translate(float x, float y, float z = 0);
	void translate(const ofVec3f & p);
	void scale(float xAmnt, float yAmnt, float zAmnt = 1);
	void rotate(float degrees, float vecX, float vecY, float vecZ);
	void rotateX(float degrees);
	void rotateY(float degrees);
	void rotateZ(float degrees);
	void rotate(float degrees);
	void matrixMode(ofMatrixMode mode);
	void loadIdentityMatrix (void);
	void loadMatrix (const ofMatrix4x4 & m);
	void loadMatrix (const float * m);
	void multMatrix (const ofMatrix4x4 & m);
	void multMatrix (const float * m);
	
	// screen coordinate things / default gl values
	void setupGraphicDefaults();
	void setupScreen();

	// drawing modes
	void setFillMode(ofFillFlag fill);
	ofFillFlag getFillMode();
	void setCircleResolution(int res);
	void setSphereResolution(int res);
	void setRectMode(ofRectMode mode);
	ofRectMode getRectMode();
	void setLineWidth(float lineWidth);
	void setLineSmoothing(bool smooth);
	void setBlendMode(ofBlendMode blendMode);
	void enablePointSprites();
	void disablePointSprites();

	// color options
	void setColor(int r, int g, int b); // 0-255
	void setColor(int r, int g, int b, int a); // 0-255
	void setColor(const ofColor & color);
	void setColor(const ofColor & color, int _a);
	void setColor(int gray); // new set a color as grayscale with one argument
	void setHexColor( int hexColor ); // hex, like web 0xFF0033;

	// bg color
	ofFloatColor & getBgColor();
	bool bClearBg();
	void background(const ofColor & c);
	void background(float brightness);
	void background(int hexColor, float _a=255.0f);
	void background(int r, int g, int b, int a=255);

	void setBackgroundAuto(bool bManual);		// default is true

	void clear(float r, float g, float b, float a=0);
	void clear(float brightness, float a=0);
	void clearAlpha();


	// drawing
	void drawLine(float x1, float y1, float z1, float x2, float y2, float z2);
	void drawRectangle(float x, float y, float z, float w, float h);
	void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
	void drawCircle(float x, float y, float z, float radius);
	void drawSphere(float x, float y, float z, float radius);
	void drawEllipse(float x, float y, float z, float width, float height);
	void drawString(string text, float x, float y, float z, ofDrawBitmapMode mode);

	
	ofMatrix4x4& getModelViewMatrix(){return currentModelViewMatrix;};
	
private:
	void startSmoothing();
	void endSmoothing();

	ofHandednessType coordHandedness;

	ofMatrixMode	currentMatrixMode;
	
	ofMatrix4x4		currentModelViewMatrix;
	ofMatrix4x4		currentProjectionMatrix;
	ofMatrix4x4		currentTextureMatrix;
	
	ofMatrix4x4*	currentMatrix;
	
	stack <ofRectangle> viewportHistory;

	stack <ofMatrix4x4> modelViewMatrixStack;
	stack <ofMatrix4x4> projectionMatrixStack;
	stack <ofMatrix4x4> textureMatrixStack;
	
	bool bBackgroundAuto;
	ofFloatColor bgColor;

	vector<ofVec3f> linePoints;
	vector<ofVec3f> rectPoints;
	vector<ofVec3f> triPoints;
	vector<ofVec3f> circlePoints;
	
	
	ofPolyline circlePolyline;
	
	ofMesh sphereMesh;

	ofFillFlag bFilled;
	bool bSmoothHinted;
	ofRectMode rectMode;

	ofFbo * currentFbo;

};
