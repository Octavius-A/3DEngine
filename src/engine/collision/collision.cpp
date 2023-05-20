#pragma once

#include "collision.h"

#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


typedef std::vector<CollisionSphere*> CollisionSphereVect;
Mesh* collisionMesh = nullptr;

CollisionSphereVect collisionSpheres;

std::vector<CollisionMesh*> collisionMeshes;

CollisionMesh::CollisionMesh(const char* path, glm::vec3 position) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error loading collision mesh " << import.GetErrorString() << std::endl;
	}
	else {
		const aiNode* rootNode = scene->mRootNode->mChildren[1];
		aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[0]]; // only support single mesh atm

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			glm::vec3 vert = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertices.push_back(vert);
		}


		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		numFaces = mesh->mNumFaces;

		for (unsigned int i = 0; i < numFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			
			aiVector3D tmpv1 = mesh->mVertices[face.mIndices[0]];
			aiVector3D tmpv2 = mesh->mVertices[face.mIndices[1]];
			aiVector3D tmpv3 = mesh->mVertices[face.mIndices[2]];
			
			glm::vec3 v1 = glm::vec3(tmpv1.x, tmpv1.y, tmpv1.z);
			glm::vec3 v2 = glm::vec3(tmpv2.x, tmpv2.y, tmpv2.z);
			glm::vec3 v3 = glm::vec3(tmpv3.x, tmpv3.y, tmpv3.z);

			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

			v1 = glm::vec3(translationMatrix * glm::vec4(v1, 1.0f));
			v2 = glm::vec3(translationMatrix * glm::vec4(v2, 1.0f));
			v3 = glm::vec3(translationMatrix * glm::vec4(v3, 1.0f));
			

			aiVector3D tmpn1 = mesh->mNormals[face.mIndices[0]];
			aiVector3D tmpn2 = mesh->mNormals[face.mIndices[1]];
			aiVector3D tmpn3 = mesh->mNormals[face.mIndices[2]];

			glm::vec3 n1 = glm::vec3(tmpn1.x, tmpn1.y, tmpn1.z);
			glm::vec3 n2 = glm::vec3(tmpn2.x, tmpn2.y, tmpn2.z);
			glm::vec3 n3 = glm::vec3(tmpn3.x, tmpn3.y, tmpn3.z);

			CollisionMeshFace cmf = {
				v1, v2, v3, n1, n2, n3
			};
			cmFaces.push_back(cmf);
		}
	}
}


void registerCollisionMesh(const char* path, glm::vec3 position) {
	CollisionMesh* cm = new CollisionMesh(path, position);
	collisionMeshes.push_back(cm);
}

void registerCollisionSphere(CollisionSphere* cs) {
	collisionSpheres.push_back(cs);
}

bool isColliding(const CollisionSphere& sphere, const CollisionMesh& mesh) {
	// Step 1: Check sphere-face collision
	for (const auto& face : mesh.cmFaces) {
		// Calculate the face's normal
		glm::vec3 faceNormal = glm::normalize(glm::cross(face.v2 - face.v1, face.v3 - face.v1));

		// Calculate the projection of the sphere's center onto the face's normal
		float projection = glm::dot(sphere.position - face.v1, faceNormal);

		// Calculate the distance between the sphere's center and the face
		float distance = glm::abs(projection) - sphere.radius;

		// If the distance is greater than zero, the sphere and face are separated along the normal axis
		if (distance > 0.0f) {
			// No collision detected, exit early
			return false;
		}
	}

	// Step 2: Check edge-edge collision
	for (unsigned int i = 0; i < mesh.numFaces; ++i) {
		const CollisionMesh::CollisionMeshFace& face = mesh.cmFaces[i];

		// Check each edge of the face
		for (unsigned int j = 0; j < 3; ++j) {
			const glm::vec3& v1 = face.v1;
			const glm::vec3& v2 = face.v2;
			const glm::vec3& v3 = face.v3;

			// Calculate the edge direction
			glm::vec3 edge = v2 - v1;

			// Calculate the face normal
			glm::vec3 faceNormal = glm::normalize(glm::cross(edge, v3 - v1));

			// Calculate the projection of the sphere's center onto the face's normal
			float projection = glm::dot(sphere.position - v1, faceNormal);

			// Calculate the distance between the sphere's center and the edge
			float distance = glm::abs(projection) - sphere.radius;

			// If the distance is greater than zero, the sphere and edge are separated along the normal axis
			if (distance > 0.0f) {
				// No collision detected, exit early
				return false;
			}
		}
	}

	// Step 3: No separating axis found, objects are colliding
	return true;
}

void updateCollisionObjects() {
	CollisionSphere* cs = collisionSpheres.at(0);
	bool colliding = isColliding(*cs, *collisionMeshes.at(0));
	if (colliding) {
		std::cout << "is colliding" << std::endl;
	}
	else {
		std::cout << "no collide" << std::endl;
	}
}

