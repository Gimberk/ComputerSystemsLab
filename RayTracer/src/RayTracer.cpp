#ifdef _MSC_VER
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include "world.h"

#include "utility/color.h"
#include "utility/vec3.h"

#include "hittables/sphere.h"

#include <filesystem>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "utility/stb_image_write.h"

bool first_mouse = true;
float last_x, last_y;

camera cam;

// mouse input is only necessary when compiled on home device
#ifdef _MSC_VER
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow* window, double x_in, double y_in) {
	const float x_pos = static_cast<float>(x_in), y_pos = static_cast<float>(y_in);

	if (first_mouse) {
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = y_pos - last_y;
	last_x = x_pos;
	last_y = y_pos;

	const float sensitivity = 0.1f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	cam.yaw -= x_offset;
	cam.pitch -= y_offset;

	cam.pitch = std::clamp(cam.pitch, -89.0, 89.0);
			
	cam.update_camera();
}
#endif

int main()
{
	world scene(&cam);

	scene.create_object(std::make_shared<sphere>(point3(0, 0, -1), 0.5, 
		std::make_unique<material>(color(0.67, 0.5, 1))));
	scene.create_object(std::make_shared<sphere>(point3(0, -100.5, -1), 100, 
		std::make_unique<material>(color(0.4, 0.95, 0.4))));

	// create the window; only run when compiled on home device
	#ifdef _MSC_VER
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// set to opengl 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(cam.image_width, cam.image_height, "Isn't this where... we came in?", NULL, NULL);
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

	GLuint fbo; // framebuffer object
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind the fbo

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

		vec3 directional;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) directional += cam.forward_vector();
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) directional -= cam.forward_vector();
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) directional -= cam.right_vector();
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) directional += cam.right_vector();

		if (!directional.near_zero()) cam.center += unit_vector(directional) * cam.speed;

		cam.update_camera();

		// upload the ray tracer data to the texture
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cam.image_width, cam.image_height, GL_RGB, GL_UNSIGNED_BYTE, scene.generate_image()->data());

		// configure blit buffers
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT);

		// blit the pixels to the screen
		glBlitFramebuffer(0, 0, cam.image_width, cam.image_height, 0, cam.image_height, cam.image_width, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texture);
	glfwTerminate();
	#else
	const int accumulation_count = 786;
	
	double total_time_taken = 0;
	for (int frame_ct = 0; frame_ct < accumulation_count - 1; frame_ct++){
		scene.generate_image(false);
		total_time_taken += scene.get_time_for_last_frame();
	}
	
	const std::vector<unsigned char>* frame = scene.generate_image();

	const int channels = 3;
	const int stride_in_bytes = cam.image_width * channels;

	stbi_write_png("out.png", cam.image_width, cam.image_height, channels, frame->data(), stride_in_bytes);
	std::cout << "\nImage written to " << std::filesystem::current_path() << " in " << total_time_taken << " seconds" << '\n' 
		<< "\t- at " << total_time_taken / accumulation_count << " seconds per frame\n\n";
	#endif
}