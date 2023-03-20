#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include"shader_class.h"
#include"camera_class.h"
#include"mesh_class.h"

// �ص������������ڱ��Ϸŵ�ʱ����е���
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// �����ڵ�����
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// ���ô��ڿ��
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ���ù�
glm::vec3 lightPos(1000.0f ,500.0f ,-50.0f);
glm::vec3 lightColor(0.7f, 0.98f, 0.94f);

int main()
{
	// glfw ��ʼ��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(800, 600, "opengl_project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// ���õ�ǰ����Ϊ������
	glfwMakeContextCurrent(window);
	// ע����ú���
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// �������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// ��ʼ��glad����ָ�룬����opengl����ǰ��Ҫ��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����Ȳ���
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ������ɫ��
	Shader ourShader("vertex_shader.vs", "fragment_shader.fs");

	// ����ģ��
	Mesh ourMesh("cat.obj");
	//ourMesh.LoopSubdivide();
	// �߿�ģʽ
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// ����
		processInput(window);

		// ��Ⱦ
		glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// ������Ⱦ��
		ourShader.Use();

		// ����任
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.SetMat4("projection", projection);
		ourShader.SetMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
		ourShader.SetMat4("model", model);

		// ����blinnphongģ�Ͳ���
		ourShader.SetVec3("viewPos", camera.Position);
		ourShader.SetVec3("lightPos", lightPos);
		ourShader.SetVec3("lightColor", lightColor);
		ourShader.SetVec3("ambient", ourMesh.Ka);
		ourShader.SetVec3("diffuse", ourMesh.Kd);
		ourShader.SetVec3("specular", ourMesh.Ks);

		// ����ģ��
		ourMesh.Draw(ourShader);

		// ������ɫ����
		glfwSwapBuffers(window);
		// ����Ƿ���û�д����¼�
		glfwPollEvents();
	}

	// ��ѡ���� : �ͷ����е���Դ

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// ��Ⱦ���ڵĳߴ��С
	glViewport(0, 0, width, height);
}

// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
