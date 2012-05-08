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


class HelloWorldApp : public ci::app::AppBasic {
public:
	void setup();
	void setupShader();
	void setupCameraStream();

	std::string loadShader( const char* fileName );

	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void mouseDown( ci::app::MouseEvent event );
	void update();
	void draw();
	void shutdown();

	ci::gl::GlslProg shader;
	ci::gl::Texture texture;

	stream::CameraStreamController* cameraStream;
};

void HelloWorldApp::prepareSettings( ci::app::AppBasic::Settings *settings ) {
	settings->setWindowSize( 800, 600 );
}

void HelloWorldApp::setup() {
	// Test loading a texture
	setupShader();
	setupCameraStream();
	texture = ci::gl::Texture( ci::loadImage( ci::app::App::get()->loadResource( RES_WHEEL ) ) );
}

void HelloWorldApp::setupCameraStream() {

	stream::VideoStreamPlayback* playback = new stream::VideoStreamPlayback();
	playback->loadMovieFile( ci::app::App::get()->getResourcePath().string() + "/LobbyFootageOne.mov" );

	cameraStream = new stream::CameraStreamController();
	cameraStream->setup();
	cameraStream->setInitialState( playback );

}

void HelloWorldApp::setupShader() {
	try {
		shader = ci::gl::GlslProg( loadShader( "passThru_vert.glsl" ).c_str() , loadShader( "gaussianBlur_frag.glsl" ).c_str()  );
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}

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

void HelloWorldApp::update() {
	cameraStream->update();
}
void HelloWorldApp::draw() {
	// clear out the window with black
	ci::Color aColor = ci::Color( 0, 0, 0 );
	aColor.r = fabs( cosf(getElapsedFrames() * 0.008) );
	aColor.g = fabs( sinf(getElapsedFrames() * 0.01) );
	aColor.b = (float) getMousePos().x / getWindowWidth();

//	ci::gl::clear( ci::Color( 0, 0, 0 ) );
	ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );


/**
 * mTexture.enableAndBind();
	mShader.bind();
	mShader.uniform( "tex0", 0 );
	mShader.uniform( "sampleOffset", Vec2f( cos( mAngle ), sin( mAngle ) ) * ( 3.0f / getWindowWidth() ) );
 */


	std::cout << cameraStream->getCurrentStream()->checkNewFrame() << std::endl;

	static int counter = 0;
	if( ++counter < 10 ) return;
//	if( cameraStream->getCurrentStream()->checkNewFrame() ) {
		float mAngle = getElapsedSeconds() * 0.1f;
		(float)cameraStream->getCurrentStream()->getWidth();
		(float)cameraStream->getCurrentStream()->getHeight();

		ci::gl::Texture aTexture = ci::gl::Texture( cameraStream->getCurrentStream()->getSurface() );
		aTexture.enableAndBind();
			shader.bind();
				shader.uniform( "text0", 0 );
				shader.uniform( "sampleOffset", ci::Vec2f( cos(mAngle), sin(mAngle) ) * ( 3.0f / getWindowWidth() ) );
				ci::gl::draw( aTexture, getWindowCenter() );
			shader.unbind();
		aTexture.unbind();


//	}

	if ( texture ) {

//		cameraStream.draw();

//		texture.enableAndBind();
//		std::cout << *cameraStream.getCurrentStream() << std::endl;

//		ci::gl::Texture aTexture = ci::gl::Texture( cameraStream.getCurrentStream()->getSurface() );
//		aTexture.enableAndBind();

//		aTexture.unbind();
//		texture.unbind();

	}

	;

}


void HelloWorldApp::shutdown() {
	std::cout << "Shutdown..." << std::endl;
	AppBasic::shutdown();
}
CINDER_APP_BASIC( HelloWorldApp, ci::app::RendererGl )
//
