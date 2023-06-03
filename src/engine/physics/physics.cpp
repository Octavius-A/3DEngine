#include "physics.h"
#include "../gameState.h"

#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btBroadphaseInterface* overlappingPairCache = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;
btDbvtBroadphase* broadPhase = nullptr;
btOverlappingPairCallback* ghostPairCallback = nullptr;
btVector3 worldMin(-1000, -1000, -1000);
btVector3 worldMax(1000, 1000, 1000);
btAxisSweep3* sweepBP = new btAxisSweep3(worldMin, worldMax);

btAlignedObjectArray<btCollisionShape*> collisionShapes;

void registerAiMesh(aiMesh* mesh);

void initPhysicsEngine() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = sweepBP;
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
	dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
}

void loadPhysicsWorld(const char* path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading collision mesh " << import.GetErrorString() << std::endl;
		return; // ?
	}

	aiNode* rootNode = scene->mRootNode;

	for (unsigned int i = 0; i < rootNode->mNumChildren; ++i) {
		aiNode* child = rootNode->mChildren[i];

		aiMesh* mesh = scene->mMeshes[child->mMeshes[0]];

		registerAiMesh(mesh);
	}
}

void registerAiMesh(aiMesh* mesh) {

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
	//meshTransform.setOrigin(btVector3(position.x, position.y, position.z));

	btScalar mass(0.);


	btVector3 localInertia(0, 0, 0);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(meshTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, box, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	//body->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
	//dynamicsWorld->addCollisionObject(body, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

}

btRigidBody* registerStaticCollisionMesh(const char* path, glm::vec3 position) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading collision mesh " << import.GetErrorString() << std::endl;
		return nullptr; // ?
	}

	// this is a shitty retarded way of doing this
	const aiNode* rootNode = scene->mRootNode->mChildren[0];
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
	//body->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	
	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
	//dynamicsWorld->addCollisionObject(body, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

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

btRigidBody* registerCollisionCapsule(glm::vec3 position, float _radius, float _height, float _mass) {
	//btCollisionShape* colShape = new btSphereShape(btScalar(_radius));
	btCollisionShape* colShape = new btCapsuleShape(_radius, _height);

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
	/*body->setFriction(1);
	body->setRollingFriction(1);*/
	// by default bullet deactivates objects that stay static for a few seconds.
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setAngularFactor(0.0f);
	body->setSleepingThresholds(0.0, 0.0);

	dynamicsWorld->addRigidBody(body);

	return body;
}

btKinematicCharacterController* registerCharacterController(glm::vec3 origin) {
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));
	btPairCachingGhostObject* ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(startTransform);

	sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btScalar characterHeight = 1.0;
	btScalar characterWidth = 0.25;
	btConvexShape* capsule = new btCapsuleShapeZ(characterWidth, characterHeight);
	
	ghostObject->setCollisionShape(capsule);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	ghostObject->setActivationState(DISABLE_DEACTIVATION);
	ghostObject->setFriction(0);
	btScalar stepHeight = btScalar(0.3f);
	btKinematicCharacterController* character = new btKinematicCharacterController(ghostObject, capsule, stepHeight);
	character->setGravity(btVector3(0, -10, 0));
	
	dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	//dynamicsWorld->addRigidBody(ghostObject);

	dynamicsWorld->addAction(character);

	return character;
}

void updatePhysicsEngine(btKinematicCharacterController* ddd) {
	dynamicsWorld->stepSimulation(globalGameState.dTime);
}

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