#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "world.h"

#include "utility/color.h"
#include "utility/ray.h"

#include "hittables/sphere.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main()
{
	camera cam;
	world scene(&cam);

	//scene.create_object(std::make_shared<sphere>(point3(0, 0, -1), 0.5, std::make_unique<material>(color(0.67, 0.5, 1))));
	//scene.create_object(std::make_shared<sphere>(point3(0, -100.5, -1), 100, std::make_unique<material>(color(0.4, 0.95, 0.4))));
	
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// set to opengl 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(cam.image_width, cam.image_height, "Ray Tracing Boi", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	// set up the texture for the Ray Tracer to output to
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, cam.image_width, cam.image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}