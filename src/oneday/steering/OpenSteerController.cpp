/*
 * OpenSteerController.cpp
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	Provides wrapper for OpenSteer
 */


#include "oneday/steering/OpenSteerController.h"

#include "OpenSteer/SimpleVehicle.h"
#include "OpenSteer/OpenSteerDemo.h"
#include "OpenSteer/Proximity.h"
#include "OpenSteer/Color.h"
#include "OpenSteer/Obstacle.h"
#include "OpenSteer/UnusedParameter.h"

#include "oneday/steering/Boid.h"

#include "cinder/app/AppBasic.h"

namespace oneday { namespace steering {

OpenSteerController::OpenSteerController() {}
OpenSteerController::~OpenSteerController() {}
void OpenSteerController::setup() {
	population = 0;
	currentTime = ci::app::App::get()->getElapsedSeconds();
}

void OpenSteerController::update() {
	double delta = ci::app::App::get()->getElapsedSeconds() - currentTime;
	currentTime = ci::app::App::get()->getElapsedSeconds();
}

void OpenSteerController::draw() {
}

void OpenSteerController::addBoidToFlock() {
	Boid* boid = new Boid (*pd);
	_flock.push_back( boid );
	population++;
}

void OpenSteerController::removeBoidFromFlock() {
	if (population > 0) {
		const Boid* aBoid = _flock.back();
		_flock.pop_back();

		// TODO: do somet stuff with the boid before removal

		delete aBoid;
		--population;
	}
}

} /* steering controller */ } /* oneday controller */

