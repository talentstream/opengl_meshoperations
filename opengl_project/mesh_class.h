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
	Vertex() :position(0), normal(0) {}
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
class Mesh
{
public:
	unsigned int VAO;
	std::vector<Vertex> vertices;

	// blinn phong 系数
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;

	std::vector<unsigned int> indices;
	Mesh(const char* path);
	void Draw(const Shader& shader);

	void LoopSubdivide()
	{

		std::vector<Vertex> newVertices;
		std::vector<unsigned int> newIndices;

		// 存储原网格每个周围点集合
		std::unordered_map<Vertex, std::vector<Vertex>> vertexNeighbors;
		std::unordered_map<std::string, std::vector<int>> oppsiteVertex;
		for (size_t i = 0; i < indices.size() / 3; i++)
		{
			// 处理原顶点的邻居
			int t0 = indices[i];
			int t1 = indices[i + 1];
			int t2 = indices[i + 2];
			// std::cout << "#" << i << " : " << t0 << " " << t1 << " " << t2 << std::endl;
			Vertex v0 = vertices[t0];
			Vertex v1 = vertices[t1];
			Vertex v2 = vertices[t2];
			Find(vertexNeighbors[v0], v1);
			Find(vertexNeighbors[v0], v2);
			Find(vertexNeighbors[v1], v0);
			Find(vertexNeighbors[v1], v2);
			Find(vertexNeighbors[v2], v0);
			Find(vertexNeighbors[v2], v1);

			// 处理新顶点的邻居
			std::string s01 = (t0 < t1 ? std::to_string(t0) + "|" + std::to_string(t1) : std::to_string(t1) + "|" + std::to_string(t0));
			std::string s12 = (t1 < t2 ? std::to_string(t1) + "|" + std::to_string(t2) : std::to_string(t2) + "|" + std::to_string(t1));
			std::string s20 = (t2 < t0 ? std::to_string(t2) + "|" + std::to_string(t0) : std::to_string(t0) + "|" + std::to_string(t2));

			Find(oppsiteVertex[s01], t2);
			Find(oppsiteVertex[s12], t0);
			Find(oppsiteVertex[s20], t1);

			/*if (std::find(vertexNeighbors[v0].begin(), vertexNeighbors[v0].end(), v1) == vertexNeighbors[v0].end()) vertexNeighbors[v0].emplace_back(v1);
			if (std::find(vertexNeighbors[v0].begin(), vertexNeighbors[v0].end(), v2) == vertexNeighbors[v0].end()) vertexNeighbors[v0].emplace_back(v2);
			if (std::find(vertexNeighbors[v1].begin(), vertexNeighbors[v1].end(), v0) == vertexNeighbors[v1].end()) vertexNeighbors[v1].emplace_back(v0);
			if (std::find(vertexNeighbors[v1].begin(), vertexNeighbors[v1].end(), v2) == vertexNeighbors[v1].end()) vertexNeighbors[v1].emplace_back(v2);
			if (std::find(vertexNeighbors[v2].begin(), vertexNeighbors[v2].end(), v0) == vertexNeighbors[v2].end()) vertexNeighbors[v2].emplace_back(v0);
			if (std::find(vertexNeighbors[v2].begin(), vertexNeighbors[v2].end(), v1) == vertexNeighbors[v2].end()) vertexNeighbors[v2].emplace_back(v1);*/
		}
		// 计算老顶点的位置
		for (size_t i = 0; i < vertices.size(); i++)
		{
			Vertex v = vertices[i];
			int n = vertexNeighbors[v].size();
			if (n == 0) continue;
			float u = 0;
			if (n == 3) u = 0.1875f;
			else if (n > 3) u = 3.0f / (8.0f * n);
			if (u == 0) continue;
			glm::vec3 surroundingPos(0.0f, 0.0f, 0.0f);
			glm::vec3 surroundingNorm(0.0f, 0.0f, 0.0f);
			for (size_t j = 0; j < n; j++)
			{
				surroundingPos += vertexNeighbors[v][j].position;
				surroundingNorm += vertexNeighbors[v][j].normal;
			}
			glm::vec3 newPos = (1 - n * u) * v.position + u * surroundingPos;
			glm::vec3 newNorm = glm::normalize((1 - n * u) * v.normal + u * surroundingNorm);

			newVertices.emplace_back(Vertex(newPos, newNorm));
		}
		
		std::unordered_map<std::string, Vertex> edgePoints;
		std::unordered_map<Vertex, int> edgePointsNum;
		// 计算新顶点的位置
		for (size_t i = 0; i < indices.size() / 3; i++)
		{
			int t0 = indices[i];
			int t1 = indices[i + 1];
			int t2 = indices[i + 2];

			Vertex v0 = vertices[t0];
			Vertex v1 = vertices[t1];
			Vertex v2 = vertices[t2];

			std::string s01 = (t0 < t1 ? std::to_string(t0) + "|" + std::to_string(t1) : std::to_string(t1) + "|" + std::to_string(t0));
			std::string s12 = (t1 < t2 ? std::to_string(t1) + "|" + std::to_string(t2) : std::to_string(t2) + "|" + std::to_string(t1));
			std::string s20 = (t2 < t0 ? std::to_string(t2) + "|" + std::to_string(t0) : std::to_string(t0) + "|" + std::to_string(t2));

			Vertex ep01{};
			if (edgePoints.find(s01) != edgePoints.end())
			{
				ep01 = edgePoints[s01];
			}
			else
			{
				glm::vec3 surroundingPos(0.0f, 0.0f, 0.0f);
				glm::vec3 surroundingNorm(0.0f, 0.0f, 0.0f);
				for (size_t j = 0; j < oppsiteVertex[s01].size(); j++)
				{
					surroundingPos += vertices[oppsiteVertex[s01][j]].position;
					surroundingNorm += vertices[oppsiteVertex[s01][j]].normal;
				}
				ep01.position = 0.375f * (v0.position + v1.position) + 0.125f * surroundingPos;
				ep01.normal = glm::normalize(0.375f * (v0.normal + v1.normal) + 0.125f * surroundingNorm);
				
				edgePoints.emplace(s01, ep01);
				edgePointsNum.emplace(ep01, newVertices.size());
				newVertices.emplace_back(ep01);
			}

			Vertex ep12{};
			if (edgePoints.find(s12) != edgePoints.end())
			{
				ep12 = edgePoints[s12];
			}
			else
			{
				glm::vec3 surroundingPos(0.0f, 0.0f, 0.0f);
				glm::vec3 surroundingNorm(0.0f, 0.0f, 0.0f);
				for (size_t j = 0; j < oppsiteVertex[s12].size(); j++)
				{
					surroundingPos += vertices[oppsiteVertex[s12][j]].position;
					surroundingNorm += vertices[oppsiteVertex[s12][j]].normal;
				}
				ep12.position = 0.375f * (v1.position + v2.position) + 0.125f * surroundingPos;
				ep12.normal = glm::normalize(0.375f * (v1.normal + v2.normal) + 0.125f * surroundingNorm);
				edgePoints.emplace(s12, ep12);
				edgePointsNum.emplace(ep12, newVertices.size());
				newVertices.emplace_back(ep12);
			}

			Vertex ep20{};
			if (edgePoints.find(s20) != edgePoints.end())
			{
				ep01 = edgePoints[s20];
			}
			else
			{
				glm::vec3 surroundingPos(0.0f, 0.0f, 0.0f);
				glm::vec3 surroundingNorm(0.0f, 0.0f, 0.0f);
				for (size_t j = 0; j < oppsiteVertex[s20].size(); j++)
				{
					surroundingPos += vertices[oppsiteVertex[s20][j]].position;
					surroundingNorm += vertices[oppsiteVertex[s20][j]].normal;
				}
				ep20.position = 0.375f * (v2.position + v0.position) + 0.125f * surroundingPos;
				ep20.normal = glm::normalize(0.375f * (v2.normal + v0.normal) + 0.125f * surroundingNorm);
				edgePoints.emplace(s20, ep20);
				edgePointsNum.emplace(ep20, newVertices.size());
				newVertices.emplace_back(ep20);
			}

			newIndices.emplace_back(t0);
			newIndices.emplace_back(edgePointsNum[ep01]);
			newIndices.emplace_back(edgePointsNum[ep20]);

			newIndices.emplace_back(t1);
			newIndices.emplace_back(edgePointsNum[ep12]);
			newIndices.emplace_back(edgePointsNum[ep01]);

			newIndices.emplace_back(t2);
			newIndices.emplace_back(edgePointsNum[ep20]);
			newIndices.emplace_back(edgePointsNum[ep12]);

			newIndices.emplace_back(edgePointsNum[ep01]);
			newIndices.emplace_back(edgePointsNum[ep12]);
			newIndices.emplace_back(edgePointsNum[ep20]);
		}

		vertices = newVertices;
		indices = newIndices;
	}
	void Find(std::vector<int>& v, int key)
	{
		if (std::find(v.begin(), v.end(), key) != v.end()) return;
		v.emplace_back(key);
	}
	void Find(std::vector<Vertex>& v, Vertex key)
	{
		if (std::find(v.begin(), v.end(), key) != v.end()) return;
		v.emplace_back(key);
	}
private:
	unsigned int VBO, EBO;
	void SetupMesh();
};
#endif // !MESH_H

