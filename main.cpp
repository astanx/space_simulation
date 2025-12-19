#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "shader.h"
#include "texture.h"

struct AppState
{
	int windowWidth = 800;
	int windowHeight = 600;
	
	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glm::mat4 ProjectionMatrix = glm::mat4(1.f);
	float fov = 45.f;
	float nearPlane = 0.1f;
	float farPlane = 100.f;

	glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 camFront = glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 camPosition = glm::vec3(0.f, 0.f, 3.f);
	glm::mat4 ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

	float deltaTime = 0.f;
	float lastFrame = 0.f;

	float yaw = -90.f;
	float pitch = 0.f;
	bool firstMouse = true;
	float lastX = windowWidth / 2.f;
	float lastY = windowHeight / 2.f;
	float sensitivity = 0.1f;
};

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texcoord;
	glm::vec3 normal;
};

Vertex vertices[] =
{
	glm::vec3(-0.5f, 0.5f, 0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(0.5f, -0.5f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(0.5f, 0.5f, 0.f), glm::vec3(1.f, 1.f, 0.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)
};


unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[] =
{
  0, 1, 2,
	0, 2, 3
};


unsigned nrOfIndicies = sizeof(indices) / sizeof(GLuint);


const double EPS = 1e-6;
const double G = 6.67430e-11;

class Object {
public:
  std::vector<double> position;     // x, y
  std::vector<double> velocity;     // vx, vy
  std::vector<double> acceleration; // ax, ay
  double mass;
  double radius;

  Object(std::vector<double> position, std::vector<double> velocity, double mass, double radius)
    : position(position), velocity(velocity), mass(mass), radius(radius)
  {
    //acceleration = {0.0, -9.81}; // Initial acceleration due to gravity
		acceleration = {0.0, 0.0};
  }

	void accelerate(const std::vector<double>& a) {
		acceleration[0] += a[0];
		acceleration[1] += a[1];
	};

	void move(double dt) {
		velocity[0] += acceleration[0] * dt;
		velocity[1] += acceleration[1] * dt;
		position[0] += velocity[0] * dt;
		position[1] += velocity[1] * dt;
		//acceleration = {0.0, -9.81}; // Reset acceleration after move
		acceleration = {0.0, 0.0};
		if (position[1] - radius < -1.0) { // Ground collision
			position[1] = -1.0 + radius;
			velocity[1] = -velocity[1] * 0.6; // Simple bounce with damping
		}
		if (position[1] + radius > 1.0) { // Ceiling collision
			position[1] = 1.0 - radius;
			velocity[1] = -velocity[1] * 0.6;
		}
		if (position[0] - radius < -1.0) { // Left wall collision
			position[0] = -1.0 + radius;
			velocity[0] = -velocity[0] * 0.6;
		}
		if (position[0] + radius > 1.0) { // Right wall collision
			position[0] = 1.0 - radius;
			velocity[0] = -velocity[0] * 0.6;
		}
	}

	void applyGravitation(const Object& other) {
		double dx = other.position[0] - position[0];
		double dy = other.position[1] - position[1];
		double distSq = dx * dx + dy * dy; 
		if (distSq < EPS) return; // Avoid singularity
		double dist = sqrt(distSq);
		double force = G * mass * other.mass / distSq;
		double ax = force * dx / (dist * mass);
		double ay = force * dy / (dist * mass);
		accelerate({ax, ay});
	}
};

int handleCollisions(Object& object, Object& object2) {
	double dx = object2.position[0] - object.position[0];
	double dy = object2.position[1] - object.position[1];
	double dist = sqrt(dx * dx + dy * dy);
	if (dist < EPS)
		return 0; // Avoid singularity
	if (dist < object.radius + object2.radius) // Simple collision response
	{
		double rvx = object2.velocity[0] - object.velocity[0];
		double rvy = object2.velocity[1] - object.velocity[1];
		double nx = dx / dist;
		double ny = dy / dist;
		double velAlongNormal = rvx * nx + rvy * ny;
		if (velAlongNormal > 0)
			return 0; // They are moving apart
		double e = 0.8; // Coefficient of restitution
		double j = -(1 + e) * velAlongNormal / (1 / object.mass + 1 / object2.mass);

		object.velocity[0] -= j * nx / object.mass;
		object.velocity[1] -= j * ny / object.mass;

		object.position[0] -= nx * (object.radius + object2.radius - dist) / 2;
		object.position[1] -= ny * (object.radius + object2.radius - dist) / 2;

		object2.velocity[0] += j * nx / object2.mass;
		object2.velocity[1] += j * ny / object2.mass;

		object2.position[0] += nx * (object.radius + object2.radius - dist) / 2;
		object2.position[1] += ny * (object.radius + object2.radius - dist) / 2;
	}
	return 0;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	AppState* appState = static_cast<AppState*>(glfwGetWindowUserPointer(window));
	if (appState->firstMouse) {
		appState->lastX = xpos;
		appState->lastY = ypos;
		appState->firstMouse = false;
	}

	float xoffset = xpos - appState->lastX;
	float yoffset = appState->lastY - ypos;
	appState->lastX = xpos;
	appState->lastY = ypos;

	xoffset *= appState->sensitivity;
	yoffset *= appState->sensitivity;

	appState->yaw += xoffset;
	appState->pitch += yoffset;

	if (appState->pitch > 89.0f)
		appState->pitch = 89.0f;
	if (appState->pitch < -89.0f)
		appState->pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(appState->yaw)) * cos(glm::radians(appState->pitch));
	direction.y = sin(glm::radians(appState->pitch));
	direction.z = sin(glm::radians(appState->yaw)) * cos(glm::radians(appState->pitch));
	appState->camFront	= glm::normalize(direction);

	appState->ViewMatrix = glm::lookAt(appState->camPosition, appState->camPosition + appState->camFront, appState->worldUp);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	AppState* appState = static_cast<AppState*>(glfwGetWindowUserPointer(window));
	
	appState->fov -= (float)yoffset;
	
	if (appState->fov < 1.0f)
		appState->fov = 1.0f;
	if (appState->fov > 45.0f)
		appState->fov = 45.0f;

	appState->ProjectionMatrix = glm::perspective(
		glm::radians(appState->fov), 
		static_cast<float>(appState->framebufferWidth) / static_cast<float>(appState->framebufferHeight), 
		appState->nearPlane, 
		appState->farPlane
	);
}

void processInput(GLFWwindow* window)
{
	AppState* appState = static_cast<AppState*>(glfwGetWindowUserPointer(window));
	float cameraSpeed = 2.5f * appState->deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		appState->camPosition += cameraSpeed * appState->camFront;
	}
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		appState->camPosition -= cameraSpeed * appState->camFront;
	}
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{	
		appState->camPosition -= glm::normalize(glm::cross(appState->camFront, appState->worldUp)) * cameraSpeed;
	}
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{	
		appState->camPosition += glm::normalize(glm::cross(appState->camFront, appState->worldUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{	
		appState->camPosition += cameraSpeed * appState->worldUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		appState->camPosition -= cameraSpeed * appState->worldUp;
	}
	appState->ViewMatrix = glm::lookAt(appState->camPosition, appState->camPosition + appState->camFront, appState->worldUp);
}


void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	if (height == 0) return;

	AppState* appState = static_cast<AppState*>(glfwGetWindowUserPointer(window));

	appState->framebufferWidth = width;
	appState->framebufferHeight = height;

	appState->ProjectionMatrix = glm::perspective(
		glm::radians(appState->fov), 
		static_cast<float>(width) / static_cast<float>(height), 
		appState->nearPlane, 
		appState->farPlane
	);

	glViewport(0, 0, width, height);
}

int main()
{
	glfwInit();

	AppState appState{};

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(appState.windowWidth, appState.windowHeight, "Gravity Simulation", NULL, NULL);
	if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
	}

	glfwGetFramebufferSize(window, &appState.framebufferWidth, &appState.framebufferHeight);
	glfwSetWindowUserPointer(window, &appState);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouseCallback);  
	glfwSetScrollCallback(window, scrollCallback);
	

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Initialization failed!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Shader core_program("shaders/vertex_core.glsl", "shaders/fragment_core.glsl");

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	// Texcoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	// Normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	Texture texture0("textures/earth.png", GL_TEXTURE_2D, 0);

	glm::mat4 ModelMatrix(1.f);
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.f),  glm::vec3(1.f, 0.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.f),  glm::vec3(0.f, 1.f, 0.f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.f),  glm::vec3(0.f, 0.f, 1.f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.f));

	appState.ProjectionMatrix = glm::perspective(
		glm::radians(appState.fov), 
		static_cast<float>(appState.framebufferWidth) / static_cast<float>(appState.framebufferHeight), 
		appState.nearPlane, 
		appState.farPlane
	);

	glm::vec3 lightPos0(0.f, 0.f, 1.f);

	core_program.use();

	core_program.set1i(texture0.getTextureUnit(), "texture0");

	core_program.setMat4fv(ModelMatrix, "ModelMatrix");
	core_program.setMat4fv(appState.ViewMatrix, "ViewMatrix");
	core_program.setMat4fv(appState.ProjectionMatrix, "ProjectionMatrix");
	
	core_program.setVec3f(lightPos0, "lightPos0");
	core_program.setVec3f(appState.camPosition, "camPosition");
	core_program.unuse();
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		appState.deltaTime = currentFrame - appState.lastFrame;
		appState.lastFrame = currentFrame; 
		glfwPollEvents();

		processInput(window);

		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		core_program.use();

		core_program.setMat4fv(ModelMatrix, "ModelMatrix");
		core_program.setMat4fv(appState.ViewMatrix, "ViewMatrix");
		core_program.setMat4fv(appState.ProjectionMatrix, "ProjectionMatrix");

		core_program.setVec3f(appState.camPosition, "camPosition");
		core_program.set1i(1, "isTexture");
	
		texture0.bind();

		glBindVertexArray(VAO);
		
		glDrawElements(GL_TRIANGLES, nrOfIndicies, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);

		glBindVertexArray(0);
		core_program.unuse();
		texture0.unbind();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}