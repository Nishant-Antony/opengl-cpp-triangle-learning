#include <iostream>
#include <glad/glad.h> // FUNCTION LOADER. ALLOWS YOU TO USE FUNCTIONS PROVIDED BY GPU DRIVER.
#include <GLFW/glfw3.h> // TO CREATE WINDOWS AND HANDLE EVENTS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // RESIZE WINDOW 
void processInput(GLFWwindow* window);

std::string loadShaderSrc(const char* filename); // FUNCTION THAT RETURNS STRING
// const -> will not changethe file in any way
// char* -> refers to C - style string

int main() {

	int success;
	char infoLog[512];

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 3.3

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // MODERN OPENGL ONLY.

# ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE)
#endif

		GLFWwindow* window = glfwCreateWindow(800, 600, "OPENGL TUTORIAL", NULL, NULL); // POINTER TO WINDOW

	if (window == NULL) { // WINDOW NOT CREATED
		std::cout << "COULD NOT CREATE WINDOW" << std::endl;
		glfwTerminate(); return -1;
	}

	glfwMakeContextCurrent(window); // MAKES window THE OFFICIAL WORKSPACE TO WORK IN.

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "FAILED TO INITIALIZE GLAD" << std::endl;
		glfwTerminate(); return -1;
	}


	glViewport(0, 0, 800, 600); // 0,0 -> START DRAWING FROM THE BOTTOM LEFT CORNER
	// 800, 600 -> 800 WIDTH, 600 HEIGHT

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // RESIZEABILITY

	/*
		SHADERS
	*/

	// COMPILE VERTEX SHADER

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER); // ASKS GPU TO CREATE A VERTEX SHADER AND RETURNS A INT [ NUMBER ID ]
	std::string vertShaderSrc = loadShaderSrc("assets/vertex_core.glsl"); // CALL THE FN. ON THE vertex_core.glsl
	const GLchar* vertShader = vertShaderSrc.c_str(); // OPENGL UNDERSTANDS ONLY C
	// TAKE THE vertShaderSrc STRING AND CONVERT
	// INTO c style STRING AND vertShader is pointer 
	// TO THAT STRING.

	glShaderSource(vertexShader, 1, &vertShader, NULL);
	// vertexShader -> THE UNQIUE ID CREATED
	// 1 -> NUMBER OF STRINGS
	// &vertShader -> the actual code
	// NULL -> ignore length, figure it out automatically

	glCompileShader(vertexShader);
	// text -> GPU instructions

	// CATCH ERROR
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) { // if not success
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Error with vertex shader comp.: " << std::endl << infoLog << std::endl;
	}

	// COMPILE FRAGMENT SHADER

	unsigned int fragmentShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragShaderSrc = loadShaderSrc("assets/fragment_core.glsl");
	const GLchar* fragShader = fragShaderSrc.c_str();
	glShaderSource(fragmentShader, 1, &fragShader, NULL);
	glCompileShader(fragmentShader);

	// CATCH ERROR

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Error with frag shader comp.: " << std::endl << infoLog << std::endl;
	}

	// main program shader
	unsigned int shaderProgram;

	shaderProgram = glCreateProgram(); // GPU, CREATE A PROGRAM

	glAttachShader(shaderProgram, vertexShader); // POSIITION LOGIC ATTATCHED
	glAttachShader(shaderProgram, fragmentShader); // FRAGMENT LOGIC ATTATCHED
	glLinkProgram(shaderProgram); // COMBINE BOTH INTO A WORKING PROGRAM

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	// CATCH ERRORS
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Linking error : " << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// VAO, VBO

	float vertices[] = {

		-0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f

	};

	unsigned int VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// SET ATTRIBUTE POINTER

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	while (!glfwWindowShouldClose(window)) { // glfwWindowShouldClose -> should the window be closed? 
		// 1 -> YES || 0 -> NO
		processInput(window);

		glClearColor(0, 0, 0, 255); // SETS THIS COLOR IN BUFFER
		glClear(GL_COLOR_BUFFER_BIT);

		// DRAWING SHAPES
		glBindVertexArray(VAO);

		glUseProgram(shaderProgram);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents(); // CHECK FOR EVENTS

	}

	glfwTerminate();

	return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

}

std::string loadShaderSrc(const char* filename) {

	std::ifstream file; // CREATE FILE READER
	std::stringstream buf; // TEMPORARY STORAGE

	std::string ret = ""; // HOLDS THE FINAL TEXT

	file.open(filename);

	if (file.is_open()) {
		buf << file.rdbuf(); // COPY EVERYTING FROM FILE TO BUFFER
		ret = buf.str(); // CONVERT BUF TO STRING AND STORE IN ret
	}

	else {
		std::cout << "COULD NOT OPEN " << filename << std::endl;
	}

	return ret; // RETURN THE FINAL TEXT

}