//void updateCollisionObjects() {
//	CollisionSphere* cs = collisionSpheres.at(0);
//
//	glm::vec3 adjustedPosition = cs->position;
//
//	std::vector<CollisionMesh*>::iterator iter;
//	for (iter = collisionMeshes.begin(); iter != collisionMeshes.end(); ++iter) {
//		CollisionMesh* cm = *iter;
//		
//		std::vector<CollisionMesh::CollisionMeshFace>::iterator cmfiter;
//		for (cmfiter = cm->cmFaces.begin(); cmfiter != cm->cmFaces.end(); ++cmfiter) {
//			CollisionMesh::CollisionMeshFace face = *cmfiter;
//			
//			glm::vec3 vert1 = face.v1;
//			glm::vec3 vert2 = face.v2;
//			glm::vec3 vert3 = face.v3;
//
//			glm::vec3 triangleCenter = (vert1 + vert2 + vert3) / 3.0f;
//
//			glm::vec3 direction = glm::normalize(triangleCenter - cs->position);
//			float distance = 0;
//			glm::vec2 bary = glm::vec2(0.0f);
//			bool intersection = glm::intersectRayTriangle(cs->position, direction, vert1, vert2, vert3, bary, distance);
//
//			if (intersection && distance < cs->radius) {;
//				//glm::vec3 collisionNormal = glm::normalize(cs->position - triangleCenter);
//				glm::vec3 collisionNormal = glm::normalize(face.n1 * bary.x + face.n2 * bary.y + face.n3 * (1.0f - bary.x - bary.y));
//
//				adjustedPosition += collisionNormal * (cs->radius - distance);  // Accumulate adjusted position
//				//glm::vec3 adjustedPosition = cs->position + collisionNormal * (cs->radius - glm::distance(cs->position, triangleCenter));
//				//cs->position = adjustedPosition;
//			}
//			else {
//			}
//		}
//	}
//	if (adjustedPosition.y != 0) {
//		adjustedPosition.y = 0;
//	}
//	cs->position = adjustedPosition;
//}

//bool isPointInTriangle(const glm::vec3& point, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
//	// Compute barycentric coordinates
//	glm::vec3 v2v1 = v2 - v1;
//	glm::vec3 v3v1 = v3 - v1;
//	glm::vec3 pv1 = point - v1;
//
//	float dot00 = glm::dot(v3v1, v3v1);
//	float dot01 = glm::dot(v3v1, v2v1);
//	float dot02 = glm::dot(v3v1, pv1);
//	float dot11 = glm::dot(v2v1, v2v1);
//	float dot12 = glm::dot(v2v1, pv1);
//
//	// Compute barycentric coordinates
//	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
//	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
//	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
//
//	// Check if point is inside the triangle
//	return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
//}
//
//void updateCollisionObjects() {
//	CollisionSphere* cs = collisionSpheres.at(0);
//
//	glm::vec3 displacement(0.0f);
//
//	for (const auto& cm : collisionMeshes) {
//		for (const auto& face : cm->cmFaces) {
//			glm::vec3 vert1 = face.v1;
//			glm::vec3 vert2 = face.v2;
//			glm::vec3 vert3 = face.v3;
//
//			// Calculate the triangle's normal
//			glm::vec3 triangleNormal = glm::normalize(glm::cross(vert2 - vert1, vert3 - vert1));
//
//			// Project the sphere center onto the triangle plane
//			glm::vec3 projectedCenter = cs->position - glm::dot(cs->position - vert1, triangleNormal) * triangleNormal;
//
//			// Calculate the distance between the sphere center and the triangle plane
//			float distanceToPlane = glm::dot(vert1 - cs->position, triangleNormal);
//
//			// If the sphere is too far from the plane, skip this face
//			if (distanceToPlane > cs->radius) {
//				continue;
//			}
//
//			// Determine if the projected center is inside the triangle
//			if (isPointInTriangle(projectedCenter, vert1, vert2, vert3)) {
//				// Calculate the penetration depth
//				float penetrationDepth = cs->radius - distanceToPlane;
//
//				// Adjust the displacement vector based on the triangle normal
//				displacement += triangleNormal * penetrationDepth;
//			}
//		}
//	}
//
//	// Apply displacement
//	cs->position += displacement;
//
//	// Project the sphere onto the ground plane (if needed)
//	if (cs->position.y < 0.0f) {
//		cs->position.y = 0.0f;
//	}
//}


//void updateCollisionObjects() {
//	CollisionSphere* cs = collisionSpheres.at(0);
//
//	glm::vec3 displacement(0.0f);
//
//	for (const auto& cm : collisionMeshes) {
//		for (const auto& face : cm->cmFaces) {
//			glm::vec3 vert1 = face.v1;
//			glm::vec3 vert2 = face.v2;
//			glm::vec3 vert3 = face.v3;
//
//			glm::vec3 triangleCenter = (vert1 + vert2 + vert3) / 3.0f;
//
//			glm::vec3 direction = glm::normalize(triangleCenter - cs->position);
//			float distance = 0;
//			glm::vec2 bary = glm::vec2(0.0f);
//			bool intersection = glm::intersectRayTriangle(cs->position, direction, vert1, vert2, vert3, bary, distance);
//
//			if (intersection && distance < cs->radius) {
//				glm::vec3 intersectionPoint = cs->position + direction * distance;
//				glm::vec3 faceNormal = glm::triangleNormal(vert1, vert2, vert3);
//
//				// Calculate the penetration depth
//				float penetrationDepth = cs->radius - distance;
//
//				// Adjust the intersection point by the penetration depth
//				intersectionPoint -= faceNormal * penetrationDepth;
//
//				// Update the displacement vector
//				displacement += intersectionPoint - cs->position;
//			}
//		}
//	}
//
//	// Apply displacement
//	cs->position += displacement;
//
//	
//	if (cs->position.y != 0.0f) {
//		cs->position.y = 0.0f;
//	}
//}
