#pragma once
#ifndef OBJLOAD_H
#define OBJLOAD_H

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include<glm/glm.hpp>

#include<vector>
#include<iostream>
#include"shape.h"

static std::vector<glm::vec3> vertices;
static std::vector<glm::vec3> normals;
static std::vector<Shape> myShapes;

static void ObjProcesser(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes)
{
	// 打印信息
	std::cout << "# of vertices : " << attrib.vertices.size() / 3 << std::endl;
	std::cout << "# of normals : " << attrib.normals.size() / 3 << std::endl;
	std::cout << "# of shapes : " << shapes.size() << std::endl;

	// 读取顶点
	for (size_t v = 0; v < attrib.vertices.size() / 3; v++)
	{
		glm::vec3 vert;
		vert.x = attrib.vertices[3 * v + 0];
		vert.y = attrib.vertices[3 * v + 1];
		vert.z = attrib.vertices[3 * v + 2];
		vertices.push_back(vert);
	}

	// 读取向量
	for (size_t v = 0; v < attrib.normals.size() / 3; v++)
	{
		glm::vec3 norm;
		norm.x = attrib.normals[3 * v + 0];
		norm.y = attrib.normals[3 * v + 1];
		norm.z = attrib.normals[3 * v + 2];
		normals.push_back(norm);
	}

	// 读取mesh
	for (size_t i = 0; i < shapes.size(); i++)
	{
		Shape shape;
		for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++)
		{

			tinyobj::index_t i0 = shapes[i].mesh.indices[3 * f + 0];
			tinyobj::index_t i1 = shapes[i].mesh.indices[3 * f + 1];
			tinyobj::index_t i2 = shapes[i].mesh.indices[3 * f + 2];

			Mesh mesh;
			mesh.vert_index.push_back(i0.vertex_index);
			mesh.vert_index.push_back(i1.vertex_index);
			mesh.vert_index.push_back(i2.vertex_index);

			mesh.norm_index.push_back(i0.normal_index);
			mesh.norm_index.push_back(i1.normal_index);
			mesh.norm_index.push_back(i2.normal_index);

			shape.meshes.push_back(mesh);
		}
		myShapes.push_back(shape);
	}
}
static void ObjLoader(const char* path)
{
	std::cout << "Loading " << path << std::endl;

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

	ObjProcesser(attrib, shapes);
}

#endif OBJLOAD_H