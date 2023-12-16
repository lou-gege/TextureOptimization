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

// ��������ת��Ϊ16.8������
int floatTo168FixedPoint(float num) {
	// ��ȡ��������
	int integerPart = static_cast<int>(num);

	// ��ȡС�����֣����������2^8���Եõ�8λС���Ķ����ʾ
	int fractionalPart = static_cast<int>(std::round((num - integerPart) * 2560.0f));

	// ����������ֺ�С�����֣�����8λ����С������
	int result = (integerPart << 8) | (fractionalPart & 0xFF);

	return result;
}

// ��16.8������ת���ظ�����
float fixedPointToFloat168(int fixedPointValue) {
	// ��ȡ�������֣������ƶ�8λ��
	int integerPart = fixedPointValue >> 8;

	// ��ȡС�����֣�ȡ��8λ��
	int fractionalPart = fixedPointValue & 0xFF;

	// ��ԭС������Ϊ������
	float floatFractionalPart = static_cast<float>(fractionalPart) / 256.0f;

	// ����������ֺ�С�����֣��õ�������
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

	// Clamp UV���굽ͼ��Χ��
	int u0 = std::max(0, std::min(static_cast<int>(std::floor(u - 0.5f)), m_width - 1));
	int v0 = std::max(0, std::min(static_cast<int>(std::floor(v - 0.5f)), m_height - 1));

	int u1 = std::min(u0 + 1, m_width - 1);
	int v1 = std::min(v0 + 1, m_height - 1);

	// �����ֵȨ��
	float tu = std::max(u - 0.5f - u0, 0.0f);
	float tv = std::max(v - 0.5f - v0, 0.0f);

	//int fixedTu = static_cast<int>(tu * 256.0f);
	//int fixedTv = static_cast<int>(tv * 256.0f);

	//tu = static_cast<float>(fixedTu) / 256.0f;
	//tv = static_cast<float>(fixedTv) / 256.0f;

	for (int channel = 0; channel < m_channels; channel++)
	{
		// ��ȡ����ֵ
		unsigned char p0 = m_data[(v0 * m_width + u0) * m_channels + channel];
		unsigned char p1 = m_data[(v0 * m_width + u1) * m_channels + channel];
		unsigned char p2 = m_data[(v1 * m_width + u0) * m_channels + channel];
		unsigned char p3 = m_data[(v1 * m_width + u1) * m_channels + channel];

		// ˫���Բ�ֵ
		float top = (1 - tu) * p0 + tu * p1;
		float bottom = (1 - tu) * p2 + tu * p3;

		float result = (1 - tv) * top + tv * bottom;
		color[channel] = static_cast<uint8_t>(result + 0.5f);
	}

	return color;
}
