/*
 * Boid.h
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	A 'Boid' object, this is pretty much lifted from the Boids.cpp file in the OpenSteer plugin
 */

#ifndef BOID_H_
#define BOID_H_


#include <sstream>
#include "OpenSteer/SimpleVehicle.h"
#include "OpenSteer/OpenSteerDemo.h"
#include "OpenSteer/Proximity.h"
#include "OpenSteer/Color.h"
#include "OpenSteer/Obstacle.h"
#include "OpenSteer/UnusedParameter.h"

#ifdef WIN32
// Windows defines these as macros :(
#undef min
#undef max
#endif

#ifndef NO_LQ_BIN_STATS
#include <iomanip> // for setprecision
#include <limits> // for numeric_limits::max()
#endif // NO_LQ_BIN_STATS


namespace oneday { namespace steering {

class Boid : public OpenSteer::SimpleVehicle {

	typedef OpenSteer::AbstractProximityDatabase< OpenSteer::AbstractVehicle* > ProximityDatabase;
	typedef OpenSteer::AbstractTokenForProximityDatabase< OpenSteer::AbstractVehicle* > ProximityToken;


	// type for a flock: an STL vector of Boid pointers
    typedef std::vector<Boid*> groupType;
    typedef OpenSteer::Vec3 OpenSteerVec3;
public:
	Boid( ProximityDatabase& pd );
	~Boid();

	void reset();
	void draw();
	void update( const float currentTime, const float elapsedTime );
private:
	static OpenSteer::ObstacleGroup obstacles;
	static OpenSteer::AVGroup neighbors;
	static float worldRadius;
#ifndef NO_LQ_BIN_STATS
	static size_t minNeighbors, maxNeighbors, totalNeighbors;
#endif // NO_LQ_BIN_STATS

	ProximityToken* proximityToken;

	OpenSteerVec3 steerToFlock();
	void sphericalWrapAround();
    void regenerateLocalSpace (const OpenSteerVec3& newVelocity,
                               const float elapsedTime);


	///// ACCESSORS
	void newPD( ProximityDatabase& pd );
};

} /* namespace steering */
} /* namespace oneday */
#endif /* BOID_H_ */
