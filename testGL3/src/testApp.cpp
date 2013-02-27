#include "testApp.h"


/**********************************************************************
 * Geometry creation functions
 *********************************************************************/

/* Map height updates */
#define MAX_CIRCLE_SIZE (5.0f)
#define MAX_DISPLACEMENT (1.0f)
#define DISPLACEMENT_SIGN_LIMIT (0.3f)
#define MAX_ITER (200)
#define NUM_ITER_AT_A_TIME (1)

/* Map general information */
#define MAP_SIZE (10.0f)
#define MAP_NUM_VERTICES (80)
#define MAP_NUM_TOTAL_VERTICES (MAP_NUM_VERTICES*MAP_NUM_VERTICES)
#define MAP_NUM_LINES (3* (MAP_NUM_VERTICES - 1) * (MAP_NUM_VERTICES - 1) + \
2 * (MAP_NUM_VERTICES - 1))

/**********************************************************************
 * Heightmap vertex and index data
 *********************************************************************/

static GLfloat map_vertices[3][MAP_NUM_TOTAL_VERTICES];
static GLuint  map_line_indices[2*MAP_NUM_LINES];

/* Store uniform location for the shaders
 * Those values are setup as part of the process of creating
 * the shader program. They should not be used before creating
 * the program.
 */
static GLuint mesh;
static GLuint mesh_vbo[4];



/* Generate vertices and indices for the heightmap
 */
static void init_map(void)
{
    int i;
    int j;
    int k;
    GLfloat step = MAP_SIZE / (MAP_NUM_VERTICES - 1);
    GLfloat x = 0.0f;
    GLfloat z = 0.0f;
    /* Create a flat grid */
    k = 0;
    for (i = 0 ; i < MAP_NUM_VERTICES ; ++i)
    {
        for (j = 0 ; j < MAP_NUM_VERTICES ; ++j)
        {
            map_vertices[0][k] = x;
            map_vertices[1][k] = 0.0f;
            map_vertices[2][k] = z;
            z += step;
            ++k;
        }
        x += step;
        z = 0.0f;
    }
#if DEBUG_ENABLED
    for (i = 0 ; i < MAP_NUM_TOTAL_VERTICES ; ++i)
    {
        printf ("Vertice %d (%f, %f, %f)\n",
                i, map_vertices[0][i], map_vertices[1][i], map_vertices[2][i]);
		
    }
#endif
    /* create indices */
    /* line fan based on i
     * i+1
     * |  / i + n + 1
     * | /
     * |/
     * i --- i + n
     */
	
    /* close the top of the square */
    k = 0;
    for (i = 0 ; i < MAP_NUM_VERTICES  -1 ; ++i)
    {
        map_line_indices[k++] = (i + 1) * MAP_NUM_VERTICES -1;
        map_line_indices[k++] = (i + 2) * MAP_NUM_VERTICES -1;
    }
    /* close the right of the square */
    for (i = 0 ; i < MAP_NUM_VERTICES -1 ; ++i)
    {
        map_line_indices[k++] = (MAP_NUM_VERTICES - 1) * MAP_NUM_VERTICES + i;
        map_line_indices[k++] = (MAP_NUM_VERTICES - 1) * MAP_NUM_VERTICES + i + 1;
    }
	
    for (i = 0 ; i < (MAP_NUM_VERTICES - 1) ; ++i)
    {
        for (j = 0 ; j < (MAP_NUM_VERTICES - 1) ; ++j)
        {
            int ref = i * (MAP_NUM_VERTICES) + j;
            map_line_indices[k++] = ref;
            map_line_indices[k++] = ref + 1;
			
            map_line_indices[k++] = ref;
            map_line_indices[k++] = ref + MAP_NUM_VERTICES;
			
            map_line_indices[k++] = ref;
            map_line_indices[k++] = ref + MAP_NUM_VERTICES + 1;
        }
    }
	
#ifdef DEBUG_ENABLED
    for (k = 0 ; k < 2 * MAP_NUM_LINES ; k += 2)
    {
        int beg, end;
        beg = map_line_indices[k];
        end = map_line_indices[k+1];
        printf ("Line %d: %d -> %d (%f, %f, %f) -> (%f, %f, %f)\n",
                k / 2, beg, end,
                map_vertices[0][beg], map_vertices[1][beg], map_vertices[2][beg],
                map_vertices[0][end], map_vertices[1][end], map_vertices[2][end]);
    }
#endif
}

static void generate_heightmap__circle(float* center_x, float* center_y,
									   float* size, float* displacement)
{
    float sign;
    /* random value for element in between [0-1.0] */
    *center_x = (MAP_SIZE * rand()) / (1.0f * RAND_MAX);
    *center_y = (MAP_SIZE * rand()) / (1.0f * RAND_MAX);
    *size = (MAX_CIRCLE_SIZE * rand()) / (1.0f * RAND_MAX);
    sign = (1.0f * rand()) / (1.0f * RAND_MAX);
    sign = (sign < DISPLACEMENT_SIGN_LIMIT) ? -1.0f : 1.0f;
    *displacement = (sign * (MAX_DISPLACEMENT * rand())) / (1.0f * RAND_MAX);
}

