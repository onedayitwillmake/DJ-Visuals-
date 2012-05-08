/*
 * VideoStreamLive.h
 *	Represents a live video stream coming from a capture device such as the webcam or PS3 eyecam
 *  Created on: Mar 31, 2012
 *      Author: onedayitwillmake
 *
 *      Notes:
 *      Install maccam to use PS3 eye - http://webcam-osx.sourceforge.net/downloads.html
 *		Once installed, Drag the component to library/applications
 */

#ifndef VIDEOSTREAMLIVE_H_
#define VIDEOSTREAMLIVE_H_

#include "cinder/Capture.h"
#include "IVideoStream.h"

namespace stream {
class VideoStreamLive : public IVideoStream {
public:
	VideoStreamLive();
	~VideoStreamLive();

	void enter();
	void update() {};
	void exit();
	void draw() {};

	const ci::Surface8u getSurface();
	bool checkNewFrame();
	int32_t getWidth() const { return _captureDevice.getWidth(); };
	int32_t getHeight() const { return _captureDevice.getHeight(); };

private:
	ci::Capture _captureDevice;
};

} /* namespace stream */
#endif /* VIDEOSTREAMLIVE_H_ */
