/*
 * Constants.cpp
 *
 *  Created on: Oct 1, 2011
 *      Author: mariogonzalez
 */
#include "Constants.h"

namespace Constants {
	namespace Defaults {
		int BOID_COUNT;
	};

	namespace Boid {
		float WANDER_STRENGTH;
		float ARRIVE_STRENGTH;
		float ARRIVE_DISTANCE;
	}

	void init() {
		Constants::Defaults::BOID_COUNT = 300;

		Constants::Boid::WANDER_STRENGTH = 0.1f;
		Constants::Boid::ARRIVE_STRENGTH = 0.1f;
		Constants::Boid::ARRIVE_DISTANCE = 100;
	}
}

