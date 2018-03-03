#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::shared_ptr<spdlog::logger> logger;
GLuint hProgram;

GLuint prepareShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename, std::shared_ptr<spdlog::logger> logger) {

	logger->debug("Creating shaders");
	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	logger->debug("Loading vertex shader from {}", vertexShaderFilename);
	std::string vertexShaderSourceCode;
	std::ifstream vsStream(vertexShaderFilename, std::ios::in);
	if (vsStream.is_open()) {
		std::stringstream sstr;
		sstr << vsStream.rdbuf();
		vertexShaderSourceCode = sstr.str();
		vsStream.close();
	}
	else {
		logger->error("Can't open {}", vertexShaderFilename);
		getchar();
		return 0;
	}

	logger->debug("Loading fragment shader from {}", fragmentShaderFilename);
	std::string fragmentShaderSourceCode;
	std::ifstream fsStream(fragmentShaderFilename, std::ios::in);
	if (fsStream.is_open()) {
		std::stringstream sstr;
		sstr << fsStream.rdbuf();
		fragmentShaderSourceCode = sstr.str();
		fsStream.close();
	}

	GLint compileStatus = GL_FALSE;
	int infoLogLength;

	logger->debug("Compiling vertex shader : {}", vertexShaderFilename);
	char const * VertexSourcePointer = vertexShaderSourceCode.c_str();
	glShaderSource(hVertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(hVertexShader);

	logger->debug("Checking vertex shader...");
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &compileStatus);
	glGetShaderiv(hVertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(hVertexShader, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
		logger->debug("{}", &VertexShaderErrorMessage[0]);
	}

	logger->debug("Compiling fragment shader : {}", fragmentShaderFilename);
	char const * FragmentSourcePointer = fragmentShaderSourceCode.c_str();
	glShaderSource(hFragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(hFragmentShader);

	logger->debug("Checking fragment shader...");
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &compileStatus);
	glGetShaderiv(hFragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(hFragmentShader, infoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		logger->debug("{}", &FragmentShaderErrorMessage[0]);
	}

	logger->debug("Linking shader program...");
	GLuint hProgram = glCreateProgram();
	glAttachShader(hProgram, hVertexShader);
	glAttachShader(hProgram, hFragmentShader);
	glLinkProgram(hProgram);

	logger->debug("Checking shader program...");
	glGetProgramiv(hProgram, GL_LINK_STATUS, &compileStatus);
	glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(hProgram, infoLogLength, NULL, &ProgramErrorMessage[0]);
		logger->debug("{}", &ProgramErrorMessage[0]);
	}
	else
	{
		logger->debug("Shader program linked successfully");
	}

	glDetachShader(hProgram, hVertexShader);
	glDetachShader(hProgram, hFragmentShader);

	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);

	return hProgram;
}

void prepareScene()
{
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static const GLfloat vertex_buffer_data[] =
	{
		-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
	};

	static const GLuint vertex_index_data[] =
	{
		0, 2, 4,
		2, 4, 6
	};

	static const GLfloat texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f 
	};

	GLuint hVBO;
	glGenBuffers(1, &hVBO);
	glBindBuffer(GL_ARRAY_BUFFER, hVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	GLuint hEBO;
	glGenBuffers(1, &hEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_index_data), vertex_index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, hVBO);

	glDisableVertexAttribArray(0);
	hProgram = prepareShaderProgram("vertex_shader.glsl", "fragment_shader.glsl", logger);
	glUseProgram(hProgram);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void draw()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableVertexAttribArray(0);
	GLfloat timeValue = (GLfloat)glfwGetTime();
	GLfloat xVal = (sin(timeValue) / 2);
	GLfloat yVal = (cos(timeValue) / 2);
	GLint shiftUniformLocation = glGetUniformLocation(hProgram, "shift");
	glUniform3f(shiftUniformLocation, xVal, yVal, 0.0f);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
}

#ifdef _WIN32

#include<windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)

#else

int main()

#endif
{
	logger = spdlog::basic_logger_mt("y_shader", "y_shader.log");
	logger->set_level(spdlog::level::trace);
	logger->info("Y_Shader starting");

	glfwInit();
	logger->debug("glfwInit() completed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Y_Shader", nullptr, nullptr);
	if (window == nullptr)
	{
		logger->error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyboardCallback);

	glewExperimental = GL_TRUE;
	glewInit();
	if (glGenVertexArrays == NULL)
	{
		logger->error("glGenVertexArrays is not supported!");
		glfwTerminate();
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	prepareScene();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		draw();
		glfwSwapBuffers(window);
	}
	logger->info("Y_Shader finishing");
	logger->flush();

	return 0;
}
