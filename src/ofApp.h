#pragma once

#include "MC.hpp"
#include "ofMain.h"
#include "ofVideoGrabber.h"

class ofApp : public ofBaseApp {

  bool _videoEnabled = true;
  
	mc::MC _mc;

	ofVideoGrabber _camera;

public:
	void setup() override;
	void update() override;
	void draw() override;

	void drawCamera(int x, int y);

	void exit() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void mouseMoved(int x, int y) override;
	void mouseDragged(int x, int y, int button) override;
	void mousePressed(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
	void mouseEntered(int x, int y) override;
	void mouseExited(int x, int y) override;
	void windowResized(int w, int h) override;
	void dragEvent(ofDragInfo dragInfo) override;
	void gotMessage(ofMessage msg) override;
};
