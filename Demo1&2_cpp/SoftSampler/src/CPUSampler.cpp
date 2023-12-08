#include <cmath>
#include "CPUSampler.h"
#include <iostream>

CPUSampler::CPUSampler(unsigned char* data, int width, int height, int channels)
	:m_data(data), m_width(width), m_height(height), m_channels(channels)
{
}

CPUSampler::CPUSampler(const std::string& path)
{
}

std::vector<uint8_t> CPUSampler::Sample(float u, float v)
{
	std::vector<uint8_t> color(m_channels);

	u *= m_width;
	v *= m_height;

	// Clamp UV坐标到图像范围内
	int u0 = std::max(0, std::min(static_cast<int>(std::floor(u + 0.5f)), m_width) - 1);
	int v0 = std::max(0, std::min(static_cast<int>(std::floor(v + 0.5f)), m_height) - 1);

	int u1 = std::min(u0 + 1, m_width - 1);
	int v1 = std::min(v0 + 1, m_height - 1);

	// 计算插值权重
	float tu = std::max(u - u0 - 1 + 0.5f, 0.0f);
	float tv = std::max(v - v0 - 1 + 0.5f, 0.0f);

	for (int channel = 0; channel < m_channels; channel++)
	{
		// 获取像素值
		unsigned char p0 = m_data[(v0 * m_width + u0) * m_channels + channel];
		unsigned char p1 = m_data[(v0 * m_width + u1) * m_channels + channel];
		unsigned char p2 = m_data[(v1 * m_width + u0) * m_channels + channel];
		unsigned char p3 = m_data[(v1 * m_width + u1) * m_channels + channel];

		// 双线性插值
		float top = (1 - tu) * p0 + tu * p1;
		float bottom = (1 - tu) * p2 + tu * p3;

		float result = (1 - tv) * top + tv * bottom;
		color[channel] = static_cast<uint8_t>(result + 0.5f);
	}

	return color;
}
