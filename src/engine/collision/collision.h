#pragma once

#include <glm/glm.hpp>
#include <vector>

// Collision primitives
struct CollisionTri {
	glm::vec3 coord[3];
};
typedef std::vector<CollisionTri> CollisionMesh;

struct CollisionSphere {
	glm::vec3 position;
	float radius;
};

// Add a mesh to an internaly managed list
void registerCollisionMesh(CollisionMesh* cm);
// Same
void registerCollisionSphere(CollisionSphere* cs);

// This function should loop over the collision objects and detect collisions.
// Should also apply "gravity" to objects...
// For now there will be one collision mesh and one sphere.

void updateCollisionObjects();