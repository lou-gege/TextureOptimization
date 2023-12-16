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

// 将浮点数转换为16.8定点数
int floatTo168FixedPoint(float num) {
	// 获取整数部分
	int integerPart = static_cast<int>(num);

	// 获取小数部分，并将其乘以2^8，以得到8位小数的定点表示
	int fractionalPart = static_cast<int>(std::round((num - integerPart) * 2560.0f));

	// 组合整数部分和小数部分，左移8位放置小数部分
	int result = (integerPart << 8) | (fractionalPart & 0xFF);

	return result;
}

// 将16.8定点数转换回浮点数
float fixedPointToFloat168(int fixedPointValue) {
	// 获取整数部分（向右移动8位）
	int integerPart = fixedPointValue >> 8;

	// 获取小数部分（取低8位）
	int fractionalPart = fixedPointValue & 0xFF;

	// 还原小数部分为浮点数
	float floatFractionalPart = static_cast<float>(fractionalPart) / 256.0f;

	// 组合整数部分和小数部分，得到浮点数
	float result = static_cast<float>(integerPart) + floatFractionalPart;

	return result;
}

std::vector<uint8_t> CPUSampler::Sample(float u, float v)
{
	std::vector<uint8_t> color(m_channels);

	u *= m_width;
	v *= m_height;

	//float uDelta = fixedPointToFloat168(floatTo168FixedPoint(u - 0.5f));
	//float vDelta = fixedPointToFloat168(floatTo168FixedPoint(v - 0.5f));

	// Clamp UV坐标到图像范围内
	int u0 = std::max(0, std::min(static_cast<int>(std::floor(u - 0.5f)), m_width - 1));
	int v0 = std::max(0, std::min(static_cast<int>(std::floor(v - 0.5f)), m_height - 1));

	int u1 = std::min(u0 + 1, m_width - 1);
	int v1 = std::min(v0 + 1, m_height - 1);

	// 计算插值权重
	float tu = std::max(u - 0.5f - u0, 0.0f);
	float tv = std::max(v - 0.5f - v0, 0.0f);

	//int fixedTu = static_cast<int>(tu * 256.0f);
	//int fixedTv = static_cast<int>(tv * 256.0f);

	//tu = static_cast<float>(fixedTu) / 256.0f;
	//tv = static_cast<float>(fixedTv) / 256.0f;

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
