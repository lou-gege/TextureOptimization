#include "GPUSampler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

GPUSampler::GPUSampler(unsigned char* data, int width, int height, int channels)
	:m_data(data), m_width(width), m_height(height), m_channels(channels)
{
}

std::vector<uint8_t> GPUSampler::Sample(float u, float v)
{
	glfwInit();
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(m_width, m_height, "GPUSampler", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	glGenTextures(1, &m_TexID);
	glBindTexture(GL_TEXTURE_2D, m_TexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);

	float vertices[] = {
	 1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f 
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 a_Position;

    void main() {
        gl_Position = vec4(a_Position, 1.0);
    }
)";

	const char* fragmentShaderSource = R"(
    #version 330 core
    layout(location = 0) out vec4 color;

    uniform sampler2D Texture;
    uniform vec2 texCoord;

    void main() {
        color = texture(Texture, texCoord);
    }
)";

	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);
	glUniform2f(glGetUniformLocation(shaderProgram, "texCoord"), u, v);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TexID);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);

	GLubyte pixel[3];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	std::vector<uint8_t> color(m_channels);
	for (int channel = 0; channel < m_channels; channel++)
	{
		color[channel] = static_cast<uint8_t>(pixel[channel]);
	}

	return color;
}

GLuint GPUSampler::CreateShaderProgram(const char* vertexShaderSrc, const char* fragmentShaderSrc)
{
	GLuint vertexShader, fragmentShader, shaderProgram;
	GLint success;
	GLchar infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
		exit(1);
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
		exit(1);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: " << infoLog << std::endl;
		exit(1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
