#pragma once

#include "collision.h"

#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/intersect.hpp>


typedef std::vector<CollisionMesh*> CollisionMeshVect;
typedef std::vector<CollisionSphere*> CollisionSphereVect;

CollisionMeshVect collisionMeshes;
CollisionSphereVect collisionSpheres;

void meshSphereCollision(CollisionMesh* cm, CollisionSphere* cs);

void registerCollisionMesh(CollisionMesh* cm) {
	collisionMeshes.push_back(cm);
}

void registerCollisionSphere(CollisionSphere* cs) {
	collisionSpheres.push_back(cs);
}

void updateCollisionObjects() {
	CollisionSphere* cs = collisionSpheres.at(0);
	glm::vec3 vert1 = glm::vec3(-0.5f, -0.5f, 0.5f);
	glm::vec3 vert2 = glm::vec3(-0.5f, -0.5f, -0.5f);
	glm::vec3 vert3 = glm::vec3(-0.5f, 0.5f, 0.5f);
	glm::vec3 triangleCenter = (vert1 + vert2 + vert3) / 3.0f;

	glm::vec3 direction = glm::normalize(triangleCenter - cs->position);
	float distance = 0;
	glm::vec2 bary = glm::vec2(0.0f);
	bool intersection = glm::intersectRayTriangle(cs->position, direction, vert1, vert2, vert3, bary, distance);
	std::cout << distance << " " << intersection << std::endl;
	
	if (intersection && distance < cs->radius) {
		std::cout << "COLLISION DETECTED" << std::endl;
		glm::vec3 collisionNormal = glm::normalize(cs->position - triangleCenter);

		glm::vec3 adjustedPosition = cs->position + collisionNormal * (cs->radius - glm::distance(cs->position, triangleCenter));
		cs->position = adjustedPosition;
	}
	else {
		std::cout << "NO COLLISION DETECTED..." << std::endl;
	}

	if (cs->position.y != 0) {
		cs->position.y = 0;
	}

	

}

void printVec(glm::vec3 vec) {
	std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
}

void meshSphereCollision(CollisionMesh* cm, CollisionSphere* cs) {
	//for (auto tri : *cm) {
		//glm::vec3 edge1 = tri.coord[1] - tri.coord[0];
		//glm::vec3 edge2 = tri.coord[2] - tri.coord[0];
		//glm::vec3 normal = glm::cross(edge1, edge2);
		//normal = glm::normalize(normal);
		////distance = (A * x + B * y + C * z - (A * x0 + B * y0 + C * z0)) / sqrt(A^2 + B^2 + C^2)
		//// omg wtf lmao
		//float distance = (normal.x * cs->position.x + normal.y * cs->position.y + normal.z * cs->position.z
		//	- (normal.x * tri.coord[0].x + normal.y * tri.coord[0].y + normal.z * tri.coord[0].z)) /
		//	sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

		//if (distance > cs->radius) {
		//	std::cout << "no collision" << std::endl;
		//}
		//else {
		//	std::cout << "maybe collision " << cs->radius << " " << distance << std::endl;
		//}

	//}
}
