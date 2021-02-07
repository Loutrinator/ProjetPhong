//
// Created by Marine on 13/01/2021.
//

#include "Engine.h"
#include <iostream>
#include "Shader.h"
#include "Object.h"
#include "Dragon.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

void Engine::setup(int width, int height, bool debug) {
	glfwInit();
	initWindow(width, height,debug);

	std::cout << "Driver: " << glGetString(GL_VERSION) << "\n";
	std::cout<< "GPU: " << glGetString(GL_RENDERER) << "\n";

	if(debug){
		activateDebugMode();
	}

	mainCamera.setPosition(glm::vec3(-9.8f, 8.3f, -1.16f));
	mainCamera.FOV = 90.0f;
	mainCamera.speed = 0.1f;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);
}

void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (Engine::instance->firstMouse)
	{
		Engine::instance->lastX = xpos;
		Engine::instance->lastY = ypos;
		Engine::instance->mainCamera.yaw   = 0;
		Engine::instance->mainCamera.pitch = 0;
		Engine::instance->firstMouse = false;
	}

	//inverser la formule ici pour inverser les axes
	float xoffset = (float)xpos - (float)Engine::instance->lastX;
	float yoffset = (float)Engine::instance->lastY - (float)ypos;
	Engine::instance->lastX = xpos;
	Engine::instance->lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Engine::instance->mainCamera.yaw   += xoffset;
	Engine::instance->mainCamera.pitch += yoffset;
}

void Engine::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int Engine::initWindow(int width, int height, bool debug) {

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(debug){
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	}

	window = glfwCreateWindow(width, height, "ProjetPhong", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, width, height); //set la zone sur laquelle tu fais ton rendu

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//resize

	glEnable(GL_DEPTH_TEST);
	return 1;
}

void Engine::messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "ERROR " << id << ": " << message << std::endl;
			//if (IsDebuggerPresent())
			//	__debugbreak();
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "WARNING " << id << ": " << message << std::endl;
			break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "INFO " << id << ": " << message << std::endl;
			break;
	}
}

void Engine::activateDebugMode(){
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(messageCallback,nullptr);
}

void Engine::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera.moveLeft();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera.moveRight();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera.moveForward();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera.moveBackward();
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		mainCamera.moveUp();
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		mainCamera.moveDown();
}

void Engine::run() {
	std::cout << "run" << "\n";

	Texture crystalTexture("lightSwirlMarble.jpg");

	Shader redMaterial("phongLighting");
	Shader blueMaterial("phongLighting");

	//creation objet
	Mesh dragonMesh;
	dragonMesh.setVertices(DragonVertices, sizeof(DragonVertices) / sizeof(float));
	dragonMesh.setIndices(DragonIndices, sizeof(DragonIndices) / sizeof(uint16_t));
	Object dragon(&dragonMesh, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, -45.0f, 0.0f), glm::vec3(1.0f));

	float planeVertices[] = {
			1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // top right
			1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
			-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // bottom left
			-1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,   // top left
	};
	uint16_t planeIndices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
	};

	Mesh planeMesh;
	planeMesh.setVertices(planeVertices,sizeof(planeVertices) / sizeof(float));
	planeMesh.setIndices(planeIndices,sizeof(planeIndices) / sizeof(uint16_t));

	Object plane(&planeMesh, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(10.0f));

	while (!glfwWindowShouldClose(window)) {

		//events
		glfwPollEvents();// check les evenements qui ont eu lieu depuis le dernier appel de cette fonction
		//inputs
		processInput();

		mainCamera.update();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//rendering

		const float radius = 20.0f;
		float lightX = sin(glfwGetTime()) * radius;
		float lightZ = cos(glfwGetTime()) * radius;
		glm::vec3 lightPos(lightX, 10.0f, lightZ);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 lightDiffuse = lightColor * 0.7f;
		glm::vec3 lightAmbient = lightDiffuse * 0.1f;
		glm::vec3 lightSpecular(1.0f);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 dragonModel = dragon.getTransform().getLocalMatrix();
		glm::mat4 dragonMvp = mainCamera.getProjection() * mainCamera.getView() * dragonModel;

		redMaterial.bind();
		redMaterial.setMat4("mvp",dragonMvp);
		redMaterial.setMat4("model",dragonModel);
		redMaterial.setVec3("camPos",mainCamera.getPosition());

		glm::vec3 materialDiffuse(1.0f, 0.0f, 0.0f);
		glm::vec3 materialAmbient = materialDiffuse;
		glm::vec3 materialSpecular(0.5f);
		float materialShininess = 256;

		redMaterial.setVec3("material.ambient",materialAmbient);
		redMaterial.setVec3("material.diffuse",materialDiffuse);
		redMaterial.setVec3("material.specular",materialSpecular);
		redMaterial.setFloat("material.shininess",materialShininess);
		glBindTexture(GL_TEXTURE_2D, crystalTexture.getId());

		redMaterial.setVec3("light.position",lightPos);
		redMaterial.setVec3("light.ambient",lightAmbient);
		redMaterial.setVec3("light.diffuse",lightDiffuse);
		redMaterial.setVec3("light.specular",lightSpecular);

		dragon.draw();

		glm::mat4 planeModel = plane.getTransform().getLocalMatrix();
		glm::mat4 planeMvp = mainCamera.getProjection() * mainCamera.getView() * planeModel;

		blueMaterial.bind();
		blueMaterial.setMat4("mvp",planeMvp);
		blueMaterial.setMat4("model",planeModel);
		blueMaterial.setVec3("camPos",mainCamera.getPosition());

		glm::vec3 groundDiffuse(0.0f, 0.3f, 0.3f);
		glm::vec3 groundAmbient = groundDiffuse;
		glm::vec3 groundSpecular(0.8f);
		float groundShininess = 128;

		blueMaterial.setVec3("material.ambient",groundAmbient);
		blueMaterial.setVec3("material.diffuse",groundDiffuse);
		blueMaterial.setVec3("material.specular",groundSpecular);
		blueMaterial.setFloat("material.shininess",groundShininess);
		glBindTexture(GL_TEXTURE_2D, crystalTexture.getId());

		blueMaterial.setVec3("light.position",lightPos);
		blueMaterial.setVec3("light.ambient",lightAmbient);
		blueMaterial.setVec3("light.diffuse",lightDiffuse);
		blueMaterial.setVec3("light.specular",lightSpecular);
		plane.draw();

		//swap
		glfwSwapBuffers(window); //échange les deux buffers (back buffer = tu fais ton rendu, front buffer = ton image affichée)
	}

	glfwTerminate();
}

std::unique_ptr<Engine> Engine::instance = std::make_unique<Engine>();


Engine::Engine() {
	deltaTime = 0.0f;
	lastFrame = glfwGetTime();
}