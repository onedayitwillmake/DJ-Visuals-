/*
 * IVideoStream.cpp
 *
 *  Created on: April 1, 2012
 *      Author: mariogonzalez
 */

#ifndef IVIDEOSTREAM_H_
#define IVIDEOSTREAM_H_

#include "cinder/Surface.h"
#include <boost/cstdint.hpp>

// Forward declerations

// Decleration
namespace stream {
	class IVideoStream {
	public:
		virtual ~IVideoStream(){};

		virtual void enter() = 0;
		virtual void update() = 0;
		virtual void exit() = 0;
		virtual void draw() = 0;

		virtual const ci::Surface8u getSurface() = 0;
		// Wrap functions previously in the capture device
		virtual bool checkNewFrame() = 0;
		virtual int32_t getWidth() const = 0;
		virtual int32_t getHeight() const = 0;

	protected:
		ci::Surface8u _surface;
	};
}

#endif /* IVIDEOSTREAM_H_ */
