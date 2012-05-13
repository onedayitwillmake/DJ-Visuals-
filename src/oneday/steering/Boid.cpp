/*
 * Boid.h
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	A 'Boid' object, this is pretty much lifted from the Boids.cpp file in the OpenSteer plugin
 */

#include "Boid.h"

namespace oneday { namespace steering {
///// Static definitions
OpenSteer::AVGroup			Boid::neighbors;
OpenSteer::ObstacleGroup	Boid::obstacles;
float						Boid::worldRadius;
#ifndef NO_LQ_BIN_STATS
	size_t Boid::minNeighbors, Boid::maxNeighbors, Boid::totalNeighbors;
#endif // NO_LQ_BIN_STATS


Boid::Boid( ProximityDatabase& pd ) {
	  // allocate a token for this boid in the proximity database
	proximityToken = NULL;
	newPD (pd);

	// reset all boid state
	reset ();
}

Boid::~Boid() {
	delete proximityToken;  // delete this boid's token in the proximity database
}

void Boid::reset() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
    using namespace OpenSteer;

	 // reset the vehicle
	SimpleVehicle::reset ();

	// steering force is clipped to this magnitude
	setMaxForce (27);

	// velocity is clipped to this magnitude
	setMaxSpeed (9);

	// initial slow speed
	setSpeed (maxSpeed() * 0.3f);

	// randomize initial orientation
	regenerateOrthonormalBasisUF( RandomUnitVector () );

	// randomize initial position
	setPosition (RandomVectorInUnitRadiusSphere () * 20);

	// notify proximity database that our position has changed
	proximityToken->updateForNewPosition(position());
}

void Boid::draw() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
    using namespace OpenSteer;

    drawBasic3dSphericalVehicle (*this, gGray70);
	drawTrail ();
}


// per frame simulation update
void Boid::update (const float currentTime, const float elapsedTime) {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
    using namespace OpenSteer;

	OPENSTEER_UNUSED_PARAMETER(currentTime);

	// steer to flock and avoid obstacles if any
	applySteeringForce (steerToFlock(), elapsedTime);

	// wrap around to contrain boid within the spherical boundary
	sphericalWrapAround ();

	// notify proximity database that our position has changed
	proximityToken->updateForNewPosition (position());
}


Boid::OpenSteerVec3 Boid::steerToFlock() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

	 // avoid obstacles if needed
	// XXX this should probably be moved elsewhere
	const Vec3 avoidance = steerToAvoidObstacles (1.0f, obstacles);
	if (avoidance != Vec3::zero) return avoidance;

	const float separationRadius =  5.0f;
	const float separationAngle  = -0.707f;
	const float separationWeight =  12.0f;

	const float alignmentRadius = 7.5f;
	const float alignmentAngle  = 0.7f;
	const float alignmentWeight = 8.0f;

	const float cohesionRadius = 9.0f;
	const float cohesionAngle  = -0.15f;
	const float cohesionWeight = 8.0f;

	const float maxRadius = maxXXX (separationRadius,
									maxXXX (alignmentRadius,
											cohesionRadius));

	// find all flockmates within maxRadius using proximity database
	neighbors.clear();
	proximityToken->findNeighbors( position(), maxRadius, neighbors );

#ifndef NO_LQ_BIN_STATS
	// maintain stats on max/min/ave neighbors per boids
	size_t count = neighbors.size();
	if (maxNeighbors < count) maxNeighbors = count;
	if (minNeighbors > count) minNeighbors = count;
	totalNeighbors += count;
#endif // NO_LQ_BIN_STATS

	// determine each of the three component behaviors of flocking
	const Vec3 separation = steerForSeparation (separationRadius,
												separationAngle,
												neighbors);
	const Vec3 alignment  = steerForAlignment  (alignmentRadius,
												alignmentAngle,
												neighbors);
	const Vec3 cohesion   = steerForCohesion   (cohesionRadius,
												cohesionAngle,
												neighbors);

	// apply weights to components (save in variables for annotation)
	const Vec3 separationW = separation * separationWeight;
	const Vec3 alignmentW = alignment * alignmentWeight;
	const Vec3 cohesionW = cohesion * cohesionWeight;

	// annotation
	// const float s = 0.1;
	// annotationLine (position, position + (separationW * s), gRed);
	// annotationLine (position, position + (alignmentW  * s), gOrange);
	// annotationLine (position, position + (cohesionW   * s), gYellow);

	return separationW + alignmentW + cohesionW;
}

void Boid::sphericalWrapAround() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

    // when outside the sphere
    if (position().length() > worldRadius) {
        // wrap around (teleport)
        setPosition (position().sphericalWrapAround (Vec3::zero,
                                                     worldRadius));
        if (this == OpenSteerDemo::selectedVehicle) {
            OpenSteerDemo::position3dCamera (*OpenSteerDemo::selectedVehicle);
            OpenSteerDemo::camera.doNotSmoothNextMove();
        }
    }
}

// control orientation for this boid
void Boid::regenerateLocalSpace (const OpenSteerVec3& newVelocity, const float elapsedTime) {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

	// 3d flight with banking
//	regenerateLocalSpaceForBanking (newVelocity, elapsedTime);

	// // follow terrain surface
	// regenerateLocalSpaceForTerrainFollowing (newVelocity, elapsedTime);
}

void Boid::newPD( ProximityDatabase& pd ) {
    delete proximityToken;
    proximityToken = pd.allocateToken(this);
}


} /* namespace steering */
} /* namespace oneday */
