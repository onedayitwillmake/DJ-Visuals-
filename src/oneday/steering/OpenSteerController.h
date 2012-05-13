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


namespace oneday { namespace steering {

// FWD
namespace OpenSteer { class AbstractProximityDatabase; class AbstractProximityDatabase; class AbstractVehicle; }
class Boid;

class OpenSteerController {

//	typedef OpenSteer::AbstractProximityDatabase< OpenSteer::AbstractVehicle* > ProximityDatabase;
//	typedef OpenSteer::AbstractTokenForProximityDatabase< OpenSteer::AbstractVehicle* > ProximityToken;

public:
	OpenSteerController();
	virtual ~OpenSteerController();

	void setup();
	void update();
	void draw();

    void addBoidToFlock();
    void removeBoidFromFlock();
private:
	double currentTime;


	OpenSteer::AbstractProximityDatabase< OpenSteer::AbstractVehicle* > *pd;

	int					population;
	std::vector< Boid* >	_flock;
};

} /* steering controller */ } /* oneday controller */

#endif /* OPENSTEERCONTROLLER_H_ */
