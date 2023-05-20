#include "physics.h"
#include "../gameState.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btBroadphaseInterface* overlappingPairCache = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;

btAlignedObjectArray<btCollisionShape*> collisionShapes;

void initPhysicsEngine() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -50, 0));

	btScalar mass(0.);



	btVector3 localInertia(0, 0, 0);


	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setFriction(1);

	dynamicsWorld->addRigidBody(body);
}

btRigidBody* registerStaticCollisionMesh(const char* path, glm::vec3 position) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading collision mesh " << import.GetErrorString() << std::endl;
		return nullptr; // ?
	}

	// this is a shitty retarded way of doing this
	const aiNode* rootNode = scene->mRootNode->mChildren[1];
	aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[0]]; // only support single mesh atm

	btTriangleMesh* triangleMesh = new btTriangleMesh();

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		// aiVextor3D -> btVector3

		// grab the 3 verts -- assuming only 3
		aiVector3D tmpv1 = mesh->mVertices[face.mIndices[0]];
		aiVector3D tmpv2 = mesh->mVertices[face.mIndices[1]];
		aiVector3D tmpv3 = mesh->mVertices[face.mIndices[2]];

		btVector3 btv1 = { tmpv1.x, tmpv1.y, tmpv1.z };
		btVector3 btv2 = { tmpv2.x, tmpv2.y, tmpv2.z };
		btVector3 btv3 = { tmpv3.x, tmpv3.y, tmpv3.z };

		triangleMesh->addTriangle(btv1, btv2, btv3);

	}

	btCollisionShape* box = new btBvhTriangleMeshShape(triangleMesh, true);

	collisionShapes.push_back(box);

	btTransform meshTransform;
	meshTransform.setIdentity();
	meshTransform.setOrigin(btVector3(position.x, position.y, position.z));

	btScalar mass(0.);


	btVector3 localInertia(0, 0, 0);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(meshTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, box, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);

	return body;
}

btRigidBody* registerCollisionSphere(glm::vec3 position, float _radius, float _mass) {
	btCollisionShape* colShape = new btSphereShape(btScalar(_radius));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(_mass);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(position.x, position.y, position.z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setFriction(1);
	body->setRollingFriction(1);
	// by default bullet deactivates objects that stay static for a few seconds.
	body->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(body);

	return body;
}

void updatePhysicsEngine() { dynamicsWorld->stepSimulation(globalGameState.dTime); }

void exitPhysicsEngine() {
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	collisionShapes.clear();
}