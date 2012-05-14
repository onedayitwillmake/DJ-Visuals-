/*
 *  Boid.h
 *  A hybrid port of SoulWire's version (AS3) of CraigReynolds boids (C++)
 *
 *  Created by Mario Gonzalez on 2/9/11.
 *  Copyright 2011 Whale Island Games. All rights reserved.
 */

#include "Boid.h"
#include "cinder/Rand.h"
#include "cinder/Color.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/gl.h"

namespace oneday  {
Boid::Boid( float aMaxSpeed, float aMaxSteeringForce ) {
	reset();
	setMaxSpeed( aMaxSpeed );
	setMaxSteeringForce( aMaxSteeringForce );
}
void Boid::reset() {
	_velocity = ci::Rand::randVec3f() * 0.001f;
	_acceleration = ci::Rand::randVec3f() * 0.001f;
	_velocity = ci::Rand::randVec3f() * 0.001f;
	_position = ci::Rand::randVec3f() * 0.001f;
	_previousPosition = ci::Rand::randVec3f() * 0.001f;

	// DEBUG
	_wanderRadius = 16.0f;
	_wanderDistance = 60.0f;
	_wanderStep = 0.25;


//	_wanderPhi = OpenSteer::frandom01()*M_PI * 2;
//	_wanderPsi = OpenSteer::frandom01()*M_PI * 2;
//	_wanderTheta = OpenSteer::frandom01()*M_PI * 2;
}

#pragma mark Tick
void Boid::update() {
	_previousPosition.set( _position );

	// Add current velocity
	_velocity += _acceleration;

	// Clamp velocity
	float maxVelSquared = _velocity.lengthSquared();
	if(maxVelSquared >= _maxSpeedSquared) {
		_velocity.normalize();
		_velocity *= _maxSpeed;
	}

	// Apply velocity
	_position += _velocity;
	
	// Rotate
	if(_doRotation) {
		// TODO: Rotation junk
	}

	// Reset acceleration
	_acceleration = ci::Vec3f::zero();
}

ci::Vec3f Boid::steerTowardsWithEaseDistance( const ci::Vec3f& aTarget, float anEaseDistance = -1.0f  ) {
	_steeringForce = ci::Vec3f( aTarget );
	_steeringForce -= _position;

	// Steer towards target if not already there
	float distanceSquared = _steeringForce.lengthSquared();
	if(distanceSquared > BOID_EPSILON) {

		// Apply less strongly as we approach
		if(anEaseDistance > 1.0f && distanceSquared < anEaseDistance * anEaseDistance )  {
			float distance = sqrtf( distanceSquared );
			_steeringForce *= _maxSpeed * ( distance / anEaseDistance );
		} else { // Apply fully 100% of the time
			_steeringForce *= _maxSpeed;
		}
		
		// Offset current velocity from steering force
		_steeringForce -= _velocity;

		// Cap steering force
		float steeringForceSquared = _steeringForce.lengthSquared();
		if( steeringForceSquared > _maxForceSquared ) {
			_steeringForce.normalize();
			_steeringForce *= _maxForce;
		}
	}

	return _steeringForce;
}

void Boid::applyBreakingForce( float aBrakingForce ) {
	_velocity *= 1.0f - aBrakingForce;
}

#pragma mark Seek
void Boid::seek( const ci::Vec3f& aTarget, float aMultiplier = 1.0f ) {
	// Apply steering force
	_steeringForce = steerTowardsWithEaseDistance( aTarget );
	_steeringForce *= aMultiplier;

	// Append to acceleration
	_acceleration += _steeringForce;
}

void Boid::seekWithingRange( const ci::Vec3f& aTarget, float aMinRange, float aMultiplier = 1.0f ) {
	float rangeSQ = aMinRange * aMinRange;
	float distanceSQ = _position.distanceSquared( aTarget );

	// we're as close as we want to get
	if(distanceSQ < rangeSQ) {
		return;
	}
	
	seek( aTarget, aMultiplier );
}
void Boid::arriveWithEaseDistance( const ci::Vec3f& aTarget, float anEaseDistance, float aMultiplier = 1.0f ) {
	_steeringForce = steerTowardsWithEaseDistance( aTarget, anEaseDistance );
	_steeringForce *= aMultiplier;

	_acceleration += _steeringForce;
}

#pragma mark Flee
void Boid::fleeIfWithinDistance( const ci::Vec3f& aTarget, float aMinRange, float aMultiplier = 1.0f ) {
	float panicDistanceSQ = aMinRange * aMinRange;
	float distanceSQ = _position.distanceSquared( aTarget );

	// we're far away enough not to care
	if(distanceSQ > panicDistanceSQ) {
		return;
	}
	
	_steeringForce = steerTowardsWithEaseDistance( aTarget, aMinRange );
	_steeringForce *= aMultiplier;
	_steeringForce *= -1;	// Flee is really the same as seek, just inverted

	_acceleration += _steeringForce;
}

#pragma mark Wander
/**
 * http://www.shiffman.net/itp/classes/nature/week06_s09/wander/Boid.pde
 void wander() {
    float wanderR = 16.0f;         // Radius for our "wander circle"
    float wanderD = 60.0f;         // Distance for our "wander circle"
    float change = 0.25f;
    wandertheta += random(-change,change);     // Randomly change wander theta

    // Now we have to calculate the new location to steer towards on the wander circle
    PVector circleloc = vel.get();  // Start with velocity
    circleloc.normalize();            // Normalize to get heading
    circleloc.mult(wanderD);          // Multiply by distance
    circleloc.add(loc);               // Make it relative to boid's location

    PVector circleOffSet = new PVector(wanderR*cos(wandertheta),wanderR*sin(wandertheta));
    PVector target = PVector.add(circleloc,circleOffSet);
    acc.add(steer(target,false));  // Steer towards it

    // Render wandering circle, etc.
    if (debug) drawWanderStuff(loc,circleloc,target,wanderR);

  }
 */
void Boid::wander( float aMultiplier = 1.0f ) {
	_wanderTheta += ci::Rand::randFloat(-_wanderStep, _wanderStep);
	_wanderPsi += ci::Rand::randFloat(-_wanderStep, _wanderStep);

	ci::Vec3f circleLoc = ci::Vec3f( _velocity );
	circleLoc.normalize();
	circleLoc *= _wanderDistance;
	circleLoc += _position;

	ci::Vec3f circleOffset = ci::Vec3f( _wanderRadius*cosf(_wanderTheta), _wanderRadius*sin(_wanderTheta), _wanderRadius*cosf( _wanderPsi ) );
	ci::Vec3f target = circleLoc + circleOffset;
	
	_steeringForce = steerTowardsWithEaseDistance( target ) * aMultiplier;
	_acceleration += _steeringForce;
}


#pragma mark Debug
void Boid::debugDraw2d () {
	ci::Vec3f velocityNormalized = _velocity.normalized();
	ci::Vec2f forward	= ci::Vec2f( velocityNormalized.x ,velocityNormalized.y );
	ci::Vec2f left		= ci::Vec2f(1, 0) * forward;
	ci::Vec2f right		= ci::Vec2f(1, 0) * -forward;


//	ci::PolyLine triangle;
//	triangle.push_back( _position + forward );
//	triangle.push_back( _position + left );
//	triangle.push_back( _position + right );
//	ci::gl::draw( triangle );
}

// From OpenSteer
void Boid::debugDrawBasic3dSphericalVehicle () {
//	ci::Color color = ci::Color( 0.25, 0.25, 0.4 );
//		//const Color& color
//		// "aspect ratio" of body (as seen from above)
//		const float x = 0.5f;
//		const float y = sqrtf(1 - (x * x));
//		
//		float radius = 5;
//		// radius and position of vehicle
//		const float r = radius;//vehicle.radius();
//		const ci::Vec3f& p = _position;//vehicle.position();
//		
//		// FakeFoward
//		ci::Vec3f fakeForward	= _velocity.normalized();
//		ci::Vec3f fakeSide		= fakeForward + ci::Vec3f::xAxis();
//		ci::Vec3f fakeUp		= fakeForward + ci::Vec3f::yAxis();
//		
//		
//		// body shape parameters
//		const ci::Vec3f f = r * fakeForward;
//		const ci::Vec3f s = r * fakeSide * x;
//		const ci::Vec3f u = r * fakeUp * x * 0.5f;
//		const ci::Vec3f b = r * fakeForward * -y;
//		
//		// vertex positions
//		const ci::Vec3f nose   = p + f;
//		const ci::Vec3f side1  = p + b - s;
//		const ci::Vec3f side2  = p + b + s;
//		const ci::Vec3f top    = p + b + u;
//		const ci::Vec3f bottom = p + b - u;
//		
//		// colors
//		const float j = +0.05f;
//		const float k = -0.05f;
//		const ci::Color color1 = color + ci::Color(j, j, k);
//		const ci::Color color2 = color + ci::Color(j, k, j);
//		const ci::Color color3 = color + ci::Color(k, j, j);
//		const ci::Color color4 = color + ci::Color(k, j, k);
//		const ci::Color color5 = color + ci::Color(k, k, j);
//		
//		ci::gl::drawCube();
//		// draw body
//		iDrawTriangle (nose,  side1,  top,    color1);  // top, side 1
//		iDrawTriangle (nose,  top,    side2,  color2);  // top, side 2
//		iDrawTriangle (nose,  bottom, side1,  color3);  // bottom, side 1
//		iDrawTriangle (nose,  side2,  bottom, color4);  // bottom, side 2
//		iDrawTriangle (side1, side2,  top,    color5);  // top back
//		iDrawTriangle (side2, side1,  bottom, color5);  // bottom back
}
}
