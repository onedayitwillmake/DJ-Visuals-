/*
 * CameraStreamController.cpp
 *
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 *      Abstract:
 *      	Use an instance of CameraStreamController to receive camera information, instead of using the camera directly,
 *      	This allows for changing of to pre-recorded video at any point during runtime useful during debugging
 *      	Currently supports the following types:
 *      		* VideoStreamLive
 *      		* VideoStreamPlayback
 *      		* VideoStreamRecorder
 */

#ifndef CAMERASTREAMCONTROLLER_H_
#define CAMERASTREAMCONTROLLER_H_

#include "IVideoStream.h"
#include "cinder/Vector.h"
#include "cinder/app/MouseEvent.h"

namespace cinder { namespace app { class App; class FileDropEvent; class MouseEvent; } };
namespace mowa { namespace sgui { class SimpleGUI; } }

namespace stream {
	class CameraStreamController {
	public:
		CameraStreamController();
		virtual ~CameraStreamController();

		void setup();		// Performance final setup, performs actions that if were called during constructor would fail. ( App not ready yet, not instantiated etc )
		void setupDebug();
		void update();
		void draw();
		void renderGUI();

		stream::IVideoStream* getCurrentStream() {  return currentState; };

		// FiniteStateMachine methods and properties
		void setInitialState( IVideoStream* aState );
		void setNextState( IVideoStream* aState ) { nextState = aState; }
		void gotoPreviousState();
		void gotoNextState();

	private:
		void changeState( IVideoStream* aState );

		IVideoStream* currentState;
		IVideoStream* previousState;
		IVideoStream* nextState;


		// FileIO - if a mov is dropped we just go to playback state
		int	filedropCallbackId;
		bool fileDrop( ci::app::FileDropEvent event );

		// Debug
		mowa::sgui::SimpleGUI* _debugGUI;


		void debugDraw();
		bool setStateLive( ci::app::MouseEvent event );
		bool setStatePlayback( ci::app::MouseEvent event );
	};

} /* stream */
#endif /* CAMERASTREAMCONTROLLER_H_ */
