#pragma once
#ifndef SHAPE_H
#define SHAPE_H

#include<glm/glm.hpp>
#include<vector>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include"shader.h"

class Mesh
{
public:
	std::vector<int> vert_index;
	std::vector<int> norm_index;

	Mesh() :vert_index(0), norm_index(0) {}
	~Mesh() {}
	void SetupMesh(std::vector<glm::vec3>& vertices,
		std::vector<glm::vec3 >& normals)
	{
		std::vector<glm::vec3> myVert;
		std::vector<glm::vec3> myNorm;
		for (int i = 0; i < vert_index.size(); i++)
		{
			myVert.push_back(vertices[vert_index[i]]);
			myNorm.push_back(normals[norm_index[i]]);
		}

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER,
			myVert.size() * sizeof(glm::vec3) +
			myNorm.size() * sizeof(glm::vec3),
			NULL, GL_STATIC_DRAW);


		unsigned int offset_position = 0;
		unsigned int size_position = myVert.size() * sizeof(glm::vec3);

		glBufferSubData(GL_ARRAY_BUFFER, offset_position, size_position, myVert.data());
		glEnableVertexAttribArray(0);//layout(location = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(offset_position));

		unsigned int offset_normal = size_position;
		unsigned int size_normal = myNorm.size() * sizeof(glm::vec3);

		glBufferSubData(GL_ARRAY_BUFFER, offset_normal, size_normal, myNorm.data());
		glEnableVertexAttribArray(1);//layout(location = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(offset_normal));

		glBindVertexArray(0);
	}
	void Draw(const Shader& shader)
	{
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, vert_index.size());
		glBindVertexArray(0);
	}

private:
	unsigned int VAO, VBO;
};

class Shape
{
public:
	std::vector<Mesh> meshes;
	std::unordered_map<int, std::unordered_set<int>> vertex_map;
	std::unordered_map<std::string, std::unordered_set<int>> oppsiteVertex_map;
	Shape() : meshes(0) {}
	~Shape() {}
	void Draw(const Shader& shader)
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}


	void LoopSubdivision(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals)
	{
		// 初始化
		UpdateAdjacentTriangle(vertices);

		std::vector<Mesh> newMeshes;
		std::vector<glm::vec3> newVertices;
		std::vector<glm::vec3> newNormals;

		// 处理原来的点
		for (size_t i = 0; i < vertices.size(); i++)
		{
			float u = 0.0f;
			int n = vertex_map[i].size();
			if (n == 3) u = 0.1875f;
			else if (n > 3) u = 3.0f / (8.0f * n);

			glm::vec3 surrounding(0.0f, 0.0f, 0.0f);
			for (auto iter = vertex_map[i].begin(); iter != vertex_map[i].end(); ++iter)
			{
				surrounding += vertices[*iter];
			}
			newVertices.emplace_back((1.0f - n * u) * vertices[i] + u * surrounding);
		}

		// 处理边点
		std::unordered_map<std::string, int> edgePoints;
		for (size_t i = 0; i < meshes.size(); i++)
		{
			int v0 = meshes[i].vert_index[0];
			int v1 = meshes[i].vert_index[1];
			int v2 = meshes[i].vert_index[2];

			std::string s01 = ProcessString(v0, v1);
			std::string s12 = ProcessString(v1, v2);
			std::string s20 = ProcessString(v2, v0);

			GetEdgePoint(vertices, newVertices, edgePoints, s01, v0, v1);
			GetEdgePoint(vertices, newVertices, edgePoints, s12, v1, v2);
			GetEdgePoint(vertices, newVertices, edgePoints, s20, v2, v0);

			CreateNewMeshes(newVertices, newNormals, newMeshes, v0, edgePoints[s01], edgePoints[s20]);
			CreateNewMeshes(newVertices, newNormals, newMeshes, v1, edgePoints[s12], edgePoints[s01]);
			CreateNewMeshes(newVertices, newNormals, newMeshes, v2, edgePoints[s20], edgePoints[s12]);
			CreateNewMeshes(newVertices, newNormals, newMeshes, edgePoints[s01], edgePoints[s12], edgePoints[s20]);
		}

		vertices = newVertices;
		normals = newNormals;
		meshes = newMeshes;
	}

private:
	void UpdateAdjacentTriangle(std::vector<glm::vec3>& vertices)
	{
		vertex_map.clear();
		oppsiteVertex_map.clear();

		// 处理点的相邻点信息
		for (size_t i = 0; i < meshes.size(); i++)
		{
			int v0 = meshes[i].vert_index[0];
			int v1 = meshes[i].vert_index[1];
			int v2 = meshes[i].vert_index[2];

			vertex_map[v0].emplace(v1);
			vertex_map[v0].emplace(v2);
			vertex_map[v1].emplace(v0);
			vertex_map[v1].emplace(v2);
			vertex_map[v2].emplace(v0);
			vertex_map[v2].emplace(v1);

			std::string s01 = ProcessString(v0, v1);
			std::string s12 = ProcessString(v1, v2);
			std::string s20 = ProcessString(v2, v0);

			oppsiteVertex_map[s01].emplace(v2);
			oppsiteVertex_map[s12].emplace(v0);
			oppsiteVertex_map[s20].emplace(v1);
		}
	}

	std::string ProcessString(int t0, int t1)
	{
		std::string result = (t0 < t1 ? std::to_string(t0) + "|" + std::to_string(t1) : std::to_string(t1) + "|" + std::to_string(t0));
		return result;
	}

	void GetEdgePoint(std::vector<glm::vec3>& vertices,
		std::vector<glm::vec3>& newVertices,
		std::unordered_map<std::string, int>& edgePoints,
		std::string s01, int v0, int v1)
	{
		if (edgePoints.find(s01) != edgePoints.end())
		{
			return;
		}

		glm::vec3 surrounding(0.0f, 0.0f, 0.0f);
		for (auto iter = oppsiteVertex_map[s01].begin(); iter != oppsiteVertex_map[s01].end(); ++iter)
		{
			surrounding += vertices[*iter];
		}

		glm::vec3 ep01 = 0.375f * (vertices[v0] + vertices[v1]) + 0.125f * surrounding;

		edgePoints.emplace(s01, newVertices.size());
		newVertices.emplace_back(ep01);

	}

	void CreateNewMeshes(std::vector<glm::vec3>& newVertices,
		std::vector<glm::vec3>& newNormals,
		std::vector<Mesh>& newMeshes, int v0, int v1, int v2)
	{
		Mesh mesh;
		mesh.vert_index.emplace_back(v0);
		mesh.vert_index.emplace_back(v1);
		mesh.vert_index.emplace_back(v2);

		glm::vec3 normal = glm::normalize(glm::cross(newVertices[v0] - newVertices[v1], newVertices[v0] - newVertices[v2]));
		mesh.norm_index.emplace_back(newNormals.size());
		mesh.norm_index.emplace_back(newNormals.size());
		mesh.norm_index.emplace_back(newNormals.size());
		newNormals.emplace_back(normal);

		newMeshes.emplace_back(mesh);
	}
};
#endif SHAPE_H