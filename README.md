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

## screenshot

### OS X 
![image](http://poniesandlight.com/static/screenshot_gl3_renderer.png "ofxGL3Renderer")

### Linux (Arch Linux 64bit)
![image](http://poniesandlight.com/static/screenshot_gl3_renderer_linux.png "ofxGL3Renderer")
