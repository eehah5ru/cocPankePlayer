#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofGraphics.h"
#include "ofLog.h"

//--------------------------------------------------------------
void ofApp::setup(){
  // ofSetLogLevel(OF_LOG_VERBOSE);

  ofSetLogLevel(OF_LOG_SILENT);

  ofHideCursor();
  _camera.setup(1280, 720);
  // _camera.setup(1920, 1080);
  
  _mc.setup("chapters", "distorted_maps");
  // _mc.setup("distorted_maps", "distorted_maps");
  _mc.start();
}

//--------------------------------------------------------------
void ofApp::update(){
  _camera.update();
  
  _mc.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(ofColor::deepPink);
  drawCamera(0, 0);
  if (_videoEnabled) {
    _mc.draw(0, 0);
  }
}


void ofApp::drawCamera(int x, int y) {
  float scaleF = mc::getScaleToWindowWidthFactor(_camera);
  int dy = (ofGetHeight() - _camera.getHeight()*scaleF) / 2;
  
  ofPushMatrix(); 
  ofScale(scaleF);
  _camera.draw(x, y+dy);

  ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'v') {
    _videoEnabled = !_videoEnabled;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

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
