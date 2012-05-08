/*
 * VideoStreamLive.cpp
 *
 *  Created on: Mar 31, 2012
 *      Author: onedayitwillmake
 */

#include "VideoStreamLive.h"

namespace stream {

VideoStreamLive::VideoStreamLive() {
	// TODO Auto-generated constructor stub

}

VideoStreamLive::~VideoStreamLive() {
	if( _captureDevice ) _captureDevice.stop();
//	_captureDevice = NULL;
}

// Install maccam to use PS3 eye - http://webcam-osx.sourceforge.net/downloads.html
// Drag the component to library/applications
void VideoStreamLive::enter() {
	try {
		// macam #0: Sony HD Eye for PS3 (SLEH 00201)
		const std::vector<ci::Capture::DeviceRef>& devices = ci::Capture::getDevices();
		for( std::vector<ci::Capture::DeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter ) {
			std::cout << (*iter)->getName() << std::endl;
		}

		// Use PS3EYE if exist
		ci::Capture::DeviceRef ps3Eye = ci::Capture::findDeviceByNameContains( "PS3" );
		if( ps3Eye && ps3Eye->isConnected() ) _captureDevice = ci::Capture( 640, 480, ps3Eye);
		else _captureDevice = ci::Capture( 640, 480 );

		_captureDevice.start();
	} catch( ... ) {
		std::cout << "Failed to initialize capture" << std::endl;
	}
}

void VideoStreamLive::exit() {
	if( _captureDevice ) _captureDevice.stop();
//	_captureDevice = NULL;
}

const ci::Surface8u VideoStreamLive::getSurface() {
	return _captureDevice.getSurface();
}

bool VideoStreamLive::checkNewFrame() {
	if(_captureDevice) return _captureDevice.checkNewFrame();
	return false;
}


} /* namespace stream */
