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

#include "cinder/app/AppBasic.h"

namespace oneday { namespace steering {

///// SETUP
void OpenSteerController::setup() {

	currentTime = ci::app::App::get()->getElapsedSeconds();

    // make default-sized flock
	population = 0;
    for (int i = 0; i < 200; i++) addBoidToFlock ();
}

///// MEMORY
OpenSteerController::~OpenSteerController() {
    while ( population > 0 ) removeBoidFromFlock (); // delete each member of the flock
    delete pd; pd = NULL;
}


void OpenSteerController::reset() {
    // reset each boid in flock
    for(std::vector< Boid* >::const_iterator itr = _flock.begin(); itr != _flock.end(); ++itr) {
    	(**itr).reset();
    }

}

void OpenSteerController::update() {
	double delta = ci::app::App::get()->getElapsedSeconds() - currentTime;

#ifndef NO_LQ_BIN_STATS
	Boid::maxNeighbors = Boid::totalNeighbors = 0;
	Boid::minNeighbors = std::numeric_limits<int>::max();
#endif // NO_LQ_BIN_STATS

    // update flock simulation for each boid
    for(std::vector< Boid* >::const_iterator itr = _flock.begin(); itr != _flock.end(); ++itr) {
        (**itr).update(currentTime, delta);
    }

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

