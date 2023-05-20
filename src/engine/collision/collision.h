#pragma once

#include "../rendering/model.h"

#include <glm/glm.hpp>
#include <vector>


struct CollisionSphere {
	glm::vec3 position;
	float radius;
};

struct CollisionMesh {
	CollisionMesh(const char* path, glm::vec3 position);

	struct CollisionMeshFace {
		glm::vec3 v1, v2, v3, n1, n2, n3;
	};

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	unsigned int numFaces;
	std::vector<CollisionMeshFace> cmFaces;
};

// Add a mesh to an internaly managed list
void registerCollisionMesh(const char* path, glm::vec3 position);
// Same
void registerCollisionSphere(CollisionSphere* cs);

// This function should loop over the collision objects and detect collisions.
// Should also apply "gravity" to objects...
// For now there will be one collision mesh and one sphere.

void updateCollisionObjects();