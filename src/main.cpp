#include "ofMain.h"
#include "ofApp.h"
#include "ofWindowSettings.h"

//========================================================================
int main( ){

	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(1280, 720);
	// settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN
  settings.windowMode = OF_FULLSCREEN;

	auto window = ofCreateWindow(settings);

	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();

}
