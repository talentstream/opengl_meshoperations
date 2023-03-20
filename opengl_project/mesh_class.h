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

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	Vertex() {}
	Vertex(glm::vec3 position, glm::vec3 normal)
	{
		this->position = position;
		this->normal = normal;
	}
	bool operator==(const Vertex& other) const {
		return position == other.position && normal == other.normal;
	}
	Vertex operator+(const Vertex& other) const {
		return Vertex(position + other.position, glm::normalize(normal + other.normal));
	}
	Vertex operator*(float k) const {
		return Vertex(k * position, normal);
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
struct Edge
{
	Vertex v1;
	Vertex v2;
	Vertex opposite_v1;
	Vertex opposite_v2;
};
struct Face
{
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
	bool HasVertex(const Vertex& vertex)
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			if (vertex == vertices[i])
			{
				return true;
			}
		}
		return false;
	}

	Vertex OppsiteVertex(const Vertex& v1, const Vertex& v2)
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			if (!(vertices[i] == v1 || vertices[i] == v2))
			{
				return vertices[i];
			}
		}
	}
};


class Mesh
{
public:
	unsigned int VAO;
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
	std::vector<Face> faces;

	// blinn phong 系数
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;

	std::vector<unsigned int> indices;
	Mesh(const char* path);
	void Draw(const Shader& shader);
	void AddFace(const std::vector<Vertex>& faceVertices)
	{
		Face newFace;
		newFace.vertices = faceVertices;
		unsigned int numVertices = static_cast<unsigned int>(faceVertices.size());
		for (unsigned int i = 0; i < numVertices; i++)
		{
			unsigned int nextIndex = (i + 1) % numVertices;

			edges.push_back(AddEdge(faceVertices[i], faceVertices[nextIndex]));
		}
		faces.push_back(newFace);
	}
	void AddVertex(const glm::vec3& position, const glm::vec3& normal)
	{
		Vertex newVertex;

		newVertex.position = position;
		newVertex.normal = normal;

		vertices.push_back(newVertex);
	}

	void LoopSubdivide()
	{

		std::vector<Vertex> newVertices;
		std::vector<unsigned int> newIndices;
		std::vector<Edge> newEdges;
		std::vector<Face> newFaces;

		// 存储原网格每个周围点集合
		std::unordered_map<Vertex, std::vector<Vertex>> vertexNeighbors;
		for (const auto& face : faces)
		{
			int n = face.vertices.size();
			for (int i = 0; i < n; i++)
			{
				int nextIndex = (i + 1) % n;
				int preIndex = (i - 1 + n) % n;
				Vertex nextVertex = face.vertices[nextIndex];
				Vertex preVertex = face.vertices[preIndex];
				// 判断是否已经存了nextVertex
				if (std::find(vertexNeighbors[face.vertices[i]].begin(),
					vertexNeighbors[face.vertices[i]].end(), nextVertex) ==
					vertexNeighbors[face.vertices[i]].end())
				{
					vertexNeighbors[face.vertices[i]].push_back(nextVertex);
				}
				// 判断是否已经存了preVertex
				if (std::find(vertexNeighbors[face.vertices[i]].begin(),
					vertexNeighbors[face.vertices[i]].end(), preVertex) ==
					vertexNeighbors[face.vertices[i]].end())
				{
					vertexNeighbors[face.vertices[i]].push_back(preVertex);
				}
			}
		}

		// 对每个面计算六个点的新位置，并且更新
		for (const auto& face : faces)
		{
			Vertex v1 = face.vertices[0];
			float u1;
			if (vertexNeighbors[v1].size() > 3)
			{
				u1 = 0.375f / (vertexNeighbors[v1].size());
			}
			else u1 = 0.1875f;

			Vertex new_v1{};
			new_v1 = new_v1 + v1 * (1 - vertexNeighbors[v1].size() * u1);
			for (size_t i = 0; i < vertexNeighbors[v1].size(); i++)
			{
				new_v1 = new_v1 + vertexNeighbors[v1][i] * u1;
			}

			Vertex v2 = face.vertices[1];
			float u2;
			if (vertexNeighbors[v2].size() > 3)
			{
				u2 = 0.375f / (vertexNeighbors[v2].size() * 8.0f);
			}
			else u2 = 0.1875f;
			Vertex new_v2{};
			new_v2 = new_v2 + v2 * (1 - vertexNeighbors[v2].size() * u2);
			for (size_t i = 0; i < vertexNeighbors[v2].size(); i++)
			{
				new_v2 = new_v2 + vertexNeighbors[v2][i] * u2;
			}

			Vertex v3 = face.vertices[2];
			float u3;
			if (vertexNeighbors[v3].size() > 3)
			{
				u3 = 0.375f / (vertexNeighbors[v3].size() * 8.0f);
			}
			else u3 = 0.1875f;
			Vertex new_v3{};
			new_v3 = new_v1 + v3 * (1 - vertexNeighbors[v3].size() * u3);
			for (size_t i = 0; i < vertexNeighbors[v3].size(); i++)
			{
				new_v3 = new_v3 + vertexNeighbors[v3][i] * u3;
			}

			Vertex v1v2 = (v1 + v2) * 0.375f + (v3 + GetOppsiteVertex(v1,v2,v3)) * 0.125f;
			Vertex v2v3 = (v2 + v3) * 0.375f + (v1 + GetOppsiteVertex(v2,v3,v1)) * 0.125f;
			Vertex v3v1 = (v3 + v1) * 0.375f + (v2 + GetOppsiteVertex(v3,v1,v2)) * 0.125f;

			newVertices.push_back(new_v1);
			newVertices.push_back(new_v2);
			newVertices.push_back(new_v3);
			newVertices.push_back(v1v2);
			newVertices.push_back(v2v3);
			newVertices.push_back(v3v1);

			int j = newVertices.size() - 6;
			// v1 v1v2 v3v1
			newIndices.push_back(j);
			newIndices.push_back(j + 3);
			newIndices.push_back(j + 5);

			// v1v2 v2 v2v3
			newIndices.push_back(j + 3);
			newIndices.push_back(j + 1);
			newIndices.push_back(j + 4);

			// v2v3 v3 v3v1
			newIndices.push_back(j + 4);
			newIndices.push_back(j + 2);
			newIndices.push_back(j + 5);

			// v1v2 v2v3 v3v1
			newIndices.push_back(j + 3);
			newIndices.push_back(j + 4);
			newIndices.push_back(j + 5);
		}

		vertices = newVertices;
		indices = newIndices;
	}
private:
	unsigned int VBO, EBO;
	void SetupMesh();
	Edge AddEdge(const Vertex& v1, const Vertex& v2)
	{
		Edge newEdge;

		newEdge.v1 = v1;
		newEdge.v2 = v2;

		return newEdge;
	}
	Vertex GetOppsiteVertex(const Vertex& v1, const Vertex& v2,const Vertex& v3)
	{
		for (auto& face : faces)
		{
			if (face.HasVertex(v1) && face.HasVertex(v2) && (!face.HasVertex(v3)))
			{
				return face.OppsiteVertex(v1, v2);
			}
		}
	}
};
#endif // !MESH_H

