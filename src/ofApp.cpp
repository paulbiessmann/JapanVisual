#include "ofApp.h"
float fps = 25;

float vidPart         = 500; //frames in one vid till pause

//--------------------------------------------------------------
void ofApp::setup(){
    
    scene1 = 3750; //3750
    scene2 = 6750; //6750
    scene3 = 9000;
    
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofSetBackgroundColor(0);
    
    //ofEnableNormalizedTexCoords();
    ofEnableDepthTest();
    depthOfField.setup(ofGetWidth(), ofGetHeight());
    depthOfField.setFocalRange(100);
    depthOfField.setFocalDistance(200);

    
    ofVec2f center = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
    
    ofEnableLighting();
    light.setDiffuseColor( ofColor(255.f, 255.f, 255.f));
    light.setSpecularColor( ofColor(255.f, 255.f, 255.f));
    light.setPosition(0, 500, 500);

    backLight.setDiffuseColor( ofColor(255.f, 50.f, 50.f));
    backLight.setSpecularColor( ofColor(255.f, 0.f, 0.f));
    backLight.setPosition(-0, 500, -500);

    material.setShininess( 256 );
    

    img.load("openInv.png");
    imgOp.load("openTrans.png");
    
    width = ofGetWidth();
    height = ofGetHeight();

   
    img.resize(1050, 1150);
    imgOp.resize(100,100);
    
    slitSize = 3; //3
    
    int imgHeight = img.getHeight();
    int imgWidth = img.getWidth();
    
    num = imgHeight/slitSize;
    imgParts.resize(num);
    
    for(int i=0; i<imgHeight; i+=10){
        for(int j=0; j<imgWidth; j+=10){
            ofColor c = img.getColor(i,j);
            float lightness = c.getLightness();
            if(lightness < 50){
            
                mesh.addVertex(ofPoint(i,j,0));
                
                float saturation = c.getSaturation();
                float invSat = ofMap(saturation, 255,0 , -100, 100); //invertieren
                
                //mesh.addColor(c);
                
            }
        }
    }
    
    posHome.resize(num);
    for(int i=0; i<num; i++) {
        
        ofImage imgCrop = img;
        imgCrop.crop(0, slitSize*i, imgWidth, slitSize );
        imgParts[i] = imgCrop;
        posHome[i].x = 0;
        posHome[i].y = i * slitSize;
        
    }
    
    
    /* Patricles */
    particleResolution = 2; //8
    numP = (width/particleResolution) * (height/particleResolution);
    p.assign(numP, customParticle());
    bInitP = false;
    mode = MODE_EXPLODE;
    for(int i=0; i<numP; i++){
        p[i].setDrawMode(PARTICLE_MODE_POINTS);
        p[i].setParticleMode(PARTICLE_MODE_SNOW);
        p[i].reset();
    }
    
    recordedFrame = 0;
    
    /************ Video Record *********/
    ofSetFrameRate((int) fps);
    ofSetLogLevel(OF_LOG_VERBOSE);
    fileName = "japSign_v2";
    fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    vidRecorder.setVideoCodec("prores");
    vidRecorder.setVideoBitrate("2000k");
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    recordFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    recordFboFlip.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    bRecording = false;
    keyReleased('r');
    bEnd = false;
    /** end recording **/
    
    screenGrab.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    float dist = cam.getDistance();
    
//   depthOfField.setFocalDistance(200);
//    depthOfField.setFocalRange(200);

    if(!bPause){
        recordedFrame = vidRecorder.getNumVideoFramesRecorded();// + vidRecorder.getVideoQueueSize();
    }
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    

    
    
    if(!bPause){
        if(bInitP){
            for(int i=0; i<numP; i++){
                if(!p[i].pOff){
                    p[i].update();
                }
            }
        }
        recordFbo.getTexture().readToPixels(recordPixels);
        if(bRecording){
            bool success = vidRecorder.addFrame(recordPixels);
            
            if (!success) {
                ofLogWarning("This frame was not added!");
            }
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    recordFbo.begin();
    ofClear(0,0,0,250);
   
    
    if(!bPause){

    ofSetBackgroundColor(0);
    //img.draw(0,0, 500, 500);
    float f = recordedFrame;
    if(f < scene2){ // nach scene1 = 3750 steht das zeichen; nach scene2 = 6750 startet particles
        drawSticks();
    }
    else{
        
        if(!bInitP){
            screenGrab.begin();
            ofClear(0,0,0,255);
            drawSticks();
            screenGrab.end();
            screenGrab.draw(0,0);
            ofPixels pixels;
            screenGrab.readToPixels(pixels);
            imgScreen.setFromPixels(pixels);
            bInitP = true;
            ofColor c = ofColor(255,255,255,255);
           
            initParticles(p, imgScreen, c);
            for(int i=0; i<numP; i++){
                p[i].setStartingTime(ofGetElapsedTimef(), f);
            }
        }
        
        for(int i=0; i<numP; i++){
            if(!p[i].pOff){
                p[i].draw();
            }
        }
        
    }
    }
    recordFbo.end();
    recordFbo.draw(0,0, fullWidth/3, fullHeight/3);
    
    
    /* recording pause */
    if((int)recordedFrame % (int)vidPart == 0 && !bPause && recordedFrame > 1){
        bPause = true;
        keyReleased('r');
    }
    
    /* recording fortsetzen */
    if(vidRecorder.getVideoQueueSize() < 5 && bPause && recordedFrame < scene3){
        bPause = false;
        keyReleased('r');
    }
    
    /** END Recording **/
    if(recordedFrame - vidRecorder.getVideoQueueSize() > scene3 && !bEnd ){
        keyReleased('r');
        bEnd = true;
        bPause = true;
    }
    /* Close Recordign */
    if(vidRecorder.getVideoQueueSize() < 1 && recordedFrame > scene3 && bEnd){
        exit();
    }

    
}

//--------------------------------------------------------------
void ofApp::drawSticks(){
    float movementSpeed = 0.002;
    float cloudSize = ofGetWidth() / 2;
    float spacing = 200        ;
    
    
    depthOfField.begin();


    //recordedFrame = ofGetFrameNum();
    
    float dist = cam.getDistance();
    float rot = (recordedFrame-scene1) * 0.1;
    if(recordedFrame <= scene1){
        cam.orbit(rot, rot, dist);
    }
//    else{
//        cam.orbit(1,1,dist);
//    }
    
    cam.begin(depthOfField.getDimensions());
    ofScale(1, -1, 1);
    ofTranslate(0, -height/2);
    
        ofDisableLighting();
//    light.enable();
//    backLight.enable();
//    material.begin();
//    //light.setOrientation(ofVec3f(0,0,0));
//    //light.rotate(10, 1, 0, 1);
//    light.draw();
//    backLight.draw();
    
    
    for(int i=0; i<num; i++) {
        ofPushMatrix();
        
        float t = ((float)recordedFrame/25.0 + i * spacing) * movementSpeed;
        float iT = 5 - t;
        float f = recordedFrame;
        
        
        ofVec3f pos(
                    ofSignedNoise(t, 0, 0),
                    ofSignedNoise(0, t, 0),
                    ofSignedNoise(0, 0, t));
        
        pos *= cloudSize;
        float dis;
        if(f <= scene1){
            dis = (1.0- (f / (scene1+20)));
        }
        else{
            dis = 1 - (scene1 / (scene1+20));
        }
        pos *= dis;
        
        ofTranslate(pos * 0.08);
        ofRotateX(pos.x * 0.12);
        ofRotateY(pos.y * 0.12);
        ofRotateZ(pos.z * 0.12);
        
        
        ofSetColor(255);
        ofEnableNormalizedTexCoords();
        imgParts[i].bind();
        //        ofDrawBox(t*0.1 , i*slitSize*t*0.2, sin(t*0.4)*50, width, slitSize, slitSize);
      //  ofDrawBox(posHome[i].x - pos.x/t , posHome[i].y - pos.y/t, posHome[i].z - pos.z/t, width, slitSize, slitSize);
          ofDrawBox(posHome[i].x - pos.x , posHome[i].y - pos.y, posHome[i].z - pos.z, width, slitSize, slitSize);
        
        imgParts[i].unbind();
        
        ofDisableNormalizedTexCoords();
        
        ofPopMatrix();
    }
    cam.end();
    material.end();
    light.disable();
      depthOfField.end();
      depthOfField.getFbo().draw(0,0);
    
    if(0){
        //    cam.begin();
        //    ofSetColor(255);
        //    mesh.draw();
        //    ofTranslate(-255, -255);
        //
        //    int height = img.getHeight();
        //    int width = img.getWidth();
        //    for(int i=0; i<height; i+=8){
        //        for(int j=0; j<width; j+=8){
        //            ofColor c = img.getColor(i,j);
        //            float lightness = c.getLightness();
        //
        //
        ////            if(lightness < 50){
        //              ofDrawBox(ofPoint(i,j,sin(ofGetElapsedTimef())*lightness*0.1), 10);
        ////            }else{
        ////                ofDrawBox(ofPoint(i,j,0), 10);
        ////            }
        //            ofSetColor(c);
        //           // imgOp.draw(i,j, sin(ofGetElapsedTimef())*lightness*0.01, 5, 10);
        //
        //        }
        //    }
        //    cam.end();
    }
}

//--------------------------------------------------------------
void ofApp::initParticles(vector <customParticle> &pThis, ofImage &imgThis, ofColor &colorThis){
    int textWidth   = imgThis.getWidth();
    int textHeight  = imgThis.getHeight();
    ofPixels pixels = imgThis.getPixels();
    int pCount = 0;
    
    
    
    //points
    if(pThis[0].drawMode == PARTICLE_MODE_POINTS){
        for(unsigned int i = 0; i < textWidth - particleResolution; i += particleResolution){
            for(unsigned int j = 0; j < textHeight - particleResolution; j += particleResolution){
                
                pThis[pCount].setStartingTime(ofGetElapsedTimef(), recordedFrame);
                
                ofColor pxColor = pixels.getColor(i, j);
                
                //                pThis[pCount].setGlobalPos();
                
                if(pxColor == ofColor(0,0,0) ){
                    pThis[pCount].pOff = true;
                }
                
                pThis[pCount].setColor(pxColor);
                pThis[pCount].setPos(ofVec3f(i, j, ofRandom(-0,0)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
                
            }
        }
    }
    else if(pThis[0].drawMode == PARTICLE_MODE_TEXTURES){ //textures
        for(unsigned int i = 0; i <= textWidth-particleResolution ; i += particleResolution){
            for(unsigned int j = 0; j <= textHeight-particleResolution; j += particleResolution){
                
                pThis[pCount].setStartingTime(ofGetElapsedTimef() + 2.0, recordedFrame);
                
                ofColor pxColor = pixels.getColor(i, j);
                ofImage pxImage = imgThis;
                pxImage.crop(i,j,particleResolution,particleResolution);
                
                pThis[pCount].setColor(colorThis);
                pThis[pCount].setParticleImg(pxImage);
                pThis[pCount].setPos(ofVec3f(i, j, 0));//ofRandom(-100,100)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
            }
        }
        
    }
    
   // ofRandomize(pThis);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            //insert your path in the following row and your settings 1920x1080 60fps - the last 2 parms are zero because in my case I don't want to record audio..
            ofFilePath f;
            vidRecorder.setup( fileName+"_"+ofGetTimestampString()+".mov", recordFbo.getWidth(),recordFbo.getHeight(), (int) fps, 0, 0);
            
            //Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}
