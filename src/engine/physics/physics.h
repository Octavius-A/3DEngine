/*
Physics engine. Uses bullet physics.
*/

#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>


#include <glm/glm.hpp>

void initPhysicsEngine();
void updatePhysicsEngine(btKinematicCharacterController* playerRB); // this is a hack remove asap
void exitPhysicsEngine();

btRigidBody* registerStaticCollisionMesh(const char* path, glm::vec3 position);

btRigidBody* registerCollisionSphere(glm::vec3 position, float radius, float mass);
btRigidBody* registerCollisionCapsule(glm::vec3 position, float radius, float height, float mass);
btKinematicCharacterController* registerCharacterController(glm::vec3 origin);

void loadPhysicsWorld(const char* path);