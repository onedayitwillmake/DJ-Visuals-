/*
 * HotSugarShow.cpp
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 *      Abstract:
 *      	A live visualizer for Ninjatune artist HotSugar, focus is on parametric datamoshing effect
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
#include "Boid.h"
#include "oneday/steering/OpenSteerController.h"

#include <algorithm>
#include <list>

#define kWINDOW_DIMENSIONS_WIDTH 1200
#define kWINDOW_DIMENSIONS_HEIGHT 800
#define kDATAMOSH_SPACING 80

#include "Constants.h"
#include "SimpleGUI.h"

//namespace mowa { namespace sgui { class SimpleGUI; } }

class HelloWorldApp : public ci::app::AppBasic {
public:
	void setup();
	void setupGui();
	void setupSwarm();
	void setupShader();
	void setupSteering();
	void setupCameraStream();

	std::string loadShader( const char* fileName );

	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void mouseDown( ci::app::MouseEvent event );
	void keyDown( ci::app::KeyEvent event );
	void update();
	void updateColorArray();
	void updateCameraTexture();
	void draw();
	void shutdown();

	ci::gl::GlslProg shader;

	ci::gl::Texture cameraTexture;
	ci::Surface8u cameraSurface;

	float colors[ (kWINDOW_DIMENSIONS_WIDTH/kDATAMOSH_SPACING) * (kWINDOW_DIMENSIONS_HEIGHT/kDATAMOSH_SPACING) * 3 ];
	float oldColors[ (kWINDOW_DIMENSIONS_WIDTH/kDATAMOSH_SPACING) * (kWINDOW_DIMENSIONS_HEIGHT/kDATAMOSH_SPACING) * 3 ];
	int colorSize;
	float requiredDelta;

	stream::CameraStreamController* cameraStream;
	bool							cameraHasNewFrame;

	// TODO: REMOVE and move into OpenSteer
	std::list< oneday::Boid* >		_swarm;
	oneday::steering::OpenSteerController *openSteerController;

	// DEBUG
	mowa::sgui::SimpleGUI* _gui;
	bool					_drawGui;
	bool debugResetBoids( ci::app::MouseEvent event );
};

void HelloWorldApp::prepareSettings( ci::app::AppBasic::Settings *settings ) {
	settings->setWindowSize( kWINDOW_DIMENSIONS_WIDTH, kWINDOW_DIMENSIONS_HEIGHT );
	Constants::init();
}

void HelloWorldApp::setup() {
	// Test loading a texture
	setupShader();
	setupCameraStream();
	setupSwarm();
	setupGui();
	setupSteering();

	// Hack to force it to show up above everything else on start -
	this->setAlwaysOnTop( true );
}

void HelloWorldApp::setupCameraStream() {
	cameraHasNewFrame = false;
	stream::VideoStreamPlayback* playback = new stream::VideoStreamPlayback();

	cameraStream = new stream::CameraStreamController();
	cameraStream->setup();
	cameraStream->setInitialState( playback );

	playback->loadMovieFile( ci::app::App::get()->getResourcePath().string() + "/LobbyFootageTwo.mov" );
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

	requiredDelta = 0;
	std::cout << "HelloWorldApp::setupShader - Shader loaded successfully..." << std::endl;
}

void HelloWorldApp::setupSwarm() {

	for( std::list< oneday::Boid* >::iterator itr = _swarm.begin(); itr != _swarm.end(); ++itr ) {
		delete (*itr);
	}
	_swarm.clear();

	//_velocity = ci::Rand::randVec3f();q
	for (int i = 0; i < Constants::Defaults::BOID_COUNT; ++i) {
		oneday::Boid* boid = new oneday::Boid( 8.0f, 2.0f );

		// Note we use the diagonal size of the window as the maximum distance an influence map can have - corner to corner
		float windowDiagonalSize = sqrtf( powf((float)getWindowWidth(), 2) + powf( (float)getWindowHeight(), 2) );
		float angle = ci::Rand::randFloat( M_PI * 2 );

		boid->setPosition( getWindowWidth()/2 + windowDiagonalSize/2 * cosf(angle), getWindowHeight()/2 + windowDiagonalSize/2*sinf(angle), 0 );
		_swarm.push_back( boid );
	}
}

void HelloWorldApp::setupSteering() {
	openSteerController = new oneday::steering::OpenSteerController();
//	OpenSteer::OpenSteerDemo::initialize ();
//	OpenSteer::initializeGraphics (argc, argv);
//	OpenSteer::runGraphics ();
}

void HelloWorldApp::setupGui() {
	_gui = new mowa::sgui::SimpleGUI( this );
	_gui->textColor = ci::ColorA(1,1,1,1);
	_gui->lightColor = ci::ColorA(1, 0, 1, 1);
	_gui->darkColor = ci::ColorA(0.05,0.05,0.05, 1);
	_gui->bgColor = ci::ColorA(0.20, 0.20, 0.20, 0.5);
	_gui->addColumn();

	// OpenCV Capture Constants
	_gui->addLabel("Boid");
	_gui->addParam("Total", &Constants::Defaults::BOID_COUNT, 0, 600, Constants::Defaults::BOID_COUNT);
	_gui->addParam("WanderStrength", &Constants::Boid::WANDER_STRENGTH, 0.0, 0.95, Constants::Boid::WANDER_STRENGTH);
	_gui->addParam("ArriveStrength", &Constants::Boid::ARRIVE_STRENGTH, 0.0, 0.95, Constants::Boid::ARRIVE_STRENGTH);
	_gui->addParam("ArriveDistance", &Constants::Boid::ARRIVE_DISTANCE, 1, 200, Constants::Boid::ARRIVE_DISTANCE);
	_gui->addColumn();
	_gui->addLabel("DataMoshing");
	_gui->addParam("requiredDelta", &requiredDelta, 0.0, 3.0, requiredDelta);

	_gui->addButton("Reset")->registerClick( this, &HelloWorldApp::debugResetBoids );
	_drawGui = false;
}
bool HelloWorldApp::debugResetBoids( ci::app::MouseEvent event ) {
	setupSwarm(); return true;
};

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

	// Quit app
	if( event.getChar() == ci::app::KeyEvent::KEY_q ) quit();

	// Reset swarm
	if( event.getChar() == ci::app::KeyEvent::KEY_r ) setupSwarm();

	// Toggle Gui
	if( event.getChar() == ci::app::KeyEvent::KEY_g ) {
		std::cout << "Toggle GUI:" << _gui->isEnabled() << std::endl;
		ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
//		_drawGui = !_drawGui;
//		_gui->setEnabled( _drawGui );
	}

}


void HelloWorldApp::updateColorArray() {
	// DEBUG - run once
//	static bool firstRun = true; if( !firstRun ) return; firstRun = false;
	static bool firstRun = true;

	memcpy( oldColors, colors, sizeof(colors) );

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

			int videoWidth = cameraStream->getCurrentStream()->getWidth();
			int videoHeight = cameraStream->getCurrentStream()->getHeight();
			int px = (float)ix/(float)columnCount * videoWidth;
			int py = (float)iy/(float)rowCount * videoHeight;
			ci::Color8u pixelColor = cameraSurface.getPixel( ci::Vec2i( px, py ) );

			colors[ colorIndex + 0 ] = (float)pixelColor.r/255.0f;//( (float)ix/(float)columnCount );
			colors[ colorIndex + 1 ] = (float)pixelColor.g/255.0f;//( (float)iy/(float)rowCount );
			colors[ colorIndex + 2 ] = (float)pixelColor.b/255.0f;

			if( firstRun ) {
//				std::cout << "px:" << px << "\tpy:" << py << std::endl;
			}


			//std::cout << "x:"<< x << "\t y:" << y << "\t iX:" << ix << "\t iY: " << iy << "\t index:" << index << "\t t:" << derrivedIndex << "\t xlength:" << length << "\t colorSize:" << colorSize << std::endl;
		}
	}

	firstRun = false;
}


void HelloWorldApp::updateCameraTexture() {
	cameraSurface = cameraStream->getCurrentStream()->getSurface();
	cameraTexture = ci::gl::Texture( cameraSurface );
}

void HelloWorldApp::update() {
	// HACK TO BRING TO FRONT AT LAUNCHq
	static bool hasBecomeFirstResponder = false;
	if( !hasBecomeFirstResponder && getElapsedSeconds() > 2 ) {
		hasBecomeFirstResponder = true;
		this->setAlwaysOnTop( false );
	}

//	for( std::list< oneday::Boid* >::iterator itr = _swarm.begin(); itr != _swarm.end(); ++itr ) {
//		(*itr)->wander( Constants::Boid::WANDER_STRENGTH );
//		(*itr)->arriveWithEaseDistance( ci::Vec3f( getWindowWidth()/2, getWindowHeight()/2, 0.0f), Constants::Boid::ARRIVE_DISTANCE, Constants::Boid::ARRIVE_STRENGTH );
//		(*itr)->update();
//	}

//	return;
	cameraStream->update();
	cameraHasNewFrame = cameraStream->getCurrentStream()->checkNewFrame();
	if( !cameraHasNewFrame ) return;

	updateCameraTexture();
	updateColorArray();
	requiredDelta *= 0.9f;
}
void HelloWorldApp::draw() {
//	ci::gl::clear( ci::Color( 0, 0, 0 ) );
	ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );

//	for( std::list< oneday::Boid* >::iterator itr = _swarm.begin(); itr != _swarm.end(); ++itr ) {
//		ci::gl::drawSolidCircle( ci::Vec2f((*itr)->_position.x, (*itr)->_position.y), 5, 6 );
//	}



	// Draw moshed camera
	if( cameraHasNewFrame ) {
		cameraTexture.enableAndBind();
			shader.bind();
				glUniform3fv( shader.getHandle(), colorSize, colors );
				glUniform3fv( shader.getHandle(), colorSize, oldColors );
				shader.uniform( "requiredDelta", (float)getMousePos().x / (float)getWindowSize().x );
				shader.uniform( "colorSize", (float)colorSize );
				shader.uniform( "windowSize", ci::Vec2f( getWindowSize().x, getWindowSize().y) );
				ci::gl::draw( cameraTexture, ci::Rectf(0,0, getWindowWidth(), getWindowHeight() ));
			shader.unbind();
		cameraTexture.unbind();
	}
}


void HelloWorldApp::shutdown() {
	std::cout << "Shutdown..." << std::endl;
	AppBasic::shutdown();
}
CINDER_APP_BASIC( HelloWorldApp, ci::app::RendererGl )
//
