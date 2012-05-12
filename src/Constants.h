/*
 * Constants.h
 *
 *  Created on: Sep 25, 2011
 *      Author: onedayitwillmake
 */
#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include "cinder/Vector.h"
namespace Constants {
	namespace Defaults {
		extern int BOID_COUNT;
	};

	namespace Boid {
		extern float WANDER_STRENGTH;
		extern float ARRIVE_STRENGTH;
		extern float ARRIVE_DISTANCE;
	}

	void init();
}
#endif /* CONSTANTS_H_ */
