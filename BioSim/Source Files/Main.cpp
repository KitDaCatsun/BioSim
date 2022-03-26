#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr unsigned int screen_width = 800;
constexpr unsigned int screen_height = 600;

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
	glViewport(0, 0, width, height);
}

struct shader_source {
	std::string vertex_source;
	std::string fragment_source;
};

static shader_source parse_shader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class shader_type {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
	};

	std::stringstream ss[2];
	shader_type type = shader_type::NONE;

	std::string line;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = shader_type::VERTEX;
			} else if (line.find("fragment") != std::string::npos) {
				type = shader_type::FRAGMENT;
			}
		} else {
			if (type == shader_type::NONE) {
				ss[0] << line << "\n";
				ss[1] << line << "\n";
			} else {
				ss[static_cast<int>(type)] << line << "\n";
			}
		}
	}

	return {
		ss[static_cast<int>(shader_type::VERTEX)].str(),
		ss[static_cast<int>(shader_type::FRAGMENT)].str()
	};
}

int main() {
	// Initialise GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW Window
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "BioSim", NULL, NULL);
	if (window == NULL) {
		std::cout << "[ERROR] Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Load OpenGL Function Pointers with GLAD
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cout << "[ERROR] Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Build and Compile Shaders
	const shader_source shader_source = parse_shader("Shaders/basic.shader");

	const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_source = shader_source.vertex_source.c_str();
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_source = shader_source.fragment_source.c_str();
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);

	// Link Shaders
	const unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Element Buffers
	constexpr float vertices[] = {
		0.5f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
	   -0.5f, -0.5f,  0.0f,
	   -0.5f,  0.5f,  0.0f,
	};

	constexpr unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	// Generate Buffers
	unsigned int vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind Buffers
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
	glEnableVertexAttribArray(0);

	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Render Loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(.23f, .35f, 0.24f, 1.0f);

		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		process_input(window);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Cleanup
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
