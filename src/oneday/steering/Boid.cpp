/*
 * Boid.h
 *  Created on: May 13, 2012
 *      Author: mario gonzalez | onedayitwillmake.com | @1dayitwillmake | mariogonzalez@gmail.com
 *      Abstract:
 *      	A 'Boid' object, this is pretty much lifted from the Boids.cpp file in the OpenSteer plugin
 */

#include "Boid.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

namespace oneday { namespace steering {
///// Static definitions
OpenSteer::AVGroup			Boid::neighbors;
OpenSteer::ObstacleGroup	Boid::obstacles;
float						Boid::worldRadius = 500.0;
#ifndef NO_LQ_BIN_STATS
	size_t Boid::minNeighbors, Boid::maxNeighbors, Boid::totalNeighbors;
#endif // NO_LQ_BIN_STATS



Boid::Boid( ProximityDatabase& pd ) {
	  // allocate a token for this boid in the proximity database
	proximityToken = NULL;
	newPD(pd);

	// reset all boid state
	reset();
}

Boid::~Boid() {
	delete proximityToken;  // delete this boid's token in the proximity database
}

void Boid::reset() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
    using namespace OpenSteer;



	 // reset the vehicle
	SimpleVehicle::reset ();

    // steering force clip magnitude
    setMaxForce (0.3f * 10);

    // velocity  clip magnitude
    setMaxSpeed (1.5f * 10);

    // initial position along X axis
//    static float startX = 0.0;
//    setPosition (0.0, startX += 2.0f, 0.0);

	// initial slow speed
//	setSpeed( maxSpeed() * 0.3f );

	// randomize initial orientation
	regenerateOrthonormalBasisUF( RandomUnitVector () );

	// randomize initial position
	setPosition ( RandomVectorInUnitRadiusSphere () * Boid::worldRadius * 0.1f	);

	// notify proximity database that our position has changed
	proximityToken->updateForNewPosition(position());

	// Reset wander properties
	// DEBUG
	_wanderRadius = 100.0f;
	_wanderDistance = 5.0f;
	_wanderStep = 1.0;

	_wanderPhi = 0.0f; // OpenSteer::frandom01()*M_PI * 2;
	_wanderPsi = 0.0f; // OpenSteer::frandom01()*M_PI * 2;
	_wanderTheta = 0.0f; // OpenSteer::frandom01()*M_PI * 2;


    // 15 seconds and 150 points along the trail
    setTrailParameters (5, 60);

    static int counter = 0;
    static float step = 0.0f;
    step += 5.0 * ( M_PI / 180.0f );

	_wanderStep += step;// * ( ++counter % 2 ) ? 1 : -1;; //OpenSteer::frandom2(-_wanderStep, _wanderStep);
}



void Boid::draw() {
	ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
    ci::Vec3f aPosition = ci::Vec3f( position().x, position().y, position().z );
	ci::gl::drawSphere( aPosition, 2, 6 );
	drawTrail();
}


// per frame simulation update
void Boid::update (const float currentTime, const float elapsedTime) {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
    using namespace OpenSteer;

	OPENSTEER_UNUSED_PARAMETER(currentTime);


	// steer to flock and avoid obstacles if any

    const Vec3 wander2d = steerForWander(elapsedTime).setYtoZero();
    const Vec3 steer = forward() + (wander2d);
    applySteeringForce (steer, elapsedTime);
	applySteeringForce( steerToFlock(), elapsedTime * 2 );
	applySteeringForce( OpenSteer::Vec3::zero - position(), elapsedTime*1.5);

//	applySteeringForce( , elapsedTime );

	// wrap around to contrain boid within the spherical boundary
	sphericalWrapAround();

	// notify proximity database that our position has changed
	proximityToken->updateForNewPosition( position() );
	recordTrailVertex (currentTime, position());
}


OpenSteerVec3 Boid::steerToFlock() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

	 // avoid obstacles if needed
	// XXX this should probably be moved elsewhere
	const Vec3 avoidance = steerToAvoidObstacles (1.0f, obstacles);
	if (avoidance != Vec3::zero) return avoidance;

	const float separationRadius =  5.0f;
	const float separationAngle  = 0;//-0.707f;
	const float separationWeight =  12.0f;

	const float alignmentRadius = 7.5f;
	const float alignmentAngle  = 0;//0.7f;
	const float alignmentWeight = 8.0f;

	const float cohesionRadius = 9.0f;
	const float cohesionAngle  = 0;//-0.15f;
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

OpenSteerVec3 Boid::wander( float aMultiplier = 1.0f ) {
	_wanderTheta += _wanderStep;
//	_wanderPsi += OpenSteer::frandom2(-_wanderStep, _wanderStep);


//	OpenSteerVec3 circleLoc = forward();
//	circleLoc.normalize();
//	circleLoc *= _wanderDistance;
//	circleLoc += position();

	// /*_wanderRadius*cosf( _wanderPsi )*/
	OpenSteerVec3 circleOffset = OpenSteerVec3( _wanderRadius*cosf(_wanderTheta), 0.0f,  _wanderRadius*sin(_wanderTheta));
//	OpenSteerVec3 target = circleLoc + circleOffset;
    std::cout << "SN:" << serialNumber << "  circleOffset:" << circleOffset << std::endl;
	return circleOffset;
}

void Boid::sphericalWrapAround() {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

    // when outside the sphere
    if ( position().length() > worldRadius ) {
        // wrap around (teleport)
        setPosition ( position().sphericalWrapAround ( OpenSteerVec3::zero, Boid::worldRadius) );
        clearTrailHistory();
    }
}

// control orientation for this boid
void Boid::regenerateLocalSpace (const OpenSteerVec3& newVelocity, const float elapsedTime) {
	// TODO:: ITERATIVELY REMOVE USING DIRECTIVE AS CLASS IS FLESHED OUT
	using namespace OpenSteer;

	// 3d flight with banking
	regenerateLocalSpaceForBanking(newVelocity, elapsedTime);

	// // follow terrain surface
	// regenerateLocalSpaceForTerrainFollowing (newVelocity, elapsedTime);
}


void Boid::regenerateLocalSpaceForBanking (const OpenSteerVec3& newVelocity, const float elapsedTime) {
    // the length of this global-upward-pointing vector controls the vehicle's
    // tendency to right itself as it is rolled over from turning acceleration
    const OpenSteerVec3 globalUp (0, 0.2f, 0);

    // acceleration points toward the center of local path curvature, the
    // length determines how much the vehicle will roll while turning
    const OpenSteerVec3 accelUp = smoothedAcceleration() * 0.05f;

    // combined banking, sum of UP due to turning and global UP
    const OpenSteerVec3 bankUp = accelUp + globalUp;

    // blend bankUp into vehicle's UP basis vector
    const float smoothRate = elapsedTime * 3;
    OpenSteerVec3 tempUp = up();
    blendIntoAccumulator (smoothRate, bankUp, tempUp);
    setUp (tempUp.normalize());

//  annotationLine (position(), position() + (globalUp * 4), gWhite);  // XXX
//  annotationLine (position(), position() + (bankUp   * 4), gOrange); // XXX
//  annotationLine (position(), position() + (accelUp  * 4), gRed);    // XXX
//  annotationLine (position(), position() + (up ()    * 1), gYellow); // XXX

    // adjust orthonormal basis vectors to be aligned with new velocity
    if (speed() > 0) regenerateOrthonormalBasisUF (newVelocity / speed());
}

void Boid::newPD( ProximityDatabase& pd ) {
    delete proximityToken;
    proximityToken = pd.allocateToken(this);
}


} /* namespace steering */
} /* namespace oneday */
