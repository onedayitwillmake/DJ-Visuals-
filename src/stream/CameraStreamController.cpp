/*
 * CameraStreamController.cpp
 *
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 *      Abstract:
 *      	This class contains a UserStreamStateMachine which can has the following or more states (classes):
 *      		* UserStreamLive
 *      		* UserStreamPlayback
 *      		* UserStreamRecorder
 *      	It determines which state the UserStreamStateMachine should be in,
 *      	and asks the UserStreamStateMachine for skeleton information each frame
 */

#include "CameraStreamController.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/app/FileDropEvent.h"

#include "IVideoStream.h"
#include "VideoStreamLive.h"
#include "VideoStreamPlayback.h"

namespace stream {
	CameraStreamController::CameraStreamController() {

		currentState = NULL;
		previousState = NULL;
		nextState = NULL;

		// Test the live stream
//		stream::VideoStreamLive* live = new stream::VideoStreamLive();
//		setInitialState( live );

		// Test using the player
//p		stream::VideoStreamPlayback* playback = new stream::VideoStreamPlayback();
//		setInitialState( playback );
//		playback->loadMovieFile( ci::app::App::get()->getResourcePath().string() + "/" + "LobbyFootageTwo.mov" );

		setupDebug();
	}

	CameraStreamController::~CameraStreamController() {
		delete currentState;
		delete previousState;
		delete nextState;
		std::cout << "CameraStreamController destructor!" << std::endl;
	}

	void CameraStreamController::setupDebug() {
//		// GUI
//		if( Constants::Defaults::USE_GUI ) {
//			_debugGUI = new mowa::sgui::SimpleGUI( ci::app::App::get() );
//			_debugGUI->textColor = ColorA(1,1,1,1);
//			_debugGUI->lightColor = ColorA(1, 0, 1, 1);
//			_debugGUI->darkColor = ColorA(0.05,0.05,0.05, 1);
//			_debugGUI->bgColor = ColorA(0.15, 0.15, 0.15, 1.0);
//			_debugGUI->addColumn();
//			_debugGUI->addColumn();
//			_debugGUI->addColumn();
//			_debugGUI->addLabel( "SetState" );
//			_debugGUI->addButton("Live")->registerClick( this, &CameraStreamController::setStateLive );
//			_debugGUI->addButton("Playback")->registerClick( this, &CameraStreamController::setStatePlayback );
//		}
	}

	void CameraStreamController::update() {
		if( currentState ) {
			currentState->update();
		}
	}

	void CameraStreamController::draw() {
//		renderGUI();

		if( currentState ) {
			currentState->draw();
		}
	}

	///// Debug drawing
	void CameraStreamController::renderGUI() {
//		if( !Constants::Defaults::USE_GUI ) return;

//		_debugGUI->draw();
		ci::gl::disableDepthRead();
		ci::gl::disableDepthWrite();
		ci::gl::enableAlphaBlending();
	}

	void CameraStreamController::setup() {
		filedropCallbackId = ci::app::App::get()->registerFileDrop( this, &CameraStreamController::fileDrop );
	}

	void CameraStreamController::setInitialState( IVideoStream* aState ) {
		setup();
		currentState = aState;
		currentState->enter();
	}

	void CameraStreamController::changeState( IVideoStream* aState ) {

		if(aState != previousState)
			delete previousState;

		currentState->exit();
		previousState = currentState;

		currentState = aState;
		currentState->enter();
	}

	void CameraStreamController::gotoPreviousState() {
		changeState( previousState );
	}

	void CameraStreamController::gotoNextState()  {
		changeState( nextState );
		nextState = NULL;
	}

	bool CameraStreamController::fileDrop( ci::app::FileDropEvent event ) {
		std::cout << "CameraStreamController::fileDrop! " << event.getFile( 0 ) << std::endl;
		if( event.getNumFiles() != 1 ) return false;

		// has .mov extension, good enough for us - change to video stream state if not already
		std::string fileRef = event.getFile(0).string();

		// NOTE: removed check to see if we're already in playback since we dont care, and just want to stack the state anyway
		if ( std::string::npos != fileRef.find(".mov") /* && dynamic_cast<VideoStreamPlayback*>( currentState ) == NULL */) {
			stream::VideoStreamPlayback* playback = new stream::VideoStreamPlayback();

			// Change state - could be 1 lined but thats just for showoffs
			setNextState( playback );
			gotoNextState();
			playback->fileDrop( event );
		}

		return true;
	}

	// Debug state switching
	bool CameraStreamController::setStateLive( ci::app::MouseEvent event ) { changeState( new stream::VideoStreamLive() ); return true; };
	bool CameraStreamController::setStatePlayback( ci::app::MouseEvent event ) { changeState( new stream::VideoStreamPlayback() ); return true; };
}
