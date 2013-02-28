ofxGL3Renderer
==============

EXPERIMENTAL OpenGL3.2 context for openframeworks using glfw3. Currently Mac-Ony, but with an eye to multiplatform, using GLFW3 

# WARNING

This will blow up in your face, possibly leak memory like a whole geriatric ward - and steal your lunch money.

Consider it about as stable as a drunk giraffe on a tightrope.

Play around with this at your own peril.

## current status

1. Creates an openGL 3.2 context successfully on mac
2. GLSL 150 seems to work
3. we haz success! works on arch linux64:
	Vendor:   NVIDIA Corporation
	Renderer: GeForce 9600M GT/PCIe/SSE2
	Version:  3.2.0 NVIDIA 310.19
	GLSL:     1.50 NVIDIA via Cg compiler

## things that *seem* to work w / ofxGL3Renderer::

* ofCamera use and operations.
* ofCircle()
* ofTriangle()
* ofRect()
* of*Matrix()
* ofTranslate()
* ofScale()
* ofRotate*()

## Screenshots

### OS X 
![image](http://poniesandlight.com/static/screenshot_gl3_renderer.png "ofxGL3Renderer on OS X")

### Linux (Arch Linux 64bit)
![image](http://poniesandlight.com/static/screenshot_gl3_renderer_linux2.png "ofxGL3Renderer on Linux")


# build notes 

to build on os x all should be set.

to build on linux using makefiles all should be set, too, use freshest openframeworks dev branch from git @ hash bbe2e01d142c2ec1ea89e9850ba79d6773fbc7b8 - remember to recompile openframeworksCompiled if pulling in new openframeworks dev branch code. 

to use the sublimeproject files, the project files need to be updated to point to the correct absolute paths, where absolute paths are given in the .sublime-project file.