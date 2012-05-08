/*
 * VideoStreamPlayback.cpp
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 *      Abstract:
 *      	Streams data from a quicktime movie.
 *      	Supports dragging and dropping onto the application to create a new movie
 */
#include "VideoStreamPlayback.h"

#include "cinder/Surface.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/FileDropEvent.h"
#include <iostream>

namespace stream {

VideoStreamPlayback::VideoStreamPlayback() {
	_blackSurface.reset();
}

VideoStreamPlayback::~VideoStreamPlayback() {
	ci::app::App::get()->unregisterFileDrop( filedropCallbackId );
}

void VideoStreamPlayback::enter() {
	_movie.reset();
	filedropCallbackId = ci::app::App::get()->registerFileDrop( this, &VideoStreamPlayback::fileDrop );
}

void VideoStreamPlayback::exit() {
	if( _movie ) _movie.stop();
	ci::app::App::get()->unregisterFileDrop( filedropCallbackId );
}

// Return a black surface if nothing is available so that app does not crash
const ci::Surface8u VideoStreamPlayback::getSurface() {
	if( _movie ) return _movie.getSurface();
	else return _blackSurface;
}

bool VideoStreamPlayback::checkNewFrame() {
	if(_movie) return _movie.checkNewFrame();
	return false;
}

bool VideoStreamPlayback::fileDrop( ci::app::FileDropEvent event ) {
	std::cout << "File dropped! " << event.getFile( 0 ) << std::endl;
	if( event.getNumFiles() != 1 ) return false;
	loadMovieFile( event.getFile( 0 ) );

	return true;
}

void VideoStreamPlayback::loadMovieFile( std::string pathString ) {
	ci::fs::path moviepath( pathString );
	loadMovieFile( moviepath );
}

void VideoStreamPlayback::loadMovieFile( const ci::fs::path &path ) {
	try {
		std::cout << path.string() << std::endl;

		// load up the movie, set it to loop, and begin playing
		_movie = ci::qtime::MovieSurface( path );
		_movie.setLoop();
		_movie.play();

		// Info about the movie
//		std::cout << _moviefilename().string() << std::endl;
		std::cout << (ci::toString( _movie.getWidth() ) + " x " + ci::toString( _movie.getHeight() ) + " pixels") << std::endl;
		std::cout << ci::toString( _movie.getDuration() ) + " seconds" << std::endl;
		std::cout << ci::toString( _movie.getNumFrames() ) + " frames" << std::endl;
		std::cout << ci::toString( _movie.getFramerate() ) + " fps" << std::endl;
	}
	catch( ... ) {
		std::cout << "VideoStreamPlayback Unable to load the movie." << std::endl;
		_movie.reset();
	}
}
}
