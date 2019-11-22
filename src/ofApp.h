#pragma once

#include "ofMain.h"
#include "ofxDOF.h"
#include "customParticle.h"
#include "ofxVideoRecorder.h"


enum sceneMode{
    MODE_EXPLODE,
    MODE_SNOW,
    MODE_TEX_GLITCH
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void exit();
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    void resetParticles();
    void initParticles(vector <customParticle>  &pThis, ofImage &imgThis, ofColor &colorThis);
    
    void drawSticks();
    
    
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    
    ofImage img;
    ofImage imgOp;
    
    ofMesh mesh;
    
    ofEasyCam cam;
    
    ofTexture tex;
    
    int slitSize;
    int num;
    vector <ofImage> imgParts;
    
    ofLight light;
    ofLight backLight;
    ofMaterial material;
    
    vector <ofVec3f> posHome;
    vector <ofBoxPrimitive> boxes;
    
    ofxDOF depthOfField;
    
  
    vector <customParticle> p;
    int particleResolution;
    
    /* Recording */
    ofxVideoRecorder    vidRecorder;
    bool                bRecording;
    ofFbo               recordFbo;
    ofFbo               recordFboFlip;
    ofPixels            recordPixels;
    string              fileName;
    string              fileExt;
    sceneMode           mode;
    ofFbo               screenData;
    int fullWidth       = 1920;
    int fullHeight      = 1080;
    bool                bEnd;
    unsigned long       recordedFrame;
    bool        bPause;

    
    int     width;
    int     height;
    
    ofImage imgScreen;
    ofFbo   screenGrab;
    bool    bInitP;
    int     numP;
    
    float   scene1;
    float   scene2;
    float   scene3;
    
};
