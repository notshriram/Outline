#include <iostream>
#include<numeric>
#include<algorithm>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include<vector>
#include<cmath>
#include<fstream>
// GLFW
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"gl_PointSize=1.5f;\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 u_Color;\n"
"void main()\n"
"{\n"
"color = u_Color;\n"
"}\n\0";
constexpr int MAXVERTS = 10000;
int numPoints[MAXVERTS] = {0};
int lineNumber = 0;

bool moving = false;
GLfloat vertices[MAXVERTS][MAXVERTS];
GLuint VBO[MAXVERTS], VAO[MAXVERTS];

void addPoint(float xpos, float ypos) {
	if (numPoints[lineNumber] >= MAXVERTS / 2)return;
	float dotX = 2 * (float)xpos / WIDTH - 1.0f;
	float dotY = 1 - 2 * (float)(ypos / HEIGHT);
	vertices[lineNumber][2 * numPoints[lineNumber]] = dotX;
	vertices[lineNumber][2 * numPoints[lineNumber] + 1] = dotY;
	numPoints[lineNumber]++;
}
void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		addPoint(xpos, ypos);
		moving = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		moving = false;
		lineNumber++;
	}
}
int main()
{
	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Outline", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	const char* glsl_version = "#version 130";


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);


	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);




	// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	glUseProgram(shaderProgram);
	glEnable(GL_PROGRAM_POINT_SIZE);
	GLuint location = glGetUniformLocation(shaderProgram, "u_Color");
	ImVec4 clear_color = ImVec4(0.82f, 0.0f, 0.23f, 1.00f);
	ImVec4 bg_color = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		static float f = 0.3f;
		static int counter = 0;

		if (moving) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			addPoint(xpos, ypos);
		}

		glBindVertexArray(0);
		glUniform4f(location, clear_color.x, clear_color.y, clear_color.z, clear_color.w);


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		for (int i = 0; i <= lineNumber; i++) {
			if (numPoints[lineNumber] > 0 && numPoints[lineNumber] < MAXVERTS / 2) {
				glGenVertexArrays(1, &VAO[i]);
				glGenBuffers(1, &VBO[i]);
				// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
				glBindVertexArray(VAO[i]);

				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * numPoints[i], &vertices[i][0], GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

				glBindVertexArray(0);
				// Draw our first triangle
			}
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions

			glBindVertexArray(VAO[i]);
			glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, numPoints[i]);
			glBindVertexArray(0);

		}
		glfwPollEvents();
		ImGui::Begin("Outline");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Options");
		ImGui::ColorEdit3("BG color", (float*)& bg_color);
		ImGui::SliderFloat("Scale", &f, 0.f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("FG color", (float*)& clear_color); // Edit 3 floats representing a color
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Swap the screen buffers
		glfwSwapBuffers(window);

	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Properly de-allocate all resources once they've outlived their purpose
	for (int i = 0; i < lineNumber; i++){
		glDeleteVertexArrays(1, &VAO[i]);
		glDeleteBuffers(1, &VBO[i]);
		}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return EXIT_SUCCESS;
}