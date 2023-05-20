/*
Physics engine. Uses bullet physics.
*/

#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

void initPhysicsEngine();
void updatePhysicsEngine();
void exitPhysicsEngine();

btRigidBody* registerStaticCollisionMesh(const char* path, glm::vec3 position);
btRigidBody* registerCollisionSphere(glm::vec3 position);