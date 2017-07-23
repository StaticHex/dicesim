#include "diceSim.h";

/********************************************************************************
* Default constructor                                                           *
* - Center of Gravity set in the center of the die                              *
********************************************************************************/
diceSim::diceSim() {
	init(0.0f, 0.0f, 0.0f);
}

/********************************************************************************
* COG Constructor                                                               *
* - Center of Gravity set using parameters                                      *
********************************************************************************/
diceSim::diceSim(float x, float y, float z) {
	init(x, y, z);
}

/********************************************************************************
* Get Face Up Method                                                            *
* - Returns which face of the die is currently pointing up, can return a        *
*   number or a color                                                           *
********************************************************************************/
string diceSim::getFaceUp(bool numeric) {
	btScalar x, y, z; // Create variables to hold the Euler Angles for our rotation
	dieRigidBody->getCenterOfMassTransform().getBasis().getEulerZYX(x, y, z);

	// Our face normal (Used to determine which face is up)
	btVector3 n = faceNormal;

	// Rotate our normal around the 3 primary axes using the Euler angles
	n = n.rotate(btVector3(1, 0, 0), x);
	n = n.rotate(btVector3(0, 1, 0), y);
	n = n.rotate(btVector3(0, 0, 1), z);

	// update x, y, and z with our rotated values
	x = n.getX();
	y = n.getY();
	z = n.getZ();

	// CHeck which direction we're facing and output a color/number (depending on mode)
	if (abs(x) > abs(y) && abs(x) > abs(z)) {
		if (x > 0) {
			if (numeric) {
				return "5";
			}
			else {
				return"Green";
			}
		}
		else {
			if (numeric) {
				return "2";
			}
			else {
				return "Purple";
			}
		}
	}
	else if (abs(y) > abs(x) && abs(y) > abs(z)) {
		if (y > 0) {
			if (numeric) {
				return "1";
			}
			else {
				return "Blue";
			}
		}
		else {
			if (numeric) {
				return "6";
			}
			else {
				return "Yellow";
			}
		}
	}
	else {
		if (z > 0) {
			if (numeric) {
				return "4";
			}
			else {
				return "Red";
			}
		}
		else {
			if (numeric) {
				return "3";
			}
			else {
				return "Cyan";
			}
		}
	}
	return "ERROR";
}

/********************************************************************************
* Update Sim Method                                                             *
* - Steps the physics simulation                                                *
********************************************************************************/
void diceSim::updateSim() {
	world->stepSimulation(1 / 60.f, 100);
}

/********************************************************************************
* Simulation Finished Method                                                    *
* - Returns true if the die is resting on the ground and not moving, returns    *
*   false otherwise                                                             *
********************************************************************************/
bool diceSim::simFinished() {
	if (dieRigidBody->getLinearVelocity() == btVector3(0, 0, 0)) {
		return true;
	}
	return false;
}

/********************************************************************************
* Cleanup Method                                                                *
* - Used to free up memory for all dynamically allocted objects and to clear    *
*   all objects in the simulation before delete                                 *
********************************************************************************/
void diceSim::cleanup() {
	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		world->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	delete groundShape;
	delete dieBox;


	//delete dynamics world
	delete world;

	//delete solver
	delete solver;

	//delete broadphase
	delete broadphase;

	//delete dispatcher
	delete dispatcher;

	// delete collision settings
	delete collisionConfiguration;
}

/********************************************************************************
* Init Method                                                                   *
* - Helper function for the constructor                                         *
* - Workhorse which sets up the initial world and objects                       *
********************************************************************************/
void diceSim::init(float x, float y, float z) {
	faceNormal = btVector3(0, 1, 0);

	// build the broadphase object
	broadphase = new btDbvtBroadphase();

	// Set up collision method and dispatcher for bullet
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

	// The actual solver for our object
	solver = new btSequentialImpulseConstraintSolver;

	// Our physics world
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -10, 0));

	// Create the table
	groundShape = new btStaticPlaneShape(btVector3(0, 1.0, 0), -1);
	groundShape->setMargin(0);

	// Create the dice
	dieBox = new btBoxShape(btVector3(5.0, 5.0, 5.0));
	diceShape = new btCompoundShape();

	// Set center of gravity for the dice
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(x, y, z));
	diceShape->addChildShape(localTrans, dieBox);

	// Create a motion state for the ground and then add the ground to the world
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(1.05);
	groundRigidBody->setFriction(0.7);
	world->addRigidBody(groundRigidBody);

	// Create a motion state and rigid body for the die and then add it to the world
	dieMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 150, 0)));
	dieMass = 10;
	dieInertia = btVector3(0, 0, 0);
	diceShape->calculateLocalInertia(dieMass, dieInertia);
	btRigidBody::btRigidBodyConstructionInfo dieRigidBodyCI(dieMass, dieMotionState, diceShape, dieInertia);
	dieRigidBody = new btRigidBody(dieRigidBodyCI);
	dieRigidBody->setRestitution(0.45);
	dieRigidBody->setFriction(0.35);
	vX = dRand()*40.0 - 20.0;
	vY = dRand()*40.0 - 20.0;
	vZ = dRand()*40.0 - 20.0;
	dieRigidBody->setLinearVelocity(btVector3(vX, vY, vZ));
	dieRigidBody->setAngularVelocity(btVector3(vX*2.0, 0.0, vZ*2.0));
	world->addRigidBody(dieRigidBody);
}

/********************************************************************************
* Double Random: Helper Function                                                *
* - Generates a random double between 0 and 1.0                                 *
********************************************************************************/
double diceSim::dRand() {
	double rNum = rand();
	double max = RAND_MAX;
	return rNum / (max*1.0);
}