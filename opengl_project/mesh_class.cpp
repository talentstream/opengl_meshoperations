#include"mesh_class.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// 构造函数
Mesh::Mesh(const char* path)
{
	// 加载模型
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

	std::cout << "# of vertices : " << attrib.vertices.size() / 3 << std::endl;
	std::cout << "# of normals : " << attrib.normals.size() / 3 << std::endl;
	std::cout << "# of shapes : " << shapes.size() << std::endl;	
	std::cout << "# of materials : " << materials.size() << std::endl;

	// 去重map
	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	// 读取 shapes 的 mesh 顶点属性
	for (const auto& shape : shapes)
	{
		std::vector<Vertex> triangle_vertices;
		int i = 0;
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

			// 顶点去重
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			triangle_vertices.push_back(vertex);
			i++;
			if (i == 3)
			{
				i = 0;
				AddFace({ triangle_vertices[0], triangle_vertices[1],triangle_vertices[2] });
				triangle_vertices.clear();
			}
			indices.push_back(uniqueVertices[vertex]);
		}
	}
	// 加载材质
	if (materials.empty())
	{
		Ka = glm::vec3(0.3373, 0.3373, 0.3373);
		Kd = glm::vec3(0.3373, 0.3373, 0.3373);
		Ks = glm::vec3(0.3500, 0.3500, 0.3500);
	}
	for (const auto& material : materials)
	{
		Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
		Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
		Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
	}
	std::cout << "before vertices faces :" << faces.size() << std::endl;
	std::cout <<"before vertices num :" << vertices.size() << std::endl;
	std::cout <<"before indices num :" << indices.size() << std::endl;
	// LoopSubdivide();
	std::cout << "after vertices num : " << vertices.size() << std::endl;
	std::cout << "after indices num : " << indices.size() << std::endl;
	SetupMesh();
}

void Mesh::SetupMesh()
{
	// 生成并连接VAO、VBO、EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	// 顶点坐标
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// 顶点法线
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void Mesh::Draw(const Shader& shader)
{
	// 绘制无纹理 Mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}