/* Run the specified number of iterations of the generation process for the
 * heightmap
 */
static void update_map(int num_iter)
{
    assert(num_iter > 0);
    while(num_iter)
    {
        /* center of the circle */
        float center_x;
        float center_z;
        float circle_size;
        float disp;
        size_t ii;
        generate_heightmap__circle(&center_x, &center_z, &circle_size, &disp);
        disp = disp / 2.0f;
        for (ii = 0u ; ii < MAP_NUM_TOTAL_VERTICES ; ++ii)
        {
            GLfloat dx = center_x - map_vertices[0][ii];
            GLfloat dz = center_z - map_vertices[2][ii];
            GLfloat pd = (2.0f * sqrtf((dx * dx) + (dz * dz))) / circle_size;
            if (fabs(pd) <= 1.0f)
            {
                /* tx,tz is within the circle */
                GLfloat new_height = disp + (float) (cos(pd*3.14f)*disp);
                map_vertices[1][ii] += new_height;
            }
        }
        --num_iter;
    }
}

/**********************************************************************
 * OpenGL helper functions
 *********************************************************************/

/* Create VBO, IBO and VAO objects for the heightmap geometry and bind them to
 * the specified program object
 */
static void make_mesh(GLuint program)
{
    GLuint attrloc;
	
    glGenVertexArrays(1, &mesh);
    glGenBuffers(4, mesh_vbo);
    glBindVertexArray(mesh);
    /* Prepare the data for drawing through a buffer indices */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* MAP_NUM_LINES * 2, map_line_indices, GL_STATIC_DRAW);

    /* Prepare the attributes for rendering */
    attrloc = glGetAttribLocation(program, "x");
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[0][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrloc);
    glVertexAttribPointer(attrloc, 1, GL_FLOAT, GL_FALSE, 0, 0);
	
    attrloc = glGetAttribLocation(program, "y");
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[1][0], GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(attrloc);
    glVertexAttribPointer(attrloc, 1, GL_FLOAT, GL_FALSE, 0, 0);

    attrloc = glGetAttribLocation(program, "z");
    glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[2][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrloc);
    glVertexAttribPointer(attrloc, 1, GL_FLOAT, GL_FALSE, 0, 0);

}

/* Update VBO vertices from source data
 */
static void update_mesh(void)
{
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[1][0]);
}

//--------------------------------------------------------------
void testApp::setup(){
	
	glGetError();
	string result = (mPassThrough.load("passthrough")) ? "OK" : "NOPE";
	ofLogNotice() << result;
	
	GL3Renderer = ofPtr<ofxGL3Renderer>(new ofxGL3Renderer());
	
	ofSetCurrentRenderer(GL3Renderer);
	
	mProjection.makePerspectiveMatrix(60, ofGetWidth()/(float)ofGetHeight(), 0.1, 200);
	mModelview.makeIdentityMatrix();
	mModelview.translate(-5, -5, -20);
	
	init_map();
	make_mesh(mPassThrough.getProgram());
	mCam1.setupPerspective(false, 60, 0.1, 200);
	mCam1.setGlobalPosition(5, 5, 20);

//	ofLogNotice() << mModelview;
//	ofLogNotice() << "---";
//	ofLogNotice() << mCam1.getModelViewMatrix();
//	ofLogNotice() << "---";
//	ofLogNotice() << mProjection;
//	ofLogNotice() << "---";
//	ofLogNotice() << mCam1.getProjectionMatrix();
	
	mCam1.disableMouseInput();
	mCam1.enableMouseInput();
	mCam1.setTranslationKey('m');
	mCam1.setDistance(20);

	mPassThrough.printActiveAttributes();
	mPassThrough.printActiveUniforms();

}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){

	mCam1.begin();
	
	mPassThrough.begin();

	mPassThrough.setUniformMatrix4f("modelview", mCam1.getModelViewMatrix());
	mPassThrough.setUniformMatrix4f("project", mCam1.getProjectionMatrix());
	
	ofClear(0,0,0);
	
	// glDrawElements(GL_LINES, 2* MAP_NUM_LINES , GL_UNSIGNED_INT, 0);
	
	ofFill();
	ofCircle(0, 0, 5);
	ofNoFill();
	ofCircle(0, 0, 6);
	
//	ofSetColor(255, 0, 0);
//	ofCircle(200,200,40);
	
	mPassThrough.end();
	mCam1.end();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofLogNotice() << "key pressed:  " << char(key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	ofLogNotice() << "key released: " << char(key);
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

	ofLogNotice() << x << "|" << y;
	
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