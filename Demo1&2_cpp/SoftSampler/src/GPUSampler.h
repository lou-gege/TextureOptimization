#pragma once

#include <vector>
#include <glad/glad.h>

class GPUSampler
{
public:
	GPUSampler(unsigned char* data, int width, int height, int channels);
	~GPUSampler() = default;

	std::vector<uint8_t> Sample(float u, float v);
private:
	GLuint CreateShaderProgram(const char* vertexShaderSrc, const char* fragmentShaderSrc);
private:
	unsigned char* m_data;
	int m_width, m_height, m_channels;
	uint32_t m_TexID;
};