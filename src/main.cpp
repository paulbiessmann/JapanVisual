#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
//    settings.setGLVersion(3,2);
        settings.setGLVersion(2, 1);
    settings.width = 1920;
    settings.height = 1080;
    ofCreateWindow(settings);
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
