/*
 * OpenSteerController.cpp
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	Provides wrapper for OpenSteer
 */

#ifndef OPENSTEERCONTROLLER_H_
#define OPENSTEERCONTROLLER_H_
#include <vector>
#include "OpenSteer/Proximity.h"
#include "oneday/steering/Boid.h"


namespace oneday { namespace steering {
class OpenSteerController {

public:
	OpenSteerController(){};
	virtual ~OpenSteerController();

	void setup();
	void reset();
	void update();
	void draw();

    void addBoidToFlock();
    void removeBoidFromFlock();
private:
	double currentTime;

	int					population;
	std::vector< Boid* >	_flock;
	ProximityDatabase*		pd;
};

} /* steering controller */ } /* oneday controller */

#endif /* OPENSTEERCONTROLLER_H_ */
