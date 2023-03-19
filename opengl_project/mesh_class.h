#pragma once
#ifndef MESH_H
#define MESH_H


#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include"shader_class.h"

#include<vector>
#include<unordered_map>
//struct MeshData {
//	size_t id;
//	std::vector<Face*> faces;
//	std::vector<Edge*> edges;
//	std::vector<Vertex*> vertices;
//};
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;

	bool operator==(const Vertex& other) const {
		return position == other.position && normal == other.normal;
	}

	Vertex* GetNextVertCC(const Vertex* v)
	{

	}

};
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return (hash<glm::vec3>()(vertex.position)) ^
				(hash<glm::vec3>()(vertex.normal));
		}
	};
}

struct Face
{

};
struct Edge
{

};


class Mesh
{
public:
	unsigned int VAO;
	std::vector<Vertex> vertices;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;

	std::vector<unsigned int> indices;
	Mesh(const char* path);
	void Draw(const Shader& shader);
private:
	unsigned int VBO, EBO;
	void SetupMesh();
};
#endif // !MESH_H

