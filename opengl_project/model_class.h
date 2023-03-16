#pragma once
#ifndef MODEL_H
#define MODEL_H


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include<glad/glad.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include"mesh_class.h"

#include<string>
#include<vector>
#include<iostream>

class Model
{
public:
	std::vector<Mesh> meshes;
	Model(const char* path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		bool ret = false;
		ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path);

		if (!err.empty())
		{
			std::cerr << "ERR: " << err << std::endl;
		}

		if (!ret)
		{
			printf("Failed to load/parse .obj.\n"); 
			return;
		}

		ProcessMesh(attrib,shapes);
	}
	void Draw(const Shader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}
private:
	void ProcessMesh(const tinyobj::attrib_t& attrib,const std::vector<tinyobj::shape_t>& shapes)
	{
		std::cout << "# of vertices : " << attrib.vertices.size() / 3 << std::endl;
		std::cout << "# of normals : " << attrib.normals.size() / 3 << std::endl;
		std::cout << "# of shapes : " << shapes.size() / 3 << std::endl;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		for (const auto&shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
				
				vertices.push_back(vertex);
				indices.push_back(indices.size());
			}
		}
		meshes.push_back(Mesh(vertices, indices));
	}
};
#endif // !MODEL_H

