/*
 * VideoStreamPlayback.h
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 *      Abstract:
 *      	Streams data from a quicktime movie.
 *      	Supports dragging and dropping onto the application to create a new movie
 */

#ifndef VIDEOSTREAMPLAYBACK_H_
#define VIDEOSTREAMPLAYBACK_H_

#include "cinder/qtime/QuickTime.h"
#include "IVideoStream.h"
#include "cinder/Utilities.h"

// FWD
namespace cinder { namespace app { class App; class FileDropEvent; class MouseEvent; } };
//namespace mowa { namespace sgui { class Control; class LabelControl; class ButtonControl; class BoolVarControl; class IntVarControl; class SimpleGUI; }}

namespace stream {
class VideoStreamPlayback : public IVideoStream {
public:
	VideoStreamPlayback();
	~VideoStreamPlayback();

	void enter();
	void update() {};
	void exit();
	void draw() {};

	// Camera Proxy
	const ci::Surface8u getSurface();
	bool checkNewFrame();
	int32_t getWidth()  const { return _movie.getWidth(); };
	int32_t getHeight()  const { return _movie.getHeight(); };

	// Movie loading
	int filedropCallbackId;
	bool fileDrop( ci::app::FileDropEvent event );

	void loadMovieFile( const ci::fs::path &path );
	void loadMovieFile( std::string pathString );
private:
	cinder::Surface8u	_blackSurface;
	cinder::qtime::MovieSurface _movie;
};

} /* namespace stream */
#endif /* VIDEOSTREAMPLAYBACK_H_ */
