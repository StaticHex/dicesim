#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <string>
#include <random>
#include <time.h>
#include <iostream>
using namespace std;

class diceSim {
	// Bullet Physics Variables
	btBroadphaseInterface*					broadphase;				// Uses the bounding boxes of objects in the world to 
																	// quickly compute a conservative approximate list of colliding pairs.

	btDefaultCollisionConfiguration*		collisionConfiguration;	// Holds the current collision determination method for bullet
	btCollisionDispatcher*					dispatcher;				// Holds the dispatcher to handle collision events
	btSequentialImpulseConstraintSolver*	solver;					// The actual solver for our physics simulation (handles all the math)
	btDiscreteDynamicsWorld*				world;					// The world in which all our shapes exist and interact with one another
	btCollisionShape*						groundShape;			// The collision shape for the ground (a really big plane)
	btCollisionShape*						dieBox;					// The collision shape for the die (a cube)
	btCompoundShape*						diceShape;				// Our physical shape for the die
	btDefaultMotionState*					groundMotionState;		// our motion state for the ground (in this case, static)
	btRigidBody*							groundRigidBody;		// The rigid body for the ground (determines what material the ground acts like)
	btDefaultMotionState*					dieMotionState;			// The motion state for the die, holds things such as the velocity and the restitution
	btScalar								dieMass;				// The mass for the die, based on mass for PMMA
	btVector3								dieInertia;				// The initial inertia for the die
	btRigidBody*							dieRigidBody;			// The rigid boxy for the die (determines what material the die acts like it's made of)
	btVector3								faceNormal;				// Used to determine what face the die landed on
public:
	float									vX;						// the initial x-velocity of the die
	float									vY;						// the initial y-velocity of the die
	float									vZ;						// the initial z-velocity of the die

	// Begin public methods
	diceSim();
	diceSim(float x, float y, float z);
	string getFaceUp(bool numeric);
	void updateSim();
	bool simFinished();
	void cleanup();
private:
	void init(float x, float y, float z);
	double dRand();
};