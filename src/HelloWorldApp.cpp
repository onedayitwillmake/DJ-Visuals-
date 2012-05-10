/**
 * EclipseLovesCinder example application
 *
 **********************************************************
 **********************************************************
 ********************* IMPORTANT **************************
 * On first run:
 * 	- Select Project -> Clean...
 *
 *  - If change the project name, you have to tell the debugger where the new one is:
 *  	Go into Run -> Debug Configurations
 *  	Set where the application lives to something like this
 *  	Debug/{APPNAME}/Contents/MacOS/{APPNAME}
 *
 **********************************************************
 **********************************************************
 *
 * This project is released under public domain, do whatever with it.
 *
 *
 * Mario Gonzalez
 * http://onedayitwillmake
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/Vector.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include "gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"

#include "stream/CameraStreamController.h"
#include "stream/VideoStreamPlayback.h"

#define kWINDOW_DIMENSIONS_WIDTH 1200
#define kWINDOW_DIMENSIONS_HEIGHT 800
#define kDATAMOSH_SPACING 100

class HelloWorldApp : public ci::app::AppBasic {
public:
	void setup();
	void setupShader();
	void setupCameraStream();

	std::string loadShader( const char* fileName );

	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void mouseDown( ci::app::MouseEvent event );
	void keyDown( ci::app::KeyEvent event );
	void update();
	void updateColorArray();
	void draw();
	void shutdown();

	ci::gl::GlslProg shader;
	ci::gl::Texture texture;
	float colors[ (kWINDOW_DIMENSIONS_WIDTH/kDATAMOSH_SPACING) * (kWINDOW_DIMENSIONS_HEIGHT/kDATAMOSH_SPACING) * 3 ];
	int colorSize;

	stream::CameraStreamController* cameraStream;
};

void HelloWorldApp::prepareSettings( ci::app::AppBasic::Settings *settings ) {
	settings->setWindowSize( kWINDOW_DIMENSIONS_WIDTH, kWINDOW_DIMENSIONS_HEIGHT );
}

void HelloWorldApp::setup() {
	// Test loading a texture
	setupShader();
	setupCameraStream();
}

void HelloWorldApp::setupCameraStream() {

	stream::VideoStreamPlayback* playback = new stream::VideoStreamPlayback();

	cameraStream = new stream::CameraStreamController();
	cameraStream->setup();
	cameraStream->setInitialState( playback );

	playback->loadMovieFile( ci::app::App::get()->getResourcePath().string() + "/LobbyFootageTwo.mov" );
}

void HelloWorldApp::updateColorArray() {
	// DEBUG - run once
	static bool firstRun = true; if( !firstRun ) return; firstRun = false;

	// determine size and number of buckets
	int spacing = kDATAMOSH_SPACING;
	int rowCount = floorf( (float)getWindowSize().y/(float)spacing );
	int columnCount = floorf( (float)getWindowSize().x/(float)spacing );
	colorSize = rowCount * columnCount;

	// Create buckets of color blocks
	for(int x = 0; x < getWindowSize().x; x+=spacing ) {
		for( int y = 0; y < getWindowSize().y; y+=spacing ) {
			int ix = x/spacing;
			int iy = y/spacing;
			int length = floorf( (float)getWindowSize().y/(float)spacing );

			// Convert the 2D array index, into a 1D
			int index = ix*length + iy;
			int colorIndex = index * 3; // RGB
			colors[ colorIndex + 0 ] = ci::Rand::randFloat();
			colors[ colorIndex + 1 ] = ci::Rand::randFloat();
			colors[ colorIndex + 2 ] = ci::Rand::randFloat();
		}
	}
}

void HelloWorldApp::setupShader() {
	try {
		shader = ci::gl::GlslProg( loadShader( "passThru_vert.vs" ).c_str() , loadShader( "dataMoshing_frag.fs" ).c_str()  );
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}

	std::cout << "HelloWorldApp::setupShader - Shader loaded successfully..." << std::endl;

}

std::string HelloWorldApp::loadShader( const char* fileName ) {
	std::string delimeter = "\n-----------------------------------\n";

	ci::fs::path vertexPath = ci::app::App::get()->getResourcePath();
	vertexPath /= fileName;

	std::string vertexShaderString = ci::loadString( ci::DataSourcePath::create( vertexPath ) );

	std::cout << vertexPath << delimeter << vertexShaderString <<  delimeter << std::endl;
	return vertexShaderString;
}

void HelloWorldApp::mouseDown( ci::app::MouseEvent event ) {
}

void HelloWorldApp::keyDown( ci::app::KeyEvent event ) {
	if( event.getChar() == ci::app::KeyEvent::KEY_q ) {
		quit();
	}
}

void HelloWorldApp::update() {
	cameraStream->update();
	updateColorArray();
}
void HelloWorldApp::draw() {
	// No new frame exit
	if( !cameraStream->getCurrentStream()->checkNewFrame() ) return;

	// clear out the window with black
	ci::Color aColor = ci::Color( 0, 0, 0 );
	aColor.r = fabs( cosf(getElapsedFrames() * 0.008) );
	aColor.g = fabs( sinf(getElapsedFrames() * 0.01) );
	aColor.b = (float) getMousePos().x / getWindowWidth();

	ci::gl::clear( ci::Color( 0, 0, 0 ) );
	ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );

	float mAngle = atan2f( (float)getMousePos().y, (float) getMousePos().x );


	ci::gl::Texture aTexture = ci::gl::Texture( cameraStream->getCurrentStream()->getSurface() );

	aTexture.enableAndBind();
		shader.bind();
		 	glUniform3fv( shader.getHandle(), colorSize, colors );
			shader.uniform( "colorSize", (float)colorSize );
			shader.uniform( "windowSize", ci::Vec2f( getWindowSize().x, getWindowSize().y) );
			ci::gl::draw( aTexture, ci::Rectf(0,0, getWindowWidth(), getWindowHeight() ));
		shader.unbind();
	aTexture.unbind();
}


void HelloWorldApp::shutdown() {
	std::cout << "Shutdown..." << std::endl;
	AppBasic::shutdown();
}
CINDER_APP_BASIC( HelloWorldApp, ci::app::RendererGl )
//
